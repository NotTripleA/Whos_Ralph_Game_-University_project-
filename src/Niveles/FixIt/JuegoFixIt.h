#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <fstream>
#include <filesystem>
#include "../../Escenas/Escena.h"
#include "../../Motor/Efectos.h"
#include "../../Motor/FuenteMini.h"
#include "../../Historia/InterfazHistoria.h"
#include "EdificioFixIt.h"
#include "PersonajesFixIt.h"
#include "GloboTexto.h"
class JuegoFixIt {
public:
    enum Modo { HISTORIA, INFINITO };
    enum Fase { INTRO, PREPARANDO, JUGANDO, SUBIENDO, FINAL, GANO, PERDIO };
private:
    Contexto& ctx;
    Modo modo;
    Fase fase;
    double tFase;
    EdificioFixIt edificio;
    FelixJugador felix;
    RalphFixIt ralph;
    std::vector<std::unique_ptr<Entidad>> objetos;
    GloboTexto globo;
    int etapa;
    int nivel;
    int vidas;
    int puntos;
    int record;
    double camY;
    double camObjetivo;
    double felixDesde, felixHasta;
    double enfriaDano;
    double temblor;
    double proximoPastel;
    double proximoPato;
    double ayuda;
    double esperaFinal;
    enum MusicaFase { M_NINGUNA, M_INTRO, M_JUEGO, M_VICTORIA } musica;
    double tIntro;
    bool introRompio;
    double tFinal;
    static const int VIDAS_MAX = 3;
    void musicaIntro() {
        musica = M_INTRO;
        if (Audio::existeMusica(L"fix-it_intro.wav")) ctx.audio.reproducirMusica(L"fix-it_intro.wav", false);
        else ctx.audio.reproducirSegmento(L"fix-it_fondo.wav", 0, 30000, false);
    }
    void musicaJuego() {
        if (musica == M_JUEGO) return;
        musica = M_JUEGO;
        if (Audio::existeMusica(L"fix-it_loop.wav")) ctx.audio.reproducirMusica(L"fix-it_loop.wav", true);
        else ctx.audio.reproducirSegmento(L"fix-it_fondo.wav", 30000, 39000, true);
    }
    void musicaVictoria() {
        musica = M_VICTORIA;
        if (Audio::existeMusica(L"fix-it_victoria.wav")) ctx.audio.reproducirMusica(L"fix-it_victoria.wav", false);
        else ctx.audio.reproducirSegmento(L"fix-it_fondo.wav", 39000, -1, false);
    }
    static std::wstring rutaRecord() { return Rutas::en(L"Guardado\\record_fixit.txt"); }
    int leerRecord() const {
        std::ifstream f{ std::filesystem::path(rutaRecord()) };
        int r = 15000;
        if (f) f >> r;
        return r;
    }
    void guardarRecord() const {
        std::error_code ec;
        std::filesystem::create_directories(std::filesystem::path(Rutas::en(L"Guardado")), ec);
        std::ofstream f{ std::filesystem::path(rutaRecord()) };
        if (f) f << record;
    }
    void sumar(int p) {
        puntos += p;
        if (puntos > record) record = puntos;
    }
    void irA(Fase f) { fase = f; tFase = 0; }
    static void mirar(Lienzo& l, double arriba, int sx = 0) {
        int alto = l.yMax() - l.yMin();
        l.camara(sx, (int)std::floor(arriba) - (alto - EdificioFixIt::FILAS_VISTA) / 2 - l.yMin());
    }
    static double vistaSuelo() { return EdificioFixIt::suelo() - 127; }
    void prepararEtapa(int e) {
        etapa = e;
        camObjetivo = camY = EdificioFixIt::vistaEtapa(e);
        ralph.ubicar(EdificioFixIt::centroVentana(2), EdificioFixIt::piesRalph(e));
        ralph.enojado();
        felix.ubicar(e == 0 ? EdificioFixIt::centroVentana(0) : felix.getX(), pisoMin());
        if (modo == HISTORIA) ralph.setDificultad(20 + 3 * e, 2.6 - 0.2 * e);
        else ralph.setDificultad(30 + 5 * e + 6 * nivel, std::max(0.9, 1.8 - 0.1 * e - 0.12 * nivel));
        enfriaDano = modo == HISTORIA ? 6.0 : 1.5;
        objetos.clear();
        proximoPastel = Azar::real(9, 14);
        proximoPato = Azar::real(6, 10);
        irA(PREPARANDO);
        int gx = (int)ralph.getX() + (ralph.getX() < 120 ? 56 : -56);
        globo.mostrar(L"\x00A1VOY A DEMOLERLO!", gx, (int)camObjetivo + 16, 2.2);
        ctx.audio.reproducirVoz(L"ralph_demolerlo.wav");
    }
    int pisoMin() const { return EdificioFixIt::pisoInferior(etapa); }
    int pisoMax() const { return EdificioFixIt::pisoSuperior(etapa); }
    double fondoVista() const { return EdificioFixIt::piesFelix(pisoMin()) + 3; }
    int rotasEnEtapa() const { return edificio.ventanasRotas(pisoMin()) + edificio.ventanasRotas(pisoMax()); }
    void actualizarJuego(double dt) {
        const Teclado& t = ctx.teclado;
        int dir = (t.mantenida('A') || t.mantenida(VK_LEFT)) ? -1 : (t.mantenida('D') || t.mantenida(VK_RIGHT)) ? 1 : 0;
        bool subir = t.presionada('W') || t.presionada(VK_UP);
        bool bajar = t.presionada('S') || t.presionada(VK_DOWN);
        bool martillo = t.aceptar();
        if (t.presionada(VK_F9))
            for (int p = pisoMin(); p <= pisoMax(); p++)
                for (int c = 0; c < EdificioFixIt::COLUMNAS; c++) while (edificio.reparar(p, c) == 1) {}
        FelixJugador::Estado antes = felix.getEstado();
        felix.controlar(dir, subir, bajar, martillo, pisoMin(), pisoMax(), dt);
        if (felix.getEstado() == FelixJugador::SALTANDO && antes != FelixJugador::SALTANDO) ctx.audio.efecto(L"salto.wav");
        felix.actualizar(dt);
        ralph.actualizar(dt);
        if (felix.golpeDeMartillo()) {
            int p = felix.getPiso();
            int col = -1;
            for (int c = 0; c < EdificioFixIt::COLUMNAS; c++)
                if (std::fabs(EdificioFixIt::centroVentana(c) - felix.getX()) <= 10) col = c;
            int r = col >= 0 ? edificio.reparar(p, col) : 0;
            if (r == 2) {
                sumar(100);
                ctx.audio.efecto(L"ventana_reparada.wav");
                objetos.push_back(std::make_unique<PuntajeFlotante>(EdificioFixIt::centroVentana(col), EdificioFixIt::topePiso(p) + 22, 100));
            }
            else ctx.audio.efecto(L"martillo.wav");
        }
        if (enfriaDano > 0) enfriaDano -= dt;
        while (ralph.consumirGolpe()) {
            temblor = 0.15;
            double probLadrillo = modo == HISTORIA ? 0.4 : 0.85;
            if (Azar::probabilidad(probLadrillo)) {
                double bx = ralph.getX() + Azar::real(-14, 14);
                objetos.push_back(std::make_unique<LadrilloFixIt>(bx, EdificioFixIt::piesRalph(etapa) - 40, fondoVista()));
            }
            bool puedeRomper = enfriaDano <= 0 && (modo == INFINITO || rotasEnEtapa() < 5);
            double probRomper = modo == HISTORIA ? 0.3 : std::min(0.9, 0.55 + 0.08 * nivel);
            if (puedeRomper && Azar::probabilidad(probRomper)) {
                enfriaDano = modo == HISTORIA ? Azar::real(6, 9) : std::max(0.6, 1.6 - 0.15 * nivel);
                int c = ralph.columna() + Azar::entero(-1, 1);
                c = c < 0 ? 0 : (c > 4 ? 4 : c);
                int p = Azar::entero(pisoMin(), pisoMax());
                edificio.danar(p, c);
                for (int i = 0; i < 2; i++)
                    objetos.push_back(std::make_unique<VidrioCayendo>(EdificioFixIt::centroVentana(c), EdificioFixIt::topePiso(p) + 20));
            }
            ctx.audio.efecto(L"golpe_ralph.wav");
        }
        proximoPastel -= dt;
        if (proximoPastel <= 0) {
            proximoPastel = Azar::real(12, 18);
            int p = Azar::entero(pisoMin(), pisoMax()), c = Azar::entero(0, 4);
            if (edificio.hayVentana(p, c)) objetos.push_back(std::make_unique<PastelFixIt>(p, c));
        }
        if (etapa > 0 || nivel > 0) {
            proximoPato -= dt;
            if (proximoPato <= 0) {
                proximoPato = Azar::real(7, 12) - nivel;
                if (proximoPato < 3) proximoPato = 3;
                int p = Azar::entero(pisoMin(), pisoMax()), dir2 = Azar::probabilidad(0.5) ? 1 : -1;
                objetos.push_back(std::make_unique<PatoFixIt>(dir2 > 0 ? -16 : 256, EdificioFixIt::piesFelix(p) - 14, dir2, 240));
            }
        }
        for (auto& o : objetos) o->actualizar(dt);
        colisiones();
        for (size_t i = 0; i < objetos.size();) {
            if (!objetos[i]->estaActiva()) objetos.erase(objetos.begin() + i);
            else i++;
        }
        if (vidas <= 0 && !felix.estaTirado()) { irA(PERDIO); ctx.audio.detenerMusica(); ctx.audio.efecto(L"gameover.wav"); return; }
        if (edificio.etapaReparada(pisoMin()) && edificio.etapaReparada(pisoMax())) {
            sumar(500);
            ctx.audio.efecto(L"etapa.wav");
            felix.celebrar();
            ralph.trepar();
            objetos.clear();
            felixDesde = felix.getY();
            felixHasta = EdificioFixIt::piesFelix(EdificioFixIt::pisoInferior(etapa + 1));
            irA(SUBIENDO);
        }
    }
    void colisiones() {
        double fx = felix.getX(), fy = felix.getY();
        for (auto& o : objetos) {
            if (auto* lad = dynamic_cast<LadrilloFixIt*>(o.get())) {
                if (lad->estaRoto()) continue;
                if (std::fabs(lad->getX() - fx) < 9 && lad->getY() > fy - 30 && lad->getY() - 8 < fy) {
                    lad->romper();
                    if (felix.golpear(pisoMin())) {
                        vidas--;
                        ctx.audio.efecto(L"herido.wav");
                    }
                }
            }
            else if (auto* pas = dynamic_cast<PastelFixIt*>(o.get())) {
                if (pas->listo() && pas->getPiso() == felix.getPiso() && std::fabs(pas->getX() - fx) < 8 && !felix.estaTirado()) {
                    felix.comerPastel();
                    sumar(300);
                    objetos.push_back(std::make_unique<PuntajeFlotante>(fx, fy - 30, 300));
                    ctx.audio.efecto(L"pastel.wav");
                    pas->desactivar();
                    break;
                }
            }
            else if (auto* pato = dynamic_cast<PatoFixIt*>(o.get())) {
                if (std::fabs(pato->getX() - fx) < 10 && std::fabs(pato->getY() - (fy - 14)) < 10 && felix.vulnerable() && felix.getEstado() != FelixJugador::SALTANDO) {
                    felix.aturdir();
                    ctx.audio.efecto(L"menu_error.wav");
                }
            }
        }
    }
    void actualizarIntro(double dt) {
        tIntro += dt;
        globo.actualizar(dt);
        auto momento = [&](double t) { return tIntro >= t && tIntro - dt < t; };
        if (momento(11.4)) { globo.mostrar(L"\x00A1HEY! \x00A1MOVIERON MI TRONCO!", 205, 66, 2.6); ctx.audio.reproducirVoz(L"ralph_tronco.wav"); }
        const int calle = (int)vistaSuelo() + 7;
        if (momento(25.2)) { globo.mostrar(L"\x00A1VOY A DEMOLERLO!", 60, calle + 40, 2.4); ctx.audio.reproducirVoz(L"ralph_demolerlo.wav"); }
        if (momento(31.0) && !introRompio) { introRompio = true; edificio.romper(50); temblor = 0.6; ctx.audio.efecto(L"golpe_ralph.wav"); }
        if (momento(33.0)) { globo.mostrar(L"\x00A1REP\x00C1RALO, F\x00C9LIX!", 120, calle + 38, 2.4); ctx.audio.reproducirVoz(L"nicelanders_reparalo.wav"); }
        if (momento(35.8)) { globo.mostrar(L"\x00A1PUEDO REPARARLO!", 120, calle + 66, 2.4); ctx.audio.reproducirVoz(L"felix_puedo_repararlo.wav"); ctx.audio.efecto(L"destello.wav"); }
        if (tIntro > 38.5 || (tIntro > 1.0 && ctx.teclado.aceptar())) {
            if (!introRompio) { introRompio = true; edificio.romper(50); }
            globo.ocultar();
            ctx.audio.detenerVoz();
            prepararEtapa(0);
            camY = vistaSuelo();
        }
    }
    void dibujarIntro(Lienzo& l) {
        auto& b = BancoSprites::instancia();
        const std::string F = SpritesFixIt::F;
        double t = tIntro;
        l.sinCamara();
        l.limpiar(Colores::NEGRO);
        if (t < 3.0) {
            hudArcade(l);
            const Sprite* logo = b.obtener(F + "UI/logo_fix_it_felix_jr");
            logo->dibujar(l, (l.getAncho() - logo->getAncho()) / 2, 12);
            if (((int)(t * 2)) % 2 == 0) FuenteMini::centrado(l, 92, L"PRESS 1P START", Color(255, 60, 60), false);
            FuenteMini::centrado(l, 104, L"\x00A9TOBIKOMI COMPANY 1982", Color(255, 60, 60), false);
            FuenteMini::centrado(l, 112, L"ALL RIGHTS RESERVED", Color(255, 60, 60), false);
            return;
        }
        const Sprite* arboles = b.obtener(F + "Escenario/arboles");
        if (t < 10.5) {
            arboles->dibujar(l, 8, 120 - arboles->getAlto());
            int tx = 120 - (t > 9.2 ? (int)((t - 9.2) * 60) : 0);
            if (t < 5.3) {
                int rx = 262 - (int)((t - 3.0) / 2.3 * 132);
                b.obtener(F + "Ralph/fila_3/ralph_f3_0" + std::to_string(1 + ((int)(t * 10)) % 8))->dibujar(l, rx - 32, 112 - 64, true);
                dibujarTronco(l, tx, 112);
            }
            else if (t < 6.1) {
                b.obtener(F + std::string(t < 5.7 ? "Ralph/fila_4/ralph_f4_02" : "Ralph/fila_4/ralph_f4_03"))->dibujar(l, 130 - 32, 112 - 64);
                dibujarTronco(l, tx, 112);
            }
            else if (t < 8.9) {
                b.obtener(F + "Ralph/fila_4/ralph_f4_01")->dibujar(l, 120 - 32, 112 - 64 + 5);
                dibujarTronco(l, tx, 112);
                for (int k = 0; k < 3; k++)
                    if (((int)(t * 2) + k) % 3 != 0) FuenteMini::caracter(l, 134 + k * 5, 86 - k * 6 - ((int)(t * 3) % 2), L'Z', Colores::BLANCO);
            }
            else {
                if (t < 9.3) { b.obtener(F + "Ralph/fila_4/ralph_f4_02")->dibujar(l, 120 - 32, 112 - 64); FuenteMini::caracter(l, 124, 50, L'!', Colores::AMARILLO); }
                else {
                    int rx = 120 - (int)((t - 9.3) * 110);
                    b.obtener(F + "Ralph/fila_3/ralph_f3_0" + std::to_string(1 + ((int)(t * 12)) % 8))->dibujar(l, rx - 32, 112 - 64, true);
                }
                dibujarTronco(l, tx, 112);
            }
            if (t > 8.4) {
                int bx = 250 - (int)((t - 8.4) * 70);
                const Sprite* bull = b.obtener(F + "Maquinaria/bulldozer/bulldozer_0" + std::to_string(1 + ((int)(t * 8)) % 2));
                bull->dibujar(l, bx, 120 - bull->getAlto());
            }
            hudArcade(l);
            return;
        }
        t -= 4.0;
        if (t < 11.0) {
            for (int y = 116; y < l.yMax(); y++)
                for (int x = l.xMin(); x < l.xMax(); x++) l.pixel(x, y, (x * 3 + y * 5) % 11 ? Color(64, 40, 22) : Color(96, 62, 34));
            const Sprite* dump = b.obtener(F + "Escenario/basurero_dump");
            dump->dibujar(l, -30, 117 - dump->getAlto());
            dibujarTronco(l, 158, 116);
            if (t < 7.4) {
                int rx = 262 - (int)((t - 6.5) / 0.9 * 52);
                b.obtener(F + "Ralph/fila_3/ralph_f3_0" + std::to_string(1 + ((int)(t * 10)) % 8))->dibujar(l, rx - 32, 116 - 64, true);
            }
            else {
                const Sprite* r = b.obtener(F + "Ralph/fila_1/ralph_f1_0" + std::to_string(4 + ((int)(t * 4)) % 2));
                r->dibujar(l, 210 - 32, 116 - 64);
            }
            hudArcade(l);
            globo.dibujar(l);
            return;
        }
        if (t < 24.0) {
            arboles->dibujar(l, 8, 120 - arboles->getAlto());
            double sube = t < 16.0 ? (t - 11.0) / 5.0 : 1.0;
            int desplaz = (int)((1.0 - sube) * 120);
            mirar(l, vistaSuelo() - desplaz);
            edificio.dibujar(l);
            l.sinCamara();
            if (t < 17.5) {
                for (int k = 0; k < 2; k++) {
                    const Sprite* g = b.obtener(F + "Maquinaria/grua/grua_0" + std::to_string(1 + ((int)(t * 3) + k) % 2));
                    g->dibujar(l, k == 0 ? -30 : 174, 120 - g->getAlto(), k == 1);
                }
                for (int i = 0; i < 70; i++) {
                    double a = i * 2.39 + t * 1.7;
                    int px = 120 + (int)(std::cos(a) * (40 + (i % 9) * 8)), py = 100 + (int)(std::sin(a * 1.3) * 14) - (i % 5) * 3;
                    wchar_t g = i % 3 == 0 ? Glifo::OSCURO : i % 3 == 1 ? Glifo::MEDIO : Glifo::CLARO;
                    for (int dx = 0; dx < 6; dx++) for (int dy = 0; dy < 4; dy++) l.sombrear(px + dx, py + dy, g, Color(235, 235, 235));
                }
            }
            if (t > 17.0 && t < 21.0) {
                for (int i = 0; i < 5; i++) {
                    double ti = t - 17.0 - i * 0.5;
                    if (ti < 0) continue;
                    int px = 240 - (int)(ti * 50);
                    if (px < 116) continue;
                    char id[64];
                    sprintf_s(id, "Nicelanders/cara_%02d", 1 + i * 2);
                    b.obtener(F + id)->dibujar(l, px, 112 - 16 - (((int)(ti * 8)) % 2));
                }
            }
            if (t > 19.5) {
                int rx = t < 21.0 ? (int)(-30 + (t - 19.5) * 60) : 60;
                const Sprite* r = (t < 21.0) ? b.obtener(F + "Ralph/fila_3/ralph_f3_0" + std::to_string(1 + ((int)(t * 10)) % 8))
                    : b.obtener(F + "Ralph/fila_1/ralph_f1_0" + std::to_string(4 + ((int)(t * 4)) % 2));
                r->dibujar(l, rx - 32, 113 - 64);
            }
            mirar(l, vistaSuelo());
            globo.dibujar(l);
            l.sinCamara();
            hudArcade(l);
            return;
        }
        const double techo = -67, calle = vistaSuelo();
        double cam;
        if (t < 27.0) cam = calle + (techo - calle) * (t - 24.0) / 3.0;
        else if (t < 28.0) cam = techo;
        else cam = techo + std::min(1.0, (t - 28.0) / 1.2) * (calle - techo);
        int sacudir = temblor > 0 ? ((int)(temblor * 40) % 2 ? 2 : -2) : 0;
        l.degradadoVertical(0, 120, Color(4, 4, 18), Color(14, 10, 30));
        mirar(l, cam, sacudir);
        edificio.dibujar(l);
        dibujarCalle(l);
        if (t < 27.0) {
            double ry = EdificioFixIt::suelo() - (t - 24.0) / 3.0 * EdificioFixIt::suelo();
            b.obtener(F + "Ralph/fila_2/ralph_f2_0" + std::to_string(3 + ((int)(t * 6)) % 2))->dibujar(l, 6, (int)ry - 64);
        }
        else b.obtener(F + "Ralph/fila_2/ralph_f2_0" + std::to_string(8 + ((int)(t * 6)) % 2))->dibujar(l, 88, -64);
        for (auto& o : objetos) o->dibujar(l);
        if (t > 28.6) {
            for (int i = 0; i < 4; i++) {
                int p = i % 2, c = (i * 2 + 1) % 5;
                if (!edificio.hayVentana(p, c)) continue;
                char id[64];
                sprintf_s(id, "Nicelanders/cara_%02d", 2 + i * 3);
                b.obtener(F + id)->dibujar(l, EdificioFixIt::centroVentana(c) - 8, EdificioFixIt::topePiso(p) + 18);
            }
        }
        if (t > 31.5) {
            const Sprite* f = b.obtener(F + "Felix/fila_1/felix_f1_" + std::string(((int)(t * 4)) % 2 ? "09" : "10"));
            f->dibujar(l, EdificioFixIt::centroVentana(2) - 20, EdificioFixIt::suelo() - 16 - 40);
            if (((int)(t * 6)) % 2) b.obtener(F + "Efectos/destello/destello_01")->dibujar(l, EdificioFixIt::centroVentana(2) + 2, EdificioFixIt::suelo() - 16 - 44);
        }
        globo.dibujar(l);
        l.sinCamara();
        hudArcade(l);
    }
    void dibujarCalle(Lienzo& l) const {
        const int suelo = EdificioFixIt::suelo();
        for (int y = suelo; y < suelo + 40; y++)
            for (int x = l.xMin(); x < l.xMax(); x++) l.pixel(x, y, (x * 7 + y * 3) % 13 ? Color(20, 70, 24) : Color(34, 100, 36));
        const Sprite* arboles = BancoSprites::instancia().obtener(SpritesFixIt::F + "Escenario/arboles");
        arboles->dibujarParte(l, 8, suelo - 16, 0, 112, arboles->getAncho(), 16);
    }
    void dibujarTronco(Lienzo& l, int x, int pie) const {
        Color corteza(110, 58, 28), oscuro(70, 34, 16), anillo(196, 140, 80), centro(150, 96, 52);
        l.rectangulo(x - 12, pie - 10, 24, 10, corteza);
        for (int j = 0; j < 10; j += 3) l.rectangulo(x - 12 + (j % 2) * 3, pie - 10 + j, 2, 2, oscuro);
        l.rectangulo(x - 12, pie - 12, 24, 3, anillo);
        l.rectangulo(x - 6, pie - 12, 12, 2, centro);
        l.rectangulo(x - 14, pie - 2, 28, 2, oscuro);
    }
    static constexpr double T_LANZA = 5.4;
    static constexpr double VISTA_TECHO = -93;
    static double yCaida(double k) { return -12 - 50 * k + 90 * k * k; }
    static double xCaida(double k) { return 192 + 10 * std::min(k, 1.0); }
    static double tAterriza() {
        double h = EdificioFixIt::suelo() + 12;
        return T_LANZA + (50 + std::sqrt(2500 + 360 * h)) / 180;
    }
    void actualizarFinal(double dt) {
        tFinal += dt;
        globo.actualizar(dt);
        auto momento = [&](double t) { return tFinal >= t && tFinal - dt < t; };
        const double cae = tAterriza();
        if (momento(3.2)) { ctx.audio.efecto(L"pastel.wav"); }
        if (momento(T_LANZA)) { ctx.audio.reproducirVoz(L"ralph_grito.wav"); }
        if (momento(cae)) { temblor = 0.5; ctx.audio.efecto(L"golpe_ralph.wav"); }
        if (momento(cae + 1.2)) { ctx.audio.reproducirVoz(L"nicelanders_risa.wav"); }
        if (tFinal > cae + 5.5 || (tFinal > cae + 1.5 && ctx.teclado.aceptar())) irA(GANO);
    }
    void dibujarFinal(Lienzo& l) {
        auto& b = BancoSprites::instancia();
        const std::string F = SpritesFixIt::F;
        double t = tFinal;
        const double cae = tAterriza();
        const int suelo = EdificioFixIt::suelo();
        l.sinCamara();
        l.degradadoVertical(0, 120, Color(4, 4, 18), Color(20, 14, 40));
        double arriba = VISTA_TECHO;
        if (t >= cae) arriba = vistaSuelo();
        else if (t > T_LANZA) arriba = std::max(VISTA_TECHO, std::min(vistaSuelo(), yCaida(t - T_LANZA) - 80));
        int sacudir = temblor > 0 ? (((int)(temblor * 40)) % 2 ? 2 : -2) : 0;
        mirar(l, arriba, sacudir);
        edificio.dibujar(l);
        dibujarCalle(l);
        for (int j = 0; j < 6; j++) for (int i = -26 + j; i < 26 - j; i++) l.pixel(202 + i, suelo - 5 + j, Color(92, 60, 30));
        {
            b.obtener(F + "Edificio/puertas/puerta_0" + std::string(t > 1.0 ? "3" : "1"))->dibujar(l, 30, -24);
            const Sprite* f = t > 3.2 ? b.obtener(F + "Felix/fila_1/felix_f1_" + std::to_string(12 + ((int)(t * 4)) % 3))
                : b.obtener(F + "Felix/fila_1/felix_f1_11");
            f->dibujar(l, 80, -40);
            if (t < 3.2) {
                int my = (int)(-150 + (t / 3.2) * 110);
                b.obtener(F + "Objetos/medallas/medalla_oro")->dibujar(l, 84, my);
            }
            else if (((int)(t * 6)) % 2) b.obtener(F + "Efectos/destello/destello_01")->dibujar(l, 92, -46);
            for (int i = 0; i < 5; i++) {
                double ti = t - 1.0 - i * 0.35;
                if (ti < 0) continue;
                int px = 46 + (int)std::min(ti * 40, 70.0 + i * 14);
                if (t > 4.5) px = 150 + i * 9;
                char id[64];
                sprintf_s(id, "Nicelanders/cara_%02d", 1 + i * 2);
                b.obtener(F + id)->dibujar(l, px, -16 - ((int)(t * 6 + i) % 2) - (t > 4.5 && t < 5.4 ? 8 : 0));
            }
            if (t < 4.5) b.obtener(F + "Ralph/fila_1/ralph_f1_01")->dibujar(l, 172, -64);
            else if (t < T_LANZA) b.obtener(F + "Ralph/fila_5/ralph_f5_06")->dibujar(l, 160, -64 - 12);
            else if (t < cae) {
                double k = t - T_LANZA;
                const Sprite* r = b.obtener(F + (((int)(k * 8)) % 2 ? "Ralph/fila_5/ralph_f5_07" : "Ralph/fila_5/ralph_f5_06"));
                r->dibujar(l, (int)xCaida(k) - 32, (int)yCaida(k) - 64);
            }
            else {
                const Sprite* lodo = b.obtener(F + "Ralph/fila_5/ralph_f5_08");
                lodo->dibujar(l, (int)xCaida(9) - lodo->getAncho() / 2, suelo - lodo->getAlto());
                double s = t - cae;
                if (s < 0.9)
                    for (int i = 0; i < 12; i++) {
                        double vx = std::cos(i * 0.52 + 0.26) * 46, vy = 60 + (i % 4) * 14;
                        int px = (int)(xCaida(9) + vx * s), py = (int)(suelo - 4 - (vy * s - 110 * s * s));
                        l.sombrear(px, py, Glifo::BLOQUE, Color(110, 72, 36));
                        l.sombrear(px + 1, py, Glifo::OSCURO, Color(92, 60, 30));
                    }
            }
        }
        l.sinCamara();
        if (t > cae + 0.6 && ((int)(t * 2)) % 2 == 0) ctx.fuentes.blanca.dibujarCentrado(l, 40, L"\x00A1NIVEL 1 COMPLETO!");
        hudArcade(l);
    }
    void hudArcade(Lienzo& l) const {
        int y0 = l.yMin() + 1;
        FuenteMini::texto(l, 40, y0, L"1UP", Color(255, 40, 40));
        wchar_t num[16];
        swprintf_s(num, L"%06d", puntos);
        FuenteMini::texto(l, 34, y0 + 7, num, Colores::BLANCO);
        FuenteMini::texto(l, 98, y0, L"HIGH SCORE", Color(255, 40, 40));
        swprintf_s(num, L"%06d", record);
        FuenteMini::texto(l, 106, y0 + 7, num, Colores::BLANCO);
    }
    void hudJuego(Lienzo& l) const {
        l.sinCamara();
        Color texto(220, 220, 235), resalte = Colores::AMARILLO;
        const int ANCHO = 44;
        const int y0 = l.yMin() + EdificioFixIt::ALTO_RALPH + 2;
        const int xI = l.xMin(), xD = l.xMax() - ANCHO;
        l.panelTranslucido(xI, y0, ANCHO, l.yMax() - y0, Colores::NEGRO, Glifo::OSCURO);
        l.panelTranslucido(xD, y0, ANCHO, l.yMax() - y0, Colores::NEGRO, Glifo::OSCURO);
        for (int y = y0; y < l.yMax(); y++) { l.pixel(xI + ANCHO - 1, y, Interfaz::DORADO_OSCURO); l.pixel(xD, y, Interfaz::DORADO_OSCURO); }
        int x = xI + 2, y = y0 + 2;
        if (modo == HISTORIA) { FuenteMini::texto(l, x, y, L"[INFO]", Color(120, 255, 140)); y += 9; }
        FuenteMini::texto(l, x, y, L"VIDAS:", texto); y += 7;
        const Sprite* cara = SPR(SpritesFixIt::F + "UI/iconos_hud/icono_01");
        for (int i = 0; i < VIDAS_MAX; i++) {
            if (i < vidas) cara->dibujar(l, x + i * 9, y);
            else cara->dibujarSombreado(l, x + i * 9, y, Glifo::CLARO, Color(120, 120, 120));
        }
        FuenteMini::texto(l, x + 29, y, std::to_wstring(vidas) + L"/" + std::to_wstring(VIDAS_MAX), resalte); y += 13;
        if (modo == HISTORIA) {
            FuenteMini::texto(l, x, y, L"REPUTACI\x00D3N", texto); y += 8;
            int r = ctx.progreso.getReputacion();
            Color medalla = r <= -2 ? Color(220, 40, 40) : r == -1 ? Color(240, 140, 40) : r == 0 ? Color(170, 210, 255)
                : r == 1 ? Color(110, 230, 110) : r == 2 ? Color(80, 230, 230) : Color(255, 210, 60);
            for (int j = 0; j < 7; j++) for (int i = 0; i < 7; i++) if ((i - 3) * (i - 3) + (j - 3) * (j - 3) <= 10) l.pixel(x + i, y + 1 + j, medalla);
            l.pixel(x + 2, y + 2, Colores::BLANCO);
            y += 9;
            FuenteMini::texto(l, x, y, ctx.progreso.etiquetaReputacion(), resalte);
        }
        else {
            FuenteMini::texto(l, x, y, L"NIVEL " + std::to_wstring(nivel + 1), resalte); y += 10;
            wchar_t num[16];
            swprintf_s(num, L"%06d", record);
            FuenteMini::texto(l, x, y, L"R\x00C9" L"CORD", Color(255, 120, 120)); y += 7;
            FuenteMini::texto(l, x, y, num, Color(255, 120, 120));
        }
        if (ayuda > 0 && fase == JUGANDO) {
            l.panelTranslucido(52, 58, 136, 22, Colores::NEGRO, Glifo::OSCURO);
            FuenteMini::centrado(l, 61, L"A/D MOVER   W SUBIR (SALTA PATOS)", Colores::AMARILLO, false);
            FuenteMini::centrado(l, 70, L"S BAJAR   X o ENTER: MARTILLO", Colores::AMARILLO, false);
        }
        x = xD + 1; y = y0 + 2;
        int est = edificio.estabilidad();
        Color c = est >= 75 ? Color(90, 230, 90) : est >= 45 ? Color(240, 210, 60) : Color(240, 70, 60);
        FuenteMini::texto(l, x, y, L"ESTABILIDAD", texto); y += 7;
        FuenteMini::texto(l, x, y, L"EDIFICIO:", texto); y += 9;
        dibujarBarra(l, x + 1, y, ANCHO - 4, 4, est / 100.0, c, Color(40, 40, 50)); y += 6;
        FuenteMini::texto(l, x + 1, y, std::to_wstring(est) + L"%", c); y += 13;
        FuenteMini::texto(l, x + 1, y, L"PTS", texto); y += 7;
        wchar_t num[16];
        swprintf_s(num, L"%06d", puntos);
        FuenteMini::texto(l, x + 1, y, num, Colores::BLANCO); y += 13;
        FuenteMini::texto(l, x + 1, y, L"ETAPA", texto); y += 7;
        FuenteMini::texto(l, x + 1, y, std::to_wstring(etapa + 1) + L"/" + std::to_wstring(EdificioFixIt::etapas()), resalte);
    }
public:
    JuegoFixIt(Contexto& c, Modo m)
        : ctx(c), modo(m), fase(INTRO), tFase(0), felix(EdificioFixIt::centroVentana(0), 0),
        ralph(EdificioFixIt::centroVentana(2), EdificioFixIt::piesRalph(0)), etapa(0), nivel(0), vidas(VIDAS_MAX), puntos(0),
        record(0), camY(EdificioFixIt::vistaEtapa(0)), camObjetivo(EdificioFixIt::vistaEtapa(0)), felixDesde(0), felixHasta(0),
        enfriaDano(0), temblor(0), proximoPastel(10), proximoPato(8), ayuda(0), esperaFinal(0),
        musica(M_NINGUNA), tIntro(0), introRompio(false), tFinal(0) {
        record = leerRecord();
        if (modo == HISTORIA) { irA(INTRO); musicaIntro(); }
        else { edificio.romper(45); musicaJuego(); prepararEtapa(0); ayuda = 4.0; }
    }
    ~JuegoFixIt() { if (modo == INFINITO) guardarRecord(); }
    void actualizar(double dt) {
        tFase += dt;
        if (temblor > 0) temblor -= dt;
        if (ayuda > 0 && fase == JUGANDO) ayuda -= dt;
        if (musica == M_INTRO && ctx.audio.musicaTerminada()) musicaJuego();
        switch (fase) {
        case INTRO:
            actualizarIntro(dt);
            break;
        case PREPARANDO:
            camY += (camObjetivo - camY) * std::min(1.0, dt * 3.0);
            globo.actualizar(dt);
            ralph.actualizar(dt);
            felix.actualizar(dt);
            if (tFase > 1.2 && tFase - dt <= 1.2 && etapa == 0 && modo == INFINITO) ctx.audio.reproducirVoz(L"felix_puedo_repararlo.wav");
            if (tFase > 2.4) {
                globo.ocultar();
                musicaJuego();
                ralph.empezar();
                if (ayuda <= 0 && etapa == 0) ayuda = 4.0;
                irA(JUGANDO);
            }
            break;
        case JUGANDO:
            camY = camObjetivo;
            actualizarJuego(dt);
            break;
        case SUBIENDO: {
            felix.actualizar(dt);
            ralph.actualizar(dt);
            const double DUR = 2.0;
            if (tFase < DUR) {
                double f = dt / DUR, sube = 2.0 * EdificioFixIt::ALTO_PISO;
                camY -= sube * f;
                ralph.moverY(-sube * f);
                felix.moverY((felixHasta - felixDesde) * f);
            }
            if (tFase >= DUR + 0.2) {
                if (etapa + 1 < EdificioFixIt::etapas()) prepararEtapa(etapa + 1);
                else if (modo == HISTORIA) { musicaVictoria(); tFinal = 0; irA(FINAL); }
                else {
                    nivel++;
                    edificio.pintar(nivel);
                    edificio.romper(45 - (nivel > 4 ? 20 : nivel * 4));
                    ctx.audio.efecto(L"etapa.wav");
                    prepararEtapa(0);
                }
            }
            break;
        }
        case FINAL:
            actualizarFinal(dt);
            break;
        case GANO:
        case PERDIO:
            break;
        }
    }
    void dibujar(Lienzo& l) {
        if (fase == INTRO) { dibujarIntro(l); return; }
        if (fase == FINAL || fase == GANO) { dibujarFinal(l); return; }
        l.sinCamara();
        l.degradadoVertical(0, l.getAncho() > 0 ? 120 : 0, Color(4, 4, 18), Color(14, 10, 30));
        int sacudir = temblor > 0 ? (((int)(temblor * 50)) % 2 ? 1 : -1) : 0;
        mirar(l, camY, sacudir);
        edificio.dibujar(l);
        for (auto& o : objetos) if (!dynamic_cast<LadrilloFixIt*>(o.get())) o->dibujar(l);
        ralph.dibujar(l);
        felix.dibujar(l);
        for (auto& o : objetos) if (dynamic_cast<LadrilloFixIt*>(o.get())) o->dibujar(l);
        globo.dibujar(l);
        l.sinCamara();
        hudJuego(l);
        if (fase == PREPARANDO) {
            std::wstring t = modo == INFINITO ? L"NIVEL " + std::to_wstring(nivel + 1) + L" - ETAPA " + std::to_wstring(etapa + 1)
                : L"ETAPA " + std::to_wstring(etapa + 1) + L" DE " + std::to_wstring(EdificioFixIt::etapas());
            l.panelTranslucido(0, 52, l.getAncho(), 14, Colores::NEGRO, Glifo::OSCURO);
            ctx.fuentes.amarilla.dibujarCentrado(l, 55, t);
        }
        if (fase == PERDIO) {
            l.panelTranslucido(0, 44, l.getAncho(), 30, Colores::NEGRO, Glifo::OSCURO);
            ctx.fuentes.roja.dibujarCentrado(l, 50, L"GAME OVER");
            if (modo == INFINITO && puntos >= record && puntos > 0) FuenteMini::centrado(l, 63, L"\x00A1NUEVO R\x00C9" L"CORD!", Colores::AMARILLO);
        }
    }
    bool gano() const { return fase == GANO; }
    bool perdio() const { return fase == PERDIO && tFase > 3.0; }
    int getPuntos() const { return puntos; }
};
