#pragma once
#include "Cinematica.h"
#include "Personajes.h"
#include "../Entidades/Actor.h"
class EscenaReunion : public EscenaCinematica {
private:
    int toma;
    Actor actorRalph;
    Actor actorFelix;
    CampoEstrellas estrellas;
    double fundido;
    static const int SUELO = 80;
protected:
    void actualizarEscenario(double dt) override {
        actorRalph.actualizar(dt);
        actorFelix.actualizar(dt);
        estrellas.actualizar(dt);
        if (fundido > 0) fundido -= dt * 2;
    }
    void dibujarEscenario(Lienzo& l) override {
        auto& b = BancoSprites::instancia();
        if (toma == 0) {
            l.degradadoVertical(0, l.getAlto(), Color(6, 6, 24), Color(30, 18, 50));
            estrellas.dibujar(l);
            const Sprite* edificio = b.obtener(Personajes::FIXIT + "Edificio/secciones/planta_baja_entrada");
            edificio->dibujar(l, -40, SUELO - edificio->getAlto());
            const Sprite* dump = b.obtener(Personajes::FIXIT + "Escenario/basurero_dump");
            dump->dibujar(l, 196, SUELO - dump->getAlto() + 2);
            for (int x = 0; x < l.getAncho(); x++) l.pixel(x, SUELO, Color(30, 110, 30));
            actorFelix.dibujar(l);
            actorRalph.dibujar(l);
        }
        else {
            l.limpiar(Color(28, 28, 36));
            const Sprite* cartel = b.obtener("12_Escenarios_y_entornos_IA/Escenario1_Sala_Bad-Anon/pared_cartel_hoy_reunion");
            const Sprite* puerta = b.obtener("12_Escenarios_y_entornos_IA/Escenario1_Sala_Bad-Anon/pared_puerta");
            const Sprite* ascensor = b.obtener("12_Escenarios_y_entornos_IA/Escenario1_Sala_Bad-Anon/pared_ascensor");
            ascensor->dibujar(l, -40, SUELO - ascensor->getAlto());
            cartel->dibujar(l, 52, SUELO - cartel->getAlto());
            puerta->dibujar(l, 150, SUELO - puerta->getAlto());
            for (int x = 0; x < l.getAncho(); x++) { l.pixel(x, SUELO, Color(70, 70, 86)); l.pixel(x, SUELO + 1, Color(50, 50, 62)); }
            actorRalph.dibujar(l);
        }
        if (fundido > 0) Efectos::fundido(l, (float)fundido);
    }
    void alTerminar() override { ctx.irA(IdEscena::CINE_BADANON); }
public:
    explicit EscenaReunion(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_REUNION), toma(0),
        actorRalph(150, SUELO, Personajes::ralphParado(), Personajes::ralphCamina()),
        actorFelix(60, SUELO, Personajes::felixParado(), Personajes::felixCamina()),
        estrellas(70, 240, 70, 5), fundido(1.0) {
        using namespace Personajes;
        esperar(0.8);
        decir("felix", L"FELIX", L"\x00A1" L"A descansar! Hicieron un buen trabajo.", felix(), C_FELIX, L"felix_a_descansar.wav");
        hacer([this] { actorFelix.caminarA(30, 20); });
        esperar(0.6);
        hacer([this] { actorFelix.mostrar(false); });
        decir("ralph", L"RALPH", L"Tal vez no me sentir\x00ED" L"a as\x00ED si las cosas fueran diferentes despu\x00E9s del trabajo.", ralphArcade(), C_RALPH, L"ralph_monologo_1.wav");
        decir("ralph", L"RALPH", L"Pero esta es la realidad.", ralphArcade(), C_RALPH, L"ralph_monologo_2.wav");
        decir("ralph", L"RALPH", L"F\x00E9lix y los Nicelanders van a descansar a sus hogares, que \x00E9l repar\x00F3, y bueno... ya saben.", ralphArcade(), C_RALPH, L"ralph_monologo_3.wav");
        hacer([this] { actorRalph.caminarA(215, 22); });
        decir("ralph", L"RALPH", L"Se van a sus casas y yo a la m\x00ED" L"a. Y de hecho... es un basurero.", ralphArcade(), C_RALPH, L"ralph_monologo_4.wav");
        esperarQue([this] { return actorRalph.llego(); });
        esperar(0.6);
        decir("ralph", L"RALPH", L"Hoy es el 30 aniversario de mi juego... Creo que hoy s\x00ED ir\x00E9 a esa reuni\x00F3n.", ralphArcade(), C_RALPH);
        hacer([this] { fundido = 1.0; toma = 1; actorRalph.ubicar(-30, SUELO); actorRalph.caminarA(197, 30); });
        esperarQue([this] { return actorRalph.llego(); });
        hacer([this] { actorRalph.mostrar(false); ctx.audio.efecto(L"puerta.wav"); });
        esperar(1.0);
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"reunion.wav"); }
};
