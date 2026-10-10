#pragma once
#include "Cinematica.h"
#include "Personajes.h"
#include "../Entidades/Actor.h"
class EscenaRegreso : public EscenaCinematica {
private:
    Actor actorRalph;
    Actor actorFelix;
    CampoEstrellas estrellas;
    double asombro;
    double fundido;
    static const int SUELO = 80;
protected:
    void actualizarEscenario(double dt) override {
        actorRalph.actualizar(dt);
        actorFelix.actualizar(dt);
        estrellas.actualizar(dt);
        if (asombro > 0) asombro -= dt;
        if (fundido > 0) fundido -= dt * 1.5;
    }
    void dibujarEscenario(Lienzo& l) override {
        auto& b = BancoSprites::instancia();
        l.degradadoVertical(0, l.getAlto(), Color(6, 6, 24), Color(30, 18, 50));
        estrellas.dibujar(l);
        const Sprite* edificio = b.obtener(Personajes::FIXIT + "Edificio/secciones/planta_baja_entrada");
        edificio->dibujar(l, 8, SUELO - edificio->getAlto());
        b.obtener(Personajes::FIXIT + "Edificio/toldo_rojo")->dibujar(l, 88, SUELO - 62);
        for (int x = 0; x < l.getAncho(); x++) l.pixel(x, SUELO, Color(30, 110, 30));
        b.obtener(Personajes::FIXIT + "Objetos/pastel/pastel_0" + std::to_string(1 + ((int)(tiempoEscena * 3)) % 2))->dibujar(l, 54, SUELO - 16);
        const int pos[5] = { 18, 34, 70, 140, 156 };
        const int caras[5] = { 1, 4, 7, 10, 12 };
        for (int i = 0; i < 5; i++) {
            char id[64];
            sprintf_s(id, "Nicelanders/cara_%02d", caras[i]);
            int salto = asombro > 0 ? ((int)(asombro * 12 + i) % 2 ? 4 : 0) : 0;
            b.obtener(Personajes::FIXIT + id)->dibujar(l, pos[i], SUELO - 16 - salto, i > 2);
        }
        actorFelix.dibujar(l);
        actorRalph.dibujar(l);
        if (fundido > 0) Efectos::fundido(l, (float)fundido);
    }
    void alTerminar() override {
        ctx.progreso.completarNivel(1);
        ctx.irA(IdEscena::NIVEL1_FIN);
    }
public:
    explicit EscenaRegreso(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_REGRESO),
        actorRalph(270, SUELO, Personajes::ralphParado(), Personajes::ralphCamina()),
        actorFelix(110, SUELO, Personajes::felixParado(), Personajes::felixCamina()),
        estrellas(70, 240, 60, 9), asombro(0), fundido(1.0) {
        using namespace Personajes;
        hacer([this] { actorRalph.caminarA(196, 26); });
        esperarQue([this] { return actorRalph.llego(); });
        hacer([this] { actorFelix.caminarA(150, 24); });
        esperarQue([this] { return actorFelix.llego(); });
        decir("felix", L"FELIX", L"\x00A1Ralph! \x00A1Regresaste!", felix(), C_FELIX);
        decir("ralph", L"RALPH", L"Hola, F\x00E9lix...", ralphArcade(), C_RALPH);
        decir("felix", L"FELIX", L"\x00BFQu\x00E9 pasa, Ralph?", felix(), C_FELIX);
        decir("ralph", L"RALPH", L"Es que... no s\x00E9, F\x00E9lix. Yo tambi\x00E9n quiero ser premiado.", ralphArcade(), C_RALPH);
        hacer([this] { asombro = 0.9; ctx.audio.efecto(L"menu_error.wav"); });
        decir("nice", L"NICELANDERS", L"\x00A1\x00BFT\x00DA?! \x00A1\x00BFPREMIADO?!", nicelander(4), C_NICE);
        elegir(L"\x00BFQU\x00C9 HAR\x00C1 FELIX?", {
            L"Invitar a Ralph a celebrar el 30 aniversario, sin importar lo que digan",
            L"Pensar en los Nicelanders e invitar a Ralph a tomar un respiro fuera del juego" },
            [this](int r) {
                using namespace Personajes;
                ctx.progreso.decidir("felix_invita", r + 1);
                if (r == 0) {
                    decir("ralph", L"RALPH", L"Gracias, F\x00E9lix, pero s\x00E9 que los Nicelanders no me quieren aqu\x00ED. Ir\x00E9 a buscar una medalla de h\x00E9roe y la traer\x00E9 como prueba.", ralphArcade(), C_RALPH);
                    hacer([this] { decisionTomada(L"RALPH RECORDAR\x00C1 ESTO...", 1, 1); });
                }
                else {
                    decir("ralph", L"RALPH", L"...", ralphArcade(), C_RALPH);
                    hacer([this] { decisionTomada(L"RALPH RECORDAR\x00C1 ESTO...", 0, -1); });
                }
                hacer([this] { actorRalph.caminarA(280, 24); });
                esperarQue([this] { return actorRalph.llego(); });
            });
        esperar(1.5);
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"regreso.wav"); }
};
