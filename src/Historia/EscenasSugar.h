#pragma once
#include <cmath>
#include "Cinematica.h"
#include "Personajes.h"
#include "../Entidades/Actor.h"
#include "../Motor/Efectos.h"
namespace SugarCine {
    const std::string S = "03_Sugar_Rush/";
    const std::string V = "02_Vanellope_y_personajes/";
    const Color C_VANE(80, 230, 255);
    const Color C_REY(255, 120, 200);
    const Color C_TURBO(255, 80, 60);
    const Color C_SIS(120, 255, 140);
    inline Retrato vanellope() { return Retrato(SPR(V + "Vanellope/fila_1/vanellope_f1_01")); }
    inline Retrato rey() { return Retrato(SPR(S + "King_Candy/pose/king_candy_pose_01"), SPR(S + "King_Candy/pose/king_candy_pose_02")); }
    inline Retrato turbo() { return Retrato(SPR(S + "Turbo/identidad_pose/turbo_pose_01")); }
    inline Animacion vanellopeParada() { return Animacion(BancoSprites::instancia().lista(V + "Vanellope/fila_1/vanellope_f1_", { 1 }), 1); }
    inline Animacion vanellopeCamina() { return Animacion(BancoSprites::instancia().secuencia(V + "Vanellope/fila_1/vanellope_f1_", 2, 9), 10); }
    inline void mundo(Lienzo& l, int suelo, double t, bool noche = false) {
        auto& b = BancoSprites::instancia();
        if (noche) l.degradadoVertical(0, 120, Color(20, 16, 50), Color(70, 30, 70));
        else l.degradadoVertical(0, 120, Color(120, 200, 250), Color(250, 200, 230));
        const Sprite* fondo = b.obtener(S + "Escenario/fondo_pista_completo");
        fondo->dibujarParte(l, -58, suelo - 64, 0, 0, fondo->getAncho(), 64);
        for (int y = suelo; y < l.yMax(); y++)
            for (int x = l.xMin(); x < l.xMax(); x++)
                l.pixel(x, y, ((x / 8 + y / 4) % 2) ? Color(246, 160, 186) : Color(238, 148, 176));
        const char* props[5] = { "Props_mundo/paletas/paleta_rosa_grande", "Escenario/bastones/baston_grande", "Props_mundo/paletas/paleta_verde_grande",
                                 "Escenario/gomitas/gomita_verde", "Escenario/bastones/baston_alto_1" };
        const int px[5] = { 6, 40, 196, 226, 150 };
        for (int i = 0; i < 5; i++) {
            const Sprite* s = b.obtener(S + props[i]);
            s->dibujar(l, px[i] - s->getAncho() / 2, suelo + 6 - s->getAlto());
        }
        if (noche) for (int i = 0; i < 40; i++) l.pixel((i * 61) % 240, (i * 37) % 50 - 6, Color(255, 255, 220));
    }
    inline void capsula(Lienzo& l, int cx, int base) {
        Color casco(186, 192, 206), sombra(120, 126, 142), borde(60, 64, 80), franja(200, 40, 40);
        for (int j = 0; j < 40; j++) {
            int media = j < 10 ? 3 + j : (j < 36 ? 13 : 13 - (j - 35) * 2);
            for (int i = -media; i <= media; i++) l.pixel(cx + i, base - 40 + j, i > media / 3 ? sombra : casco);
            l.pixel(cx - media - 1, base - 40 + j, borde); l.pixel(cx + media + 1, base - 40 + j, borde);
            if (j == 24 || j == 25) for (int i = -media; i <= media; i++) l.pixel(cx + i, base - 40 + j, franja);
        }
        for (int j = -5; j <= 5; j++) for (int i = -5; i <= 5; i++) if (i * i + j * j <= 25) l.pixel(cx + i, base - 26 + j, (i * i + j * j >= 18) ? borde : Color(70, 150, 210));
    }
}
class EscenaLlegadaSugar : public EscenaCinematica {
private:
    Actor actorRalph, actorVane;
    bool enNave;
    double naveY, humo;
    bool naveCaida, bichoHuye, mostrarMedalla, noche;
    double xBicho, fundido;
    static const int SUELO = 92;
protected:
    void actualizarEscenario(double dt) override {
        actorRalph.actualizar(dt);
        actorVane.actualizar(dt);
        if (enNave && !naveCaida) { naveY += 120 * dt; if (naveY >= SUELO) { naveY = SUELO; naveCaida = true; humo = 1.5; ctx.audio.efecto(L"golpe_ralph.wav"); } }
        if (humo > 0) humo -= dt;
        if (bichoHuye) xBicho += 70 * dt;
        if (fundido > 0) fundido -= dt * 1.5;
    }
    void dibujarEscenario(Lienzo& l) override {
        auto& b = BancoSprites::instancia();
        SugarCine::mundo(l, SUELO, tiempoEscena, noche);
        if (enNave) {
            SugarCine::capsula(l, 70, (int)naveY);
            if (!naveCaida) for (int j = 0; j < 12; j++) l.sombrear(70 + (j % 3) - 1, (int)naveY + j, Glifo::MEDIO, Color(255, 160, 60));
            if (humo > 0) for (int i = 0; i < 30; i++) {
                double a = i * 2.4;
                l.sombrear(70 + (int)(std::cos(a) * (40 - humo * 20)), SUELO - 6 + (int)(std::sin(a) * 6), Glifo::MEDIO, Color(240, 240, 240));
            }
        }
        if (bichoHuye && xBicho < 260) {
            const Sprite* g = b.obtener("04_Heros_Duty/Cy-Bugs/explorador_chico/cybug_explorador_chico_0" + std::to_string(1 + ((int)(tiempoEscena * 12)) % 5));
            g->dibujar(l, (int)xBicho - g->getAncho() / 2, SUELO + 4 - g->getAlto(), true);
        }
        actorVane.dibujar(l);
        actorRalph.dibujar(l);
        if (mostrarMedalla) {
            int mx = (int)actorRalph.getX() + 14, my = SUELO - 46;
            for (int j = 0; j < 5; j++) { l.pixel(mx - 2 + j / 2, my - 6 + j, Color(200, 30, 40)); l.pixel(mx + 2 - j / 2, my - 6 + j, Color(200, 30, 40)); }
            for (int j = -4; j <= 4; j++) for (int i = -4; i <= 4; i++) if (i * i + j * j <= 16) l.pixel(mx + i, my + 2 + j, (i + j) < 0 ? Color(255, 205, 60) : Color(190, 130, 20));
            if (((int)(tiempoEscena * 4)) % 2) l.sombrear(mx + 6, my - 2, Glifo::MEDIO, Color(255, 240, 150));
        }
        if (noche && hablante.empty()) for (int k = 0; k < 3; k++) FuenteMini::caracter(l, (int)actorRalph.getX() + 10 + k * 4, SUELO - 56 - k * 4 - ((int)(tiempoEscena * 2) % 2), L'Z', Colores::BLANCO);
        if (fundido > 0) Efectos::fundido(l, (float)fundido);
    }
    void alTerminar() override { ctx.irA(IdEscena::CINE_SUGAR_CARRO); }
public:
    explicit EscenaLlegadaSugar(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_SUGAR_LLEGADA),
        actorRalph(-30, SUELO, Personajes::ralphParado(), Personajes::ralphCamina()),
        actorVane(260, SUELO, SugarCine::vanellopeParada(), SugarCine::vanellopeCamina()),
        enNave(c.progreso.decision("heros") != 2), naveY(-60), humo(0), naveCaida(false), bichoHuye(false), mostrarMedalla(false), noche(false),
        xBicho(70), fundido(1.0) {
        using namespace Personajes;
        using namespace SugarCine;
        const Progreso& p = c.progreso;
        actorVane.mostrar(false);
        esperar(1.0);
        if (enNave) {
            narrar(L"La nave de escape de Hero's Duty atraviesa la Central del Juego y cae en un mundo de dulces: SUGAR RUSH.");
            esperarQue([this] { return naveCaida; });
            esperar(1.2);
            hacer([this] { actorRalph.ubicar(70, SUELO); actorRalph.caminarA(110, 26); bichoHuye = true; xBicho = 70; ctx.audio.efecto(L"herido.wav"); });
            narrar(L"El Cy-Bug que se aferr\x00F3 a la nave se escapa y se esconde en el juego...");
        }
        else {
            narrar(L"Calhoun parte a Sugar Rush a cazar al Cy-Bug que escap\x00F3. No dej\x00F3 que Ralph la acompa\x00F1" L"ara... as\x00ED que Ralph la sigui\x00F3 a escondidas.");
            hacer([this] { actorRalph.caminarA(110, 20); bichoHuye = true; xBicho = 30; });
            esperarQue([this] { return actorRalph.llego(); });
            narrar(L"El Cy-Bug se escabulle entre las golosinas y se esconde en el juego...");
        }
        esperarQue([this] { return actorRalph.llego(); });
        hacer([this] { mostrarMedalla = true; ctx.audio.efecto(L"destello.wav"); });
        decir("ralph", L"RALPH", L"Bueno... al menos todav\x00ED" L"a tengo la medalla de Hero's Duty. Veamos qu\x00E9 clase de lugar es este.", ralphArcade(), C_RALPH);
        hacer([this] { mostrarMedalla = false; actorVane.mostrar(true); actorVane.caminarA(170, 34); });
        esperarQue([this] { return actorVane.llego(); });
        decir("vane", L"VANELLOPE", L"\x00A1" L"Ay! \x00A1Qu\x00E9 susto me diste! \x00BF" L"C\x00F3mo te llamas?", vanellope(), C_VANE);
        decir("ralph", L"RALPH", L"Ah... Ralph. Ralph el Demoledor.", ralphArcade(), C_RALPH);
        decir("vane", L"VANELLOPE", L"No eres de por aqu\x00ED, \x00BF" L"cierto?", vanellope(), C_VANE);
        decir("ralph", L"RALPH", L"No, bueno, s\x00ED... o sea, no de esta \x00E1rea. Solo vine a trabajar en algo.", ralphArcade(), C_RALPH);
        decir("vane", L"VANELLOPE", L"Oye... \x00BF" L"eres un vagabundo?", vanellope(), C_VANE);
        decir("ralph", L"RALPH", L"\x00A1No, no soy un vagabundo! Y estoy ocupado, as\x00ED que ya, vete a casa.", ralphArcade(), C_RALPH);
        decir("vane", L"VANELLOPE", L"\x00BFQu\x00E9 dices? No te oigo, te apesta tanto la boca que me dejas sorda.", vanellope(), C_VANE);
        decir("vane", L"VANELLOPE", L"\x00BFPor qu\x00E9 tus manos son tan espantosamente grandes?", vanellope(), C_VANE);
        decir("ralph", L"RALPH", L"No lo s\x00E9. \x00BFPor qu\x00E9 eres tan espantosamente irritante?", ralphArcade(), C_RALPH);
        decir("ralph", L"RALPH", L"Mira, niña... soy el villano de Fix-It Felix Jr. Hace 30 a\x00F1os que destruyo un edificio y nadie me lo agradece.", ralphArcade(), C_RALPH);
        decir("ralph", L"RALPH", p.decision("badanon") == 1
            ? L"En la reuni\x00F3n de villanos me recordaron mi lema: soy malo, y eso es bueno. Pero ya no me alcanza con eso."
            : L"En la reuni\x00F3n de villanos les dije que yo tambi\x00E9n pod\x00ED" L"a ganar una medalla de bueno. Nadie me crey\x00F3.", ralphArcade(), C_RALPH);
        decir("ralph", L"RALPH", p.decision("felix_invita") == 1
            ? L"F\x00E9lix quiso invitarme a su fiesta... pero los Nicelanders no me quieren ah\x00ED."
            : L"F\x00E9lix me pidi\x00F3 que me tomara un respiro fuera del juego. Nadie me detuvo.", ralphArcade(), C_RALPH);
        decir("ralph", L"RALPH", p.decision("heros") == 2
            ? L"En Hero's Duty, Calhoun me salv\x00F3 de un Cy-Bug... y por eso su escuadr\x00F3n la abandon\x00F3. Fue mi culpa."
            : L"En Hero's Duty consegu\x00ED esta medalla, pero el escuadr\x00F3n eligi\x00F3 salvarse y yo escap\x00E9 con un Cy-Bug colgado de mi nave.", ralphArcade(), C_RALPH);
        decir("vane", L"VANELLOPE", L"Yo soy Vanellope von Schweetz. Aqu\x00ED todos dicen que soy una falla... un error que nunca debe interactuar con los jugadores.", vanellope(), C_VANE);
        decir("vane", L"VANELLOPE", L"El rey lo prohibi\x00F3: no puedo correr. Pero mi sue\x00F1o siempre ha sido correr en esa pista.", vanellope(), C_VANE);
        decir("ralph", L"RALPH", L"... S\x00E9 lo que es querer que te acepten. Que te reconozcan.", ralphArcade(), C_RALPH);
        decir("ralph", L"RALPH", L"Est\x00E1 bien, niña. Te voy a ayudar.", ralphArcade(), C_RALPH);
        if (p.getEmpatia() >= 2) {
            hacer([this] { mostrarMedalla = true; ctx.audio.efecto(L"destello.wav"); });
            decir("ralph", L"RALPH", L"Toma. Es mi medalla de h\x00E9roe. Con esto puedes pagar tu inscripci\x00F3n a la carrera.", ralphArcade(), C_RALPH);
            decir("vane", L"VANELLOPE", L"\x00BF" L"En serio? \x00A1Gracias, apestoso! \x00A1Nadie hab\x00ED" L"a hecho algo as\x00ED por m\x00ED!", vanellope(), C_VANE);
            hacer([this] { mostrarMedalla = false; ctx.progreso.decidir("medalla", 1); decisionTomada(L"VANELLOPE RECORDAR\x00C1 ESTO...", 1, 0); });
            esperar(2.0);
        }
        else {
            hacer([this] { noche = true; fundido = 1.0; actorVane.mostrar(false); actorRalph.ponerAnimacion(Personajes::ralphTriste()); });
            esperar(1.5);
            narrar(L"Esa noche, mientras Ralph dorm\x00ED" L"a, Vanellope tom\x00F3 su medalla para inscribirse en la carrera...");
            hacer([this] { ctx.progreso.decidir("medalla", 2); actorVane.mostrar(true); actorVane.ubicar(150, SUELO); actorVane.caminarA(270, 50); });
            esperar(1.6);
            decir("ralph", L"RALPH", L"\x00A1" L"Ey! \x00A1Mi medalla! \x00A1Vuelve aqu\x00ED, niña!", ralphArcade(), C_RALPH);
        }
        esperar(0.8);
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"sugar_llegada.wav"); }
};
class EscenaCarroSugar : public EscenaCinematica {
private:
    Actor actorRalph, actorVane;
    int toma;
    double armado;
    bool roto, reyVisible;
    double chispas, fundido;
    int eleccion;
    static const int SUELO = 92;
protected:
    void actualizarEscenario(double dt) override {
        actorRalph.actualizar(dt);
        actorVane.actualizar(dt);
        if (toma == 0 && armado < 1) armado = std::min(1.0, armado + dt / 12.0);
        if (chispas > 0) chispas -= dt;
        if (fundido > 0) fundido -= dt * 1.5;
    }
    void dibujarCarro(Lienzo& l, int cx) const {
        const Sprite* k = BancoSprites::instancia().obtener(SugarCine::S + "Karts/kart_rosa_vacio");
        int x0 = cx - k->getAncho() / 2, y0 = SUELO - k->getAlto() + 2;
        if (roto) {
            for (int i = 0; i < 4; i++) k->dibujarParte(l, x0 + i * 13 - 6 + (i % 2) * 4, y0 + 8 + (i % 2) * 3, i * 11, 0, 11, k->getAlto());
            return;
        }
        int filas = (int)(armado * k->getAlto());
        k->dibujarParte(l, x0, y0 + k->getAlto() - filas, 0, k->getAlto() - filas, k->getAncho(), filas);
        if (chispas > 0 || (toma == 0 && armado < 1))
            for (int i = 0; i < 8; i++) {
                double a = tiempoEscena * 9 + i * 0.8;
                l.sombrear(cx + (int)(std::cos(a) * 18), y0 + 10 + (int)(std::sin(a) * 8), Glifo::MEDIO, i % 2 ? Color(255, 240, 120) : Color(255, 140, 200));
            }
    }
    void dibujarEscenario(Lienzo& l) override {
        auto& b = BancoSprites::instancia();
        if (toma == 1) {
            l.degradadoVertical(0, 120, Color(120, 40, 90), Color(60, 16, 50));
            for (int x = l.xMin(); x < l.xMax(); x += 30) for (int y = l.yMin(); y < SUELO; y++) if (y % 4 == 0) l.pixel(x, y, Color(255, 170, 220));
            for (int y = SUELO; y < l.yMax(); y++) for (int x = l.xMin(); x < l.xMax(); x++) l.pixel(x, y, (std::abs(x - 120) < 40) ? Color(200, 30, 60) : Color(250, 200, 120));
            const Sprite* r = b.obtener(SugarCine::S + "King_Candy/baston/king_candy_baston_0" + std::string(((int)(tiempoEscena * 3)) % 2 ? "2" : "1"));
            r->dibujar(l, 120 - r->getAncho() / 2, SUELO - r->getAlto() + 2);
        }
        else {
            SugarCine::mundo(l, SUELO, tiempoEscena);
            dibujarCarro(l, 120);
            actorVane.dibujar(l);
            actorRalph.dibujar(l);
            if (reyVisible) {
                const Sprite* r = b.obtener(SugarCine::S + "King_Candy/pose/king_candy_pose_0" + std::string(((int)(tiempoEscena * 2)) % 2 ? "2" : "1"));
                r->dibujar(l, 200 - r->getAncho() / 2, SUELO - r->getAlto() + 2, true);
            }
        }
        if (fundido > 0) Efectos::fundido(l, (float)fundido);
    }
    void alTerminar() override { ctx.irA(IdEscena::CINE_SUGAR_INICIO); }
public:
    explicit EscenaCarroSugar(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_SUGAR_CARRO),
        actorRalph(70, SUELO, Personajes::ralphParado(), Personajes::ralphCamina()),
        actorVane(165, SUELO, SugarCine::vanellopeParada(), SugarCine::vanellopeCamina()),
        toma(0), armado(0), roto(false), reyVisible(false), chispas(0), fundido(1.0), eleccion(0) {
        using namespace Personajes;
        using namespace SugarCine;
        esperar(0.8);
        decir("vane", L"VANELLOPE", L"\x00A1Ok, enci\x00E9ndelo! \x00A1Vamos a construir el carro m\x00E1s veloz de Sugar Rush!", vanellope(), C_VANE);
        decir("ralph", L"RALPH", L"Yo demuelo cosas, niña, no las construyo... pero creo que puedo intentarlo.", ralphArcade(), C_RALPH);
        hacer([this] { chispas = 2.0; ctx.audio.efecto(L"martillo.wav"); });
        esperar(4.0);
        hacer([this] { ctx.audio.efecto(L"martillo.wav"); chispas = 1.5; });
        esperar(4.0);
        hacer([this] { toma = 1; fundido = 1.0; });
        esperar(1.0);
        decir("rey", L"REY CANDY", L"\x00BFQu\x00E9 es esto? Mis sensores detectan un intruso... alguien de otro juego anda suelto en mi reino.", rey(), C_REY);
        decir("rey", L"REY CANDY", L"Hay un turbo... \x00A1" L"ejem! Hay un TURBO-problema. \x00A1Quiero decir, un problema enorme! \x00A1" L"Encu\x00E9ntrenlo!", rey(), C_REY);
        hacer([this] { toma = 2; fundido = 1.0; armado = 1; actorVane.mostrar(false); reyVisible = true; });
        esperar(1.0);
        decir("rey", L"REY CANDY", L"Ralph, \x00BFverdad? Escuche bien, buen hombre... esa niña es una falla.", rey(), C_REY);
        decir("rey", L"REY CANDY", L"Si corre y los jugadores la ven glitchearse, creer\x00E1n que el juego est\x00E1 roto y lo desconectar\x00E1n: \x00A1" L"FUERA DE SERVICIO!", rey(), C_REY);
        decir("rey", L"REY CANDY", L"Y como ella no puede salir del juego... morir\x00E1 con \x00E9l. Si de verdad la quiere ayudar... no la deje correr.", rey(), C_REY);
        hacer([this] { reyVisible = false; });
        decir("ralph", L"RALPH", L"...", ralphArcade(), C_RALPH);
        elegir(L"\x00BFQU\x00C9 HAR\x00C1 RALPH?", {
            L"Romper el carro de Vanellope (piensas que es por el bien de ella)",
            L"Abandonar a Vanellope antes de que te encuentre (las chicas del juego le rompen el carro)" },
            [this](int r) {
                using namespace Personajes;
                using namespace SugarCine;
                eleccion = r + 1;
                ctx.progreso.decidir("carro", r + 1);
                if (r == 0) {
                    hacer([this] { ctx.audio.efecto(L"golpe_ralph.wav"); roto = true; actorRalph.ponerAnimacion(Animacion(BancoSprites::instancia().lista("01_Fix-It_Felix_Jr/Ralph/fila_2/ralph_f2_", { 1, 2 }), 6)); });
                    esperar(1.2);
                    hacer([this] { actorRalph.ponerAnimacion(Personajes::ralphParado()); actorVane.mostrar(true); actorVane.ubicar(260, SUELO); actorVane.caminarA(175, 50); });
                    esperarQue([this] { return actorVane.llego(); });
                    decir("vane", L"VANELLOPE", L"\x00BFQu\x00E9... qu\x00E9 hiciste? \x00A1" L"Es mi carro! \x00A1" L"Eres un verdadero villano!", vanellope(), C_VANE);
                    hacer([this] { decisionTomada(L"VANELLOPE RECORDAR\x00C1 ESTO...", -1, 0); });
                }
                else {
                    hacer([this] { actorRalph.mirar(true); actorRalph.caminarA(-40, 30); });
                    esperarQue([this] { return actorRalph.llego(); });
                    narrar(L"Ralph se va sin decir nada. Las otras corredoras encuentran el carro de Vanellope... y lo destrozan.");
                    hacer([this] { ctx.audio.efecto(L"golpe_ralph.wav"); roto = true; actorVane.mostrar(true); actorVane.ubicar(260, SUELO); actorVane.caminarA(175, 50); });
                    esperarQue([this] { return actorVane.llego(); });
                    decir("vane", L"VANELLOPE", L"\x00BFRalph? ... Se fue. Como todos.", vanellope(), C_VANE);
                    hacer([this] { decisionTomada(L"VANELLOPE RECORDAR\x00C1 ESTO...", -1, -1); });
                }
                esperar(2.2);
            });
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"carro_vanellope.wav"); }
};
class EscenaInicioSugar : public EscenaCinematica {
private:
    int etapa;
    double carga, pixeles;
    Actor actorRalph, actorVane;
    bool carroNuevo, identidad;
    double destello;
    static const int SUELO = 92;
protected:
    void actualizarEscenario(double dt) override {
        actorRalph.actualizar(dt);
        actorVane.actualizar(dt);
        if (etapa == 0) pixeles += dt;
        if (etapa == 1) carga = std::min(1.0, carga + dt / 2.5);
        if (destello > 0) destello -= dt;
    }
    void dibujarEscenario(Lienzo& l) override {
        if (etapa < 2) {
            l.limpiar(Colores::NEGRO);
            int n = (int)(pixeles * 120);
            for (int i = 0; i < n && i < 900; i++) {
                unsigned h = (unsigned)i * 2654435761u;
                Color c((h >> 3) % 200 + 55, (h >> 11) % 200 + 55, (h >> 19) % 200 + 55);
                l.pixel((int)(h % 240), (int)((h >> 8) % 135) - 7, c);
            }
            if (etapa == 1) {
                FuenteMini::centrado(l, 40, L"[SISTEMA]", SugarCine::C_SIS);
                FuenteMini::centrado(l, 50, L"DESTINO DETECTADO...  SUGAR RUSH.", Colores::BLANCO);
                FuenteMini::centrado(l, 58, L"CARGANDO ENTORNO...", Colores::BLANCO);
                int w = 160, x0 = 40, llenos = (int)(carga * w);
                for (int i = 0; i < w; i++) { l.sombrear(x0 + i, 70, i < llenos ? Glifo::BLOQUE : Glifo::CLARO, i < llenos ? Color(255, 120, 200) : Color(120, 80, 110)); l.sombrear(x0 + i, 71, i < llenos ? Glifo::BLOQUE : Glifo::CLARO, i < llenos ? Color(255, 120, 200) : Color(120, 80, 110)); }
                FuenteMini::centrado(l, 76, std::to_wstring((int)(carga * 100)) + L"%", Colores::AMARILLO);
            }
            return;
        }
        SugarCine::mundo(l, SUELO, tiempoEscena);
        if (carroNuevo) {
            const Sprite* k = BancoSprites::instancia().obtener(SugarCine::S + "Karts/kart_rosa_vacio");
            k->dibujar(l, 135 - k->getAncho() / 2, SUELO - k->getAlto() + 2);
            if (destello > 0) for (int i = 0; i < 12; i++) {
                double a = i * 0.52 + tiempoEscena * 4;
                l.sombrear(135 + (int)(std::cos(a) * 26 * (1.5 - destello)), SUELO - 12 + (int)(std::sin(a) * 12), Glifo::BLOQUE, Color(255, 240, 150));
            }
        }
        actorVane.dibujar(l);
        actorRalph.dibujar(l);
        if (identidad) {
            l.panelTranslucido(l.xMin(), 2, l.xMax() - l.xMin(), 30, Colores::NEGRO, Glifo::OSCURO);
            FuenteMini::texto(l, 8, 4, L"[SISTEMA]  IDENTIDAD DETECTADA: VANELLOPE VON SCHWEETZ", SugarCine::C_SIS);
            FuenteMini::texto(l, 8, 12, L"ESTADO:", Colores::BLANCO);
            if (((int)(tiempoEscena * 6)) % 3) FuenteMini::texto(l, 40, 12, L"GLITCH", Color(255, 70, 220));
            FuenteMini::texto(l, 8, 20, L"ACCESO A LA CARRERA:", Colores::BLANCO);
            FuenteMini::texto(l, 92, 20, L"RESTRINGIDO", Color(255, 80, 80));
            if (((int)(tiempoEscena * 5)) % 4 == 0) {
                const Sprite* s = BancoSprites::instancia().obtener(SugarCine::V + "Vanellope/fila_1/vanellope_f1_01");
                s->dibujarSombreado(l, (int)actorVane.getX() - s->getAncho() / 2 + 3, SUELO - s->getAlto(), Glifo::MEDIO, Color(80, 240, 255));
            }
        }
    }
    void alTerminar() override { ctx.irA(IdEscena::NIVEL3_CARRERA); }
public:
    explicit EscenaInicioSugar(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_SUGAR_INICIO), etapa(0), carga(0), pixeles(0),
        actorRalph(-30, SUELO, Personajes::ralphParado(), Personajes::ralphCamina()),
        actorVane(110, SUELO, SugarCine::vanellopeParada(), SugarCine::vanellopeCamina()), carroNuevo(false), identidad(false), destello(0) {
        using namespace Personajes;
        using namespace SugarCine;
        esperar(2.5);
        hacer([this] { etapa = 1; ctx.audio.efecto(L"inicio.wav"); });
        esperarQue([this] { return carga >= 1; });
        esperar(0.8);
        hacer([this] { etapa = 2; ctx.audio.efecto(L"destello.wav"); });
        esperar(1.0);
        hacer([this] { identidad = true; });
        esperar(1.0);
        narrar(L"En Sugar Rush, todos los corredores compiten para convertirse en los mejores pilotos del arcade.");
        narrar(L"Sin embargo... Vanellope no es considerada una corredora. El sistema la identifica como un error.");
        narrar(L"Un error que nunca debi\x00F3 existir.");
        hacer([this] { identidad = false; });
        decir("vane", L"VANELLOPE", L"Alg\x00FAn d\x00ED" L"a voy a correr en esa pista.", vanellope(), C_VANE);
        hacer([this] { actorRalph.caminarA(60, 40); });
        esperarQue([this] { return actorRalph.llego(); });
        decir("ralph", L"RALPH", L"\x00BFY qu\x00E9 estamos esperando?", ralphArcade(), C_RALPH);
        hacer([this] { actorVane.mirar(true); });
        decir("vane", L"VANELLOPE", L"\x00BFT\x00FA otra vez? (molesta)", vanellope(), C_VANE);
        decir("ralph", L"RALPH", L"Necesito una medalla. T\x00FA necesitas una carrera. Parece un trato justo.", ralphArcade(), C_RALPH);
        hacer([this] { carroNuevo = true; destello = 1.5; ctx.audio.efecto(L"ventana_reparada.wav"); });
        esperar(1.6);
        decir("vane", L"VANELLOPE", L"(sonr\x00ED" L"e) Entonces... vamos a ganar.", vanellope(), C_VANE);
        esperar(0.5);
    }
    void entrarHistoria() override {
        if (Audio::existeMusica(L"sugar_rush_fondo.wav")) ctx.audio.reproducirMusica(L"sugar_rush_fondo.wav");
        else ctx.audio.detenerMusica();
    }
};
class EscenaRevelacionTurbo : public EscenaCinematica {
private:
    Actor actorRalph, actorVane;
    int etapa;
    double glitchRey, corrupcion, congelado, empujon;
    static const int SUELO = 92;
protected:
    void actualizarEscenario(double dt) override {
        actorRalph.actualizar(dt);
        actorVane.actualizar(dt);
        if (etapa >= 1) glitchRey += dt;
        if (etapa >= 3) corrupcion = std::min(1.0, corrupcion + dt / 4.0);
        if (congelado > 0) congelado -= dt;
        if (empujon > 0) { empujon -= dt; actorRalph.mover(-60 * dt, 0); actorVane.mover(-80 * dt, 0); }
    }
    void dibujarEscenario(Lienzo& l) override {
        auto& b = BancoSprites::instancia();
        SugarCine::mundo(l, SUELO, tiempoEscena, etapa >= 3);
        if (corrupcion > 0) {
            for (int y = l.yMin(); y < l.yMax(); y++)
                for (int x = l.xMin(); x < l.xMax(); x++) {
                    unsigned h = (unsigned)(x * 73856093u ^ y * 19349663u ^ (unsigned)(tiempoEscena * 8) * 83492791u);
                    if ((h % 1000) < corrupcion * 260) l.sombrear(x, y, (h & 1) ? Glifo::OSCURO : Glifo::MEDIO, (h & 2) ? Color(20, 6, 24) : Color(90, 20, 100));
                }
            for (int x = l.xMin(); x < l.xMax(); x++) {
                int alto = (int)(corrupcion * (8 + 6 * std::sin(x * 0.3 + tiempoEscena * 2)));
                for (int j = 0; j < alto; j++) l.pixel(x, SUELO + 6 - j, Color(16, 8, 20));
            }
        }
        if (etapa == 0 || etapa >= 1) {
            if (etapa < 4) {
                const Sprite* k = b.obtener(SugarCine::S + "Karts/kart_rosa_vacio");
                k->dibujar(l, 60 - k->getAncho() / 2, SUELO - k->getAlto() + 2);
            }
        }
        if (etapa >= 1 && etapa < 4) {
            const Sprite* rey = b.obtener(SugarCine::S + "King_Candy/pose/king_candy_pose_01");
            const Sprite* tur = b.obtener(SugarCine::S + "Turbo/identidad_brazos_arriba/turbo_brazos_0" + std::string(((int)(tiempoEscena * 3)) % 2 ? "2" : "1"));
            int cx = 185;
            bool muestraTurbo = etapa >= 2 || (((int)(glitchRey * (4 + glitchRey * 6))) % 3 == 0 && glitchRey > 1.0);
            const Sprite* s = muestraTurbo ? tur : rey;
            int dx = (etapa == 1 && ((int)(glitchRey * 20)) % 2) ? 3 : 0;
            s->dibujarSombreado(l, cx - s->getAncho() / 2 - 3 + dx, SUELO - s->getAlto() + 2, Glifo::MEDIO, Color(80, 240, 255), true);
            s->dibujarSombreado(l, cx - s->getAncho() / 2 + 3 - dx, SUELO - s->getAlto() + 2, Glifo::MEDIO, Color(255, 70, 220), true);
            s->dibujar(l, cx - s->getAncho() / 2 + dx, SUELO - s->getAlto() + 2, true);
        }
        if (etapa == 4) {
            const Sprite* bug = b.obtener("04_Heros_Duty/Cy-Bugs/reina_grande_2/cybug_reina_grande_2_0" + std::to_string(1 + ((int)(tiempoEscena * 9)) % 3));
            const Sprite* tur = b.obtener(SugarCine::S + "Turbo/identidad_pose/turbo_pose_01");
            int cx = 180, by = SUELO - 6 + (int)(std::sin(tiempoEscena * 3) * 3);
            bug->dibujarSombreado(l, cx - bug->getAncho() / 2 - 2, by - bug->getAlto(), Glifo::CLARO, Color(255, 60, 60), true);
            bug->dibujar(l, cx - bug->getAncho() / 2, by - bug->getAlto(), true);
            tur->dibujar(l, cx - tur->getAncho() / 2 + 2, by - bug->getAlto() - tur->getAlto() + 16, true);
        }
        if (etapa == 4 && hablante == "turbo" && ((int)(tiempoEscena * 2)) % 3 == 0) {
            const Sprite* viejo = b.obtener(SugarCine::S + "Turbo/arcade_caminar/turbo_caminar_0" + std::to_string(1 + ((int)(tiempoEscena * 6)) % 4));
            l.panelTranslucido(8, 8, 60, 50, Colores::NEGRO, Glifo::OSCURO);
            FuenteMini::texto(l, 12, 10, L"TURBOTIME", Color(255, 80, 60));
            viejo->dibujar(l, 38 - viejo->getAncho() / 2, 54 - viejo->getAlto());
        }
        actorVane.dibujar(l);
        actorRalph.dibujar(l);
        if (congelado > 0) {
            for (int y = l.yMin(); y < l.yMax(); y += 2) for (int x = l.xMin(); x < l.xMax(); x++) if ((x + y) % 4 == 0) l.sombrear(x, y, Glifo::CLARO, Color(80, 240, 255));
        }
        if (etapa == 3 && corrupcion > 0.3) {
            l.panelTranslucido(l.xMin(), 20, l.xMax() - l.xMin(), 52, Colores::NEGRO, Glifo::OSCURO);
            ctx.fuentes.roja.dibujarCentrado(l, 24, L"SISTEMA CORROMPIDO");
            FuenteMini::centrado(l, 40, L"[SISTEMA] UNA ENTIDAD EXTERNA HA INTERVENIDO EN EL C\x00D3" L"DIGO.", Colores::BLANCO);
            FuenteMini::centrado(l, 50, L"IDENTIFICACI\x00D3N:", Colores::BLANCO);
            if (((int)(tiempoEscena * 4)) % 2) ctx.fuentes.roja.dibujarCentrado(l, 58, L"TURBO");
        }
    }
    void alTerminar() override { ctx.irA(IdEscena::NIVEL_SECRETO); }
public:
    explicit EscenaRevelacionTurbo(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_TURBO),
        actorRalph(-30, SUELO, Personajes::ralphParado(), Personajes::ralphCamina()),
        actorVane(92, SUELO, SugarCine::vanellopeParada(), SugarCine::vanellopeCamina()),
        etapa(0), glitchRey(0), corrupcion(0), congelado(0), empujon(0) {
        using namespace Personajes;
        using namespace SugarCine;
        esperar(0.6);
        narrar(L"\x00A1VANELLOPE CRUZA LA META!");
        hacer([this] { actorRalph.caminarA(120, 40); });
        esperarQue([this] { return actorRalph.llego(); });
        decir("ralph", L"RALPH", L"\x00A1Lo lograste!", ralphArcade(), C_RALPH);
        decir("vane", L"VANELLOPE", L"\x00A1Lo hicimos!", vanellope(), C_VANE);
        hacer([this] { etapa = 1; ctx.audio.efecto(L"menu_error.wav"); ctx.audio.reproducirMusica(L"fondo_turbo.wav"); });
        decir("rey", L"REY CANDY", L"NO... ESTO ES IMPOSIBLE... ELLA NO PUE-E-ED-E-DE-DE-E..... \x00A1NOOOOOOOOOOOOO!", rey(), C_REY);
        hacer([this] { etapa = 2; ctx.audio.efecto(L"destello.wav"); });
        decir("turbo", L"TURBO", L"YO... SOY... \x00A1TURBOT\x00C1STICO!", turbo(), C_TURBO, L"turbo_turbotastico.wav");
        hacer([this] { ctx.audio.efecto(L"herido.wav"); });
        narrar(L"El Cy-Bug que lleg\x00F3 con Ralph aparece... y se contagia del c\x00F3" L"digo de Turbo.");
        hacer([this] { etapa = 3; congelado = 0; });
        decir("vane", L"VANELLOPE", L"\x00BFQu\x00E9 est\x00E1 pasando?", vanellope(), C_VANE);
        decir("ralph", L"RALPH", L"\x00A1Vanellope!", ralphArcade(), C_RALPH);
        hacer([this] { congelado = 2.5; ctx.audio.efecto(L"gameover.wav"); });
        esperar(4.5);
        hacer([this] { etapa = 4; congelado = 0; actorVane.mirar(false); });
        esperar(1.0);
        decir("turbo", L"TURBO", L"\x00BF" L"De verdad creyeron que pod\x00ED" L"an arreglar todo sin m\x00ED?", turbo(), C_TURBO);
        hacer([this] { actorRalph.caminarA(105, 30); });
        decir("ralph", L"RALPH", L"\x00BFT\x00FA hiciste esto?", ralphArcade(), C_RALPH);
        decir("turbo", L"TURBO", L"Yo hice mucho m\x00E1s que esto. Yo fui olvidado. Reemplazado. Borrado de mi propio juego.", turbo(), C_TURBO);
        decir("turbo", L"TURBO", L"Ustedes tienen su lugar. Ralph tiene su juego. F\x00E9lix tiene su medalla. Calhoun tiene su ej\x00E9rcito. Y ella...", turbo(), C_TURBO);
        decir("turbo", L"TURBO", L"...tiene un mundo entero dispuesto a aceptarla. \x00BFY yo qu\x00E9 tengo? \x00A1NADA!", turbo(), C_TURBO);
        decir("turbo", L"TURBO", L"Si no puedo tener mi propio juego... nadie tendr\x00E1 el suyo.", turbo(), C_TURBO);
        hacer([this] { corrupcion = 1; ctx.audio.efecto(L"golpe_ralph.wav"); });
        decir("ralph", L"RALPH", L"No dejar\x00E9 que borres lo que a mi amiga tanto le cost\x00F3 conseguir.", ralphArcade(), C_RALPH);
        decir("turbo", L"TURBO", L"\x00A1\x00BFY QU\x00C9 VAS A HACER T\x00DA SOLO?!", turbo(), C_TURBO);
        hacer([this] { empujon = 0.8; ctx.audio.efecto(L"golpe_ralph.wav"); });
        esperar(1.5);
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"sugar_rush_fondo.wav"); }
};
