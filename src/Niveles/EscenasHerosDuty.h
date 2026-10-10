#pragma once
#include <memory>
#include "../Historia/EscenaHistoria.h"
#include "../Historia/Cinematica.h"
#include "../Historia/Personajes.h"
#include "../Escenas/InterfazMenu.h"
#include "HerosDuty/JuegoHerosDuty.h"
class EscenaNivel2 : public EscenaHistoria {
private:
    std::unique_ptr<JuegoHerosDuty> juego;
    bool decisionAplicada;
protected:
    void entrarHistoria() override { juego = std::make_unique<JuegoHerosDuty>(ctx, JuegoHerosDuty::HISTORIA); }
    void actualizarHistoria(double dt) override {
        juego->actualizar(dt);
        int e = juego->getEleccion();
        if (e != 0 && !decisionAplicada) {
            decisionAplicada = true;
            ctx.progreso.decidir("heros", e);
            if (e == 1) decisionTomada(L"EL ESCUADR\x00D3N RECORDAR\x00C1 ESTO...", 0, -1);
            else decisionTomada(L"RALPH RECORDAR\x00C1 ESTO...", 1, 1);
        }
        if (juego->termino()) { ctx.progreso.sumarPuntos(juego->getPuntos()); ctx.irA(IdEscena::CINE_FINAL2); }
        else if (juego->perdio()) { ctx.reintentar = IdEscena::NIVEL2_HEROS; ctx.irA(IdEscena::PERDISTE); }
    }
    void dibujarHistoria(Lienzo& l) override { juego->dibujar(l); }
public:
    explicit EscenaNivel2(Contexto& c) : EscenaHistoria(c, IdEscena::NIVEL2_HEROS), decisionAplicada(false) {}
};
class EscenaFinal2 : public EscenaCinematica {
private:
    int desenlace;
    double capsulaY;
    double velNave;
    bool capsulaVuela;
    bool ralphEnCapsula;
    bool ralphCamina;
    double xRalph;
    bool bichoSalta;
    double xBicho, yBicho;
    double xEscuadron;
    bool escuadronSeVa;
    double negro;
    double tCartel;
    static const int PIES = 89;
    static const int NAVE_X = 206;
    void dibujarNave(Lienzo& l, int cx, int base) const {
        Color casco(186, 192, 206), sombra(120, 126, 142), borde(60, 64, 80), franja(200, 40, 40);
        const int ALTO_NAVE = 54;
        for (int j = 0; j < ALTO_NAVE; j++) {
            int y = base - ALTO_NAVE + j;
            int media = j < 14 ? 4 + j : (j < 48 ? 18 : 18 - (j - 47) * 2);
            for (int i = -media; i <= media; i++) l.pixel(cx + i, y, i > media / 3 ? sombra : casco);
            l.pixel(cx - media - 1, y, borde);
            l.pixel(cx + media + 1, y, borde);
            if (j >= 32 && j <= 34) for (int i = -media; i <= media; i++) l.pixel(cx + i, y, franja);
        }
        for (int j = 0; j < 12; j++) for (int k = 0; k < 3; k++) {
            l.pixel(cx - 19 - j / 2 - k, base - 14 + j, borde);
            l.pixel(cx + 19 + j / 2 + k, base - 14 + j, borde);
        }
        Color vidrio = ralphEnCapsula ? Color(230, 150, 120) : Color(70, 150, 210);
        int vy = base - 34;
        for (int j = -8; j <= 8; j++) for (int i = -8; i <= 8; i++)
            if (i * i + j * j <= 64) l.pixel(cx + i, vy + j, (i * i + j * j >= 50) ? borde : vidrio);
        if (ralphEnCapsula) {
            for (int i = -6; i <= 6; i++) for (int j = -7; j <= -4; j++) if (i * i + j * j <= 49) l.pixel(cx + i, vy + j, Color(110, 50, 30));
            l.rectangulo(cx - 3, vy - 2, 2, 2, Colores::NEGRO);
            l.rectangulo(cx + 2, vy - 2, 2, 2, Colores::NEGRO);
            l.rectangulo(cx - 2, vy + 3, 5, 2, Color(90, 20, 20));
        }
        else { l.sombrear(cx - 4, vy - 4, Glifo::MEDIO, Colores::BLANCO); l.sombrear(cx - 3, vy - 5, Glifo::CLARO, Colores::BLANCO); }
    }
    Retrato calhoun() const { return Retrato(SPR(SpritesHD::H + "Calhoun/caminar/calhoun_caminar_01")); }
    Retrato marine() const { return Retrato(SPR(SpritesHD::H + "Marines/fila_1/marine_fila_1_01")); }
    Retrato ralph() const { return Retrato(SPR(SpritesHD::H + "Ralph_armadura/victoria_sin_casco/ralph_victoria_01"), SPR(SpritesHD::H + "Ralph_armadura/victoria_sin_casco/ralph_victoria_02")); }
protected:
    void actualizarEscenario(double dt) override {
        tCartel += dt;
        if (ralphCamina && !ralphEnCapsula) {
            xRalph += 34 * dt;
            if (xRalph >= NAVE_X) { ralphEnCapsula = true; ctx.audio.efecto(L"puerta.wav"); }
        }
        if (capsulaVuela) { velNave += 60 * dt; capsulaY -= velNave * dt; }
        if (bichoSalta) {
            double k = std::min(1.0, dt * 6.0);
            xBicho += (NAVE_X - xBicho) * k;
            yBicho += (capsulaY + 26 - yBicho) * k;
        }
        if (escuadronSeVa) xEscuadron -= 30 * dt;
        if (negro >= 0) negro += dt;
    }
    void dibujarEscenario(Lienzo& l) override {
        auto& b = BancoSprites::instancia();
        JuegoHerosDuty::dibujarSala(l, 4, tCartel);
        for (int i = 0; i < 4; i++) {
            std::string f = std::to_string(1 + i % 2);
            const Sprite* m = b.obtener(SpritesHD::H + "Marines/fila_" + f + "/marine_fila_" + f + "_0" + std::to_string(1 + (i % 2) * 4));
            m->dibujar(l, (int)xEscuadron + (i % 2) * 18 - m->getAncho() / 2, PIES - 6 + i * 2 - m->getAlto(), escuadronSeVa);
        }
        const Sprite* c = b.obtener(SpritesHD::H + "Calhoun/caminar/calhoun_caminar_01");
        c->dibujar(l, 92 - c->getAncho() / 2, PIES - c->getAlto());
        if (desenlace == 1) {
            int base = (int)capsulaY;
            if (capsulaVuela) for (int j = 0; j < 22; j++) for (int i = -8 + j / 3; i <= 8 - j / 3; i++)
                l.sombrear(NAVE_X + i, base + j, j < 4 ? Glifo::BLOQUE : j < 9 ? Glifo::OSCURO : Glifo::MEDIO, j < 5 ? Color(255, 240, 150) : Color(255, 120, 40));
            dibujarNave(l, NAVE_X, base);
            int cuadro = bichoSalta ? 1 + ((int)(tCartel * 9)) % 3 : 4;
            const Sprite* jefe = b.obtener(SpritesHD::H + "Cy-Bugs/reina_grande_2/cybug_reina_grande_2_0" + std::to_string(cuadro));
            jefe->dibujar(l, (int)xBicho - jefe->getAncho() / 2, (int)yBicho - jefe->getAlto());
            if (!ralphEnCapsula) {
                int cuadroR = ralphCamina ? 1 + ((int)(tCartel * 6)) % 5 : 1;
                const Sprite* r = b.obtener(SpritesHD::H + "Ralph_armadura/fila_1/ralph_armadura_f1_0" + std::to_string(cuadroR));
                r->dibujar(l, (int)xRalph - r->getAncho() / 2, PIES - r->getAlto());
            }
        }
        else {
            const Sprite* r = b.obtener(SpritesHD::H + "Ralph_armadura/victoria_sin_casco/ralph_victoria_0" + std::to_string(1 + ((int)(tCartel * 2)) % 2));
            r->dibujar(l, 156 - r->getAncho() / 2, PIES - r->getAlto());
        }
        if (tCartel < 3.0) {
            l.panelTranslucido(l.xMin(), 30, l.xMax() - l.xMin(), 26, Colores::NEGRO, Glifo::OSCURO);
            FuenteMini::centrado(l, 34, L"FINAL DEL NIVEL 2", Colores::BLANCO);
            ctx.fuentes.amarilla.dibujarCentrado(l, 44, desenlace == 1 ? L"HACER LO CORRECTO" : L"UN NUEVO AMIGO");
        }
        if (negro >= 0) {
            l.cubrirTodo(negro < 0.4 ? Glifo::MEDIO : Glifo::BLOQUE, Colores::NEGRO);
            if (negro > 0.5) ctx.fuentes.amarilla.dibujarCentrado(l, 56, L"NIVEL 2 COMPLETADO");
        }
    }
    void alTerminar() override {
        ctx.progreso.completarNivel(2);
        ctx.irA(IdEscena::NIVEL2_FIN);
    }
public:
    explicit EscenaFinal2(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_FINAL2), desenlace(c.progreso.decision("heros") == 2 ? 2 : 1), capsulaY(PIES), velNave(10),
        capsulaVuela(false), ralphEnCapsula(false), ralphCamina(false), xRalph(150), bichoSalta(false), xBicho(176), yBicho(PIES),
        xEscuadron(30), escuadronSeVa(false), negro(-1), tCartel(0) {
        const Color C_CAL(255, 220, 120), C_ESC(150, 230, 150), C_RAL(255, 120, 90);
        esperar(3.2);
        if (desenlace == 1) {
            decir("", L"SISTEMA", L"Ralph encuentra una nave de escape y huye...", Retrato(), Color(120, 255, 140));
            hacer([this] { ralphCamina = true; });
            esperarQue([this] { return ralphEnCapsula; });
            esperar(0.5);
            hacer([this] { capsulaVuela = true; ctx.audio.efecto(L"salto.wav"); });
            esperar(0.4);
            hacer([this] { bichoSalta = true; ctx.audio.efecto(L"golpe_ralph.wav"); });
            esperar(2.6);
            decir("", L"SISTEMA", L"...pero el Cy-Bug se agarr\x00F3 de la nave y se fue con \x00E9l. Ahora va a infectar otro juego...", Retrato(), Color(120, 255, 140));
            decir("calhoun", L"CALHOUN", L"\x00BFQu\x00E9 he hecho...?", calhoun(), C_CAL);
            decir("esc", L"ESCUADR\x00D3N 1", L"Tranquila, jefa. Encontraremos al bicho y lo exterminaremos.", marine(), C_ESC);
            decir("esc", L"ESCUADR\x00D3N COMPLETO", L"\x00A1S\x00CD!", marine(), C_ESC);
            decir("calhoun", L"CALHOUN", L"No. Yo tengo que encargarme de esto... yo lo ocasion\x00E9.", calhoun(), C_CAL);
        }
        else {
            decir("", L"SISTEMA", L"Decides ayudar a Ralph, pero tu equipo no te mira con buenos ojos... ni a Ralph.", Retrato(), Color(120, 255, 140));
            decir("ralph", L"RALPH", L"Gracias, se\x00F1orita, eh... \x00A1Peach!", ralph(), C_RAL);
            decir("calhoun", L"CALHOUN", L"Me llamo Calhoun. \x00BFY t\x00FA? \x00BFT\x00FA qui\x00E9n eres?", calhoun(), C_CAL);
            decir("ralph", L"RALPH", L"Ralph. Ralph el Demoledor, del juego Fix-It Felix Jr.", ralph(), C_RAL);
            decir("esc", L"ESCUADR\x00D3N 1", L"Jefa, el bicho escap\x00F3...", marine(), C_ESC);
            decir("calhoun", L"CALHOUN", L"\x00BFQU\x00C9?! \x00A1\x00BFPOR QU\x00C9 NADIE LO REMAT\x00D3?!", calhoun(), C_CAL);
            decir("esc", L"ESCUADR\x00D3N 2", L"Est\x00E1" L"bamos viendo a los que estaban heridos... ya que no importan, parece.", marine(), C_ESC);
            decir("calhoun", L"CALHOUN", L"Chicos, no, esperen...", calhoun(), C_CAL);
            decir("esc", L"ESCUADR\x00D3N COMPLETO", L"Iremos a descansar, no podemos seguir parados. Ya no s\x00E9 si podemos confiar en usted, jefa.", marine(), C_ESC);
            hacer([this] { escuadronSeVa = true; });
            esperar(2.5);
            decir("ralph", L"RALPH", L"Ey... Calhoun, tranquila. Estoy aqu\x00ED para ti.", ralph(), C_RAL);
            decir("calhoun", L"CALHOUN", L"\x00A1" L"C\x00C1LLATE! \x00A1TODO ESTO ES CULPA TUYA! \x00BFPOR QU\x00C9 TUVISTE QUE VENIR A MI JUEGO? \x00A1VOY A BUSCAR A ESE CY-BUG!", calhoun(), C_CAL);
        }
        hacer([this] { negro = 0.001; });
        esperar(3.0);
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"herosduty_final.wav"); }
};
class EscenaMiniHerosDuty : public Escena {
private:
    std::unique_ptr<JuegoHerosDuty> juego;
    DialogoConfirmacion salir;
public:
    explicit EscenaMiniHerosDuty(Contexto& c) : Escena(c), salir(L"\x00BFSALIR DEL MINIJUEGO?", L"SE GUARDA TU R\x00C9" L"CORD") {}
    void entrar() override { juego = std::make_unique<JuegoHerosDuty>(ctx, JuegoHerosDuty::INFINITO); }
    void actualizar(double dt) override {
        if (salir.estaAbierto()) {
            int r = salir.manejar(ctx, dt);
            if (r == 1) ctx.irA(IdEscena::MINIJUEGOS);
            else if (r == 0) ctx.audio.reanudarMusica();
            return;
        }
        if (ctx.teclado.cancelar()) { ctx.audio.efecto(L"menu_atras.wav"); ctx.audio.pausarMusica(); salir.abrir(); return; }
        juego->actualizar(dt);
        if (juego->perdio()) ctx.irA(IdEscena::MINIJUEGOS);
    }
    void dibujar(Lienzo& l) override {
        juego->dibujar(l);
        if (salir.estaAbierto()) salir.dibujar(l, ctx.fuentes);
    }
};
