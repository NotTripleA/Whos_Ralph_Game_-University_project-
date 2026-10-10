#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include "../../Escenas/Escena.h"
#include "../../Motor/FuenteMini.h"
#include "../../Motor/Efectos.h"
#include "../../Historia/InterfazHistoria.h"
#include "PersonajesHerosDuty.h"
inline void TextoFlotanteHD::dibujar(Lienzo& l) const {
    if (t > 0.55 && ((int)(t * 16)) % 2) return;
    FuenteMini::textoConSombra(l, (int)x - FuenteMini::medir(texto) / 2, (int)y, texto, color);
}
namespace MapasHD {
    inline const std::vector<std::string>& piso(int i) {
        static const std::vector<std::string> p1 = {
            "############################",
            "#mmE.....#.........,,,,....#",
            "#........#.........,,,,....#",
            "#..CC....#....CC...........#",
            "#........#.................#",
            "#....,,,,#.......#####.....#",
            "#....,,,,........#...#.....#",
            "#####.......CC...#.NN#.....#",
            "#.......#........#.NN......#",
            "#.......#........#.........#",
            "#..CC...#####.####......CC.#",
            "#.......#...........#......#",
            "#.......#...........#......#",
            "#S......#....CC.....#......#",
            "#.......,...........,......#",
            "############################" };
        static const std::vector<std::string> p2 = {
            "############################",
            "#..........#.......#.......#",
            "#..NN......#..CC...#...S...#",
            "#..NN......#.......#.......#",
            "#..........#.......#.......#",
            "#....CC....,.......,.......#",
            "#..........#.......#.......#",
            "####.#######.......####.####",
            "#......#...............#...#",
            "#......#....,,,,,,.....#...#",
            "#..CC..#....,,,,,,.....#.CC#",
            "#......#...............#...#",
            "#......######..#########...#",
            "#..........................#",
            "#mmE......CC...........,,..#",
            "############################" };
        static const std::vector<std::string> p3 = {
            "############################",
            "#S.....#..............#....#",
            "#......#..CC......CC..#.NN.#",
            "#......#..............#.NN.#",
            "#......#....######....#....#",
            "#.,,,,.#....#....#....#....#",
            "#.,,,,......#....#.........#",
            "#......#....#....#....#....#",
            "####.###....##.###....######",
            "#.........................##",
            "#..CC.......,,,,,,.....CC..#",
            "#...........,,,,,,.........#",
            "#.....####........####.....#",
            "#.....#..............#.....#",
            "#mmE..#......CC......#.....#",
            "############################" };
        return i % 3 == 0 ? p1 : i % 3 == 1 ? p2 : p3;
    }
    inline const std::vector<std::string>& cima() {
        static const std::vector<std::string> c = {
            "####################",
            "#........M.......R.#",
            "#..................#",
            "#..CC..........CC..#",
            "#..................#",
            "#........NN........#",
            "#........NN........#",
            "#..................#",
            "#..CC..........CC..#",
            "#..................#",
            "#.......mmE........#",
            "####################" };
        return c;
    }
}
class JuegoHerosDuty {
public:
    enum Modo { HISTORIA, INFINITO };
    enum Fase { BRIEFING, JUGANDO, ASCENSOR, CIMA, JEFE, ELECCION, DESENLACE, TERMINADO, PERDIO };
private:
    Contexto& ctx;
    Modo modo;
    Fase fase;
    double tFase;
    MapaHD mapa;
    CalhounHD calhoun;
    std::vector<std::unique_ptr<MarineHD>> escuadron;
    std::vector<std::unique_ptr<CyBugHD>> bichos;
    std::vector<std::unique_ptr<Entidad>> efectos;
    NidoHD nido;
    std::unique_ptr<JefeHD> jefe;
    CuadroDialogo dialogo;
    MenuEleccion eleccion;
    std::vector<int> campo;
    std::vector<double> cdMarines;
    double recalculo;
    int piso;
    int municion;
    double recarga, cadencia;
    int integridad;
    int eliminados;
    int resueltos;
    double proximoBicho;
    int puntos, record, nidosVencidos;
    int dialogoCima, eleccionTomada;
    double temblor, alerta, golpeJefe;
    double camX, camY;
    int ultimaDir;
    bool salidaAbierta;
    int salidaC, salidaR;
    double xRalph, yRalph, xMedalla, yMedalla;
    int vozBriefing;
    double tVoz;
    static const int ENEMIGOS_POR_NIDO = 10;
    static const int NIDOS = 3;
    static const int MUNICION_MAX = 10;
    static const int FILAS = 135;
    static std::wstring rutaRecord() { return Rutas::en(L"Guardado\\record_herosduty.txt"); }
    int leerRecord() const { std::ifstream f{ std::filesystem::path(rutaRecord()) }; int r = 0; if (f) f >> r; return r; }
    void guardarRecord() const {
        std::error_code ec;
        std::filesystem::create_directories(std::filesystem::path(Rutas::en(L"Guardado")), ec);
        std::ofstream f{ std::filesystem::path(rutaRecord()) };
        if (f) f << record;
    }
    void irA(Fase f) { fase = f; tFase = 0; }
    bool enCima() const { return modo == HISTORIA && piso > NIDOS; }
    int dificultad() const { return modo == INFINITO ? nidosVencidos : piso - 1; }
    int nidosActivos() const {
        if (modo == INFINITO) return 1;
        if (enCima()) return 0;
        return NIDOS - piso + (nido.estaDestruido() ? 0 : 1);
    }
    void prepararPiso(int p) {
        piso = p;
        mapa.cargar(enCima() ? MapasHD::cima() : MapasHD::piso(p - 1));
        int ec = 1, er = 1;
        mapa.buscar('E', ec, er);
        calhoun.ubicar(ec, er);
        auto ms = mapa.todas('m');
        std::sort(ms.begin(), ms.end(), [&](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return std::abs(a.first - ec) + std::abs(a.second - er) < std::abs(b.first - ec) + std::abs(b.second - er); });
        escuadron.clear();
        for (size_t i = 0; i < ms.size(); i++) {
            escuadron.push_back(std::make_unique<MarineHD>(ms[i].first, ms[i].second, (int)i));
            mapa.poner(ms[i].first, ms[i].second, '.');
        }
        int nc = 0, nr = 0;
        mapa.buscar('N', nc, nr);
        nido.ubicar(nc, nr, ENEMIGOS_POR_NIDO);
        salidaC = salidaR = -1;
        mapa.buscar('S', salidaC, salidaR);
        salidaAbierta = false;
        int c = 0, r = 0;
        if (mapa.buscar('R', c, r)) { xRalph = MapaHD::centroX(c); yRalph = MapaHD::piesY(r); }
        if (mapa.buscar('M', c, r)) { xMedalla = MapaHD::centroX(c); yMedalla = MapaHD::piesY(r); }
        bichos.clear();
        efectos.clear();
        jefe.reset();
        resueltos = 0;
        proximoBicho = 2.0;
        dialogoCima = 0;
        recalculo = 0;
        recalcularCampo();
        moverCamara(1.0, true);
        irA(enCima() ? CIMA : JUGANDO);
        if (!enCima()) {
            int k = modo == INFINITO ? (p - 1) % 3 : p - 1;
            ctx.audio.reproducirVoz(k == 0 ? L"calhoun_mision.wav" : k == 1 ? L"calhoun_huevos.wav" : L"calhoun_cuidado.wav");
        }
    }
    void recalcularCampo() {
        std::vector<std::pair<int, int>> o;
        o.push_back({ calhoun.celdaC(), calhoun.celdaR() });
        for (auto& m : escuadron) o.push_back({ m->getC(), m->getR() });
        mapa.distancias(o, campo);
    }
    void moverCamara(double dt, bool instantaneo) { moverCamara(dt, instantaneo, calhoun.getX(), calhoun.getY()); }
    void moverCamara(double dt, bool instantaneo, double px, double py) {
        double objX = px - 120, objY = py - 34 - FILAS / 2;
        double maxX = mapa.anchoPx() - 240.0, maxY = mapa.altoPx() + 9.0 - FILAS;
        objX = maxX < 0 ? maxX / 2 : std::max(0.0, std::min(maxX, objX));
        objY = std::max(-44.0, std::min(maxY, objY));
        if (instantaneo) { camX = objX; camY = objY; return; }
        double k = std::min(1.0, dt * 8.0);
        camX += (objX - camX) * k;
        camY += (objY - camY) * k;
    }
    void soltarBicho() {
        std::vector<std::pair<int, int>> libres;
        for (int r = nido.getR() - 1; r <= nido.getR() + 2; r++)
            for (int c = nido.getC() - 1; c <= nido.getC() + 2; c++)
                if (!mapa.bloquea(c, r)) libres.push_back({ c, r });
        if (libres.empty()) return;
        auto p = libres[Azar::entero(0, (int)libres.size() - 1)];
        double x = MapaHD::centroX(p.first), y = MapaHD::piesY(p.second);
        int dif = dificultad();
        double vel = 20 + dif * 4.0;
        int tipo = Azar::entero(0, 9);
        if (dif == 0) bichos.push_back(std::make_unique<CyBugRastreroHD>(x, y, vel));
        else if (dif == 1) {
            if (tipo < 5) bichos.push_back(std::make_unique<CyBugRastreroHD>(x, y, vel));
            else bichos.push_back(std::make_unique<CyBugExploradorHD>(x, y, vel));
        }
        else {
            if (tipo < 3) bichos.push_back(std::make_unique<CyBugRastreroHD>(x, y, vel));
            else if (tipo < 7) bichos.push_back(std::make_unique<CyBugExploradorHD>(x, y, vel));
            else bichos.push_back(std::make_unique<CyBugVoladorHD>(x, y, vel));
        }
        efectos.push_back(std::make_unique<ChispaHD>(x, y - 10));
    }
    void sumar(int p) { puntos += p; if (puntos > record) record = puntos; }
    void danarEscuadron(int porcentaje) {
        integridad -= porcentaje;
        if (integridad < 0) integridad = 0;
        temblor = 0.3;
        alerta = 1.0;
        calhoun.lastimar();
        for (auto& m : escuadron) m->recibirAtaque();
        ctx.audio.efecto(L"herido.wav");
    }
    void controlar(double dt, bool puedeMoverse = true) {
        const Teclado& t = ctx.teclado;
        const int vk[4][2] = { { VK_UP, 'W' }, { VK_DOWN, 'S' }, { VK_LEFT, 'A' }, { VK_RIGHT, 'D' } };
        bool recien = false;
        for (int d = 0; d < 4; d++) if (t.presionada(vk[d][0]) || t.presionada(vk[d][1])) { ultimaDir = d; recien = true; }
        if (ultimaDir >= 0 && !(t.mantenida(vk[ultimaDir][0]) || t.mantenida(vk[ultimaDir][1]))) {
            ultimaDir = -1;
            for (int d = 0; d < 4; d++) if (t.mantenida(vk[d][0]) || t.mantenida(vk[d][1])) ultimaDir = d;
        }
        int antesC = calhoun.getC(), antesR = calhoun.getR();
        calhoun.controlar(puedeMoverse ? ultimaDir : -1, recien, mapa, dt);
        if (calhoun.empezoPaso()) {
            int pc = antesC, pr = antesR;
            for (auto& m : escuadron) {
                int mc = m->getC(), mr = m->getR();
                m->seguir(pc, pr, calhoun.duracionPaso());
                pc = mc; pr = mr;
            }
        }
        calhoun.actualizar(dt);
        for (auto& m : escuadron) m->actualizar(dt);
        if (cadencia > 0) cadencia -= dt;
        if (recarga > 0) {
            recarga -= dt;
            if (recarga <= 0) { municion = MUNICION_MAX; ctx.audio.efecto(L"recarga.wav"); }
        }
        else if (t.presionada(VK_SPACE) && cadencia <= 0 && municion > 0) {
            municion--;
            cadencia = 0.16;
            calhoun.disparar();
            DirHD d = calhoun.getDir();
            efectos.push_back(std::make_unique<BalaHD>(calhoun.getX() + dxDe(d) * 16, calhoun.getY() + dyDe(d) * 4, d));
            ctx.audio.efecto(L"disparo.wav");
            if (municion == 0) recarga = 1.4;
        }
    }
    void escuadronDispara(double dt) {
        if (cdMarines.size() != escuadron.size()) cdMarines.assign(escuadron.size(), 1.0);
        for (size_t i = 0; i < escuadron.size(); i++) {
            cdMarines[i] -= dt;
            if (cdMarines[i] > 0) continue;
            const MarineHD& m = *escuadron[i];
            for (auto& g : bichos) {
                double dx = g->getX() - m.getX(), dy = g->getY() - m.getY();
                bool enFila = std::fabs(dy) < 10 && std::fabs(dx) < 90, enColumna = std::fabs(dx) < 10 && std::fabs(dy) < 70;
                if (!enFila && !enColumna) continue;
                DirHD d = enFila ? (dx < 0 ? DirHD::IZQUIERDA : DirHD::DERECHA) : (dy < 0 ? DirHD::ARRIBA : DirHD::ABAJO);
                efectos.push_back(std::make_unique<BalaHD>(m.getX() + dxDe(d) * 14, m.getY() + dyDe(d) * 4, d));
                ctx.audio.efecto(L"disparo.wav");
                cdMarines[i] = modo == HISTORIA ? Azar::real(1.1, 1.6) : Azar::real(1.8, 2.6);
                break;
            }
        }
    }
    void impactos() {
        for (auto& e : efectos) {
            auto* b = dynamic_cast<BalaHD*>(e.get());
            if (!b || !b->estaActiva()) continue;
            int c = MapaHD::celdaDeX(b->getX()), r = MapaHD::celdaDeY(b->getY());
            if (mapa.pared(c, r) || mapa.en(c, r) == 'C') {
                b->desactivar();
                efectos.push_back(std::make_unique<ChispaHD>(b->getX(), b->getY() - BalaHD::ALTURA));
                continue;
            }
            for (auto& g : bichos) {
                if (!g->estaActiva()) continue;
                bool horizontal = b->getDir() == DirHD::IZQUIERDA || b->getDir() == DirHD::DERECHA;
                double ancho = horizontal ? 12 : 14, alto = horizontal ? 14 : 12;
                if (std::fabs(g->getX() - b->getX()) < ancho && std::fabs(g->getY() - b->getY()) < alto) {
                    b->desactivar();
                    efectos.push_back(std::make_unique<ExplosionHD>(g->getX(), g->centroY(), false));
                    if (g->recibirDisparo()) {
                        g->desactivar();
                        eliminados++;
                        resueltos++;
                        sumar(100);
                        efectos.push_back(std::make_unique<TextoFlotanteHD>(g->getX(), g->centroY() - 14, L"+100", Color(120, 255, 140)));
                        ctx.audio.efecto(L"explosion.wav");
                    }
                    break;
                }
            }
            if (b->estaActiva() && jefe && fase == JEFE && !jefe->derrotado()) {
                if (std::fabs(jefe->getX() - b->getX()) < 22 && std::fabs(jefe->getY() - b->getY()) < 18) {
                    b->desactivar();
                    double dano = 3.0 + 7.0 * integridad / 100.0;
                    jefe->herir(dano);
                    efectos.push_back(std::make_unique<ExplosionHD>(b->getX(), b->getY() - BalaHD::ALTURA, true));
                    efectos.push_back(std::make_unique<TextoFlotanteHD>(jefe->getX(), jefe->centroY() - 22, L"-" + std::to_wstring((int)(dano + 0.5)), Color(255, 220, 80)));
                    ctx.audio.efecto(L"explosion.wav");
                }
            }
        }
    }
    bool tocaEscuadron(double x, double y, double rx, double ry) const {
        if (std::fabs(calhoun.getX() - x) < rx && std::fabs(calhoun.getY() - y) < ry) return true;
        for (auto& m : escuadron) if (std::fabs(m->getX() - x) < rx && std::fabs(m->getY() - y) < ry) return true;
        return false;
    }
    void actualizarBichos(double dt) {
        recalculo -= dt;
        if (recalculo <= 0) { recalculo = 0.25; recalcularCampo(); }
        for (auto& g : bichos) {
            g->actualizar(dt);
            g->moverse(dt, mapa, campo, calhoun.getX(), calhoun.getY());
            if (g->estaActiva() && tocaEscuadron(g->getX(), g->getY(), 12, 9)) {
                g->desactivar();
                resueltos++;
                efectos.push_back(std::make_unique<ExplosionHD>(g->getX(), g->centroY(), false));
                danarEscuadron(10);
            }
        }
        bichos.erase(std::remove_if(bichos.begin(), bichos.end(), [](const std::unique_ptr<CyBugHD>& b) { return !b->estaActiva(); }), bichos.end());
    }
    void actualizarEfectos(double dt) {
        for (auto& e : efectos) e->actualizar(dt);
        efectos.erase(std::remove_if(efectos.begin(), efectos.end(), [](const std::unique_ptr<Entidad>& e) { return !e->estaActiva(); }), efectos.end());
    }
    bool revisarDerrota() {
        if (integridad > 0) return false;
        irA(PERDIO);
        ctx.audio.detenerMusica();
        ctx.audio.efecto(L"gameover.wav");
        return true;
    }
    void actualizarJuego(double dt) {
        controlar(dt);
        if (ctx.teclado.presionada(VK_F9)) {
            if (salidaAbierta) { subirAscensor(); return; }
            eliminados += ENEMIGOS_POR_NIDO - resueltos;
            resueltos = ENEMIGOS_POR_NIDO;
            while (nido.quedan()) nido.soltar();
            bichos.clear();
        }
        proximoBicho -= dt;
        if (proximoBicho <= 0 && nido.quedan()) {
            proximoBicho = std::max(1.0, Azar::real(2.2, 3.0) - dificultad() * 0.3);
            soltarBicho();
            nido.soltar();
        }
        escuadronDispara(dt);
        actualizarBichos(dt);
        impactos();
        actualizarEfectos(dt);
        moverCamara(dt, false);
        if (revisarDerrota()) return;
        if (!nido.estaDestruido() && resueltos >= ENEMIGOS_POR_NIDO && bichos.empty()) {
            nido.destruir();
            salidaAbierta = true;
            efectos.push_back(std::make_unique<ExplosionHD>(nido.centroX(), nido.piesY() - 16, false));
            ctx.audio.efecto(L"etapa.wav");
            sumar(500);
            if (modo == INFINITO) nidosVencidos++;
        }
        if (salidaAbierta && !calhoun.seMueve() && calhoun.getC() == salidaC && calhoun.getR() == salidaR) subirAscensor();
    }
    void subirAscensor() {
        ctx.audio.efecto(L"puerta.wav");
        if (modo == HISTORIA && piso == NIDOS) ctx.audio.reproducirVoz(L"calhoun_laboratorio.wav");
        irA(ASCENSOR);
    }
    void actualizarAscensor(double dt) {
        actualizarEfectos(dt);
        if (tFase > 3.0) prepararPiso(piso + 1);
    }
    void siguienteDialogoCima() {
        using namespace SpritesHD;
        Retrato calhounR(SPR(H + "Calhoun/caminar/calhoun_caminar_01"));
        Retrato ralphR(SPR(H + "Ralph_armadura/fila_1/ralph_armadura_f1_01"), SPR(H + "Ralph_armadura/fila_1/ralph_armadura_f1_03"));
        switch (dialogoCima) {
        case 0:
            dialogo.mostrar(L"RALPH", L"\x00BF" L"Desde cu\x00E1ndo los videojuegos son tan violentos y aterradores? \x00A1S\x00E1lvame! \x00A1Por favor, s\x00E1" L"came de aqu\x00ED!", ralphR, Color(255, 120, 90));
            ctx.audio.reproducirVoz(L"ralph_salvame.wav");
            break;
        case 1: dialogo.mostrar(L"CALHOUN", L"\x00A1" L"Ah\x00ED est\x00E1s! T\x00FA despertaste a los Cy-Bugs para llevarte la medalla.", calhounR, Color(255, 220, 120)); break;
        case 2: dialogo.mostrar(L"RALPH", L"\x00A1Yo solo quer\x00ED" L"a la medalla! \x00A1No sab\x00ED" L"a que esos bichos iban a despertar!", ralphR, Color(255, 120, 90)); break;
        case 3: dialogo.mostrar(L"CALHOUN", L"\x00A1Silencio! ... Esperen... \x00A1" L"El nido final! \x00A1" L"Escuadr\x00F3n, en posici\x00F3n!", calhounR, Color(255, 220, 120)); break;
        default: break;
        }
    }
    static const int DIALOGOS_CIMA = 4;
    void actualizarCima(double dt) {
        bool hablando = dialogo.estaActivo();
        controlar(dt, !hablando && tFase < 1.2);
        actualizarEfectos(dt);
        if (tFase < 1.2) moverCamara(dt, false);
        else if (dialogoCima < DIALOGOS_CIMA - 1) moverCamara(dt * 0.5, false, xRalph - 50, yRalph + 20);
        else moverCamara(dt * 0.5, false, nido.centroX(), nido.piesY());
        dialogo.actualizar(dt, ctx.audio, !ctx.audio.vozSonando());
        if (tFase < 1.2) return;
        if (!dialogo.estaActivo()) { siguienteDialogoCima(); return; }
        if (ctx.teclado.aceptar()) {
            if (!dialogo.completo()) { dialogo.completar(); return; }
            dialogo.ocultar();
            ctx.audio.detenerVoz();
            dialogoCima++;
            if (dialogoCima >= DIALOGOS_CIMA) {
                const double C = MapaHD::CELDA;
                jefe = std::make_unique<JefeHD>(nido.centroX(), nido.piesY() + 4, 120, C + 16, mapa.anchoPx() - C - 16, C * 2, mapa.altoPx() - C - 4);
                nido.destruir();
                efectos.push_back(std::make_unique<ExplosionHD>(nido.centroX(), nido.piesY() - 20, true));
                temblor = 0.6;
                golpeJefe = 1.5;
                ctx.audio.efecto(L"golpe_ralph.wav");
                irA(JEFE);
            }
            else siguienteDialogoCima();
        }
    }
    void actualizarJefe(double dt) {
        controlar(dt);
        if (ctx.teclado.presionada(VK_F9)) jefe->herir(1000);
        jefe->actualizar(dt);
        jefe->perseguir(calhoun.getX(), calhoun.getY(), dt);
        if (jefe->consumirCria())
            for (int i = 0; i < (modo == HISTORIA ? 1 : 2); i++) bichos.push_back(std::make_unique<CyBugExploradorHD>(jefe->getX() + (i ? 14 : -14), jefe->getY(), 18));
        if (golpeJefe > 0) golpeJefe -= dt;
        if (golpeJefe <= 0 && tocaEscuadron(jefe->getX(), jefe->getY(), 20, 14)) {
            golpeJefe = 2.0;
            danarEscuadron(10);
            jefe->rebotar(calhoun.getX(), calhoun.getY());
        }
        escuadronDispara(dt);
        actualizarBichos(dt);
        impactos();
        actualizarEfectos(dt);
        moverCamara(dt, false);
        if (revisarDerrota()) return;
        if (jefe->derrotado()) {
            jefe->atontar();
            for (auto& g : bichos) efectos.push_back(std::make_unique<ExplosionHD>(g->getX(), g->centroY(), false));
            bichos.clear();
            sumar(3000);
            ctx.audio.efecto(L"etapa.wav");
            irA(ELECCION);
        }
    }
    void actualizarEleccion(double dt) {
        calhoun.actualizar(dt);
        actualizarEfectos(dt);
        moverCamara(dt * 0.6, false, jefe->getX(), jefe->getY());
        if (tFase > 1.0) jefe->irHacia(xRalph - 20, yRalph + 6, dt, 10);
        jefe->actualizar(0);
        if (tFase > 1.6 && !eleccion.estaActivo() && eleccionTomada == 0)
            eleccion.mostrar(L"EL CY-BUG VA POR RALPH... \x00BFQU\x00C9 HAR\x00C1 CALHOUN?", {
                L"Salvas al escuadr\x00F3n (recuperas el 100% de la integridad del escuadr\x00F3n)",
                L"Salvas a Ralph (no ganas ni pierdes integridad, pero el bicho recupera su vida)" });
        int r = eleccion.manejar(ctx, dt);
        if (r >= 0) {
            eleccionTomada = r + 1;
            if (r == 0) integridad = 100;
            else jefe->recuperar();
            irA(DESENLACE);
        }
    }
    void actualizarDesenlace(double dt) {
        calhoun.actualizar(dt);
        actualizarEfectos(dt);
        jefe->actualizar(dt);
        if (eleccionTomada == 2) jefe->irHacia(jefe->getX() + 40, -120, dt, 90);
        if (tFase > 3.5) irA(TERMINADO);
    }
public:
    static void dibujarSala(Lienzo& l, int piso, double t) {
        auto& b = BancoSprites::instancia();
        const std::string T = SpritesHD::H + "Construccion/tiles/tile_";
        int y1 = l.yMax();
        for (int y = l.yMin() - 10; y < 82; y += 41)
            for (int x = l.xMin() - 10; x < l.xMax(); x += 37) {
                int k = ((x / 37) + (y / 41) * 3 + piso) % 7;
                const char* id = k == 0 ? "03" : k == 3 ? "02" : k == 5 && y > 20 ? "20" : "01";
                b.obtener(T + id)->dibujar(l, x, y);
            }
        for (int x = l.xMin() - 10; x < l.xMax(); x += 37) b.obtener(T + "12")->dibujar(l, x, 76);
        for (int y = 86; y < y1; y += 39)
            for (int x = l.xMin() - 10; x < l.xMax(); x += 37) b.obtener(T + "08")->dibujar(l, x, y);
        for (int y = 86; y < y1; y++) for (int x = l.xMin(); x < l.xMax(); x++) if ((x + y) % 5 == 0) l.sombrear(x, y, Glifo::CLARO, Colores::NEGRO);
        bool rojo = ((int)(t * 2)) % 2 == 0;
        for (int x = 20; x < 240; x += 74) {
            Color c = rojo ? Color(255, 40, 40) : Color(90, 10, 10);
            l.rectangulo(x, 2, 6, 3, c);
            if (rojo) for (int j = 5; j < 18; j++) for (int i = -j / 2; i <= j / 2; i++) l.sombrear(x + 3 + i, j, Glifo::CLARO, Color(255, 40, 40));
        }
    }
private:
    void dibujarMedalla(Lienzo& l) const {
        int cx = (int)xMedalla, base = (int)yMedalla;
        l.rectangulo(cx - 9, base - 20, 18, 20, Color(70, 70, 86));
        l.rectangulo(cx - 11, base - 22, 22, 3, Color(150, 150, 170));
        bool brilla = ((int)(tFase * 4)) % 2 == 0;
        Color oro(255, 205, 60), oroOsc(190, 130, 20), cinta(200, 30, 40);
        int my = base - 30;
        for (int j = 0; j < 8; j++) { l.pixel(cx - 4 + j / 2, my - 14 + j, cinta); l.pixel(cx + 3 - j / 2, my - 14 + j, cinta); }
        for (int j = -6; j <= 6; j++) for (int i = -6; i <= 6; i++) if (i * i + j * j <= 36) l.pixel(cx + i, my + j, (i + j) < 0 ? oro : oroOsc);
        l.pixel(cx - 2, my - 2, Colores::BLANCO);
        if (brilla) { l.sombrear(cx, my - 16, Glifo::MEDIO, oro); l.sombrear(cx + 10, my - 4, Glifo::MEDIO, oro); l.sombrear(cx - 10, my - 4, Glifo::MEDIO, oro); }
    }
    void dibujarRalph(Lienzo& l) const {
        if (eleccionTomada == 2 && fase == DESENLACE) return;
        const Sprite* r = BancoSprites::instancia().obtener(SpritesHD::H + "Ralph_armadura/fila_2/ralph_armadura_f2_0" + std::to_string(1 + ((int)(tFase * 6)) % 2));
        int temb = ((int)(tFase * 24)) % 2;
        r->dibujar(l, (int)xRalph - r->getAncho() / 2 + temb, (int)yRalph - r->getAlto() + 4);
    }
    void cajas(std::vector<std::pair<double, int>>& orden) const {
        for (int r = 0; r < mapa.getAlto(); r++)
            for (int c = 0; c < mapa.getAncho(); c++)
                if (mapa.en(c, r) == 'C' && mapa.en(c - 1, r) != 'C') orden.push_back({ MapaHD::piesY(r) + 1, c * 1000 + r });
    }
    void dibujarMundo(Lienzo& l) {
        int alto = l.yMax() - l.yMin();
        int sx = temblor > 0 ? (((int)(temblor * 40)) % 2 ? 2 : -2) : 0;
        int izq = (int)camX, arriba = (int)camY - (alto - FILAS) / 2;
        l.camara(izq - l.xMin() + sx, arriba - l.yMin());
        mapa.dibujar(l, izq, arriba, piso, salidaAbierta, tFase);
        struct Item { double y; int tipo; int i; };
        std::vector<Item> items;
        items.push_back({ calhoun.getY(), 0, 0 });
        for (size_t i = 0; i < escuadron.size(); i++) items.push_back({ escuadron[i]->getY(), 1, (int)i });
        for (size_t i = 0; i < bichos.size(); i++) items.push_back({ bichos[i]->getY(), 2, (int)i });
        std::vector<std::pair<double, int>> cs;
        cajas(cs);
        for (auto& c : cs) items.push_back({ c.first, 3, c.second });
        if (nido.getC() > 0 || nido.getR() > 0) items.push_back({ nido.piesY(), 4, 0 });
        if (jefe) items.push_back({ jefe->getY(), 5, 0 });
        if (enCima()) { items.push_back({ yRalph, 6, 0 }); items.push_back({ yMedalla, 7, 0 }); }
        std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b) { return a.y < b.y; });
        auto& b = BancoSprites::instancia();
        for (auto& it : items) {
            switch (it.tipo) {
            case 0: calhoun.dibujar(l); break;
            case 1: escuadron[it.i]->dibujar(l); break;
            case 2: bichos[it.i]->dibujar(l); break;
            case 3: {
                int c = it.i / 1000, r = it.i % 1000;
                const Sprite* s = b.obtener(SpritesHD::H + "Props_destructibles/fila_1/prop_fila_1_0" + std::string((c + r) % 2 ? "1" : "2"));
                s->dibujar(l, (c + 1) * MapaHD::CELDA - s->getAncho() / 2, (r + 1) * MapaHD::CELDA - s->getAlto() + 1);
                break;
            }
            case 4: nido.dibujar(l, tFase); break;
            case 5: jefe->dibujar(l); break;
            case 6: dibujarRalph(l); break;
            case 7: dibujarMedalla(l); break;
            }
        }
        for (auto& e : efectos) e->dibujar(l);
        l.sinCamara();
    }
    void minimapa(Lienzo& l) const {
        const int px = 1;
        int w = mapa.getAncho() * px, h = mapa.getAlto() * px;
        int x0 = l.xMax() - w - 3, y0 = l.yMin() + 12;
        l.panelTranslucido(x0 - 2, y0 - 2, w + 4, h + 4, Colores::NEGRO, Glifo::OSCURO);
        for (int r = 0; r < mapa.getAlto(); r++)
            for (int c = 0; c < mapa.getAncho(); c++) {
                char k = mapa.en(c, r);
                Color col;
                bool pinta = true;
                if (k == '#') col = Color(110, 114, 130);
                else if (k == 'C') col = Color(80, 70, 50);
                else if (k == 'S') col = salidaAbierta ? Color(80, 255, 120) : Color(160, 60, 60);
                else if (k == 'N') col = nido.estaDestruido() ? Color(70, 70, 70) : Color(255, 80, 80);
                else pinta = false;
                if (pinta) l.rectangulo(x0 + c * px, y0 + r * px, px, px, col);
            }
        for (auto& g : bichos) l.pixel(x0 + MapaHD::celdaDeX(g->getX()) * px, y0 + MapaHD::celdaDeY(g->getY()) * px, Color(255, 60, 60));
        for (auto& m : escuadron) l.rectangulo(x0 + m->getC() * px, y0 + m->getR() * px, px, px, Color(90, 200, 90));
        if (((int)(tFase * 4)) % 2 == 0) l.rectangulo(x0 + calhoun.celdaC() * px, y0 + calhoun.celdaR() * px, px, px, Colores::AMARILLO);
    }
    void hud(Lienzo& l) const {
        const int yA = l.yMin(), yB = l.yMax() - 9;
        Color texto(220, 230, 240), cian(80, 230, 255);
        l.panelTranslucido(l.xMin(), yA, l.xMax() - l.xMin(), 9, Colores::NEGRO, Glifo::OSCURO);
        l.panelTranslucido(l.xMin(), yB, l.xMax() - l.xMin(), 9, Colores::NEGRO, Glifo::OSCURO);
        FuenteMini::texto(l, 2, yA, L"MUNICI\x00D3N:", texto);
        for (int i = 0; i < MUNICION_MAX; i++) l.rectangulo(40 + i * 3, yA + 2, 2, 5, i < municion ? cian : Color(40, 60, 70));
        if (recarga > 0 && ((int)(recarga * 6)) % 2 == 0) FuenteMini::texto(l, 72, yA, L"RECARGANDO", Color(255, 200, 60));
        else FuenteMini::texto(l, 72, yA, std::to_wstring(municion) + L"/" + std::to_wstring(MUNICION_MAX), Colores::AMARILLO);
        FuenteMini::texto(l, 114, yA, L"INTEGRIDAD DEL ESCUADR\x00D3N:", texto);
        Color ci = integridad >= 70 ? Color(90, 230, 90) : integridad >= 40 ? Color(240, 210, 60) : Color(240, 60, 60);
        if (alerta > 0 && ((int)(alerta * 10)) % 2) ci = Colores::BLANCO;
        FuenteMini::texto(l, 216, yA, std::to_wstring(integridad) + L"%", ci);
        FuenteMini::texto(l, 2, yB + 1, L"NIDOS ACTIVOS:", texto);
        if (modo == INFINITO) FuenteMini::texto(l, 60, yB + 1, L"INF", Color(255, 120, 120));
        else if (enCima()) FuenteMini::texto(l, 60, yB + 1, fase == CIMA || fase == JEFE ? L"FINAL" : L"0/3", Color(255, 120, 120));
        else FuenteMini::texto(l, 60, yB + 1, std::to_wstring(nidosActivos()) + L"/" + std::to_wstring(NIDOS), Color(255, 120, 120));
        FuenteMini::texto(l, 84, yB + 1, L"ENEMIGOS ELIMINADOS:", texto);
        std::wstring cuenta = std::to_wstring(eliminados) + (modo == HISTORIA ? L"/" + std::to_wstring(NIDOS * ENEMIGOS_POR_NIDO) : L"");
        FuenteMini::texto(l, 166, yB + 1, cuenta, Colores::AMARILLO);
        wchar_t num[16];
        swprintf_s(num, L"%06d", modo == INFINITO ? record : puntos);
        FuenteMini::texto(l, 190, yB + 1, (modo == INFINITO ? L"R " : L"P ") + std::wstring(num), Colores::BLANCO);
        std::wstring obj;
        Color co = Color(120, 255, 140);
        if (fase == JUGANDO) obj = salidaAbierta ? L"\x00A1NIDO DESTRUIDO! VE AL ASCENSOR \x2191" : L"OBJETIVO: ELIMINA LOS CY-BUGS DEL NIDO";
        if (fase == JEFE) { obj = L"\x00A1" L"DERROTA AL CY-BUG GIGANTE!"; co = Color(255, 120, 120); }
        if (!obj.empty() && (!salidaAbierta || ((int)(tFase * 3)) % 3)) {
            l.panelTranslucido(l.xMin(), yA + 9, FuenteMini::medir(obj) + 6, 8, Colores::NEGRO, Glifo::MEDIO);
            FuenteMini::texto(l, 2, yA + 9, obj, co);
        }
        if (fase == JUGANDO || fase == JEFE || fase == CIMA) minimapa(l);
    }
    void dibujarAscensor(Lienzo& l) const {
        l.sinCamara();
        l.degradadoVertical(0, 120, Color(10, 12, 26), Color(40, 20, 30));
        for (int i = 0; i < 60; i++) {
            int sx = (i * 73) % 240, sy = (i * 41) % 120 - 7;
            if (((int)(tFase * 3) + i) % 5) l.pixel(sx, sy, Color(150, 160, 200));
        }
        const Sprite* torre = BancoSprites::instancia().obtener(SpritesHD::H + "Construccion/torre/torre_01");
        auto altura = [&](int p) { const double h[5] = { 220, 200, 150, 100, 36 }; return h[std::max(0, std::min(4, p))]; };
        int desde = modo == INFINITO ? 1 + (piso - 1) % 3 : piso;
        int hasta = modo == INFINITO ? (desde == 3 ? 1 : desde + 1) : piso + 1;
        double k = std::max(0.0, std::min(1.0, (tFase - 0.4) / 2.1));
        k = k * k * (3 - 2 * k);
        double luzY = altura(desde) + (altura(hasta) - altura(desde)) * k;
        if (modo == INFINITO && desde == 3) luzY = altura(3) + (altura(4) - altura(3)) * k;
        int tx = (l.getAncho() - torre->getAncho()) / 2;
        int arriba = (int)std::max((double)(l.yMax() - torre->getAlto()), std::min(18.0, 60 - luzY));
        torre->dibujar(l, tx, arriba);
        int ex = tx + 90, ey = arriba + (int)luzY;
        for (int j = -6; j <= 6; j++) for (int i = -6; i <= 6; i++) if (i * i + j * j <= 36) l.sombrear(ex + i, ey + j, Glifo::CLARO, Color(255, 230, 120));
        l.rectangulo(ex - 3, ey - 3, 7, 6, Color(255, 240, 160));
        std::wstring t = (modo == HISTORIA && hasta > NIDOS) ? L"\x00A1" L"A LA CIMA DE LA TORRE!" : L"SUBIENDO AL PISO " + std::to_wstring(modo == INFINITO ? piso + 1 : hasta);
        l.panelTranslucido(l.xMin(), 2, l.xMax() - l.xMin(), 14, Colores::NEGRO, Glifo::OSCURO);
        ctx.fuentes.amarilla.dibujarCentrado(l, 5, t);
    }
    void dibujarBriefing(Lienzo& l) const {
        auto& b = BancoSprites::instancia();
        l.limpiar(Color(4, 8, 14));
        for (int y = l.yMin(); y < l.yMax(); y += 3) for (int x = l.xMin(); x < l.xMax(); x++) l.sombrear(x, y, Glifo::CLARO, Color(20, 60, 80));
        const Sprite* logo = b.obtener(SpritesHD::H + "UI/logo_heros_duty");
        logo->dibujar(l, 6, -4);
        const Sprite* c = b.obtener(SpritesHD::H + "Calhoun/disparar/calhoun_disparar_0" + std::to_string(1 + ((int)(tFase * 3)) % 4));
        c->dibujar(l, 186 - c->getAncho() / 2, 80 - c->getAlto());
        Interfaz::panel(l, 6, 80, 228, 38, true);
        FuenteMini::texto(l, 12, 84, L"CONTROLAS A:", Color(150, 220, 255));
        FuenteMini::texto(l, 62, 84, L"SARGENTO CALHOUN", Colores::AMARILLO);
        FuenteMini::texto(l, 12, 93, L"MOVIMIENTO: \x2191 \x2193 \x2190 \x2192   DISPARO: ESPACIO", Colores::BLANCO);
        FuenteMini::texto(l, 12, 102, L"\x00A1" L"ALERTA! Destruye los nidos y sube la torre.", Color(255, 140, 120));
        if (((int)(tFase * 2)) % 2 == 0) FuenteMini::texto(l, 12, 110, L"PRESIONA X O ENTER PARA EMPEZAR LA MISI\x00D3N", Color(120, 255, 140));
        static const wchar_t* subs[3] = {
            L"NARRADOR: En un planeta sin nombre, un experimento ultrasecreto ha fallado por completo.",
            L"CALHOUN: Atenci\x00F3n, solo voy a decirlo una vez. El miedo es para los cobardes, se\x00F1oritas. \x00A1Pero hagan que su mam\x00E1 est\x00E9 orgullosa!",
            L"ESCUADR\x00D3N: \x00A1Viva mam\x00E1, viva!" };
        if (vozBriefing > 0 && (ctx.audio.vozSonando() || tVoz < 0.6)) {
            auto lineas = FuenteMini::partir(subs[vozBriefing - 1], 56);
            int alto = (int)lineas.size() * 8 + 3, y0 = 78 - alto;
            l.panelTranslucido(l.xMin(), y0, l.xMax() - l.xMin(), alto, Colores::NEGRO, Glifo::OSCURO);
            for (size_t i = 0; i < lineas.size(); i++) FuenteMini::centrado(l, y0 + 2 + (int)i * 8, lineas[i], Color(255, 240, 200));
        }
    }
