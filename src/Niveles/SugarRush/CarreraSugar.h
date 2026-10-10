#pragma once
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include "../../Escenas/Escena.h"
#include "../../Motor/FuenteMini.h"
#include "../../Motor/Efectos.h"
#include "../../Motor/Azar.h"
#include "../../Historia/InterfazHistoria.h"
namespace SpritesSR {
    const std::string S = "03_Sugar_Rush/";
    const std::string V = "02_Vanellope_y_personajes/";
}
namespace PistaSR {
    const double LARGO_VUELTA = 2600;
    const int VUELTAS = 3;
    const double MEDIO_ANCHO = 46;
    const int Y_JUGADOR = 104;
    inline double centro(double d) { return 120 + 22 * std::sin(d / 260.0) + 8 * std::sin(d / 97.0 + 1.3); }
}
class CorredorSR {
public:
    std::string nombre;
    std::string sprite;
    Color color;
    double d, x, vel, objetivoVel, carril;
    double golpe;
    bool jugador;
    CorredorSR(const std::string& n, const std::string& s, Color c, double d0, double x0, bool j)
        : nombre(n), sprite(s), color(c), d(d0), x(x0), vel(0), objetivoVel(160), carril(0), golpe(0), jugador(j) {}
    int vuelta() const { return std::min(PistaSR::VUELTAS, (int)(d / PistaSR::LARGO_VUELTA) + 1); }
    double avance() const { return std::min(1.0, d / (PistaSR::LARGO_VUELTA * PistaSR::VUELTAS)); }
    void dibujar(Lienzo& l, double dJugador, double t, bool glitch) const {
        int sy = PistaSR::Y_JUGADOR - (int)(d - dJugador);
        if (sy < l.yMin() - 40 || sy > l.yMax() + 40) return;
        auto& b = BancoSprites::instancia();
        bool dos = sprite.find("vanellope") != std::string::npos || sprite.find("rancis") != std::string::npos;
        const Sprite* s = b.obtener(sprite + ((dos && ((int)(t * (4 + vel / 30))) % 2) ? "02" : "01"));
        int sx = (int)x - s->getAncho() / 2 + (golpe > 0 ? (((int)(golpe * 30)) % 2 ? 2 : -2) : 0);
        for (int i = -10; i <= 10; i++) l.sombrear((int)x + i, sy, Glifo::MEDIO, Colores::NEGRO);
        if (glitch) {
            int dx = ((int)(t * 20)) % 3 - 1;
            s->dibujarSombreado(l, sx - 3 + dx, sy - s->getAlto() + 1, Glifo::MEDIO, Color(80, 240, 255));
            s->dibujarSombreado(l, sx + 3 - dx, sy - s->getAlto() + 1, Glifo::MEDIO, Color(255, 70, 220));
            if (((int)(t * 12)) % 3) s->dibujar(l, sx, sy - s->getAlto() + 1);
        }
        else s->dibujar(l, sx, sy - s->getAlto() + 1);
    }
};
class ObstaculoSR {
protected:
    double d, offset;
    double ancho, fondo;
    bool activo;
public:
    ObstaculoSR(double dd, double off, double w, double h) : d(dd), offset(off), ancho(w), fondo(h), activo(true) {}
    virtual ~ObstaculoSR() {}
    double getD() const { return d; }
    double x() const { return PistaSR::centro(d) + offset; }
    bool estaActivo() const { return activo; }
    void quitar() { activo = false; }
    bool toca(double px, double pd) const {
        return activo && std::fabs(px - x()) < ancho / 2 + 6 && pd + 4 > d - fondo && pd - 6 < d;
    }
    virtual bool esZonaGlitch() const { return false; }
    virtual bool esGigante() const { return false; }
    virtual std::wstring nombre() const = 0;
    virtual void dibujar(Lienzo& l, int sx, int sy, double t) const = 0;
};
inline void sombraObstaculo(Lienzo& l, int sx, int sy, int w) {
    for (int i = -w / 2 - 2; i <= w / 2 + 2; i++) { l.pixel(sx + i, sy, Color(70, 40, 20)); l.sombrear(sx + i, sy + 1, Glifo::MEDIO, Color(40, 20, 10)); }
}
class BarreraChocolate : public ObstaculoSR {
public:
    BarreraChocolate(double dd, double off) : ObstaculoSR(dd, off, 30, 8) {}
    std::wstring nombre() const override { return L"BARRERA DE CHOCOLATE"; }
    void dibujar(Lienzo& l, int sx, int sy, double t) const override {
        sombraObstaculo(l, sx, sy, 30);
        l.rectangulo(sx - 15, sy - 8, 30, 8, Color(92, 52, 26));
        for (int i = 0; i < 30; i += 6) { l.rectangulo(sx - 15 + i, sy - 8, 5, 3, Color(130, 78, 40)); l.pixel(sx - 15 + i, sy - 1, Color(60, 30, 14)); }
        for (int i = 0; i < 30; i++) l.pixel(sx - 15 + i, sy - 9, Color(160, 100, 56));
    }
};
class DonaSR : public ObstaculoSR {
public:
    DonaSR(double dd, double off) : ObstaculoSR(dd, off, 20, 14) {}
    std::wstring nombre() const override { return L"DONA"; }
    void dibujar(Lienzo& l, int sx, int sy, double t) const override {
        sombraObstaculo(l, sx, sy, 20);
        for (int j = -8; j <= 8; j++) for (int i = -11; i <= 11; i++) {
            int r = i * i + j * j * 2;
            if (r > 121 || r < 12) continue;
            Color c;
            if (r > 96 || r < 20) c = Color(60, 20, 40);
            else if (j < 1) c = ((i * 3 + j * 5) % 7 == 0) ? Color(255, 255, 140) : ((i * 5 + j) % 9 == 0 ? Color(90, 220, 255) : Color(255, 60, 160));
            else c = Color(200, 130, 60);
            l.pixel(sx + i, sy - 9 + j, c);
        }
        if (((int)(t * 4)) % 2) l.pixel(sx - 5, sy - 14, Colores::BLANCO);
    }
};
class BloqueoSR : public ObstaculoSR {
public:
    BloqueoSR(double dd, double off) : ObstaculoSR(dd, off, 22, 10) {}
    std::wstring nombre() const override { return L"BLOQUEO DE PISTA"; }
    void dibujar(Lienzo& l, int sx, int sy, double t) const override {
        sombraObstaculo(l, sx, sy, 22);
        l.rectangulo(sx - 11, sy - 10, 22, 10, Color(200, 200, 215));
        for (int i = 0; i < 22; i += 4) l.rectangulo(sx - 11 + i, sy - 10, 2, 10, Color(230, 60, 80));
        l.marco(sx - 12, sy - 11, 24, 12, Color(40, 40, 60));
    }
};
class ZonaGlitchSR : public ObstaculoSR {
public:
    ZonaGlitchSR(double dd, double off) : ObstaculoSR(dd, off, 34, 16) {}
    bool esZonaGlitch() const override { return true; }
    std::wstring nombre() const override { return L"ZONA GLITCHEADA"; }
    void dibujar(Lienzo& l, int sx, int sy, double t) const override {
        sombraObstaculo(l, sx, sy, 34);
        l.rectangulo(sx - 17, sy - 16, 34, 16, Color(40, 6, 60));
        for (int j = 0; j < 16; j += 2) {
            int corr = ((j * 7 + (int)(t * 25)) % 9) - 4;
            Color c = ((j / 2 + (int)(t * 8)) % 2) ? Color(255, 40, 200) : Color(40, 240, 255);
            for (int i = -14; i <= 14; i++) if (((i + corr) & 7) < 5) l.pixel(sx + i + corr, sy - 15 + j, c);
        }
        Color borde = ((int)(t * 6)) % 2 ? Colores::BLANCO : Color(255, 230, 60);
        l.marco(sx - 18, sy - 17, 36, 18, borde);
        const char* rayo[7] = { "..##", ".##.", "####", ".##.", "##..", "#...", "...." };
        for (int j = 0; j < 7; j++) for (int i = 0; i < 4; i++) if (rayo[j][i] == '#') l.pixel(sx - 2 + i, sy - 23 + j, Color(255, 230, 60));
    }
};
class GiganteSR : public ObstaculoSR {
public:
    GiganteSR(double dd) : ObstaculoSR(dd, 0, 2 * PistaSR::MEDIO_ANCHO + 10, 24) {}
    bool esGigante() const override { return true; }
    std::wstring nombre() const override { return L"OBST\x00C1" L"CULO GIGANTE"; }
    void dibujar(Lienzo& l, int sx, int sy, double t) const override {
        int w = (int)ancho;
        l.rectangulo(sx - w / 2, sy - 24, w, 24, Color(80, 44, 22));
        for (int j = 0; j < 24; j += 6) for (int i = 0; i < w; i += 10)
            l.rectangulo(sx - w / 2 + i + (j / 6 % 2) * 5, sy - 24 + j, 8, 4, Color(118, 70, 36));
        for (int i = 0; i < w; i++) l.pixel(sx - w / 2 + i, sy - 25, Color(255, 120, 170));
        for (int i = 0; i < w; i += 3) l.sombrear(sx - w / 2 + i, sy - 27, Glifo::MEDIO, Color(255, 240, 250));
    }
};
class CarreraSugar {
public:
    enum Modo { HISTORIA, INFINITO };
    enum Fase { BRIEFING, CUENTA, CORRIENDO, EVENTO, META, PERDIO };
private:
    Contexto& ctx;
    Modo modo;
    Fase fase;
    double tFase, t;
    std::vector<CorredorSR> corredores;
    std::vector<std::unique_ptr<ObstaculoSR>> obstaculos;
    int integridad;
    double glitch;
    double carga;
    double invulnerable;
    double avisoChoque;
    std::wstring textoChoque;
    double avisoGlitch;
    bool ultimaVuelta;
    bool eventoHecho;
    int eleccionEvento;
    int subEvento;
    bool glitchExito;
    double tEvento;
    double ralphX;
    CuadroDialogo dialogo;
    MenuEleccion eleccion;
    int puntos, record;
    int posicionFinal;
    bool continuar;
    int dificultad;
    CorredorSR& yo() { return corredores[0]; }
    const CorredorSR& yo() const { return corredores[0]; }
    static std::wstring rutaRecord() { return Rutas::en(L"Guardado\\record_sugarrush.txt"); }
    int leerRecord() const { std::ifstream f{ std::filesystem::path(rutaRecord()) }; int r = 0; if (f) f >> r; return r; }
    void guardarRecord() const {
        std::error_code ec;
        std::filesystem::create_directories(std::filesystem::path(Rutas::en(L"Guardado")), ec);
        std::ofstream f{ std::filesystem::path(rutaRecord()) };
        if (f) f << record;
    }
    void irA(Fase f) { fase = f; tFase = 0; }
    void preparar() {
        corredores.clear();
        const std::string K = SpritesSR::S + "Corredores/";
        corredores.emplace_back("VANELLOPE", K + "Vanellope/kart_atras/vanellope_kart_atras_", Color(80, 230, 255), 0, PistaSR::centro(0) + 12, true);
        struct R { const char* n; const char* s; Color c; };
        R rivales[5] = { { "TAFFYTA", "Taffyta/kart_atras/taffyta_kart_atras_", Color(255, 120, 200) }, { "RANCIS", "Rancis/kart_atras/rancis_kart_atras_", Color(255, 220, 80) },
                         { "CANDLEHEAD", "Candlehead/kart_atras/candlehead_kart_atras_", Color(120, 160, 255) }, { "JUBILEENA", "Jubileena/kart_atras/jubileena_kart_atras_", Color(255, 90, 90) },
                         { "ADORABEEZLE", "Adorabeezle/kart_atras/adorabeezle_kart_atras_", Color(120, 255, 200) } };
        const double ds[5] = { 46, 46, 0, -24, -24 };
        const double xs[5] = { -18, 18, -12, -18, 18 };
        for (int i = 0; i < 5; i++) {
            corredores.emplace_back(rivales[i].n, K + rivales[i].s, rivales[i].c, ds[i], PistaSR::centro(ds[i]) + xs[i], false);
            corredores.back().objetivoVel = 150 + Azar::real(0, 22) + dificultad * 10;
            corredores.back().carril = xs[i];
        }
        corredores[0].d = 0;
        obstaculos.clear();
        double d = 260;
        double fin = PistaSR::LARGO_VUELTA * PistaSR::VUELTAS - 120;
        while (d < fin) {
            int vuelta = (int)(d / PistaSR::LARGO_VUELTA);
            double off = Azar::real(-PistaSR::MEDIO_ANCHO + 14, PistaSR::MEDIO_ANCHO - 14);
            int tipo = Azar::entero(0, 9);
            if (tipo < 3) obstaculos.push_back(std::make_unique<BarreraChocolate>(d, off));
            else if (tipo < 6) obstaculos.push_back(std::make_unique<DonaSR>(d, off));
            else if (tipo < 8) obstaculos.push_back(std::make_unique<BloqueoSR>(d, off));
            else obstaculos.push_back(std::make_unique<ZonaGlitchSR>(d, off));
            d += Azar::real(150, 230) - vuelta * 25 - dificultad * 8;
        }
        if (modo == HISTORIA) {
            double mitad = PistaSR::LARGO_VUELTA * PistaSR::VUELTAS / 2;
            obstaculos.erase(std::remove_if(obstaculos.begin(), obstaculos.end(), [&](const std::unique_ptr<ObstaculoSR>& o) {
                return std::fabs(o->getD() - mitad) < 140; }), obstaculos.end());
            obstaculos.push_back(std::make_unique<GiganteSR>(mitad));
        }
        integridad = 100;
        glitch = 0; carga = 1; invulnerable = 0; avisoChoque = 0; avisoGlitch = 0;
        ultimaVuelta = false; eventoHecho = false; eleccionEvento = 0; subEvento = 0; glitchExito = false; tEvento = 0;
        posicionFinal = 0;
    }
    int posicion() const {
        int p = 1;
        for (size_t i = 1; i < corredores.size(); i++) if (corredores[i].d > yo().d) p++;
        return p;
    }
    void chocar(const std::wstring& que, int dano) {
        if (invulnerable > 0) return;
        integridad -= dano;
        if (integridad < 0) integridad = 0;
        yo().vel *= 0.35;
        yo().golpe = 0.5;
        invulnerable = 1.0;
        avisoChoque = 1.2;
        textoChoque = que;
        ctx.audio.efecto(L"herido.wav");
    }
    void actualizarJugador(double dt) {
        const Teclado& tk = ctx.teclado;
        CorredorSR& v = yo();
        bool acelera = tk.mantenida(VK_UP) || tk.mantenida('W');
        bool frena = tk.mantenida(VK_DOWN) || tk.mantenida('S');
        int dir = (tk.mantenida(VK_LEFT) || tk.mantenida('A')) ? -1 : (tk.mantenida(VK_RIGHT) || tk.mantenida('D')) ? 1 : 0;
        double centro = PistaSR::centro(v.d);
        bool fuera = std::fabs(v.x - centro) > PistaSR::MEDIO_ANCHO - 4;
        double vmax = glitch > 0 ? 270 : (fuera ? 85 : 195);
        if (acelera) v.vel += 80 * dt;
        else if (frena) v.vel -= 150 * dt;
        else v.vel -= 25 * dt;
        if (v.vel > vmax) v.vel -= std::min(v.vel - vmax, 120 * dt);
        if (v.vel < 0) v.vel = 0;
        v.x += dir * (36 + v.vel * 0.32) * dt;
        v.x = std::max(centro - PistaSR::MEDIO_ANCHO - 18, std::min(centro + PistaSR::MEDIO_ANCHO + 18, v.x));
        v.d += v.vel * dt;
        if (v.golpe > 0) v.golpe -= dt;
        if (glitch > 0) { glitch -= dt; if (glitch <= 0) { glitch = 0; carga = 0; avisoGlitch = 1.5; } }
        else if (carga < 1) carga = std::min(1.0, carga + dt / 8.0);
        if (tk.presionada(VK_SPACE) && glitch <= 0 && carga >= 1) {
            glitch = 3.0;
            v.vel = std::max(v.vel, 160.0);
            ctx.audio.efecto(L"destello.wav");
        }
    }
    void actualizarRivales(double dt) {
        for (size_t i = 1; i < corredores.size(); i++) {
            CorredorSR& r = corredores[i];
            if (r.golpe > 0) r.golpe -= dt;
            r.vel += (r.objetivoVel - r.vel) * std::min(1.0, dt * 0.8);
            for (auto& o : obstaculos) {
                if (!o->estaActivo() || o->esGigante()) continue;
                double falta = o->getD() - r.d;
                if (falta > 0 && falta < 70 && std::fabs(o->x() - (PistaSR::centro(r.d) + r.carril)) < 22)
                    r.carril = r.carril > 0 ? r.carril - 30 : r.carril + 30;
            }
            r.carril = std::max(-PistaSR::MEDIO_ANCHO + 10, std::min(PistaSR::MEDIO_ANCHO - 10, r.carril));
            double objetivoX = PistaSR::centro(r.d) + r.carril;
            r.x += (objetivoX - r.x) * std::min(1.0, dt * 3);
            r.d += r.vel * dt;
        }
    }
    void colisiones() {
        CorredorSR& v = yo();
        for (auto& o : obstaculos) {
            if (!o->toca(v.x, v.d)) continue;
            if (o->esGigante()) continue;
            if (glitch > 0) { if (o->esZonaGlitch()) v.vel += 20; continue; }
            o->quitar();
            chocar(o->nombre(), 20);
        }
        for (size_t i = 1; i < corredores.size(); i++) {
            CorredorSR& r = corredores[i];
            if (std::fabs(r.d - v.d) < 14 && std::fabs(r.x - v.x) < 16 && glitch <= 0 && invulnerable <= 0) {
                chocar(L"OTRO CORREDOR: " + std::wstring(r.nombre.begin(), r.nombre.end()), 10);
                r.golpe = 0.5;
                r.x += (r.x > v.x ? 10 : -10);
            }
        }
    }
    void actualizarEvento(double dt) {
        tEvento += dt;
        dialogo.actualizar(dt, ctx.audio, !ctx.audio.vozSonando());
        if (eleccionEvento == 0) {
            if (tEvento > 0.8 && !eleccion.estaActivo())
                eleccion.mostrar(L"\x00A1UN OBST\x00C1" L"CULO GIGANTE BLOQUEA LA PISTA! \x00BFQU\x00C9 HACEN?", {
                    L"Ralph destruye el obst\x00E1" L"culo",
                    L"Vanellope usa el glitch (puede fallar)" });
            int r = eleccion.manejar(ctx, dt);
            if (r >= 0) {
                eleccionEvento = r + 1;
                subEvento = 0;
                tEvento = 0;
                glitchExito = Azar::probabilidad(0.7);
                if (eleccionEvento == 1) {
                    dialogo.mostrar(L"RALPH", L"\x00A1VOY A DEMOLERLO! Terminaremos esta carrera como d\x00E9 lugar...",
                        Retrato(SPR("01_Fix-It_Felix_Jr/Ralph/fila_1/ralph_f1_04"), SPR("01_Fix-It_Felix_Jr/Ralph/fila_1/ralph_f1_05")), Color(255, 120, 90));
                    ctx.audio.reproducirVoz(L"ralph_demolerlo.wav");
                }
                else dialogo.mostrar(L"VANELLOPE", L"Creo que puedo hacerlo.", Retrato(SPR(SpritesSR::V + "Vanellope/fila_1/vanellope_f1_01")), Color(80, 230, 255));
            }
            return;
        }
        if (dialogo.estaActivo()) {
            if (ctx.teclado.aceptar()) {
                if (!dialogo.completo()) { dialogo.completar(); return; }
                dialogo.ocultar();
                ctx.audio.detenerVoz();
                subEvento++;
                tEvento = 0;
                if (eleccionEvento == 2 && subEvento == 1)
                    dialogo.mostrar(L"RALPH", L"\x00A1" L"Entonces hazlo!", Retrato(SPR("01_Fix-It_Felix_Jr/Ralph/fila_1/ralph_f1_01")), Color(255, 120, 90));
            }
            return;
        }
        GiganteSR* g = nullptr;
        for (auto& o : obstaculos) if (o->esGigante() && o->estaActivo()) g = static_cast<GiganteSR*>(o.get());
        if (eleccionEvento == 1) {
            ralphX += (PistaSR::centro(yo().d) - ralphX) * std::min(1.0, 0.1 + tEvento * 0.05);
            if (tEvento > 0.9 && g) { g->quitar(); temblor = 0.6; ctx.audio.efecto(L"golpe_ralph.wav"); }
            if (tEvento > 2.0) { irA(CORRIENDO); eventoHecho = true; }
        }
        else {
            if (tEvento < 0.1) { glitch = 2.2; ctx.audio.efecto(L"destello.wav"); }
            if (glitchExito) {
                yo().d += 80 * (1.0 / 30);
                if (tEvento > 1.6 && g) g->quitar();
                if (tEvento > 2.4) { irA(CORRIENDO); eventoHecho = true; avisoChoque = 0; avisoExito = 2.5; }
            }
            else if (tEvento > 1.2) {
                integridad = 0;
                ctx.audio.efecto(L"golpe_ralph.wav");
                irA(PERDIO);
                ctx.audio.detenerMusica();
            }
        }
    }
    double temblor = 0, avisoExito = 0;
    void dibujarPista(Lienzo& l) const {
        const CorredorSR& v = yo();
        int sacudir = temblor > 0 ? (((int)(temblor * 40)) % 2 ? 2 : -2) : 0;
        for (int y = l.yMin(); y < l.yMax(); y++) {
            double d = v.d + (PistaSR::Y_JUGADOR - y);
            double c = PistaSR::centro(d) + sacudir;
            int id = (int)std::floor(d);
            bool meta = d > 0 && std::fmod(d, PistaSR::LARGO_VUELTA) < 8;
            for (int x = l.xMin(); x < l.xMax(); x++) {
                double dx = x - c;
                Color col;
                double ad = std::fabs(dx);
                unsigned h = (unsigned)(x * 73856093u ^ (unsigned)id * 19349663u);
                if (ad <= PistaSR::MEDIO_ANCHO) {
                    if (meta) col = (((x / 4) + (id / 4)) % 2) ? Colores::BLANCO : Colores::NEGRO;
                    else {
                        double borde = ad / PistaSR::MEDIO_ANCHO;
                        if (borde > 0.86) col = Color(200, 140, 62);
                        else col = (((x / 8) + (id / 6)) % 2) ? Color(242, 200, 98) : Color(232, 184, 82);
                        if (h % 97 == 0) col = Color(110, 60, 30);
                    }
                }
                else if (ad <= PistaSR::MEDIO_ANCHO + 3) col = ((id / 4) % 2) ? Color(230, 40, 60) : Colores::BLANCO;
                else {
                    col = (((x / 10 + id / 8) % 2) ? Color(246, 162, 188) : Color(240, 152, 180));
                    if (h % 113 == 0) col = Color(255, 236, 244);
                }
                l.pixel(x, y, col);
            }
        }
        auto& b = BancoSprites::instancia();
        const char* props[8] = { "Escenario/bastones/baston_alto_1", "Props_mundo/paletas/paleta_rosa_grande", "Escenario/gomitas/gomita_verde", "Props_mundo/paletas/paleta_verde_grande",
                                 "Escenario/bastones/baston_grande", "Escenario/gomitas/gomita_azul", "Escenario/paletas/paleta_morada", "Escenario/gomitas/gomita_rosa" };
        int primero = (int)std::floor((v.d - 40) / 36.0);
        for (int k = primero; k < primero + 6; k++) {
            double d = k * 36.0;
            int sy = PistaSR::Y_JUGADOR - (int)(d - v.d);
            for (int lado = -1; lado <= 1; lado += 2) {
                unsigned h = (unsigned)(k * 2654435761u + (lado + 2) * 97u);
                const Sprite* s = b.obtener(SpritesSR::S + props[h % 8]);
                double px = PistaSR::centro(d) + lado * (PistaSR::MEDIO_ANCHO + 14 + (h >> 8) % 30);
                s->dibujar(l, (int)px - s->getAncho() / 2, sy - s->getAlto());
            }
        }
    }
    void dibujarMundo(Lienzo& l) {
        dibujarPista(l);
        const CorredorSR& v = yo();
        for (auto& o : obstaculos) {
            if (!o->estaActivo()) continue;
            int sy = PistaSR::Y_JUGADOR - (int)(o->getD() - v.d);
            if (sy < l.yMin() - 30 || sy > l.yMax() + 10) continue;
            o->dibujar(l, (int)o->x(), sy, t);
        }
        std::vector<const CorredorSR*> orden;
        for (auto& c : corredores) orden.push_back(&c);
        std::sort(orden.begin(), orden.end(), [](const CorredorSR* a, const CorredorSR* b) { return a->d > b->d; });
        for (auto* c : orden) {
            if (c->jugador && invulnerable > 0 && ((int)(invulnerable * 12)) % 2 && glitch <= 0) continue;
            c->dibujar(l, v.d, t, c->jugador && glitch > 0);
        }
        if (fase == EVENTO && eleccionEvento == 1 && !dialogo.estaActivo()) {
            const Sprite* r = BancoSprites::instancia().obtener(std::string("01_Fix-It_Felix_Jr/Ralph/fila_2/ralph_f2_0") + (((int)(t * 6)) % 2 ? "8" : "9"));
            int gy = PistaSR::Y_JUGADOR - (int)(PistaSR::LARGO_VUELTA * PistaSR::VUELTAS / 2 - v.d);
            r->dibujar(l, (int)ralphX - 32, gy - 60);
        }
        if (glitch > 0)
            for (int y = l.yMin(); y < l.yMax(); y += 7)
                if (((y + (int)(t * 30)) % 3) == 0)
                    for (int x = l.xMin(); x < l.xMax(); x += 2) l.sombrear(x, y, Glifo::CLARO, ((x / 10) % 2) ? Color(80, 240, 255) : Color(255, 70, 220));
    }
    void barra(Lienzo& l, int x, int y, int w, double f, Color c) const {
        int llenos = (int)(f * w + 0.5);
        for (int i = 0; i < w; i++) {
            if (i < llenos) l.pixel(x + i, y, c);
            else l.sombrear(x + i, y, Glifo::CLARO, Color(150, 150, 170));
            if (i < llenos) l.pixel(x + i, y + 1, c); else l.sombrear(x + i, y + 1, Glifo::CLARO, Color(150, 150, 170));
        }
    }
    void hud(Lienzo& l) const {
        const int ANCHO = 44;
        const int xI = l.xMin(), xD = l.xMax() - ANCHO, y0 = l.yMin();
        l.panelTranslucido(xI, y0, ANCHO, l.yMax() - y0, Colores::NEGRO, Glifo::OSCURO);
        l.panelTranslucido(xD, y0, ANCHO, l.yMax() - y0, Colores::NEGRO, Glifo::OSCURO);
        Color texto(220, 220, 235), am = Colores::AMARILLO;
        const Progreso& p = ctx.progreso;
        int x = xI + 2, y = y0 + 2;
        FuenteMini::texto(l, x, y, L"[INFO]", Color(120, 255, 140)); y += 9;
        FuenteMini::texto(l, x, y, L"VELOCIDAD", texto); y += 7;
        FuenteMini::texto(l, x, y, std::to_wstring((int)(yo().vel * 1.1)) + L" KM/H", am); y += 10;
        FuenteMini::texto(l, x, y, L"VEH\x00CD" L"CULO", texto); y += 7;
        Color ci = integridad >= 60 ? Color(90, 230, 90) : integridad >= 30 ? Color(240, 210, 60) : Color(240, 60, 60);
        barra(l, x, y + 1, 38, integridad / 100.0, ci); y += 4;
        FuenteMini::texto(l, x, y, std::to_wstring(integridad) + L"%", ci); y += 10;
        if (modo == HISTORIA) {
            FuenteMini::texto(l, x, y, L"REPUTACI\x00D3N", texto); y += 7;
            FuenteMini::texto(l, x, y, p.etiquetaReputacion(), am); y += 9;
            FuenteMini::texto(l, x, y, L"EMPAT\x00CD" L"A", texto); y += 7;
            FuenteMini::texto(l, x, y, p.etiquetaEmpatia(), Color(255, 150, 190)); y += 9;
            FuenteMini::texto(l, x, y, L"NIVELES", texto); y += 7;
            FuenteMini::texto(l, x, y, std::to_wstring(p.getNivelesCompletados()) + L"/3", am);
        }
        else {
            wchar_t num[16];
            swprintf_s(num, L"%06d", record);
            FuenteMini::texto(l, x, y, L"R\x00C9" L"CORD", texto); y += 7;
            FuenteMini::texto(l, x, y, num, Color(255, 120, 120)); y += 9;
            swprintf_s(num, L"%06d", puntos);
            FuenteMini::texto(l, x, y, L"PUNTOS", texto); y += 7;
            FuenteMini::texto(l, x, y, num, am);
        }
        x = xD + 2; y = y0 + 2;
        FuenteMini::texto(l, x, y, L"POSICI\x00D3N", texto); y += 7;
        FuenteMini::texto(l, x, y, std::to_wstring(posicion()) + L"/6", am); y += 9;
        FuenteMini::texto(l, x, y, L"VUELTA", texto); y += 7;
        FuenteMini::texto(l, x, y, std::to_wstring(yo().vuelta()) + L"/" + std::to_wstring(PistaSR::VUELTAS), am); y += 9;
        FuenteMini::texto(l, x, y, L"PROGRESO", texto); y += 7;
        barra(l, x, y + 1, 38, yo().avance(), Color(255, 120, 200)); y += 4;
        FuenteMini::texto(l, x, y, std::to_wstring((int)(yo().avance() * 100)) + L"%", Color(255, 150, 210)); y += 9;
        FuenteMini::texto(l, x, y, L"GLITCH", texto); y += 7;
        bool lleno = carga >= 1 && glitch <= 0;
        barra(l, x, y + 1, 38, glitch > 0 ? glitch / 3.0 : carga, glitch > 0 ? Color(255, 70, 220) : Color(80, 230, 255)); y += 4;
        if (lleno && ((int)(t * 3)) % 2) FuenteMini::texto(l, x, y, L"[ESPACIO]", Color(80, 230, 255));
        y += 9;
        int mx = xD + 22, my = y + 13, rx = 17, ry = 11;
        for (int k = 0; k < 64; k++) {
            double a = k / 64.0 * 6.2832;
            l.pixel(mx + (int)(std::cos(a) * rx), my + (int)(std::sin(a) * ry), Color(240, 196, 92));
        }
        for (size_t i = corredores.size(); i-- > 0;) {
            const CorredorSR& c = corredores[i];
            double a = std::fmod(c.d / PistaSR::LARGO_VUELTA, 1.0) * 6.2832 - 1.5708;
            int px = mx + (int)(std::cos(a) * rx), py = my + (int)(std::sin(a) * ry);
            if (c.jugador) {
                if (((int)(t * 4)) % 2 == 0) { l.rectangulo(px - 1, py - 1, 3, 3, Colores::BLANCO); l.pixel(px, py, c.color); }
                FuenteMini::texto(l, mx - 2, my - 2, L"V", c.color);
            }
            else l.rectangulo(px - 1, py, 2, 2, c.color);
        }
    }
    void dibujarBriefing(Lienzo& l) const {
        l.limpiar(Color(40, 14, 30));
        for (int y = l.yMin(); y < l.yMax(); y += 3) for (int x = l.xMin(); x < l.xMax(); x++) l.sombrear(x, y, Glifo::CLARO, Color(120, 40, 90));
        auto& b = BancoSprites::instancia();
        const Sprite* logo = b.obtener(SpritesSR::S + "UI/logo_sugar_rush");
        logo->dibujar(l, (l.getAncho() - logo->getAncho()) / 2, 0);
        Interfaz::panel(l, 6, 32, 228, 86, true);
        FuenteMini::texto(l, 12, 36, L"[SISTEMA]  OBJETIVO:", Color(120, 255, 140));
        FuenteMini::texto(l, 16, 44, L"> PARTICIPA EN LA CARRERA    > LLEGA A LA META", Colores::BLANCO);
        FuenteMini::texto(l, 16, 51, L"> SOBREVIVE A LOS OBST\x00C1" L"CULOS  > USA EL GLITCH", Colores::BLANCO);
        FuenteMini::texto(l, 12, 61, L"CONTROLES:", Color(150, 220, 255));
        FuenteMini::texto(l, 16, 69, L"\x2191/W ACELERAR   \x2193/S FRENAR   [ESPACIO] GLITCH", Colores::BLANCO);
        FuenteMini::texto(l, 16, 76, L"\x2190/A GIRAR IZQ.  \x2192/D GIRAR DER.  [ESC] PAUSA", Colores::BLANCO);
        FuenteMini::texto(l, 12, 86, L"GLITCH: atraviesa obst\x00E1" L"culos y zonas glitch.", Color(255, 150, 220));
        FuenteMini::texto(l, 12, 93, L"Cada choque quita integridad. Con 0% pierdes.", Color(255, 150, 150));
        if (((int)(t * 2)) % 2 == 0) FuenteMini::centrado(l, 109, L"PRESIONA X O ENTER PARA IR A LA PARRILLA", Color(120, 255, 140));
        const Sprite* v = b.obtener(SpritesSR::S + "Corredores/Vanellope/kart_frente/vanellope_kart_frente_0" + std::string(((int)(t * 4)) % 2 ? "2" : "1"));
        v->dibujar(l, 200, 2);
    }
public:
    CarreraSugar(Contexto& c, Modo m, int dif = 0)
        : ctx(c), modo(m), fase(BRIEFING), tFase(0), t(0), integridad(100), glitch(0), carga(1), invulnerable(0), avisoChoque(0),
        avisoGlitch(0), ultimaVuelta(false), eventoHecho(false), eleccionEvento(0), subEvento(0), glitchExito(false), tEvento(0), ralphX(260),
        puntos(0), record(0), posicionFinal(0), continuar(false), dificultad(dif) {
        record = leerRecord();
        preparar();
        if (!Audio::existeMusica(L"sugar_rush_fondo.wav")) ctx.audio.reproducirMusica(L"sugar_rush.wav");
        else ctx.audio.reproducirMusica(L"sugar_rush_fondo.wav");
    }
    ~CarreraSugar() { if (modo == INFINITO) guardarRecord(); }
    void actualizar(double dt) {
        t += dt;
        tFase += dt;
        if (temblor > 0) temblor -= dt;
        if (avisoChoque > 0) avisoChoque -= dt;
        if (avisoGlitch > 0) avisoGlitch -= dt;
        if (avisoExito > 0) avisoExito -= dt;
        if (invulnerable > 0) invulnerable -= dt;
        switch (fase) {
        case BRIEFING:
            if (tFase > 0.8 && ctx.teclado.aceptar()) { ctx.audio.efecto(L"menu_aceptar.wav"); irA(CUENTA); }
            break;
        case CUENTA:
            if ((int)(tFase) != (int)(tFase - dt) && tFase < 3.5) ctx.audio.efecto(tFase < 3.0 ? L"menu_mover.wav" : L"inicio.wav");
            if (tFase > 3.6) irA(CORRIENDO);
            break;
        case CORRIENDO: {
            actualizarJugador(dt);
            actualizarRivales(dt);
            colisiones();
            if (ctx.teclado.presionada(VK_F9)) yo().d += PistaSR::LARGO_VUELTA;
            if (!ultimaVuelta && yo().vuelta() == PistaSR::VUELTAS) {
                ultimaVuelta = true;
                ctx.audio.efecto(L"etapa.wav");
                if (Audio::existeMusica(L"sugar_rush_ultima.wav")) ctx.audio.reproducirMusica(L"sugar_rush_ultima.wav");
            }
            if (modo == HISTORIA && !eventoHecho)
                for (auto& o : obstaculos)
                    if (o->esGigante() && o->estaActivo() && o->getD() - yo().d < 70) {
                        yo().vel = 0;
                        ralphX = 260;
                        irA(EVENTO);
                        tEvento = 0;
                        break;
                    }
            if (integridad <= 0) { irA(PERDIO); ctx.audio.detenerMusica(); ctx.audio.efecto(L"gameover.wav"); break; }
            if (yo().d >= PistaSR::LARGO_VUELTA * PistaSR::VUELTAS) {
                posicionFinal = posicion();
                puntos += 1000 + (7 - posicionFinal) * 200 + integridad * 5;
                if (puntos > record) record = puntos;
                ctx.audio.efecto(L"etapa.wav");
                irA(META);
            }
            break;
        }
        case EVENTO:
            actualizarEvento(dt);
            break;
        case META:
            yo().vel *= (1 - std::min(1.0, dt * 2));
            yo().d += yo().vel * dt;
            actualizarRivales(dt);
            if (modo == INFINITO && tFase > 2.5) {
                if (ctx.teclado.aceptar()) { continuar = true; ctx.audio.efecto(L"menu_aceptar.wav"); dificultad++; preparar(); irA(CUENTA); }
            }
            break;
        default: break;
        }
    }
    void dibujar(Lienzo& l) {
        l.sinCamara();
        if (fase == BRIEFING) { dibujarBriefing(l); return; }
        dibujarMundo(l);
        hud(l);
        if (fase == CUENTA) {
            int n = 3 - (int)tFase;
            if (n > 0) {
                const Sprite* s = BancoSprites::instancia().obtener(SpritesSR::S + "UI/numeros/num_" + std::to_string(n));
                for (int k = 0; k < 3; k++) s->dibujar(l, 120 - s->getAncho() / 2 + k - 1, 40);
            }
            else ctx.fuentes.amarilla.dibujarCentrado(l, 44, L"\x00A1YA!");
        }
        if (avisoChoque > 0 && fase == CORRIENDO) {
            ctx.fuentes.roja.dibujarCentrado(l, 20, L"\x00A1" L"COLISI\x00D3N!");
            FuenteMini::centrado(l, 30, textoChoque + L"  -  INTEGRIDAD: " + std::to_wstring(integridad) + L"%", Colores::BLANCO);
        }
        if (glitch > 0 && fase == CORRIENDO && ((int)(t * 6)) % 2) ctx.fuentes.azul.dibujarCentrado(l, 20, L"GLITCH ACTIVADO");
        if (avisoGlitch > 0) FuenteMini::centrado(l, 40, L"GLITCH ENFRI\x00C1NDOSE...", Color(80, 230, 255));
        if (avisoExito > 0) { FuenteMini::centrado(l, 30, L"[SISTEMA] HABILIDAD UTILIZADA CON \x00C9XITO.", Color(120, 255, 140)); }
        if (ultimaVuelta && fase == CORRIENDO && yo().d < PistaSR::LARGO_VUELTA * 2 + 300 && ((int)(t * 3)) % 2)
            ctx.fuentes.amarilla.dibujarCentrado(l, 40, L"\x00DALTIMA VUELTA");
        if (fase == EVENTO && eleccionEvento == 2 && !dialogo.estaActivo()) {
            ctx.fuentes.azul.dibujarCentrado(l, 20, L"GLITCH");
            if (!glitchExito && tEvento > 0.7) ctx.fuentes.roja.dibujarCentrado(l, 34, L"\x00A1" L"EL GLITCH FALL\x00D3!");
        }
        if (fase == META) {
            l.panelTranslucido(l.xMin() + 44, 30, l.xMax() - l.xMin() - 88, 44, Colores::NEGRO, Glifo::OSCURO);
            ctx.fuentes.amarilla.dibujarCentrado(l, 34, L"\x00A1META!");
            FuenteMini::centrado(l, 46, L"POSICI\x00D3N FINAL: " + std::to_wstring(posicionFinal) + L"/6     +1000 PTS", Colores::BLANCO);
            if (modo == INFINITO && tFase > 2.5) FuenteMini::centrado(l, 58, L"ENTER: OTRAS 3 VUELTAS   ESC: SALIR", Color(120, 255, 140));
            for (int i = 0; i < 40; i++) {
                int px = (i * 53 + (int)(tFase * 40 * (1 + i % 3))) % 150 + 45, py = (i * 31 + (int)(tFase * 60)) % 120 - 7;
                l.sombrear(px, py, Glifo::BLOQUE, i % 3 == 0 ? Color(255, 120, 200) : i % 3 == 1 ? Color(120, 230, 255) : Color(255, 230, 90));
            }
        }
        if (fase == PERDIO) {
            l.panelTranslucido(l.xMin() + 44, 40, l.xMax() - l.xMin() - 88, 30, Colores::NEGRO, Glifo::OSCURO);
            ctx.fuentes.roja.dibujarCentrado(l, 44, L"VEH\x00CD" L"CULO DESTRUIDO");
            FuenteMini::centrado(l, 58, L"HAS PERDIDO.", Colores::BLANCO);
        }
        dialogo.dibujar(l);
        eleccion.dibujar(l);
    }
    bool termino() const { return fase == META && tFase > 3.5 && modo == HISTORIA; }
    bool perdio() const { return fase == PERDIO && tFase > 3.0; }
    bool enMeta() const { return fase == META; }
    int getEleccionEvento() const { return eleccionEvento; }
    bool exitoGlitch() const { return glitchExito; }
    int getPuntos() const { return puntos; }
    int getPosicion() const { return posicionFinal; }
};
