#pragma once
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "../../Escenas/Escena.h"
#include "../../Motor/FuenteMini.h"
#include "../../Motor/Efectos.h"
#include "../../Motor/Azar.h"
#include "../../Historia/InterfazHistoria.h"
#include "../../Historia/Personajes.h"
struct CajaEsquive {
    static const int X = 70, Y = 34, W = 100, H = 56;
};
struct AtaqueTurbo {
    double x, y, vx, vy, t;
    int tipo;
    bool vivo;
};
class Batalla;
class MiembroEquipo {
protected:
    std::wstring nombre;
    std::string sprite;
    Color color;
    int vidaMax, vida, ataque;
    int enfriamiento, cdActual;
    double habilidad;
    double cdHabilidad;
public:
    bool defendiendo;
    double golpeado;
    double x;
    MiembroEquipo(const std::wstring& n, const std::string& s, Color c, int v, int a, int cd)
        : nombre(n), sprite(s), color(c), vidaMax(v), vida(v), ataque(a), enfriamiento(cd), cdActual(0), habilidad(0), cdHabilidad(0),
        defendiendo(false), golpeado(0), x(0) {}
    virtual ~MiembroEquipo() {}
    const std::wstring& getNombre() const { return nombre; }
    Color getColor() const { return color; }
    int getVida() const { return vida; }
    int getVidaMax() const { return vidaMax; }
    int getAtaque() const { return ataque; }
    bool vivo() const { return vida > 0; }
    bool especialListo() const { return cdActual <= 0; }
    int getCd() const { return cdActual; }
    void usarEspecialCd() { cdActual = enfriamiento; }
    void pasarRonda() { if (cdActual > 0) cdActual--; defendiendo = false; }
    void curar(int v) { if (vivo()) vida = std::min(vidaMax, vida + v); }
    virtual void recibir(int dano) { vida -= defendiendo ? dano / 2 : dano; if (vida < 0) vida = 0; golpeado = 0.5; }
    virtual std::wstring nombreEspecial() const = 0;
    virtual std::wstring descripcionEspecial() const = 0;
    virtual double multiplicadorEspecial() const { return 2.0; }
    virtual bool especialCura() const { return false; }
    virtual double velocidadAlma() const { return 60; }
    virtual std::wstring ayudaEsquive() const = 0;
    virtual void usarHabilidad(double& ax, double& ay, int dx, int dy, std::vector<AtaqueTurbo>& ataques) = 0;
    bool invulnerable() const { return habilidad > 0; }
    void actualizarEsquive(double dt) { if (habilidad > 0) habilidad -= dt; if (cdHabilidad > 0) cdHabilidad -= dt; if (golpeado > 0) golpeado -= dt; }
    bool puedeHabilidad() const { return cdHabilidad <= 0; }
    virtual void dibujar(Lienzo& l, int pies, double t) const {
        const Sprite* s = BancoSprites::instancia().obtener(sprite);
        if (!vivo()) { s->dibujarSombreado(l, (int)x - s->getAncho() / 2, pies - s->getAlto() + 10, Glifo::CLARO, Color(90, 90, 100)); return; }
        if (golpeado > 0 && ((int)(golpeado * 14)) % 2) return;
        s->dibujar(l, (int)x - s->getAncho() / 2, pies - s->getAlto());
        if (defendiendo) for (int j = 0; j < s->getAlto(); j += 3) l.sombrear((int)x + s->getAncho() / 2 + 2, pies - j, Glifo::MEDIO, Color(120, 200, 255));
    }
    const Sprite* retratoSprite() const { return BancoSprites::instancia().obtener(sprite); }
};
class RalphCombate : public MiembroEquipo {
public:
    int corazones;
    RalphCombate() : MiembroEquipo(L"RALPH", "02_Vanellope_y_personajes/Personajes_escala/ralph", Color(255, 120, 90), 120, 16, 3), corazones(3) {}
    std::wstring nombreEspecial() const override { return L"DEMOLER"; }
    std::wstring descripcionEspecial() const override { return L"Golpe demoledor: x2.5 de da\x00F1o"; }
    double multiplicadorEspecial() const override { return 2.5; }
    double velocidadAlma() const override { return 48; }
    std::wstring ayudaEsquive() const override { return L"ESPACIO: PU\x00D1" L"ETAZO (rompe ataques cercanos)"; }
    void usarHabilidad(double& ax, double& ay, int, int, std::vector<AtaqueTurbo>& ataques) override {
        if (!puedeHabilidad()) return;
        cdHabilidad = 0.6;
        for (auto& a : ataques) if (a.vivo && a.tipo != 1 && std::fabs(a.x - ax) < 14 && std::fabs(a.y - ay) < 14) a.vivo = false;
    }
    bool levantarse() { if (corazones > 1) { corazones--; vida = vidaMax; return true; } corazones = 0; return false; }
};
class VanellopeCombate : public MiembroEquipo {
public:
    VanellopeCombate() : MiembroEquipo(L"VANELLOPE", "02_Vanellope_y_personajes/Vanellope/fila_1/vanellope_f1_01", Color(80, 230, 255), 70, 11, 3) {}
    std::wstring nombreEspecial() const override { return L"GLITCH"; }
    std::wstring descripcionEspecial() const override { return L"Se glitchea a trav\x00E9s de Turbo: x1.8"; }
    double multiplicadorEspecial() const override { return 1.8; }
    double velocidadAlma() const override { return 78; }
    std::wstring ayudaEsquive() const override { return L"ESPACIO: GLITCH (salta y no recibe da\x00F1o)"; }
    void usarHabilidad(double& ax, double& ay, int dx, int dy, std::vector<AtaqueTurbo>&) override {
        if (!puedeHabilidad()) return;
        cdHabilidad = 0.9;
        habilidad = 0.45;
        ax += dx * 18; ay += dy * 18;
    }
};
class FelixCombate : public MiembroEquipo {
public:
    FelixCombate() : MiembroEquipo(L"F\x00C9LIX", "02_Vanellope_y_personajes/Personajes_escala/felix", Color(110, 170, 255), 80, 7, 3) {}
    std::wstring nombreEspecial() const override { return L"MARTILLO DORADO"; }
    std::wstring descripcionEspecial() const override { return L"Repara a todo el equipo (+35 de vida)"; }
    bool especialCura() const override { return true; }
    double velocidadAlma() const override { return 62; }
    std::wstring ayudaEsquive() const override { return L"ESPACIO: SALTO (no recibe da\x00F1o un momento)"; }
    void usarHabilidad(double&, double&, int, int, std::vector<AtaqueTurbo>&) override {
        if (!puedeHabilidad()) return;
        cdHabilidad = 1.0;
        habilidad = 0.5;
    }
};
class CalhounCombate : public MiembroEquipo {
    bool conEscuadron;
public:
    explicit CalhounCombate(bool escuadron)
        : MiembroEquipo(L"CALHOUN", "02_Vanellope_y_personajes/Personajes_escala/calhoun", Color(255, 220, 120), escuadron ? 110 : 90, escuadron ? 18 : 10, 3),
        conEscuadron(escuadron) {}
    std::wstring nombreEspecial() const override { return conEscuadron ? L"R\x00C1" L"FAGA + SOLDADOS" : L"R\x00C1" L"FAGA"; }
    std::wstring descripcionEspecial() const override { return conEscuadron ? L"Fuego del escuadr\x00F3n: x2.6" : L"R\x00E1" L"faga (sin su escuadr\x00F3n): x1.8"; }
    double multiplicadorEspecial() const override { return conEscuadron ? 2.6 : 1.8; }
    double velocidadAlma() const override { return 64; }
    std::wstring ayudaEsquive() const override { return L"ESPACIO: DISPARO HACIA ARRIBA (destruye ataques)"; }
    void usarHabilidad(double& ax, double& ay, int, int, std::vector<AtaqueTurbo>& ataques) override {
        if (!puedeHabilidad()) return;
        cdHabilidad = 0.7;
        for (auto& a : ataques) if (a.vivo && a.tipo != 1 && std::fabs(a.x - ax) < 6 && a.y < ay) a.vivo = false;
    }
    void dibujar(Lienzo& l, int pies, double t) const override {
        if (conEscuadron && vivo()) {
            const Sprite* m = BancoSprites::instancia().obtener("04_Heros_Duty/Marines/fila_1/marine_fila_1_01");
            m->dibujar(l, (int)x - 8 - m->getAncho() / 2, pies - 10 - m->getAlto());
            m->dibujar(l, (int)x + 10 - m->getAncho() / 2, pies - 10 - m->getAlto());
        }
        MiembroEquipo::dibujar(l, pies, t);
    }
};
class VillanosCombate : public MiembroEquipo {
public:
    VillanosCombate() : MiembroEquipo(L"VILLANOS", "11_Bad-Anon_villanos_IA/Clyde/base/clyde_base_01", Color(255, 170, 60), 150, 15, 3) {}
    std::wstring nombreEspecial() const override { return L"SOMOS MALOS"; }
    std::wstring descripcionEspecial() const override { return L"Ataque de todos los villanos: x2"; }
    double velocidadAlma() const override { return 55; }
    std::wstring ayudaEsquive() const override { return L"ESPACIO: ESCUDO (no reciben da\x00F1o un momento)"; }
    void usarHabilidad(double&, double&, int, int, std::vector<AtaqueTurbo>&) override {
        if (!puedeHabilidad()) return;
        cdHabilidad = 1.6;
        habilidad = 0.8;
    }
    void dibujar(Lienzo& l, int pies, double t) const override {
        auto& b = BancoSprites::instancia();
        if (vivo()) {
            const Sprite* bowser = b.obtener("11_Bad-Anon_villanos_IA/Bowser/base/bowser_base_01");
            bowser->dibujar(l, (int)x + 14 - bowser->getAncho() / 2, pies - 8 - bowser->getAlto());
        }
        MiembroEquipo::dibujar(l, pies, t);
    }
};
class Batalla {
public:
    enum Fase { INTRO, MENU, BARRA, TURNO_TURBO, ESQUIVE, NUCLEO, DECISION, SOLO, COOPERACION, GANO, PERDIO };
private:
    Contexto& ctx;
    Fase fase;
    double tFase, t;
    std::vector<std::unique_ptr<MiembroEquipo>> equipo;
    int vidaTurbo, vidaTurboMax;
    int actual;
    int opcion;
    bool usandoEspecial;
    double marcador;
    double numeroDano; int ultimoDano; double xNumero;
    int objetivo, tipoAtaque, ataquesRestantes;
    double almaX, almaY, invAlma;
    int ultDx, ultDy;
    std::vector<AtaqueTurbo> ataques;
    double genAtaque;
    double glitchVanellope;
    struct Linea { std::wstring quien, texto; Retrato r; Color c; std::wstring voz; int aparece; };
    std::vector<Linea> cola;
    size_t enCola;
    CuadroDialogo dialogo;
    MenuEleccion eleccion;
    std::wstring aviso; double tAviso;
    std::vector<int> secuencia; size_t pasoSecuencia; double tPaso; bool falloSecuencia;
    int etapaCoop; double tCoop;
    double temblor, flash;
    int ronda;
    bool nucleoMostrado;
    RalphCombate& ralph() { return *static_cast<RalphCombate*>(equipo[0].get()); }
    const RalphCombate& ralph() const { return *static_cast<const RalphCombate*>(equipo[0].get()); }
    void irA(Fase f) { fase = f; tFase = 0; }
    void avisar(const std::wstring& s) { aviso = s; tAviso = 2.2; }
    int aliadosVivos() const { int n = 0; for (size_t i = 1; i < equipo.size(); i++) if (equipo[i]->vivo()) n++; return n; }
    void decir(const std::wstring& q, const std::wstring& txt, const Retrato& r, Color c, int aparece = -1, const std::wstring& voz = L"") {
        cola.push_back({ q, txt, r, c, voz, aparece });
    }
    bool procesarCola(double dt) {
        dialogo.actualizar(dt, ctx.audio, !ctx.audio.vozSonando());
        if (!dialogo.estaActivo()) {
            if (enCola >= cola.size()) return false;
            const Linea& ln = cola[enCola];
            dialogo.mostrar(ln.quien, ln.texto, ln.r, ln.c);
            if (!ln.voz.empty()) ctx.audio.reproducirVoz(ln.voz);
            if (ln.aparece >= 0 && ln.aparece < (int)equipo.size()) { aparecidos[ln.aparece] = true; ctx.audio.efecto(L"menu_aceptar.wav"); }
            return true;
        }
        if (ctx.teclado.aceptar()) {
            if (!dialogo.completo()) { dialogo.completar(); return true; }
            dialogo.ocultar();
            ctx.audio.detenerVoz();
            enCola++;
            return enCola < cola.size();
        }
        return true;
    }
    std::vector<bool> aparecidos;
    void siguienteMiembro() {
        do { actual++; } while (actual < (int)equipo.size() && !equipo[actual]->vivo());
        if (actual >= (int)equipo.size()) empezarTurnoTurbo();
        else { opcion = 0; irA(MENU); }
    }
    void empezarRonda() {
        ronda++;
        for (auto& m : equipo) m->pasarRonda();
        actual = -1;
        siguienteMiembro();
    }
    void empezarTurnoTurbo() {
        ataquesRestantes = equipo.size() >= 3 ? 2 : 1;
        irA(TURNO_TURBO);
        prepararAtaque();
    }
    void prepararAtaque() {
        std::vector<int> vivos;
        for (size_t i = 0; i < equipo.size(); i++) if (equipo[i]->vivo()) vivos.push_back((int)i);
        objetivo = vivos[Azar::entero(0, (int)vivos.size() - 1)];
        tipoAtaque = Azar::entero(0, 3);
        const wchar_t* nombres[4] = { L"\x00A1PROYECTILES DE DULCE!", L"\x00A1GOLPES TURBO!", L"\x00A1" L"CY-BUGS!", L"\x00A1GLITCHES!" };
        avisar(L"TURBO ATACA A " + equipo[objetivo]->getNombre() + L": " + nombres[tipoAtaque]);
        almaX = CajaEsquive::X + CajaEsquive::W / 2.0;
        almaY = CajaEsquive::Y + CajaEsquive::H / 2.0;
        ataques.clear();
        genAtaque = 0;
        invAlma = 0;
        ultDx = 0; ultDy = -1;
        ctx.audio.efecto(L"menu_error.wav");
    }
    double duracionEsquive() const { return glitchVanellope > 0 ? 2.6 : 4.2; }
    int danoGolpe() const { return 11 + (int)equipo.size(); }
    void actualizarEsquive(double dt) {
        MiembroEquipo& m = *equipo[objetivo];
        m.actualizarEsquive(dt);
        const Teclado& tk = ctx.teclado;
        int dx = (tk.mantenida(VK_LEFT) || tk.mantenida('A')) ? -1 : (tk.mantenida(VK_RIGHT) || tk.mantenida('D')) ? 1 : 0;
        int dy = (tk.mantenida(VK_UP) || tk.mantenida('W')) ? -1 : (tk.mantenida(VK_DOWN) || tk.mantenida('S')) ? 1 : 0;
        if (dx || dy) { ultDx = dx; ultDy = dy; }
        almaX += dx * m.velocidadAlma() * dt;
        almaY += dy * m.velocidadAlma() * dt;
        if (tk.presionada(VK_SPACE)) { m.usarHabilidad(almaX, almaY, ultDx, ultDy, ataques); ctx.audio.efecto(L"martillo.wav"); }
        almaX = std::max((double)CajaEsquive::X + 3, std::min((double)CajaEsquive::X + CajaEsquive::W - 4, almaX));
        almaY = std::max((double)CajaEsquive::Y + 3, std::min((double)CajaEsquive::Y + CajaEsquive::H - 4, almaY));
        if (invAlma > 0) invAlma -= dt;
        genAtaque -= dt;
        const double X0 = CajaEsquive::X, Y0 = CajaEsquive::Y, W = CajaEsquive::W, H = CajaEsquive::H;
        if (genAtaque <= 0 && tFase < duracionEsquive() - 0.6) {
            switch (tipoAtaque) {
            case 0: genAtaque = 0.22; ataques.push_back({ X0 + W, Y0 + Azar::real(4, H - 4), -Azar::real(60, 85), 0, 0, 0, true }); break;
            case 1: {
                genAtaque = 1.2;
                bool vertical = Azar::probabilidad(0.5);
                double hueco = vertical ? Y0 + Azar::real(4, H - 20) : X0 + Azar::real(4, W - 22);
                if (vertical) ataques.push_back({ X0, hueco, 48, 0, 0, 1, true });
                else ataques.push_back({ hueco, Y0, 0, 40, 0, 1, true });
                break;
            }
            case 2: {
                genAtaque = 0.9;
                int lado = Azar::entero(0, 3);
                double x = lado == 0 ? X0 + 2 : lado == 1 ? X0 + W - 2 : X0 + Azar::real(4, W - 4);
                double y = lado == 2 ? Y0 + 2 : lado == 3 ? Y0 + H - 2 : Y0 + Azar::real(4, H - 4);
                ataques.push_back({ x, y, 0, 0, 0, 2, true });
                break;
            }
            default: genAtaque = 0.35; ataques.push_back({ X0 + Azar::real(2, W - 14), Y0 + Azar::real(2, H - 14), 0, 0, 0, 3, true }); break;
            }
        }
        for (auto& a : ataques) {
            if (!a.vivo) continue;
            a.t += dt;
            if (a.tipo == 2) {
                double ddx = almaX - a.x, ddy = almaY - a.y, n = std::sqrt(ddx * ddx + ddy * ddy) + 0.01;
                a.x += ddx / n * 26 * dt; a.y += ddy / n * 26 * dt;
            }
            else { a.x += a.vx * dt; a.y += a.vy * dt + (a.tipo == 0 ? std::sin(a.t * 6) * 10 * dt : 0); }
            if (a.tipo == 3 && a.t > 1.1) a.vivo = false;
            if (a.x < X0 - 6 || a.x > X0 + W + 6 || a.y < Y0 - 6 || a.y > Y0 + H + 6) a.vivo = false;
            bool toca = false;
            if (a.tipo == 0 || a.tipo == 2) toca = std::fabs(a.x - almaX) < 4 && std::fabs(a.y - almaY) < 4;
            else if (a.tipo == 1) {
                bool vertical = a.vx != 0;
                double hueco = a.y;
                if (vertical) toca = std::fabs(a.x - almaX) < 3 && !(almaY > hueco && almaY < hueco + 16);
                else toca = std::fabs(a.y - almaY) < 3 && !(almaX > a.x && almaX < a.x + 18);
            }
            else if (a.tipo == 3) toca = a.t > 0.6 && almaX > a.x && almaX < a.x + 12 && almaY > a.y && almaY < a.y + 12;
            if (toca && invAlma <= 0 && !m.invulnerable()) {
                m.recibir(danoGolpe());
                invAlma = 0.6;
                temblor = 0.2;
                ctx.audio.efecto(L"herido.wav");
                if (a.tipo != 1 && a.tipo != 3) a.vivo = false;
            }
        }
        ataques.erase(std::remove_if(ataques.begin(), ataques.end(), [](const AtaqueTurbo& a) { return !a.vivo; }), ataques.end());
        if (!ralph().vivo()) {
            if (ralph().levantarse()) avisar(L"\x00A1RALPH SE LEVANTA! LE QUEDAN " + std::to_wstring(ralph().corazones) + L" CORAZONES");
            else { irA(PERDIO); ctx.audio.detenerMusica(); ctx.audio.efecto(L"gameover.wav"); return; }
        }
        if (tFase > duracionEsquive() || !m.vivo()) {
            if (!m.vivo() && objetivo != 0) avisar(m.getNombre() + L" CAY\x00D3...");
            ataquesRestantes--;
            if (ataquesRestantes > 0) { tFase = 0; prepararAtaque(); }
            else { glitchVanellope = 0; empezarRonda(); }
        }
    }
    void aplicarAtaque(double mult) {
        MiembroEquipo& m = *equipo[actual];
        if (usandoEspecial && m.especialCura()) {
            for (auto& o : equipo) o->curar(35);
            avisar(L"\x00A1" L"F\x00C9LIX REPARA AL EQUIPO! +35 DE VIDA");
            ctx.audio.efecto(L"ventana_reparada.wav");
            return;
        }
        double base = m.getAtaque() * (usandoEspecial ? m.multiplicadorEspecial() : 1.0);
        int dano = std::max(1, (int)(base * mult + 0.5));
        if (usandoEspecial && m.getNombre() == L"VANELLOPE") glitchVanellope = 1;
        vidaTurbo = std::max(0, vidaTurbo - dano);
        ultimoDano = dano; numeroDano = 1.0; xNumero = 185;
        temblor = 0.25; flash = 0.15;
        ctx.audio.efecto(usandoEspecial ? L"golpe_ralph.wav" : L"explosion.wav");
    }
    void revisarNucleo() {
        if (!nucleoMostrado && vidaTurbo <= vidaTurboMax / 10) {
            nucleoMostrado = true;
            cola.clear(); enCola = 0;
            decir(L"TURBO", L"\x00A1Si no puedo ser la estrella del juego, nadie lo ser\x00E1!", Retrato(SPR("03_Sugar_Rush/Turbo/identidad_pose/turbo_pose_01")), Color(255, 80, 60));
            decir(L"SISTEMA", L"EL N\x00DA" L"CLEO DE TURBO EMPIEZA A ABSORBER ENERG\x00CD" L"A PARA DESTRUIR TODO EL ARCADE...", Retrato(), Color(120, 255, 140));
            irA(NUCLEO);
        }
    }
    void dibujarFondo(Lienzo& l) const {
        l.degradadoVertical(0, 120, Color(30, 6, 30), Color(70, 14, 50));
        for (int y = l.yMin(); y < l.yMax(); y++)
            for (int x = l.xMin(); x < l.xMax(); x++) {
                unsigned h = (unsigned)(x * 73856093u ^ y * 19349663u ^ (unsigned)(t * 6) * 83492791u);
                if (h % 97 == 0) l.sombrear(x, y, Glifo::MEDIO, (h & 4) ? Color(255, 70, 220) : Color(80, 240, 255));
            }
        for (int y = 88; y < l.yMax(); y++)
            for (int x = l.xMin(); x < l.xMax(); x++)
                l.pixel(x, y, ((x / 6 + y / 3) % 2) ? Color(70, 26, 50) : Color(54, 18, 40));
        for (int x = l.xMin(); x < l.xMax(); x++) {
            int alto = 3 + (int)(3 * std::sin(x * 0.4 + t * 2));
            for (int j = 0; j < alto; j++) l.pixel(x, 88 - j + 2, Color(14, 6, 18));
        }
    }
    void dibujarTurbo(Lienzo& l) const {
        auto& b = BancoSprites::instancia();
        int sx = temblor > 0 ? (((int)(temblor * 40)) % 2 ? 2 : -2) : 0;
        const Sprite* bug = b.obtener("04_Heros_Duty/Cy-Bugs/reina_grande_2/cybug_reina_grande_2_0" + std::to_string(1 + ((int)(t * 9)) % 3));
        const Sprite* tur = b.obtener("03_Sugar_Rush/Turbo/identidad_brazos_arriba/turbo_brazos_0" + std::string(((int)(t * 2)) % 2 ? "2" : "1"));
        int cx = 190 + sx, base = 82 + (int)(std::sin(t * 2.5) * 3);
        if (flash > 0) { bug->dibujarSilueta(l, cx - bug->getAncho() / 2, base - bug->getAlto(), Colores::BLANCO, true); return; }
        bug->dibujarSombreado(l, cx - bug->getAncho() / 2 - 2, base - bug->getAlto(), Glifo::CLARO, Color(255, 60, 60), true);
        bug->dibujar(l, cx - bug->getAncho() / 2, base - bug->getAlto(), true);
        tur->dibujar(l, cx - tur->getAncho() / 2 + 3, base - bug->getAlto() - tur->getAlto() + 16, true);
        if (fase == NUCLEO || fase == DECISION || fase == SOLO || fase == COOPERACION) {
            int r = 6 + (int)(std::sin(t * 8) * 2);
            for (int j = -r; j <= r; j++) for (int i = -r; i <= r; i++) if (i * i + j * j <= r * r) l.pixel(cx + i, base - 30 + j, (i * i + j * j > r * r / 2) ? Color(255, 60, 220) : Colores::BLANCO);
            for (int k = 0; k < 10; k++) {
                double a = k * 0.63 + t * 3, d = 30 - std::fmod(t * 40 + k * 7, 30.0);
                l.sombrear(cx + (int)(std::cos(a) * d), base - 30 + (int)(std::sin(a) * d), Glifo::MEDIO, Color(80, 240, 255));
            }
        }
    }
    void dibujarEquipo(Lienzo& l) const {
        for (size_t i = 0; i < equipo.size(); i++) {
            if (!aparecidos[i]) continue;
            equipo[i]->dibujar(l, 90, t);
            if (fase == MENU && (int)i == actual && ((int)(t * 4)) % 2) {
                int x = (int)equipo[i]->x;
                for (int j = 0; j < 4; j++) for (int k = -j; k <= j; k++) l.pixel(x + k, 26 + j, Colores::AMARILLO);
            }
        }
    }
    void hud(Lienzo& l) const {
        int y0 = l.yMin();
        l.panelTranslucido(l.xMin(), y0, l.xMax() - l.xMin(), 18, Colores::NEGRO, Glifo::OSCURO);
        FuenteMini::texto(l, 2, y0 + 1, L"TURBO", Color(255, 80, 60));
        FuenteMini::texto(l, 28, y0 + 1, L"VIDA:", Color(220, 220, 235));
        dibujarBarra(l, 50, y0 + 3, 150, 4, (double)vidaTurbo / vidaTurboMax, Color(230, 50, 50), Color(50, 20, 20));
        FuenteMini::texto(l, 204, y0 + 1, std::to_wstring(vidaTurbo * 100 / vidaTurboMax) + L"%", Color(255, 120, 120));
        FuenteMini::texto(l, 2, y0 + 9, L"RALPH:", Color(255, 120, 90));
        for (int i = 0; i < 3; i++) FuenteMini::caracter(l, 30 + i * 6, y0 + 9, L'\x2665', i < ralph().corazones ? Color(255, 60, 80) : Color(80, 60, 70));
        FuenteMini::texto(l, 54, y0 + 9, L"ALIADOS:", Color(220, 220, 235));
        int x = 90;
        if (equipo.size() == 1) FuenteMini::texto(l, x, y0 + 9, L"NINGUNO", Color(160, 160, 170));
        for (size_t i = 1; i < equipo.size(); i++) {
            l.rectangulo(x, y0 + 11, 4, 4, equipo[i]->vivo() ? equipo[i]->getColor() : Color(70, 70, 80));
            std::wstring corto = equipo[i]->getNombre().substr(0, 3);
            FuenteMini::texto(l, x + 6, y0 + 9, corto, equipo[i]->vivo() ? equipo[i]->getColor() : Color(90, 90, 100));
            x += 8 + FuenteMini::medir(corto) + 6;
        }
        int yb = l.yMax() - 22;
        l.panelTranslucido(l.xMin(), yb, l.xMax() - l.xMin(), 22, Colores::NEGRO, Glifo::OSCURO);
        int ancho = (l.xMax() - l.xMin()) / (int)equipo.size();
        for (size_t i = 0; i < equipo.size(); i++) {
            const MiembroEquipo& m = *equipo[i];
            int bx = l.xMin() + 2 + (int)i * ancho;
            FuenteMini::texto(l, bx, yb + 1, m.getNombre(), m.vivo() ? m.getColor() : Color(90, 90, 100));
            int w = std::min(ancho - 6, 40);
            dibujarBarra(l, bx, yb + 9, w, 3, (double)m.getVida() / m.getVidaMax(), Color(90, 230, 90), Color(40, 40, 50));
            FuenteMini::texto(l, bx, yb + 13, std::to_wstring(m.getVida()) + L"/" + std::to_wstring(m.getVidaMax()), Colores::BLANCO);
        }
    }
    void dibujarMenu(Lienzo& l) const {
        const MiembroEquipo& m = *equipo[actual];
        int x0 = 4, y0 = l.yMax() - 22 - 46, w = 150, h = 45;
        l.panelTranslucido(x0, y0, w, h, Colores::NEGRO, Glifo::OSCURO);
        l.marco(x0, y0, w, h, Interfaz::DORADO);
        FuenteMini::texto(l, x0 + 4, y0 + 3, L"TURNO DE " + m.getNombre(), m.getColor());
        std::wstring ops[3] = { L"ATACAR", m.nombreEspecial() + (m.especialListo() ? L"" : L" (" + std::to_wstring(m.getCd()) + L")"), L"DEFENDER" };
        for (int i = 0; i < 3; i++) {
            Color c = i == opcion ? Colores::AMARILLO : (i == 1 && !m.especialListo() ? Color(110, 110, 120) : Colores::BLANCO);
            FuenteMini::texto(l, x0 + 10, y0 + 12 + i * 8, (i == opcion ? L"> " : L"  ") + ops[i], c);
        }
        std::wstring desc = opcion == 0 ? L"Golpe normal (precisi\x00F3n con ESPACIO)" : opcion == 1 ? m.descripcionEspecial() : L"Recibe la mitad del da\x00F1o";
        FuenteMini::texto(l, x0 + 4, y0 + 39, desc, Color(170, 200, 255));
    }
    void dibujarBarraAtaque(Lienzo& l) const {
        int x0 = 50, y0 = 52, w = 140;
        l.rectangulo(x0, y0, w, 12, Colores::NEGRO);
        l.marco(x0, y0, w, 12, Colores::BLANCO);
        for (int i = 1; i < w - 1; i++) {
            double d = std::fabs((double)i / w - 0.5);
            Color c = d < 0.06 ? Color(255, 230, 90) : d < 0.2 ? Color(120, 220, 120) : Color(70, 90, 140);
            l.sombrear(x0 + i, y0 + 4, Glifo::OSCURO, c); l.sombrear(x0 + i, y0 + 7, Glifo::OSCURO, c);
        }
        int mx = x0 + (int)(marcador * w);
        l.rectangulo(mx - 1, y0 - 2, 3, 16, Colores::BLANCO);
        FuenteMini::centrado(l, y0 + 16, L"\x00A1PRESIONA ESPACIO EN EL CENTRO!", Colores::AMARILLO);
    }
    void dibujarCaja(Lienzo& l) const {
        const int X0 = CajaEsquive::X, Y0 = CajaEsquive::Y, W = CajaEsquive::W, H = CajaEsquive::H;
        l.rectangulo(X0, Y0, W, H, Colores::NEGRO);
        l.marco(X0 - 1, Y0 - 1, W + 2, H + 2, Colores::BLANCO);
        l.marco(X0 - 2, Y0 - 2, W + 4, H + 4, Colores::BLANCO);
        for (const auto& a : ataques) {
            switch (a.tipo) {
            case 0: l.rectangulo((int)a.x - 1, (int)a.y - 1, 3, 3, Color(255, 120, 200)); l.pixel((int)a.x, (int)a.y, Colores::BLANCO); break;
            case 1:
                if (a.vx != 0) { for (int j = Y0; j < Y0 + H; j++) if (!(j > a.y && j < a.y + 16)) { l.pixel((int)a.x, j, Color(255, 80, 60)); l.pixel((int)a.x + 1, j, Color(255, 160, 60)); } }
                else { for (int i = X0; i < X0 + W; i++) if (!(i > a.x && i < a.x + 18)) { l.pixel(i, (int)a.y, Color(255, 80, 60)); l.pixel(i, (int)a.y + 1, Color(255, 160, 60)); } }
                break;
            case 2: l.rectangulo((int)a.x - 2, (int)a.y - 1, 5, 3, Color(80, 200, 160)); l.pixel((int)a.x - 2, (int)a.y + 2, Color(40, 120, 90)); l.pixel((int)a.x + 2, (int)a.y + 2, Color(40, 120, 90)); break;
            default:
                for (int j = 0; j < 12; j++) for (int i = 0; i < 12; i++) {
                    if (a.t < 0.6) { if (((int)(a.t * 12)) % 2) l.sombrear((int)a.x + i, (int)a.y + j, Glifo::CLARO, Color(255, 70, 220)); }
                    else l.pixel((int)a.x + i, (int)a.y + j, ((i + j) % 3) ? Color(80, 240, 255) : Color(255, 70, 220));
                }
                break;
            }
        }
        const MiembroEquipo& m = *equipo[objetivo];
        if (!(invAlma > 0 && ((int)(invAlma * 16)) % 2)) {
            Color c = m.invulnerable() ? Colores::BLANCO : m.getColor();
            const char* forma[5] = { ".#.#.", "#####", "#####", ".###.", "..#.." };
            for (int j = 0; j < 5; j++) for (int i = 0; i < 5; i++) if (forma[j][i] == '#') l.pixel((int)almaX - 2 + i, (int)almaY - 2 + j, c);
        }
        FuenteMini::centrado(l, Y0 + H + 4, m.ayudaEsquive(), Color(200, 200, 220));
        FuenteMini::centrado(l, Y0 - 9, m.getNombre() + L"  " + std::to_wstring(m.getVida()) + L"/" + std::to_wstring(m.getVidaMax()), m.getColor());
    }
    void dibujarSecuencia(Lienzo& l) const {
        l.panelTranslucido(l.xMin(), 26, l.xMax() - l.xMin(), 40, Colores::NEGRO, Glifo::OSCURO);
        FuenteMini::centrado(l, 28, L"\x00A1GOLPEA EL N\x00DA" L"CLEO! PRESIONA LAS TECLAS A TIEMPO", Colores::AMARILLO);
        const wchar_t simbolos[5] = { L'\x2191', L'\x2193', L'\x2190', L'\x2192', L'_' };
        int x0 = 120 - (int)secuencia.size() * 10;
        for (size_t i = 0; i < secuencia.size(); i++) {
            int x = x0 + (int)i * 20;
            Color c = i < pasoSecuencia ? Color(90, 230, 90) : i == pasoSecuencia ? Colores::AMARILLO : Color(160, 160, 170);
            l.marco(x, 38, 16, 14, c);
            if (secuencia[i] == 4) FuenteMini::texto(l, x + 2, 42, L"ESP", c);
            else FuenteMini::caracter(l, x + 6, 42, simbolos[secuencia[i]], c);
        }
        if (pasoSecuencia < secuencia.size()) dibujarBarra(l, 70, 58, 100, 3, 1.0 - tPaso / 1.1, Color(255, 120, 90), Color(50, 30, 30));
    }
    void dibujarCooperacion(Lienzo& l) const {
        auto& b = BancoSprites::instancia();
        l.panelTranslucido(l.xMin(), l.yMin(), l.xMax() - l.xMin(), l.yMax() - l.yMin(), Colores::NEGRO, Glifo::OSCURO);
        const Sprite* tur = b.obtener("03_Sugar_Rush/Turbo/identidad_pose/turbo_pose_01");
        tur->dibujar(l, 120 - tur->getAncho() / 2, 0);
        for (int j = 44; j < 52; j++) if (((j + (int)(t * 20)) % 3) == 0) for (int i = -30; i <= 30; i += 4) l.sombrear(120 + i + (j % 2) * 2, j, Glifo::MEDIO, Color(255, 240, 90));
        const char* nombres[4] = { "Personajes_escala/calhoun", "Personajes_escala/felix", "Vanellope/fila_1/vanellope_f1_01", "Personajes_escala/ralph" };
        const bool hay[4] = { tiene(L"CALHOUN"), tiene(L"F\x00C9LIX"), tiene(L"VANELLOPE"), true };
        int y = 60;
        for (int k = 0; k < 4; k++) {
            if (!hay[k]) continue;
            const Sprite* s = b.obtener(std::string("02_Vanellope_y_personajes/") + nombres[k]);
            int px = 120 + (k % 2 ? 30 : -30);
            int py = y + 20;
            if (k == 3) { px = 120; py = 128 - (etapaCoop >= 4 ? (int)std::min(80.0, tCoop * 80) : 0); }
            s->dibujar(l, px - s->getAncho() / 2, py - s->getAlto());
            if (k == 1 && etapaCoop == 1 && ((int)(t * 8)) % 2) l.sombrear(px + 10, py - 30, Glifo::BLOQUE, Color(255, 230, 90));
            if (k == 0 && etapaCoop == 2) for (int j = py - 50; j > 40; j -= 4) l.pixel(px + 2, j - (int)(t * 60) % 4, Color(255, 240, 160));
            if (k == 2 && etapaCoop == 3) s->dibujarSombreado(l, px - s->getAncho() / 2 + 2, py - s->getAlto(), Glifo::MEDIO, Color(80, 240, 255));
            y += 10;
        }
        if (etapaCoop >= 5) for (int i = 0; i < 40; i++) {
            double a = i * 0.157 + t, d = tCoop * 120;
            l.sombrear(120 + (int)(std::cos(a) * d), 30 + (int)(std::sin(a) * d * 0.6), Glifo::BLOQUE, i % 2 ? Color(255, 240, 120) : Colores::BLANCO);
        }
        const wchar_t* textos[6] = { L"", L"F\x00C9LIX REPARA LA PLATAFORMA", L"CALHOUN DESTRUYE LAS DEFENSAS", L"VANELLOPE ABRE EL C\x00D3" L"DIGO", L"\x00A1RALPH SALTA!", L"\x00A1SOY RALPH! \x00A1Y ESTE ES MI ARCADE!" };
        if (etapaCoop >= 1) ctx.fuentes.amarilla.dibujarCentrado(l, 100, textos[std::min(5, etapaCoop)]);
    }
    bool tiene(const std::wstring& n) const {
        for (size_t i = 1; i < equipo.size(); i++) if (equipo[i]->getNombre() == n && equipo[i]->vivo()) return true;
        return false;
    }
public:
    Batalla(Contexto& c) : ctx(c), fase(INTRO), tFase(0), t(0), vidaTurbo(0), vidaTurboMax(0), actual(0), opcion(0), usandoEspecial(false), marcador(0),
        numeroDano(0), ultimoDano(0), xNumero(0), objetivo(0), tipoAtaque(0), ataquesRestantes(0), almaX(0), almaY(0), invAlma(0), ultDx(0), ultDy(-1),
        genAtaque(0), glitchVanellope(0), enCola(0), tAviso(0), pasoSecuencia(0), tPaso(0), falloSecuencia(false), etapaCoop(0), tCoop(0),
        temblor(0), flash(0), ronda(0), nucleoMostrado(false) {
        using namespace Personajes;
        const Progreso& p = ctx.progreso;
        int rep = p.getReputacion();
        equipo.push_back(std::make_unique<RalphCombate>());
        if (rep >= 1) equipo.push_back(std::make_unique<VanellopeCombate>());
        if (rep >= 2) equipo.push_back(std::make_unique<FelixCombate>());
        if (rep >= 3) equipo.push_back(std::make_unique<CalhounCombate>(p.escuadronSalvado()));
        if (rep >= 4) equipo.push_back(std::make_unique<VillanosCombate>());
        for (size_t i = 0; i < equipo.size(); i++) equipo[i]->x = 24 + (double)i * 32;
        aparecidos.assign(equipo.size(), false);
        aparecidos[0] = true;
        int total = 0;
        for (auto& m : equipo) total += m->getVidaMax();
        vidaTurboMax = vidaTurbo = std::max(200, (int)(total * 1.1));
        Retrato rTurbo(SPR("03_Sugar_Rush/Turbo/identidad_pose/turbo_pose_01"));
        Color cs(120, 255, 140);
        decir(L"SISTEMA", L"NIVEL SECRETO: EL REINICIO DEL C\x00D3" L"DIGO", Retrato(), cs);
        decir(L"SISTEMA", L"OBJETIVO FINAL. NUEVO OBJETIVO: DETENER A TURBO. IMPEDIR LA DESTRUCCI\x00D3N DEL ARCADE. RECUPERAR EL JUEGO.", Retrato(), cs);
        Retrato rV(SPR("02_Vanellope_y_personajes/Vanellope/fila_1/vanellope_f1_01")), rF(SPR("02_Vanellope_y_personajes/Personajes_escala/felix")),
            rC(SPR("02_Vanellope_y_personajes/Personajes_escala/calhoun")), rR = ralphArcade();
        if (rep <= 0) decir(L"SISTEMA", L"Nadie vino a ayudar a Ralph. Tendr\x00E1 que enfrentar a Turbo solo.", Retrato(), cs);
        int idx = 1;
        if (rep >= 1) {
            decir(L"VANELLOPE", L"\x00A1No est\x00E1s solo en esta, Ralph! \x00A1" L"Ataqu\x00E9moslo juntos a este usurpador!", rV, Color(80, 230, 255), idx++);
            decir(L"RALPH", L"Vanellope...", rR, C_RALPH);
        }
        if (rep >= 2) {
            decir(L"F\x00C9LIX", L"No te olvides de m\x00ED, Ralph. Regresemos juntos al juego, amigo...", rF, C_FELIX, idx++);
            decir(L"RALPH", L"F\x00E9lix...", rR, C_RALPH);
        }
        if (rep >= 3) {
            if (p.escuadronSalvado()) {
                decir(L"CALHOUN", L"\x00A1Ralph! \x00A1" L"Esta vez no te dejar\x00E9 solo! \x00A1Soldados, \x00FAnanse!", rC, Color(255, 220, 120), idx++);
                decir(L"SOLDADOS", L"\x00A1S\x00CD!", Retrato(SPR("04_Heros_Duty/Marines/fila_1/marine_fila_1_01")), Color(150, 230, 150));
            }
            else decir(L"CALHOUN", L"Esto es parte de mi culpa... Hoy tendr\x00E9 que dejar nuestros asuntos de lado.", rC, Color(255, 220, 120), idx++);
        }
        if (rep >= 4) {
            decir(L"VILLANOS", L"\x00A1Ralph! \x00A1Te ayudaremos porque somos malos, y eso no es malo!", clyde(), C_CLYDE, idx++);
            decir(L"RALPH", L"Chicos...", rR, C_RALPH);
        }
        decir(L"TURBO", L"\x00A1Veamos de qu\x00E9 est\x00E1n hechos, perdedores!", rTurbo, Color(255, 80, 60));
    }
    void actualizar(double dt) {
        t += dt;
        tFase += dt;
        if (temblor > 0) temblor -= dt;
        if (flash > 0) flash -= dt;
        if (numeroDano > 0) numeroDano -= dt;
        if (tAviso > 0) tAviso -= dt;
        const Teclado& tk = ctx.teclado;
        if (tk.presionada(VK_F9) && (fase == MENU || fase == BARRA || fase == TURNO_TURBO || fase == ESQUIVE)) {
            vidaTurbo = std::max(1, vidaTurboMax / 10);
            ataques.clear();
            revisarNucleo();
            return;
        }
        switch (fase) {
        case INTRO:
            if (tFase > 0.6 && !procesarCola(dt)) { for (size_t i = 0; i < aparecidos.size(); i++) aparecidos[i] = true; empezarRonda(); }
            break;
        case MENU: {
            MiembroEquipo& m = *equipo[actual];
            if (tk.presionada(VK_UP) || tk.presionada('W')) { opcion = (opcion + 2) % 3; ctx.audio.efecto(L"menu_mover.wav"); }
            if (tk.presionada(VK_DOWN) || tk.presionada('S')) { opcion = (opcion + 1) % 3; ctx.audio.efecto(L"menu_mover.wav"); }
            if (tk.aceptar() && tFase > 0.2) {
                if (opcion == 1 && !m.especialListo()) { ctx.audio.efecto(L"menu_error.wav"); break; }
                ctx.audio.efecto(L"menu_aceptar.wav");
                if (opcion == 2) { m.defendiendo = true; avisar(m.getNombre() + L" SE DEFIENDE"); siguienteMiembro(); break; }
                usandoEspecial = opcion == 1;
                if (usandoEspecial) m.usarEspecialCd();
                if (usandoEspecial && m.especialCura()) { aplicarAtaque(1); siguienteMiembro(); break; }
                marcador = 0;
                irA(BARRA);
            }
            break;
        }
        case BARRA:
            marcador += dt / 1.1;
            if (tk.presionada(VK_SPACE) || tk.aceptar() || marcador >= 1) {
                double d = std::fabs(marcador - 0.5);
                double mult = marcador >= 1 ? 0.3 : std::max(0.4, 2.0 - d * 4.0);
                aplicarAtaque(mult);
                revisarNucleo();
                if (fase == BARRA) siguienteMiembro();
            }
            break;
        case TURNO_TURBO:
            if (tFase > 1.4) irA(ESQUIVE);
            break;
        case ESQUIVE:
            actualizarEsquive(dt);
            break;
        case NUCLEO:
            if (!procesarCola(dt)) {
                bool bloqueado = equipo.size() == 1 || aliadosVivos() == 0;
                eleccion.mostrar(L"DECISI\x00D3N FINAL: \x00BFQU\x00C9 HAR\x00C1 RALPH?", {
                    L"Lanzarse al n\x00FA" L"cleo solo",
                    bloqueado ? L"Llamar a todo el equipo (BLOQUEADO: no tienes aliados en pie)" : L"Llamar a todo el equipo" });
                irA(DECISION);
            }
            break;
        case DECISION: {
            int r = eleccion.manejar(ctx, dt);
            if (r < 0) break;
            bool bloqueado = equipo.size() == 1 || aliadosVivos() == 0;
            if (r == 1 && bloqueado) {
                ctx.audio.efecto(L"menu_error.wav");
                eleccion.mostrar(L"DECISI\x00D3N FINAL: \x00BFQU\x00C9 HAR\x00C1 RALPH?", { L"Lanzarse al n\x00FA" L"cleo solo", L"Llamar a todo el equipo (BLOQUEADO: no tienes aliados en pie)" });
                break;
            }
            ctx.progreso.decidir("nucleo", r + 1);
            cola.clear(); enCola = 0;
            if (r == 0) {
                decir(L"RALPH", L"No necesito que nadie me diga qu\x00E9 debo hacer.", Personajes::ralphArcade(), Personajes::C_RALPH);
                secuencia.clear();
                for (int i = 0; i < 8; i++) secuencia.push_back(Azar::entero(0, 4));
                pasoSecuencia = 0; tPaso = 0; falloSecuencia = false;
                irA(SOLO);
            }
            else {
                if (tiene(L"F\x00C9LIX")) decir(L"F\x00C9LIX", L"\x00A1Listo para reparar!", Retrato(SPR("02_Vanellope_y_personajes/Personajes_escala/felix")), Personajes::C_FELIX);
                if (tiene(L"CALHOUN")) decir(L"CALHOUN", L"\x00A1" L"Cobertura asegurada!", Retrato(SPR("02_Vanellope_y_personajes/Personajes_escala/calhoun")), Color(255, 220, 120));
                if (tiene(L"VANELLOPE")) decir(L"VANELLOPE", L"\x00A1Lista calista!", Retrato(SPR("02_Vanellope_y_personajes/Vanellope/fila_1/vanellope_f1_01")), Color(80, 230, 255));
                if (tiene(L"VILLANOS")) decir(L"VILLANOS", L"\x00A1Somos malos, y eso es bueno!", Personajes::clyde(), Personajes::C_CLYDE);
                decir(L"RALPH", L"Entonces... \x00A1TODOS JUNTOS!", Personajes::ralphArcade(), Personajes::C_RALPH);
                etapaCoop = 0; tCoop = 0;
                irA(COOPERACION);
            }
            break;
        }
        case SOLO: {
            if (procesarCola(dt)) break;
            if (pasoSecuencia >= secuencia.size()) {
                if (tFase > 0) { vidaTurbo = 0; temblor = 1.0; ctx.audio.efecto(L"golpe_ralph.wav"); irA(GANO); }
                break;
            }
            tPaso += dt;
            int esperado = secuencia[pasoSecuencia];
            int pulsado = -1;
            if (tk.presionada(VK_UP) || tk.presionada('W')) pulsado = 0;
            if (tk.presionada(VK_DOWN) || tk.presionada('S')) pulsado = 1;
            if (tk.presionada(VK_LEFT) || tk.presionada('A')) pulsado = 2;
            if (tk.presionada(VK_RIGHT) || tk.presionada('D')) pulsado = 3;
            if (tk.presionada(VK_SPACE)) pulsado = 4;
            if (tk.presionada(VK_F9)) pulsado = esperado;
            if (pulsado == esperado) { pasoSecuencia++; tPaso = 0; temblor = 0.15; flash = 0.08; ctx.audio.efecto(L"golpe_ralph.wav"); }
            else if (pulsado >= 0 || tPaso > 1.1) {
                falloSecuencia = true;
                ctx.audio.detenerMusica();
                ctx.audio.efecto(L"gameover.wav");
                irA(PERDIO);
            }
            break;
        }
        case COOPERACION:
            if (procesarCola(dt)) break;
            tCoop += dt;
            if (tCoop > 1.6) {
                etapaCoop++; tCoop = 0;
                if (etapaCoop == 1) ctx.audio.efecto(L"martillo.wav");
                if (etapaCoop == 2) ctx.audio.efecto(L"disparo.wav");
                if (etapaCoop == 3) ctx.audio.efecto(L"destello.wav");
                if (etapaCoop == 4) ctx.audio.efecto(L"salto.wav");
                if (etapaCoop == 5) { ctx.audio.efecto(L"golpe_ralph.wav"); temblor = 1.2; vidaTurbo = 0; }
                if (etapaCoop == 6) { ctx.audio.efecto(L"etapa.wav"); irA(GANO); }
            }
            break;
        default: break;
        }
    }
    void dibujar(Lienzo& l) {
        l.sinCamara();
        dibujarFondo(l);
        dibujarTurbo(l);
        dibujarEquipo(l);
        if (numeroDano > 0) ctx.fuentes.amarilla.dibujar(l, 176, 30 - (int)((1 - numeroDano) * 8), L"-" + std::to_wstring(ultimoDano));
        hud(l);
        if (fase == MENU) dibujarMenu(l);
        if (fase == BARRA) dibujarBarraAtaque(l);
        if (fase == ESQUIVE) dibujarCaja(l);
        if (fase == SOLO && !dialogo.estaActivo()) dibujarSecuencia(l);
        if (fase == COOPERACION && !dialogo.estaActivo()) dibujarCooperacion(l);
        if (tAviso > 0 && fase != COOPERACION) {
            int w = FuenteMini::medir(aviso) + 10;
            l.panelTranslucido(120 - w / 2, 20, w, 9, Colores::NEGRO, Glifo::OSCURO);
            FuenteMini::centrado(l, 21, aviso, Colores::AMARILLO);
        }
        if (fase == GANO) {
            for (int y = l.yMin(); y < l.yMax(); y++) for (int x = l.xMin(); x < l.xMax(); x++)
                if (((x + y + (int)(tFase * 60)) % 7) < std::min(7.0, tFase * 4)) l.sombrear(x, y, Glifo::CLARO, Color(255, 240, 250));
            ctx.fuentes.amarilla.dibujarCentrado(l, 46, L"\x00A1TURBO DERROTADO!");
            FuenteMini::centrado(l, 60, L"LA CORRUPCI\x00D3N COMIENZA A DESAPARECER...", Colores::BLANCO);
        }
        if (fase == PERDIO) {
            l.panelTranslucido(l.xMin(), 36, l.xMax() - l.xMin(), 36, Colores::NEGRO, Glifo::OSCURO);
            ctx.fuentes.roja.dibujarCentrado(l, 40, L"GAME OVER");
            FuenteMini::centrado(l, 56, falloSecuencia ? L"EL ARCADE SE GLITCHE\x00D3... RALPH FUE CONSUMIDO." : L"RALPH NO PUDO LEVANTARSE OTRA VEZ.", Colores::BLANCO);
        }
        dialogo.dibujar(l);
        eleccion.dibujar(l);
    }
    bool gano() const { return fase == GANO && tFase > 3.0; }
    bool perdio() const { return fase == PERDIO && tFase > 3.0; }
};