public:
    JuegoHerosDuty(Contexto& c, Modo m)
        : ctx(c), modo(m), fase(BRIEFING), tFase(0), recalculo(0), piso(1), municion(MUNICION_MAX), recarga(0), cadencia(0),
        integridad(100), eliminados(0), resueltos(0), proximoBicho(2), puntos(0), record(0), nidosVencidos(0),
        dialogoCima(0), eleccionTomada(0), temblor(0), alerta(0), golpeJefe(0), camX(0), camY(0), ultimaDir(-1),
        salidaAbierta(false), salidaC(-1), salidaR(-1), xRalph(0), yRalph(0), xMedalla(0), yMedalla(0), vozBriefing(0), tVoz(1) {
        record = leerRecord();
        ctx.audio.reproducirMusica(L"herosduty.wav");
    }
    ~JuegoHerosDuty() { if (modo == INFINITO) guardarRecord(); }
    void actualizar(double dt) {
        tFase += dt;
        if (temblor > 0) temblor -= dt;
        if (alerta > 0) alerta -= dt;
        switch (fase) {
        case BRIEFING: {
            static const wchar_t* voces[3] = { L"hd_narrador.wav", L"calhoun_briefing.wav", L"escuadron_viva.wav" };
            tVoz += dt;
            if (vozBriefing < 3 && tFase > 0.4 && tVoz > 0.6 && !ctx.audio.vozSonando()) {
                ctx.audio.reproducirVoz(voces[vozBriefing]);
                vozBriefing++;
                tVoz = 0;
            }
            if (tFase > 0.8 && ctx.teclado.aceptar()) { ctx.audio.detenerVoz(); ctx.audio.efecto(L"menu_aceptar.wav"); prepararPiso(1); }
            break;
        }
        case JUGANDO: actualizarJuego(dt); break;
        case ASCENSOR: actualizarAscensor(dt); break;
        case CIMA: actualizarCima(dt); break;
        case JEFE: actualizarJefe(dt); break;
        case ELECCION: actualizarEleccion(dt); break;
        case DESENLACE: actualizarDesenlace(dt); break;
        default: break;
        }
    }
    void dibujar(Lienzo& l) {
        l.sinCamara();
        if (fase == BRIEFING) { dibujarBriefing(l); return; }
        if (fase == ASCENSOR) { dibujarAscensor(l); return; }
        l.limpiar(Color(8, 8, 14));
        dibujarMundo(l);
        if (jefe && (fase == JEFE || fase == ELECCION || fase == DESENLACE)) {
            FuenteMini::texto(l, 60, l.yMin() + 19, L"CY-BUG GIGANTE", Color(255, 120, 120));
            dibujarBarra(l, 118, l.yMin() + 21, 100, 4, jefe->fraccionVida(), Color(230, 50, 50), Color(50, 20, 20));
        }
        hud(l);
        if (fase == JUGANDO && tFase < 2.0) {
            l.panelTranslucido(l.xMin(), 46, l.xMax() - l.xMin(), 22, Colores::NEGRO, Glifo::OSCURO);
            int n = modo == HISTORIA ? piso : nidosVencidos + 1;
            ctx.fuentes.blanca.dibujarCentrado(l, 53, L"PISO " + std::to_wstring(piso) + L" - NIDO " + std::to_wstring(n));
        }
        if (fase == CIMA && tFase < 1.2) {
            l.panelTranslucido(l.xMin(), 46, l.xMax() - l.xMin(), 22, Colores::NEGRO, Glifo::OSCURO);
            ctx.fuentes.amarilla.dibujarCentrado(l, 53, L"CIMA DE LA TORRE");
        }
        if (fase == JEFE && tFase < 2.0) ctx.fuentes.roja.dibujarCentrado(l, 40, L"\x00A1NIDO FINAL!");
        if (fase == DESENLACE) {
            std::wstring t = eleccionTomada == 1 ? L"\x00A1" L"ESCUADR\x00D3N A SALVO!" : L"\x00A1RALPH A SALVO!";
            ctx.fuentes.amarilla.dibujarCentrado(l, 50, t);
        }
        if (fase == PERDIO) {
            l.panelTranslucido(l.xMin(), 44, l.xMax() - l.xMin(), 30, Colores::NEGRO, Glifo::OSCURO);
            ctx.fuentes.roja.dibujarCentrado(l, 50, L"GAME OVER");
            FuenteMini::centrado(l, 63, L"EL ESCUADR\x00D3N CAY\x00D3", Colores::BLANCO);
        }
        dialogo.dibujar(l);
        eleccion.dibujar(l);
    }
    bool termino() const { return fase == TERMINADO; }
    bool perdio() const { return fase == PERDIO && tFase > 3.0; }
    int getEleccion() const { return eleccionTomada; }
    int getPuntos() const { return puntos; }
};
