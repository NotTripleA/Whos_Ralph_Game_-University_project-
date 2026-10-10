#pragma once
#include <cmath>
#include <fstream>
#include <filesystem>
#include "Cinematica.h"
#include "Personajes.h"
#include "EscenasSugar.h"
#include "../Niveles/FixIt/PersonajesFixIt.h"
class EscenaFinales : public EscenaCinematica {
private:
    int cualFinal;
    int fondo;
    bool caja, medallaHeroe, notaRoca, ralphVisible, felixVisible, vaneVisible, preguntaHecha;
    double tCaja, alejar;
    IdEscena destino;
    EdificioFixIt edificio;
    FelixJugador felixAuto;
    RalphFixIt ralphAuto;
    double tEpilogo;
    bool enEpilogo;
    int pisoObjetivo, colObjetivo;
    static std::wstring nombreFinal(int f) {
        return f == 1 ? L"ARCADE RALPH" : f == 2 ? L"ESTO NO ES LO QUE ESPERABA, PERO..." : L"SIEMPRE SER\x00C9 EL VILLANO";
    }
    struct Sub { double t0, t1; const wchar_t* texto; };
    static const Sub* subtitulos(int& n) {
        static const Sub s[] = {
            { 1.4, 9.6, L"Ey, Ralph aqu\x00ED. \x00BFNo es grandioso? Todos est\x00E1n aqu\x00ED." },
            { 9.6, 19.0, L"Y bien, me alegra reportar, y estar\x00E1n felices de o\x00EDrlo, que vivo mi vida un juego a la vez." },
            { 19.0, 21.7, L"Claro que mi trabajo no cambi\x00F3." },
            { 21.7, 26.3, L"Pero los Nicelanders son m\x00E1s amables conmigo." },
            { 26.3, 38.1, L"Les pedimos a unos amigos que nos ayudaran en los niveles superiores... se los digo, no hab\x00ED" L"amos sido tan populares en a\x00F1os." },
            { 38.1, 45.5, L"Los videojugadores dicen que somos retro, que creo que significa antiguo pero cool." },
            { 45.5, 55.7, L"Y decid\x00ED que vivir en el basurero no me hac\x00ED" L"a sentir muy bien, as\x00ED que lo arregl\x00E9: me constru\x00ED una casa, y otras para los chicos nuevos, con ayuda de F\x00E9lix." },
            { 55.7, 68.3, L"\x00BFY qui\x00E9n creen que fue su padrino en su boda con Calhoun? S\x00ED, adivinaron: su amigo Nudillos, en persona. Fue un evento muy elegante." },
            { 72.5, 79.0, L"Y solo digamos que hubo algunas l\x00E1grimas." },
            { 79.0, 88.7, L"Pero tengo que admitir que la mejor parte de mi d\x00ED" L"a es cuando me arrojan del tejado, porque cuando los Nicelanders me levantan..." },
            { 88.7, 100.2, L"...tengo una vista perfecta de Sugar Rush, y puedo ver correr a Vanellope. Es natural en ella. Los jugadores la aman." },
            { 100.2, 109.0, L"Y s\x00E9 que no es necesaria una medalla para saber que soy un chico bueno..." },
            { 110.9, 116.0, L"Porque si esa ni\x00F1" L"a me quiere..." },
            { 116.4, 121.0, L"\x00BFqu\x00E9 tan malvado puedo ser?" } };
        n = (int)(sizeof(s) / sizeof(s[0]));
        return s;
    }
    void actualizarEpilogo(double dt) {
        tEpilogo += dt;
        if (edificio.etapaReparada(0) && edificio.etapaReparada(1)) edificio.romper(55);
        if (pisoObjetivo < 0 || edificio.ventanaSana(pisoObjetivo, colObjetivo)) {
            pisoObjetivo = -1;
            for (int p = 0; p < 2 && pisoObjetivo < 0; p++)
                for (int c = 0; c < EdificioFixIt::COLUMNAS; c++)
                    if (!edificio.ventanaSana(p, c)) { pisoObjetivo = p; colObjetivo = c; break; }
        }
        int dir = 0; bool subir = false, bajar = false, martillar = false;
        if (pisoObjetivo >= 0) {
            double tx = EdificioFixIt::centroVentana(colObjetivo);
            if (felixAuto.getPiso() < pisoObjetivo) subir = true;
            else if (felixAuto.getPiso() > pisoObjetivo) bajar = true;
            else if (std::fabs(tx - felixAuto.getX()) > 3) dir = tx > felixAuto.getX() ? 1 : -1;
            else martillar = ((int)(tEpilogo * 3)) % 2 == 0;
        }
        felixAuto.controlar(dir, subir, bajar, martillar, 0, 1, dt);
        felixAuto.actualizar(dt);
        if (felixAuto.golpeDeMartillo()) edificio.reparar(felixAuto.getPiso(), colObjetivo);
        ralphAuto.actualizar(dt);
        while (ralphAuto.consumirGolpe()) {}
    }
    void escenaArcadeJugando(Lienzo& l, bool ayudantes) {
        l.degradadoVertical(0, 120, Color(4, 4, 18), Color(14, 10, 30));
        int alto = l.yMax() - l.yMin();
        l.camara(0, EdificioFixIt::vistaEtapa(0) - (alto - EdificioFixIt::FILAS_VISTA) / 2 - l.yMin());
        edificio.dibujar(l);
        if (ayudantes) {
            auto& b = BancoSprites::instancia();
            const char* caras[4] = { "01_Fix-It_Felix_Jr/Nicelanders/cara_02", "01_Fix-It_Felix_Jr/Nicelanders/cara_05",
                                     "01_Fix-It_Felix_Jr/Nicelanders/cara_08", "01_Fix-It_Felix_Jr/Nicelanders/cara_11" };
            for (int i = 0; i < 4; i++) {
                int c = (i * 2 + 1) % 5, p = 1;
                int salto = ((int)(tEpilogo * 4) + i) % 2;
                b.obtener(caras[i])->dibujar(l, EdificioFixIt::centroVentana(c) - 8, EdificioFixIt::topePiso(p) + 18 - salto);
            }
        }
        ralphAuto.dibujar(l);
        felixAuto.dibujar(l);
        l.sinCamara();
        FuenteMini::texto(l, 40, l.yMin() + 1, L"1UP", Color(255, 40, 40));
        FuenteMini::texto(l, 98, l.yMin() + 1, L"HIGH SCORE", Color(255, 40, 40));
    }
    static void cielo(Lienzo& l, double t, Color arriba, Color abajo) {
        l.degradadoVertical(0, 120, arriba, abajo);
        for (int i = 0; i < 50; i++) if (((int)(t * 2) + i) % 7) l.pixel((i * 61) % 240, (i * 37) % 130 - 7, Color(230, 230, 255));
    }
    void escenaBadAnon(Lienzo& l) {
        auto& b = BancoSprites::instancia();
        const std::string B = "12_Escenarios_y_entornos_IA/Escenario1_Sala_Bad-Anon/";
        l.limpiar(Color(28, 28, 36));
        const Sprite* cartel = b.obtener(B + "pared_cartel_hoy_reunion");
        cartel->dibujar(l, 120 - cartel->getAncho() / 2, 84 - cartel->getAlto());
        b.obtener(B + "pared_ascensor")->dibujar(l, -40, 84 - 67);
        b.obtener(B + "pared_puerta")->dibujar(l, 186, 84 - 67);
        for (int y = 84; y < l.yMax() + 30; y++) for (int x = l.xMin(); x < l.xMax(); x++) l.pixel(x, y, ((x / 10 + y / 5) % 2) ? Color(70, 70, 86) : Color(60, 60, 74));
        const char* villanos[5] = { "11_Bad-Anon_villanos_IA/Satan_(Satans_Hollow)/base/satan_(satans_hollow)_base_01", "11_Bad-Anon_villanos_IA/Bowser/base/bowser_base_01",
                                    "11_Bad-Anon_villanos_IA/Clyde/base/clyde_base_01", "11_Bad-Anon_villanos_IA/M_Bison/base/m_bison_base_01",
                                    "11_Bad-Anon_villanos_IA/Dr_Eggman/base/dr_eggman_base_01" };
        const int xs[5] = { 22, 58, 150, 184, 218 };
        for (int i = 0; i < 5; i++) { const Sprite* s = b.obtener(villanos[i]); s->dibujar(l, xs[i] - s->getAncho() / 2, 112 - s->getAlto(), i >= 2); }
        const Sprite* r = b.obtener(((int)(tEpilogo * 5)) % 2 ? "01_Fix-It_Felix_Jr/Ralph/fila_1/ralph_f1_03" : "01_Fix-It_Felix_Jr/Ralph/fila_1/ralph_f1_01");
        r->dibujar(l, 104 - r->getAncho() / 2, 114 - r->getAlto());
    }
    void escenaLodoPastel(Lienzo& l, double s) {
        auto& b = BancoSprites::instancia();
        const std::string F = SpritesFixIt::F;
        cielo(l, tEpilogo, Color(4, 4, 18), Color(20, 14, 40));
        const Sprite* pb = b.obtener(F + "Edificio/secciones/planta_baja_entrada");
        pb->dibujar(l, 8, 104 - pb->getAlto());
        for (int y = 104; y < l.yMax() + 30; y++) for (int x = l.xMin(); x < l.xMax(); x++) l.pixel(x, y, ((x * 7 + y * 3) % 13) ? Color(20, 70, 24) : Color(34, 100, 36));
        for (int j = 0; j < 6; j++) for (int i = -30 + j; i < 30 - j; i++) l.pixel(70 + i, 102 + j, Color(92, 60, 30));
        if (s < 4.0) {
            const Sprite* lodo = b.obtener(F + "Ralph/fila_5/ralph_f5_08");
            lodo->dibujar(l, 70 - lodo->getAncho() / 2, 106 - lodo->getAlto());
        }
        else {
            const Sprite* r = b.obtener(F + "Ralph/fila_1/ralph_f1_0" + std::string(((int)(tEpilogo * 2)) % 2 ? "4" : "1"));
            r->dibujar(l, 70 - 32, 106 - 64);
            for (int i = 0; i < 4; i++) {
                char id[64];
                sprintf_s(id, "Nicelanders/cara_%02d", 1 + i * 3);
                b.obtener(F + id)->dibujar(l, 120 + i * 18, 104 - 16 - (((int)(tEpilogo * 4) + i) % 2));
            }
            b.obtener(F + "Objetos/pastel/pastel_01")->dibujar(l, 100, 104 - 22);
            l.rectangulo(118, 64, 92, 16, Color(150, 60, 170));
            l.marco(118, 64, 92, 16, Color(255, 220, 120));
            FuenteMini::texto(l, 122, 66, L"PARA NUESTRO", Colores::AMARILLO);
            FuenteMini::texto(l, 122, 73, L"VILLANO FAVORITO", Colores::AMARILLO);
        }
    }
    void escenaArcadePopular(Lienzo& l) {
        auto& b = BancoSprites::instancia();
        l.limpiar(Color(10, 6, 20));
        for (int x = l.xMin(); x < l.xMax(); x += 3) for (int y = 96; y < l.yMax(); y++) l.pixel(x, y, ((x / 3 + y) % 2) ? Color(40, 20, 60) : Color(26, 12, 40));
        const Sprite* p = b.obtener("12_Escenarios_y_entornos_IA/Escenario2_Estacion_GCS/pantallas_juegos_con_personas");
        p->dibujar(l, 120 - p->getAncho() / 2, 98 - p->getAlto());
        if (((int)(tEpilogo * 3)) % 2) ctx.fuentes.amarilla.dibujarCentrado(l, 6, L"\x00A1RETRO!");
    }
    void escenaCasa(Lienzo& l, double s) {
        auto& b = BancoSprites::instancia();
        const std::string F = SpritesFixIt::F;
        cielo(l, tEpilogo, Color(10, 8, 30), Color(40, 20, 50));
        for (int y = 104; y < l.yMax() + 30; y++) for (int x = l.xMin(); x < l.xMax(); x++) l.pixel(x, y, ((x * 5 + y * 3) % 11) ? Color(64, 40, 22) : Color(96, 62, 34));
        const Sprite* dump = b.obtener(F + "Escenario/basurero_dump");
        dump->dibujar(l, -60, 105 - dump->getAlto());
        double k = std::min(1.0, s / 7.0);
        int x0 = 120, ancho = 60, alto = 40, filas = (int)(alto * k);
        for (int j = 0; j < filas; j++)
            for (int i = 0; i < ancho; i++) {
                int y = 104 - j;
                bool junta = (j % 5 == 0) || ((i + (j / 5 % 2) * 5) % 10 == 0);
                l.pixel(x0 + i, y, junta ? Color(200, 190, 170) : Color(170, 50, 40));
            }
        if (k >= 1) {
            for (int j = 0; j < 14; j++) for (int i = -j * 2; i <= j * 2; i++) if (std::abs(i) <= 34) l.pixel(x0 + 30 + i, 64 - 14 + j, Color(90, 60, 50));
            l.rectangulo(x0 + 24, 84, 12, 20, Color(110, 70, 160));
            l.rectangulo(x0 + 6, 74, 12, 10, Color(255, 230, 140));
            l.rectangulo(x0 + 42, 74, 12, 10, Color(255, 230, 140));
        }
        const Sprite* r = b.obtener(F + "Ralph/fila_2/ralph_f2_0" + std::string(((int)(tEpilogo * 4)) % 2 ? "8" : "9"));
        r->dibujar(l, 70 - 32, 106 - 64);
        const Sprite* f = b.obtener(F + "Felix/fila_1/felix_f1_" + std::string(((int)(tEpilogo * 6)) % 2 ? "09" : "05"));
        f->dibujar(l, 196 - 20, 106 - 40);
        if (((int)(tEpilogo * 6)) % 2) b.obtener(F + "Efectos/destello/destello_01")->dibujar(l, 182, 76);
    }
    void escenaBoda(Lienzo& l, double s) {
        auto& b = BancoSprites::instancia();
        const std::string E = "02_Vanellope_y_personajes/Personajes_escala/";
        l.degradadoVertical(0, 120, Color(255, 200, 90), Color(200, 90, 30));
        for (int y = l.yMin(); y < 96; y++)
            for (int x = l.xMin(); x < l.xMax(); x++) {
                bool plomo = ((x - 120) * (x - 120) / 40 + y) % 22 == 0 || (x % 30 == 0) || ((x + y) % 41 == 0);
                if (plomo) l.pixel(x, y, Color(90, 40, 20));
            }
        for (int y = 96; y < l.yMax() + 30; y++) for (int x = l.xMin(); x < l.xMax(); x++) l.pixel(x, y, ((x / 8 + y / 4) % 2) ? Color(240, 225, 200) : Color(225, 205, 175));
        l.rectangulo(96, 74, 48, 22, Color(250, 245, 235));
        for (int i = 0; i < 12; i++) l.sombrear(98 + i * 4, 72 - (i % 3), Glifo::BLOQUE, i % 2 ? Color(255, 255, 255) : Color(255, 200, 220));
        const Sprite* fel = b.obtener(E + "felix");
        const Sprite* cal = b.obtener(E + "calhoun");
        const Sprite* ral = b.obtener(E + "ralph");
        const Sprite* van = b.obtener("02_Vanellope_y_personajes/Vanellope/fila_1/vanellope_f1_01");
        van->dibujar(l, 40 - van->getAncho() / 2, 104 - van->getAlto());
        for (int i = 0; i < 10; i++) {
            int px = 48 + (int)(std::fmod(s * 14 + i * 9, 50.0)), py = 60 + (int)(std::fmod(s * 20 + i * 13, 44.0));
            l.pixel(px, py, i % 2 ? Color(255, 140, 180) : Color(255, 255, 255));
        }
        fel->dibujar(l, 108 - fel->getAncho() / 2, 104 - fel->getAlto());
        cal->dibujar(l, 132 - cal->getAncho() / 2, 104 - cal->getAlto());
        for (int j = 0; j < 30; j++) for (int i = -3 - j / 4; i <= 3 + j / 4; i++) if (j > 1) l.sombrear(132 + i + 2, 104 - cal->getAlto() + j, Glifo::CLARO, Colores::BLANCO);
        ral->dibujar(l, 196 - ral->getAncho() / 2, 104 - ral->getAlto());
        l.rectangulo(193, 104 - ral->getAlto() + 21, 7, 3, Colores::NEGRO);
        if (tEpilogo > 70.0)
            for (int k = 0; k < 3; k++) {
                int ty = 104 - ral->getAlto() + 13 + (int)std::fmod(s * 18 + k * 6, 16.0);
                l.pixel(190 + (k % 2) * 7, ty, Color(120, 200, 255));
            }
    }
    void escenaTecho(Lienzo& l, double s) {
        auto& b = BancoSprites::instancia();
        const std::string F = SpritesFixIt::F;
        cielo(l, tEpilogo, Color(4, 4, 18), Color(20, 14, 40));
        l.rectangulo(l.xMin(), 92, l.xMax() - l.xMin(), 60, Color(230, 220, 205));
        for (int x = l.xMin(); x < l.xMax(); x++) l.pixel(x, 92, Color(255, 250, 240));
        b.obtener(F + "Edificio/puertas/puerta_03")->dibujar(l, 20, 92 - 32);
        const Sprite* f = b.obtener(F + "Felix/fila_1/felix_f1_" + std::to_string(12 + ((int)(tEpilogo * 4)) % 3));
        f->dibujar(l, 70, 92 - 40);
        if (s < 3.0) {
            int my = (int)(-30 + s / 3.0 * 52);
            b.obtener(F + "Objetos/medallas/medalla_oro")->dibujar(l, 74, my);
        }
        for (int i = 0; i < 5; i++) {
            char id[64];
            sprintf_s(id, "Nicelanders/cara_%02d", 1 + i * 2);
            b.obtener(F + id)->dibujar(l, 132 + i * 14, 92 - 16 - (s > 4.0 && s < 5.0 ? 8 : 0) - (((int)(tEpilogo * 6) + i) % 2));
        }
        if (s < 4.0) b.obtener(F + "Ralph/fila_1/ralph_f1_01")->dibujar(l, 150, 92 - 64);
        else if (s < 5.0) b.obtener(F + "Ralph/fila_5/ralph_f5_06")->dibujar(l, 150, 92 - 64 - 12);
        else {
            double k = s - 5.0;
            b.obtener(F + "Ralph/fila_5/ralph_f5_06")->dibujar(l, 150 + (int)(k * 20), 92 - 76 - (int)(k * 60));
        }
    }
    void escenaRalphAire(Lienzo& l) {
        const Sprite* r = BancoSprites::instancia().obtener("02_Vanellope_y_personajes/Personajes_escala/ralph");
        cielo(l, tEpilogo, Color(6, 4, 24), Color(40, 16, 50));
        int dy = (int)(std::sin(tEpilogo * 1.5) * 4);
        r->dibujar(l, 120 - r->getAncho() / 2, 70 - r->getAlto() / 2 + dy);
        for (int i = 0; i < 18; i++) l.sombrear(120 + (int)(std::cos(i * 0.35 + tEpilogo) * 40), 70 + (int)(std::sin(i * 0.35 + tEpilogo) * 34), Glifo::CLARO, Color(255, 120, 200));
    }
    void escenaSugarRush(Lienzo& l, bool carrera) {
        auto& b = BancoSprites::instancia();
        cielo(l, tEpilogo, Color(6, 4, 24), Color(30, 10, 40));
        if (!carrera) {
            const Sprite* p = b.obtener("12_Escenarios_y_entornos_IA/Escenario2_Estacion_GCS/portal_sugar_rush_con_vanellope");
            p->dibujar(l, 120 - p->getAncho() / 2, 124 - p->getAlto());
            return;
        }
        const Sprite* fondo = b.obtener("03_Sugar_Rush/Escenario/fondo_pista_completo");
        int x0 = 20, y0 = 8, w = 200, h = 96;
        l.rectangulo(x0 - 4, y0 - 4, w + 8, h + 8, Color(255, 90, 180));
        fondo->dibujarParte(l, x0, y0, (int)std::fmod(tEpilogo * 30, (double)(fondo->getAncho() - w)), 0, w, std::min(h, fondo->getAlto()));
        const Sprite* v = b.obtener("03_Sugar_Rush/Corredores/Vanellope/kart_lado/vanellope_kart_lado_0" + std::to_string(1 + ((int)(tEpilogo * 8)) % 4));
        const Sprite* t2 = b.obtener("03_Sugar_Rush/Corredores/Taffyta/kart_lado/taffyta_kart_lado_0" + std::to_string(1 + ((int)(tEpilogo * 8)) % 4));
        t2->dibujar(l, 60 + (int)(std::sin(tEpilogo) * 6), 90 - t2->getAlto());
        v->dibujar(l, 120 + (int)(std::fmod(tEpilogo * 8, 40.0)), 98 - v->getAlto());
        if (((int)(tEpilogo * 6)) % 3 == 0) v->dibujarSombreado(l, 117 + (int)(std::fmod(tEpilogo * 8, 40.0)), 98 - v->getAlto(), Glifo::MEDIO, Color(80, 240, 255));
    }
    void escenaTrofeo(Lienzo& l) {
        auto& b = BancoSprites::instancia();
        for (int y = l.yMin(); y < l.yMax() + 30; y++) for (int x = l.xMin(); x < l.xMax(); x++)
            l.pixel(x, y, (((x / 10) + (y / 10)) % 2) ? Color(255, 120, 200) : Color(250, 200, 230));
        const Sprite* v = b.obtener("02_Vanellope_y_personajes/Vanellope/fila_1/vanellope_f1_01");
        v->dibujar(l, 112 - v->getAncho() / 2, 108 - v->getAlto());
        int cx = 130, cy = 64;
        for (int j = 0; j < 14; j++) for (int i = -7 + j / 3; i <= 7 - j / 3; i++) l.pixel(cx + i, cy + j, i < 0 ? Color(255, 215, 80) : Color(200, 140, 30));
        l.rectangulo(cx - 2, cy + 14, 4, 6, Color(200, 140, 30));
        l.rectangulo(cx - 6, cy + 20, 12, 3, Color(255, 215, 80));
        if (((int)(tEpilogo * 4)) % 2) l.sombrear(cx - 4, cy - 3, Glifo::BLOQUE, Colores::BLANCO);
    }
    void dibujarEpilogo(Lienzo& l) {
        double t = tEpilogo;
        l.sinCamara();
        if (t < 4.5) {
            cielo(l, t, Color(10, 6, 24), Color(30, 12, 40));
            const Sprite* p = BancoSprites::instancia().obtener("12_Escenarios_y_entornos_IA/Escenario2_Estacion_GCS/portal_fix-it_felix_con_ralph_y_felix");
            p->dibujar(l, 120 - p->getAncho() / 2, 124 - p->getAlto());
        }
        else if (t < 9) escenaArcadeJugando(l, false);
        else if (t < 13) { l.camara(0, 22); escenaBadAnon(l); }
        else if (t < 23) { l.camara(0, 16); escenaLodoPastel(l, t - 13); }
        else if (t < 31) escenaArcadeJugando(l, true);
        else if (t < 37) { l.camara(0, 8); escenaArcadePopular(l); }
        else if (t < 45) escenaArcadeJugando(l, false);
        else if (t < 56) { l.camara(0, 16); escenaCasa(l, t - 45); }
        else if (t < 74) { l.camara(0, 16); escenaBoda(l, t - 56); }
        else if (t < 83) { l.camara(0, 16); escenaTecho(l, t - 74); }
        else if (t < 89) escenaSugarRush(l, false);
        else if (t < 93) escenaRalphAire(l);
        else if (t < 103) { l.camara(0, 8); escenaSugarRush(l, true); }
        else if (t < 108) escenaRalphAire(l);
        else if (t < 113) { l.camara(0, 12); escenaTrofeo(l); }
        else escenaRalphAire(l);
        l.sinCamara();
        int n;
        const Sub* s = subtitulos(n);
        for (int i = 0; i < n; i++)
            if (tEpilogo >= s[i].t0 && tEpilogo < s[i].t1) {
                auto lineas = FuenteMini::partir(s[i].texto, 56);
                int h = (int)lineas.size() * 8 + 12, y0 = l.yMax() - h;
                l.panelTranslucido(l.xMin(), y0, l.xMax() - l.xMin(), h, Colores::NEGRO, Glifo::OSCURO);
                FuenteMini::texto(l, 4, y0 + 1, L"RALPH (NARRADOR):", Color(255, 120, 90));
                for (size_t k = 0; k < lineas.size(); k++) FuenteMini::centrado(l, y0 + 9 + (int)k * 8, lineas[k], Colores::BLANCO);
            }
        if (tEpilogo > 3 && ((int)(tEpilogo * 2)) % 2) FuenteMini::texto(l, l.xMax() - 64, l.yMin() + 1, L"ENTER: SALTAR", Color(150, 150, 170));
    }
    void dibujarCaja(Lienzo& l) const {
        const Progreso& p = ctx.progreso;
        int x0 = 20, y0 = 2, w = 200, h = 117;
        l.cubrirTodo(Glifo::MEDIO, Colores::NEGRO);
        Interfaz::panel(l, x0, y0, w, h, true);
        FuenteMini::centrado(l, y0 + 4, L"[SISTEMA]", Color(120, 255, 140));
        FuenteMini::centrado(l, y0 + 12, L"\x00A1LOGRASTE DESBLOQUEAR EL FINAL!", Colores::BLANCO);
        ctx.fuentes.amarilla.dibujarCentrado(l, y0 + 21, cualFinal == 2 ? L"ESTO NO ES LO QUE..." : nombreFinal(cualFinal));
        bool cuales[Progreso::FINALES_TOTALES];
        int n = Progreso::finalesDescubiertos(cuales);
        FuenteMini::centrado(l, y0 + 33, L"FINALES DESBLOQUEADOS: " + std::to_wstring(n) + L"/" + std::to_wstring(Progreso::FINALES_TOTALES), Color(255, 150, 210));
        int sx = x0 + 10, sy = y0 + 44;
        auto fila = [&](const std::wstring& a, const std::wstring& b, Color c) { FuenteMini::texto(l, sx, sy, a, Color(200, 200, 215)); FuenteMini::texto(l, sx + 104, sy, b, c); sy += 7; };
        fila(L"REPUTACI\x00D3N:", p.etiquetaReputacion() + L" (" + std::to_wstring(p.getReputacion()) + L")", Colores::AMARILLO);
        fila(L"MEDIDOR DE EMPAT\x00CD" L"A:", p.etiquetaEmpatia() + L" (" + std::to_wstring(p.getEmpatia()) + L")", Color(255, 150, 190));
        fila(L"NIVELES COMPLETADOS:", std::to_wstring(p.getNivelesCompletados()) + L"/3", Colores::AMARILLO);
        fila(L"DECISIONES POSITIVAS:", std::to_wstring(p.getPositivas()), Color(120, 255, 140));
        fila(L"DECISIONES NEGATIVAS:", std::to_wstring(p.getNegativas()), Color(255, 120, 120));
        fila(L"ESCUADR\x00D3N SALVADO:", p.escuadronSalvado() ? L"S\x00CD" : L"NO", Colores::BLANCO);
        fila(L"ALIADOS DISPONIBLES:", std::to_wstring(p.aliadosDisponibles()), Colores::BLANCO);
        wchar_t num[16];
        swprintf_s(num, L"%07d", p.getPuntuacion());
        fila(L"PUNTUACI\x00D3N:", num, Colores::AMARILLO);
        if (tCaja > 1.0 && ((int)(tCaja * 2)) % 2 == 0) FuenteMini::centrado(l, y0 + h - 9, L"PRESIONA X O ENTER PARA CONTINUAR", Colores::BLANCO);
    }
protected:
    void actualizarEscenario(double dt) override {
        if (caja) tCaja += dt;
        if (enEpilogo) {
            actualizarEpilogo(dt);
            if (ctx.teclado.presionada(VK_F9)) tEpilogo += 10;
        }
        if (alejar >= 0) alejar += dt;
    }
    void dibujarEscenario(Lienzo& l) override {
        auto& b = BancoSprites::instancia();
        const std::string E = "02_Vanellope_y_personajes/Personajes_escala/";
        switch (fondo) {
        case 0: SugarCine::mundo(l, 92, tiempoEscena); break;
        case 1:
            SugarCine::mundo(l, 92, tiempoEscena, true);
            for (int y = l.yMin(); y < l.yMax() + 30; y++) for (int x = l.xMin(); x < l.xMax(); x++)
                if (((x * 13 + y * 7) % 29) == 0) l.sombrear(x, y, Glifo::MEDIO, Color(20, 8, 24));
            break;
        case 2: l.limpiar(Colores::NEGRO); break;
        case 3: {
            l.limpiar(Color(10, 0, 0));
            for (int i = 0; i < 12; i++) if (((int)(tiempoEscena * 3) + i) % 3) FuenteMini::texto(l, (i * 47) % 200, (i * 23) % 110, L"[ERROR]", Color(255, 40, 40));
            const wchar_t* juegos[3] = { L"FIX-IT FELIX JR.", L"HERO'S DUTY", L"SUGAR RUSH" };
            for (int i = 0; i < 3; i++) {
                if (tiempoEscena < 1.5 + i) continue;
                FuenteMini::texto(l, 50, 40 + i * 12, juegos[i], Colores::BLANCO);
                FuenteMini::texto(l, 150, 40 + i * 12, L"OFFLINE", Color(255, 60, 60));
            }
            break;
        }
        case 4: dibujarEpilogo(l); break;
        default: l.limpiar(Colores::NEGRO); break;
        }
        if (fondo <= 2) {
            int pies = 92;
            if (felixVisible) { const Sprite* s = b.obtener(E + "felix"); s->dibujar(l, 70 - s->getAncho() / 2, pies - s->getAlto()); }
            if (ralphVisible) {
                const Sprite* s = b.obtener(E + "ralph");
                if (fondo == 2) {
                    double k = alejar >= 0 ? std::min(1.0, alejar / 6.0) : 0;
                    int f = k < 0.25 ? 1 : k < 0.5 ? 2 : k < 0.75 ? 3 : 4;
                    if (f == 1) s->dibujar(l, 120 - s->getAncho() / 2, 80 - s->getAlto());
                    else s->dibujarReducido(l, 120 - s->getAncho() / (2 * f), 80 - s->getAlto() / f, f);
                    if (k > 0.5) Efectos::fundido(l, (float)((k - 0.5) * 2));
                }
                else s->dibujar(l, 120 - s->getAncho() / 2, pies - s->getAlto());
            }
            if (vaneVisible) { const Sprite* s = b.obtener("02_Vanellope_y_personajes/Vanellope/fila_1/vanellope_f1_01"); s->dibujar(l, 165 - s->getAncho() / 2, pies - s->getAlto()); }
            if (medallaHeroe) {
                int mx = 145, my = 50;
                for (int j = 0; j < 6; j++) { l.pixel(mx - 3 + j / 2, my - 7 + j, Color(80, 200, 255)); l.pixel(mx + 3 - j / 2, my - 7 + j, Color(80, 200, 255)); }
                for (int j = -6; j <= 6; j++) for (int i = -6; i <= 6; i++) if (i * i + j * j <= 36) l.pixel(mx + i, my + 4 + j, (i + j) < 0 ? Color(255, 205, 60) : Color(190, 130, 20));
                l.panelTranslucido(mx - 34, my + 12, 68, 9, Colores::NEGRO, Glifo::OSCURO);
                FuenteMini::centrado(l, my + 13, L"\x00A1" L"ERES MI H\x00C9ROE!", Colores::AMARILLO);
            }
            if (notaRoca) {
                l.rectangulo(196, 80, 30, 12, Color(110, 100, 110));
                l.rectangulo(200, 76, 22, 6, Color(130, 120, 130));
                l.rectangulo(204, 82, 14, 8, Color(250, 250, 230));
                FuenteMini::texto(l, 150, 60, L"\"EL QUE SACRIFIC\x00D3 TODO PARA", Color(250, 250, 230));
                FuenteMini::texto(l, 150, 67, L"SALVAR A TODOS...\"", Color(250, 250, 230));
            }
        }
        if (caja) dibujarCaja(l);
    }
    void alTerminar() override { ctx.irA(destino); }
    static void guardarRespuesta(const std::string& linea) {
        std::error_code ec;
        std::filesystem::create_directories(std::filesystem::path(Rutas::en(L"Guardado")), ec);
        std::ofstream f{ std::filesystem::path(Rutas::en(L"Guardado\\respuestas.txt")), std::ios::app };
        if (f) f << linea << "\n";
    }
public:
    explicit EscenaFinales(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_FINAL), cualFinal(1), fondo(0), caja(false), medallaHeroe(false), notaRoca(false),
        ralphVisible(true), felixVisible(false), vaneVisible(false), preguntaHecha(false), tCaja(0), alejar(-1), destino(IdEscena::MENU_PRINCIPAL),
        felixAuto(EdificioFixIt::centroVentana(0), 0), ralphAuto(EdificioFixIt::centroVentana(2), EdificioFixIt::piesRalph(0)),
        tEpilogo(0), enEpilogo(false), pisoObjetivo(-1), colObjetivo(0) {
        using namespace Personajes;
        using namespace SugarCine;
        int emp = c.progreso.getEmpatia();
        cualFinal = emp >= 2 ? 1 : (emp >= -1 ? 2 : 3);
        Retrato rR = ralphArcade(), rF = felix(), rV = vanellope();
        Color cs(120, 255, 140);
        auto mostrarCaja = [this] {
            hacer([this] { Progreso::descubrirFinal(cualFinal); caja = true; tCaja = 0; ctx.audio.efecto(L"etapa.wav"); });
            esperarQue([this] { return tCaja > 1.0 && ctx.teclado.aceptar(); });
            hacer([this] { caja = false; });
        };
        auto lineaNucleo = [this] {
            if (ctx.progreso.decision("nucleo") == 2)
                narrar(L"Turbo no fue derrotado solo por la fuerza de Ralph: fue derrotado por los amigos que eligieron estar a su lado.");
            else
                narrar(L"Ralph enfrent\x00F3 el n\x00FA" L"cleo solo, sin pedir ayuda a nadie... y eso tambi\x00E9n dice qui\x00E9n es.");
        };
        esperar(1.0);
        if (cualFinal == 1) {
            fondo = 0; felixVisible = vaneVisible = true;
            narrar(L"La corrupci\x00F3n desaparece. Todos los mundos vuelven a funcionar.");
            lineaNucleo();
            decir("ralph", L"RALPH", L"\x00BF" L"Entonces este es el final? Supongo que nunca necesit\x00E9 una medalla para demostrar qui\x00E9n era...", rR, C_RALPH);
            decir("felix", L"F\x00C9LIX", L"Nunca la necesitaste. Siempre fuiste parte de nosotros.", rF, C_FELIX);
            decir("ralph", L"RALPH", L"(sonr\x00ED" L"e) Supongo que tienes raz\x00F3n. Lo importante es que logramos salvar el arcade.", rR, C_RALPH);
            decir("vane", L"VANELLOPE", L"Se\x00F1or apestoso, tengo algo para ti...", rV, C_VANE);
            decir("ralph", L"RALPH", L"Princesa olorosa, ser\x00E1 un placer tener cualquier cosa de su majestad.", rR, C_RALPH);
            hacer([this] { medallaHeroe = true; ctx.audio.efecto(L"destello.wav"); });
            decir("vane", L"VANELLOPE", L"S\x00E9 que dijiste que no necesitas una medalla, pero quiero que sepas que eres mi h\x00E9roe. Gracias, Ralph.", rV, C_VANE);
            decir("ralph", L"RALPH", L"Vanellope... no s\x00E9 qu\x00E9 decir. Yo cambi\x00E9 gracias a ti...", rR, C_RALPH);
            decir("", L"TODOS", L"\x00A1Gracias, Ralph! \x00A1Has salvado el arcade!", Retrato(), Color(220, 220, 220));
            decir("felix", L"F\x00C9LIX", L"Bueno, Ralph, es hora de irnos... \x00BFJuntos?", rF, C_FELIX);
            decir("ralph", L"RALPH", L"Juntos, compa\x00F1" L"ero.", rR, C_RALPH);
            mostrarCaja();
            hacer([this] { fondo = 4; enEpilogo = true; tEpilogo = 0; edificio.romper(55); ctx.audio.detenerMusica(); ctx.audio.reproducirVoz(L"ralph_epilogo.wav"); ralphAuto.empezar(); });
            esperarQue([this] { return tEpilogo > 121 || (tEpilogo > 2 && ctx.teclado.aceptar()); });
            hacer([this] { ctx.audio.detenerVoz(); enEpilogo = false; fondo = 5; ctx.audio.reproducirMusica(L"final_bueno.wav"); });
        }
        else if (cualFinal == 2) {
            fondo = 1; felixVisible = vaneVisible = true;
            narrar(L"Turbo es derrotado. Pero el da\x00F1o permanece.");
            lineaNucleo();
            decir("ralph", L"RALPH", L"Ganamos... \x00BFverdad?", rR, C_RALPH);
            decir("felix", L"F\x00C9LIX", L"Ralph, tendr\x00E1s que responsabilizarte por esto...", rF, C_FELIX);
            decir("ralph", L"RALPH", L"Soy consciente. Por mi ego\x00EDsmo pas\x00F3 todo esto... me convert\x00ED en lo que no quer\x00ED" L"a: en Turbo.", rR, C_RALPH);
            narrar(L"Ralph decide arreglarlo todo sacrific\x00E1ndose: se lanza al n\x00FA" L"cleo destrozado para sobreexplotarlo... y desaparece.");
            hacer([this] { ralphVisible = false; ctx.audio.efecto(L"golpe_ralph.wav"); });
            decir("vane", L"VANELLOPE", L"Ralph...", rV, C_VANE);
            decir("felix", L"F\x00C9LIX", L"Bueno, chicos, tendr\x00E9 que buscar alojamiento para los de nuestro juego. Ralph no se habr\x00E1 sacrificado por nada: llevaremos el arcade a lo que \x00E9l quer\x00ED" L"a.", rF, C_FELIX);
            hacer([this] { notaRoca = true; });
            narrar(L"Vanellope deja una nota al costado de una roca: \"El que sacrific\x00F3 todo para salvar a todos, pero nadie lo sabe\".");
            mostrarCaja();
            hacer([this] { fondo = 5; felixVisible = vaneVisible = false; notaRoca = false; });
            narrar(L"30 a\x00F1os despu\x00E9s, los Nicelanders encontraron alojamiento en otros juegos. Siempre odiaron a Ralph: pensaron que los abandon\x00F3 y que por su culpa el juego qued\x00F3 en stand by.");
            narrar(L"F\x00E9lix siempre recordar\x00E1 a Ralph: no como h\x00E9roe, no como villano, ni como un amigo... sino como alguien que buscaba encontrarse a s\x00ED mismo.");
            narrar(L"Vanellope visita la roca cada a\x00F1o para recordar lo que hizo por ella su gran amigo...");
            narrar(L"En fin, 30 a\x00F1os pasaron. El arcade sigue funcionando, pero algo ha cambiado: se siente la falta de alguien.");
            narrar(L"Para algunos, una persona vil y mala; para otros, el h\x00E9roe de su vida; pero para todos es, y ser\x00E1, una persona como todas.");
        }
        else {
            fondo = 3; ralphVisible = false;
            narrar(L"Turbo cae... pero el n\x00FA" L"cleo explota. [SISTEMA] ADVERTENCIA. CORRUPCI\x00D3N CR\x00CDTICA. CORRUPCI\x00D3N: 100%");
            lineaNucleo();
            decir("ralph", L"RALPH", L"No... No puede terminar as\x00ED...", rR, C_RALPH);
            narrar(L"[ERROR] [ERROR] [ERROR] [ERROR] Los mundos empiezan a desaparecer.");
            esperar(3.0);
            mostrarCaja();
            hacer([this] { fondo = 2; ralphVisible = true; });
            narrar(L"Ralph abre los ojos, pero est\x00E1 en un lugar oscuro. No hay nadie alrededor. \x00BFPor qu\x00E9 empez\x00F3 todo esto? \x00BFTan dif\x00ED" L"cil era quedarse en su rutina diaria...?");
            narrar(L"Solo queda Ralph.");
            decir("ralph", L"RALPH", L"\x00BF" L"Esto es lo que pasa cuando todos quieren ganar?", rR, C_RALPH);
            narrar(L"Silencio. El sistema muestra: ARCADE OFFLINE.");
            decir("ralph", L"RALPH", L"Al fin y al cabo... siempre ser\x00E9 el villano, eh... ni\x00F1" L"a...", rR, C_RALPH);
            hacer([this] { alejar = 0; });
            esperar(6.5);
            hacer([this] { fondo = 5; ralphVisible = false; });
        }
        esperar(1.0);
        narrar(L"LA MEDALLA, VISTA DESDE TRES MIRADAS:");
        narrar(L"F\x00C9LIX: para F\x00E9lix, la medalla era su premio de cada d\x00ED" L"a... y Ralph, el que se la quer\x00ED" L"a quitar.");
        narrar(L"CALHOUN: para Calhoun, la medalla fue el cebo que despert\x00F3 a los Cy-Bugs... y Ralph, el intruso que lo arruin\x00F3 todo.");
        narrar(L"VANELLOPE: para Vanellope, la medalla fue su boleto para correr... y Ralph, el primero que le dio una oportunidad.");
        narrar(L"PREGUNTA REFLEXIVA FINAL: viviste esta historia desde los ojos de F\x00E9lix, Calhoun y Vanellope... y desde el propio Ralph. \x00BFQui\x00E9n es Ralph para ti?");
        elegir(L"\x00BF" L"CON QUI\x00C9N TE IDENTIFICASTE M\x00C1S?", {
            L"F\x00C9LIX: el que siempre arregla lo que otros rompen",
            L"CALHOUN: la que carga con la culpa de sus decisiones",
            L"VANELLOPE: la que sue\x00F1" L"a aunque el mundo la vea como un error",
            L"RALPH: el que solo quer\x00ED" L"a ser aceptado" },
            [this](int r) {
                const wchar_t* reflexion[4] = {
                    L"F\x00E9lix ve a Ralph como el que destruye su trabajo... hasta que entiende que reparar tambi\x00E9n es aceptar a los dem\x00E1s.",
                    L"Calhoun aprendi\x00F3 que una sola decisi\x00F3n cambia c\x00F3mo nos ven los dem\x00E1s. Para ella, Ralph fue un error... o una segunda oportunidad.",
                    L"Vanellope sabe lo que es ser un error ante los ojos del sistema. Para ella, Ralph fue el primero en verla como una corredora.",
                    L"Ralph descubri\x00F3 que nadie es solo un villano o solo un h\x00E9roe: depende de qui\x00E9n lo mire... y de lo que decidimos hacer." };
                narrar(reflexion[r]);
                const char* quien[4] = { "Felix", "Calhoun", "Vanellope", "Ralph" };
                guardarRespuesta(std::string("Final ") + std::to_string(cualFinal) + " -> se identifico con " + quien[r]);
            });
        elegir(L"\x00BF" L"CAMBI\x00D3 TU LECTURA DE RALPH AL VERLO DESDE OTROS OJOS?", {
            L"S\x00CD: parec\x00ED" L"a solo un villano, pero entend\x00ED por qu\x00E9 actuaba as\x00ED",
            L"EN PARTE: hizo cosas buenas y malas; depende de qui\x00E9n cuente la historia",
            L"NO: siempre lo vi igual, sin importar desde qu\x00E9 ojos lo mirara" },
            [this](int r) {
                const wchar_t* porque[3] = {
                    L"Conocer las razones de alguien cambia la forma en que lo juzgamos.",
                    L"Ninguna mirada sola cuenta toda la verdad: cada personaje vio una parte de Ralph.",
                    L"Aun as\x00ED, F\x00E9lix, Calhoun y Vanellope lo vieron de formas muy distintas." };
                narrar(porque[r]);
                narrar(L"\x00BFQui\x00E9n es Ralph? Depende de los ojos con los que lo mires. Gracias por jugar WHO'S RALPH?");
                const char* lectura[3] = { "si cambio", "en parte", "no cambio" };
                guardarRespuesta(std::string("   lectura de Ralph: ") + lectura[r]);
            });
        hacer([this] { fondo = 5; });
        elegir(L"\x00BF" L"DESEAS COMENZAR UNA NUEVA PARTIDA?", { L"S\x00CD", L"NO" },
            [this](int r) {
                if (r == 0) { ctx.progreso.reiniciar(); ctx.progreso.guardar(); destino = IdEscena::PROLOGO; }
                else destino = IdEscena::MENU_PRINCIPAL;
            });
    }
    void entrarHistoria() override {
        int emp = ctx.progreso.getEmpatia();
        int f = emp >= 2 ? 1 : (emp >= -1 ? 2 : 3);
        ctx.audio.reproducirMusica(f == 1 ? L"final_bueno.wav" : f == 2 ? L"final_neutral.wav" : L"final_malo.wav");
    }
};
