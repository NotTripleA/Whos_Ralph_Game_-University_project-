#pragma once
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include "../../Entidades/PersonajeAnimado.h"
#include "../../Motor/Azar.h"
namespace SpritesHD {
    const std::string H = "04_Heros_Duty/";
    inline Animacion anim(const std::string& carpeta, const std::string& prefijo, int desde, int hasta, double fps, bool bucle = true) {
        return Animacion(BancoSprites::instancia().secuencia(H + carpeta + "/" + prefijo, desde, hasta), fps, bucle);
    }
    inline Animacion lista(const std::string& prefijo, std::initializer_list<int> cuadros, double fps, bool bucle = true) {
        return Animacion(BancoSprites::instancia().lista(H + prefijo, std::vector<int>(cuadros)), fps, bucle);
    }
}
enum class DirHD { ARRIBA, ABAJO, IZQUIERDA, DERECHA };
inline int dxDe(DirHD d) { return d == DirHD::IZQUIERDA ? -1 : d == DirHD::DERECHA ? 1 : 0; }
inline int dyDe(DirHD d) { return d == DirHD::ARRIBA ? -1 : d == DirHD::ABAJO ? 1 : 0; }
class MapaHD {
public:
    static const int CELDA = 20;
private:
    std::vector<std::string> filas;
    int ancho, alto;
public:
    MapaHD() : ancho(0), alto(0) {}
    void cargar(const std::vector<std::string>& f) {
        filas = f;
        alto = (int)filas.size();
        ancho = 0;
        for (const auto& s : filas) ancho = std::max(ancho, (int)s.size());
        for (auto& s : filas) while ((int)s.size() < ancho) s += '#';
    }
    int getAncho() const { return ancho; }
    int getAlto() const { return alto; }
    int anchoPx() const { return ancho * CELDA; }
    int altoPx() const { return alto * CELDA; }
    char en(int c, int r) const { return (c < 0 || r < 0 || c >= ancho || r >= alto) ? '#' : filas[r][c]; }
    void poner(int c, int r, char k) { if (c >= 0 && r >= 0 && c < ancho && r < alto) filas[r][c] = k; }
    bool pared(int c, int r) const { return en(c, r) == '#'; }
    bool bloquea(int c, int r) const { char k = en(c, r); return k == '#' || k == 'C' || k == 'N' || k == 'M' || k == 'R'; }
    bool buscar(char k, int& c, int& r) const {
        for (int j = 0; j < alto; j++) for (int i = 0; i < ancho; i++) if (filas[j][i] == k) { c = i; r = j; return true; }
        return false;
    }
    std::vector<std::pair<int, int>> todas(char k) const {
        std::vector<std::pair<int, int>> v;
        for (int j = 0; j < alto; j++) for (int i = 0; i < ancho; i++) if (filas[j][i] == k) v.push_back({ i, j });
        return v;
    }
    static double centroX(int c) { return c * CELDA + CELDA / 2.0; }
    static double piesY(int r) { return r * CELDA + CELDA - 3.0; }
    static int celdaDeX(double x) { return (int)std::floor(x / CELDA); }
    static int celdaDeY(double y) { return (int)std::floor(y / CELDA); }
    void distancias(const std::vector<std::pair<int, int>>& origenes, std::vector<int>& d) const {
        d.assign(ancho * alto, -1);
        std::queue<std::pair<int, int>> q;
        for (auto& o : origenes) {
            if (o.first < 0 || o.second < 0 || o.first >= ancho || o.second >= alto) continue;
            if (d[o.second * ancho + o.first] == 0) continue;
            d[o.second * ancho + o.first] = 0;
            q.push(o);
        }
        const int dc[4] = { 1, -1, 0, 0 }, dr[4] = { 0, 0, 1, -1 };
        while (!q.empty()) {
            auto p = q.front(); q.pop();
            int base = d[p.second * ancho + p.first];
            for (int k = 0; k < 4; k++) {
                int nc = p.first + dc[k], nr = p.second + dr[k];
                if (bloquea(nc, nr) || d[nr * ancho + nc] >= 0) continue;
                d[nr * ancho + nc] = base + 1;
                q.push({ nc, nr });
            }
        }
    }
    int distancia(const std::vector<int>& d, int c, int r) const {
        if (c < 0 || r < 0 || c >= ancho || r >= alto) return -1;
        return d[r * ancho + c];
    }
    void dibujar(Lienzo& l, int izq, int arriba, int piso, bool salidaAbierta, double t) const {
        auto& b = BancoSprites::instancia();
        const std::string T = "04_Heros_Duty/Construccion/tiles/tile_";
        const int TW = 37, TH = 40;
        for (int j = std::max(0, arriba / TH - 1); j * TH < arriba + 140 && j * TH < altoPx(); j++)
            for (int i = std::max(0, izq / TW - 1); i * TW < izq + 245 && i * TW < anchoPx(); i++) {
                int h = (i * 7 + j * 13 + piso * 5) % 11;
                const char* id = h == 0 ? "03" : h == 4 ? "06" : h == 7 ? "02" : "01";
                const Sprite* s = b.obtener(T + id);
                s->dibujarParte(l, i * TW, j * TH, 0, 0, std::min(TW, s->getAncho()), std::min(TH, s->getAlto()));
            }
        int c0 = (int)std::floor(izq / (double)CELDA) - 1, c1 = (izq + 245) / CELDA + 1;
        int r0 = (int)std::floor(arriba / (double)CELDA) - 1, r1 = (arriba + 140) / CELDA + 1;
        Color tope(78, 82, 96), cara(40, 42, 52), borde(22, 22, 30), remache(120, 124, 140), linea(60, 64, 76);
        for (int r = r0; r <= r1; r++)
            for (int c = c0; c <= c1; c++) {
                char k = en(c, r);
                int x = c * CELDA, y = r * CELDA;
                if (c < 0 || r < 0 || c >= ancho || r >= alto) { l.rectangulo(x, y, CELDA, CELDA, Color(12, 12, 18)); continue; }
                if (k == ',') {
                    for (int j = 0; j < CELDA; j++) for (int i = 0; i < CELDA; i++)
                        l.pixel(x + i, y + j, (j % 4 == 0 || i % 5 == 0) ? Color(28, 30, 36) : Color(66, 70, 80));
                }
                else if (k == '#') {
                    bool frente = !pared(c, r + 1);
                    int caraAlto = frente ? 9 : 0;
                    for (int j = 0; j < CELDA - caraAlto; j++)
                        for (int i = 0; i < CELDA; i++) {
                            Color cc = tope.mezclar(Color(58, 62, 74), (float)j / (CELDA + 4));
                            if (((x + i) * 3 + (y + j) * 7) % 23 == 0) cc = cc.mezclar(Color(140, 146, 162), 0.5f);
                            if ((x + i) % 10 == 0 || (y + j) % 10 == 0) cc = cc.mezclar(Color(34, 36, 44), 0.45f);
                            l.pixel(x + i, y + j, cc);
                        }
                    for (int i = 0; i < CELDA; i += 5) l.pixel(x + i, y + (CELDA - caraAlto) / 2, linea);
                    if (!pared(c, r - 1)) for (int i = 0; i < CELDA; i++) l.pixel(x + i, y, Color(110, 114, 130));
                    if (!pared(c - 1, r)) for (int j = 0; j < CELDA; j++) l.pixel(x, y + j, borde);
                    if (!pared(c + 1, r)) for (int j = 0; j < CELDA; j++) l.pixel(x + CELDA - 1, y + j, borde);
                    if (frente) {
                        for (int j = 0; j < caraAlto; j++)
                            for (int i = 0; i < CELDA; i++) {
                                Color cc = cara.mezclar(Color(14, 14, 20), (float)j / (caraAlto + 2));
                                if ((x + i) % 5 == 0) cc = cc.mezclar(Color(70, 74, 88), 0.35f);
                                l.pixel(x + i, y + CELDA - caraAlto + j, cc);
                            }
                        for (int i = 0; i < CELDA; i++) l.pixel(x + i, y + CELDA - caraAlto, Color(96, 100, 116));
                        l.pixel(x + 4, y + CELDA - 5, remache); l.pixel(x + 15, y + CELDA - 5, remache);
                        if ((c * 3 + r) % 7 == 0 && ((int)(t * 2)) % 2 == 0) {
                            l.rectangulo(x + 8, y + CELDA - 7, 4, 2, Color(255, 40, 40));
                            for (int j = 0; j < 6; j++) for (int i = -j; i <= j; i++) l.sombrear(x + 10 + i, y + CELDA + j, Glifo::CLARO, Color(255, 40, 40));
                        }
                    }
                }
                else if (k == 'E' || k == 'S') {
                    bool salida = k == 'S';
                    l.rectangulo(x + 1, y + 1, CELDA - 2, CELDA - 2, Color(30, 34, 44));
                    l.marco(x + 1, y + 1, CELDA - 2, CELDA - 2, Color(150, 154, 170));
                    l.marco(x + 3, y + 3, CELDA - 6, CELDA - 6, Color(70, 74, 90));
                    Color luz = !salida ? Color(90, 160, 255) : salidaAbierta ? Color(80, 255, 120) : Color(255, 60, 60);
                    bool prende = !salida || !salidaAbierta || ((int)(t * 4)) % 2 == 0;
                    if (prende) {
                        for (int j = 0; j < 4; j++) for (int i = -j; i <= j; i++) l.pixel(x + 10 + i, y + 6 + j, luz);
                        l.rectangulo(x + 9, y + 10, 3, 4, luz);
                    }
                    if (salida && salidaAbierta) for (int j = 0; j < 10; j++) for (int i = -6; i <= 6; i++)
                        if ((i + j + (int)(t * 10)) % 3 == 0) l.sombrear(x + 10 + i, y - j, Glifo::CLARO, Color(80, 255, 120));
                }
            }
    }
};
class CalhounHD : public PersonajeAnimado {
private:
    int c, r, cDest, rDest;
    double avance;
    bool moviendo;
    DirHD dir;
    bool haciaIzquierda;
    double disparo;
    double giro;
    bool pasoNuevo;
    double herida;
    static constexpr double DUR_PASO = 0.18;
    void posicionar() {
        double k = moviendo ? avance : 0;
        x = MapaHD::centroX(c) + (MapaHD::centroX(cDest) - MapaHD::centroX(c)) * k;
        y = MapaHD::piesY(r) + (MapaHD::piesY(rDest) - MapaHD::piesY(r)) * k;
    }
public:
    CalhounHD() : PersonajeAnimado(0, 0), c(0), r(0), cDest(0), rDest(0), avance(0), moviendo(false), dir(DirHD::DERECHA),
        haciaIzquierda(false), disparo(0), giro(0), pasoNuevo(false), herida(0) {
        cambiarAnimacion(SpritesHD::anim("Calhoun/caminar", "calhoun_caminar_", 5, 7, 9));
    }
    void ubicar(int nc, int nr) { c = cDest = nc; r = rDest = nr; moviendo = false; avance = 0; posicionar(); }
    void controlar(int dirTecla, bool recien, const MapaHD& m, double dt) {
        pasoNuevo = false;
        if (giro > 0) giro -= dt;
        if (moviendo) {
            avance += dt / DUR_PASO;
            if (avance >= 1) { c = cDest; r = rDest; moviendo = false; avance = 0; }
        }
        if (!moviendo && dirTecla >= 0) {
            DirHD d = (DirHD)dirTecla;
            if (d != dir && recien) { dir = d; giro = 0.08; }
            else if (giro <= 0) {
                dir = d;
                int nc = c + dxDe(d), nr = r + dyDe(d);
                if (!m.bloquea(nc, nr)) { cDest = nc; rDest = nr; moviendo = true; avance = 0; pasoNuevo = true; }
            }
            if (d == DirHD::IZQUIERDA) haciaIzquierda = true;
            if (d == DirHD::DERECHA) haciaIzquierda = false;
        }
        posicionar();
    }
    void actualizar(double dt) override {
        if (moviendo) animacion.actualizar(dt);
        if (disparo > 0) disparo -= dt;
        if (herida > 0) herida -= dt;
    }
    void dibujar(Lienzo& l) const override {
        auto& b = BancoSprites::instancia();
        const Sprite* s;
        if (disparo > 0) s = b.obtener(SpritesHD::H + "Calhoun/disparar/calhoun_disparar_0" + std::string(disparo > 0.1 ? "3" : "2"));
        else if (moviendo) s = animacion.actual();
        else s = b.obtener(SpritesHD::H + "Calhoun/disparar/calhoun_disparar_04");
        if (!s) return;
        for (int i = -9; i <= 9; i++) l.sombrear((int)x + i, (int)y, Glifo::MEDIO, Colores::NEGRO);
        int sx = (int)x - s->getAncho() / 2;
        if (herida > 0 && ((int)(herida * 14)) % 2) return;
        s->dibujar(l, sx, (int)y - s->getAlto() + 2, haciaIzquierda);
    }
    void disparar() { disparo = 0.18; }
    void lastimar() { herida = 0.6; }
    bool empezoPaso() const { return pasoNuevo; }
    bool seMueve() const { return moviendo; }
    int getC() const { return c; }
    int getR() const { return r; }
    int celdaC() const { return moviendo && avance > 0.5 ? cDest : c; }
    int celdaR() const { return moviendo && avance > 0.5 ? rDest : r; }
    DirHD getDir() const { return dir; }
    double duracionPaso() const { return DUR_PASO; }
};
class MarineHD : public PersonajeAnimado {
private:
    int c, r, cDest, rDest;
    double avance, dur;
    bool moviendo;
    double golpe;
public:
    MarineHD(int nc, int nr, int variante) : PersonajeAnimado(0, 0), c(nc), r(nr), cDest(nc), rDest(nr), avance(0), dur(0.18), moviendo(false), golpe(0) {
        std::string f = std::to_string(1 + variante % 2);
        cambiarAnimacion(Animacion(BancoSprites::instancia().lista(SpritesHD::H + "Marines/fila_" + f + "/marine_fila_" + f + "_", { 1, 2, 3, 4 }), 8));
        ubicar(nc, nr);
    }
    void ubicar(int nc, int nr) { c = cDest = nc; r = rDest = nr; moviendo = false; x = MapaHD::centroX(c); y = MapaHD::piesY(r); }
    void seguir(int nc, int nr, double duracion) {
        if (moviendo) { c = cDest; r = rDest; moviendo = false; }
        if (nc == c && nr == r) return;
        cDest = nc; rDest = nr; avance = 0; dur = duracion; moviendo = true;
        if (nc < c) espejo = true;
        if (nc > c) espejo = false;
    }
    void actualizar(double dt) override {
        if (golpe > 0) golpe -= dt;
        if (!moviendo) return;
        animacion.actualizar(dt);
        avance += dt / dur;
        if (avance >= 1) { avance = 1; moviendo = false; }
        x = MapaHD::centroX(c) + (MapaHD::centroX(cDest) - MapaHD::centroX(c)) * avance;
        y = MapaHD::piesY(r) + (MapaHD::piesY(rDest) - MapaHD::piesY(r)) * avance;
        if (!moviendo) { c = cDest; r = rDest; }
    }
    void dibujar(Lienzo& l) const override {
        const Sprite* s = animacion.actual();
        if (!s) return;
        for (int i = -8; i <= 8; i++) l.sombrear((int)x + i, (int)y, Glifo::MEDIO, Colores::NEGRO);
        int sx = (int)x - s->getAncho() / 2, sy = (int)y - s->getAlto() + 2;
        if (golpe > 0 && ((int)(golpe * 12)) % 2) return;
        s->dibujar(l, sx, sy, espejo);
    }
    void recibirAtaque() { golpe = 0.5; }
    int getC() const { return moviendo ? cDest : c; }
    int getR() const { return moviendo ? rDest : r; }
};
class CyBugHD : public PersonajeAnimado {
protected:
    double vel;
    int vida;
    double tiempo;
    double tx, ty;
    bool conDestino;
    void elegirCelda(const MapaHD& m, const std::vector<int>& d, double desvio) {
        int c = MapaHD::celdaDeX(x), r = MapaHD::celdaDeY(y);
        int aqui = m.distancia(d, c, r);
        const int dc[4] = { 1, -1, 0, 0 }, dr[4] = { 0, 0, 1, -1 };
        int mejor = -1, valor = 1 << 30;
        for (int k = 0; k < 4; k++) {
            int v = m.distancia(d, c + dc[k], r + dr[k]);
            if (v < 0) continue;
            if (Azar::probabilidad(desvio)) v -= Azar::entero(0, 2);
            if (v < valor) { valor = v; mejor = k; }
        }
        if (mejor < 0 || aqui < 0) { conDestino = false; return; }
        tx = MapaHD::centroX(c + dc[mejor]);
        ty = MapaHD::piesY(r + dr[mejor]);
        conDestino = true;
    }
public:
    CyBugHD(double px, double py, double v, int vidas) : PersonajeAnimado(px, py), vel(v), vida(vidas), tiempo(0), tx(px), ty(py), conDestino(false) {}
    void actualizar(double dt) override { PersonajeAnimado::actualizar(dt); tiempo += dt; }
    virtual void moverse(double dt, const MapaHD& m, const std::vector<int>& d, double objX, double objY) {
        if (!conDestino || (std::fabs(tx - x) < 1 && std::fabs(ty - y) < 1)) elegirCelda(m, d, 0.0);
        double ax = conDestino ? tx : objX, ay = conDestino ? ty : objY;
        double dx = ax - x, dy = ay - y, n = std::sqrt(dx * dx + dy * dy);
        if (n > 0.01) {
            double paso = std::min(n, vel * dt);
            x += dx / n * paso; y += dy / n * paso;
            if (std::fabs(dx) > 0.5) espejo = dx > 0;
        }
    }
    virtual double altura() const { return 0; }
    bool recibirDisparo() { vida--; return vida <= 0; }
    double centroY() const { return y - 12 - altura(); }
    void dibujar(Lienzo& l) const override {
        const Sprite* s = animacion.actual();
        if (!s) return;
        for (int i = -7; i <= 7; i++) l.sombrear((int)x + i, (int)y, Glifo::CLARO, Colores::NEGRO);
        s->dibujar(l, (int)x - s->getAncho() / 2, (int)(y - altura()) - s->getAlto() + 2, espejo);
    }
};
class CyBugRastreroHD : public CyBugHD {
public:
    CyBugRastreroHD(double px, double py, double v) : CyBugHD(px, py, v, 1) {
        cambiarAnimacion(SpritesHD::anim("Cy-Bugs/dron_estandar_1", "cybug_dron_estandar_1_", 1, 5, 10));
    }
};
class CyBugExploradorHD : public CyBugHD {
public:
    CyBugExploradorHD(double px, double py, double v) : CyBugHD(px, py, v * 1.45, 1) {
        cambiarAnimacion(SpritesHD::anim("Cy-Bugs/explorador_chico", "cybug_explorador_chico_", 1, 5, 14));
    }
    void moverse(double dt, const MapaHD& m, const std::vector<int>& d, double objX, double objY) override {
        if (!conDestino || (std::fabs(tx - x) < 1 && std::fabs(ty - y) < 1)) elegirCelda(m, d, 0.3);
        CyBugHD::moverse(dt, m, d, objX, objY);
    }
};
class CyBugVoladorHD : public CyBugHD {
public:
    CyBugVoladorHD(double px, double py, double v) : CyBugHD(px, py, v * 0.85, 2) {
        cambiarAnimacion(SpritesHD::anim("Cy-Bugs/volador_2", "cybug_volador_2_", 1, 5, 12));
    }
    void moverse(double dt, const MapaHD& m, const std::vector<int>& d, double objX, double objY) override {
        double dx = objX - x, dy = objY - y, n = std::sqrt(dx * dx + dy * dy);
        if (n > 0.01) {
            x += dx / n * vel * dt + std::cos(tiempo * 3) * 8 * dt;
            y += dy / n * vel * dt + std::sin(tiempo * 2.4) * 8 * dt;
            espejo = dx > 0;
        }
    }
    double altura() const override { return 12 + std::sin(tiempo * 4) * 2; }
};
class BalaHD : public Entidad {
private:
    DirHD dir;
    double recorrido;
public:
    static constexpr double ALTURA = 30;
    BalaHD(double px, double py, DirHD d) : Entidad(px, py), dir(d), recorrido(0) {}
    void actualizar(double dt) override {
        double paso = 300 * dt;
        x += dxDe(dir) * paso; y += dyDe(dir) * paso; recorrido += paso;
        if (recorrido > 260) desactivar();
    }
    void dibujar(Lienzo& l) const override {
        Color nucleo(255, 255, 210), brillo(255, 200, 60);
        int bx = (int)x, by = (int)(y - ALTURA);
        for (int i = 0; i < 7; i++) {
            int px = bx - dxDe(dir) * i, py = by - dyDe(dir) * i;
            if (i < 2) l.pixel(px, py, nucleo); else l.sombrear(px, py, i < 4 ? Glifo::OSCURO : Glifo::CLARO, brillo);
        }
    }
    DirHD getDir() const { return dir; }
};
class ExplosionHD : public Entidad {
private:
    Animacion anim;
public:
    ExplosionHD(double px, double py, bool energia = false) : Entidad(px, py) {
        auto& b = BancoSprites::instancia();
        if (energia) anim = Animacion(b.lista(SpritesHD::H + "Efectos/efecto_", { 13, 12 }), 10, false);
        else anim = Animacion(b.lista(SpritesHD::H + "Efectos/efecto_", { 9, 10, 11, 12 }), 12, false);
    }
    void actualizar(double dt) override { anim.actualizar(dt); if (anim.haTerminado()) desactivar(); }
    void dibujar(Lienzo& l) const override {
        const Sprite* s = anim.actual();
        if (s) s->dibujar(l, (int)x - s->getAncho() / 2, (int)y - s->getAlto() / 2);
    }
};
class ChispaHD : public Entidad {
private:
    double t;
public:
    ChispaHD(double px, double py) : Entidad(px, py), t(0) {}
    void actualizar(double dt) override { t += dt; if (t > 0.25) desactivar(); }
    void dibujar(Lienzo& l) const override {
        for (int k = 0; k < 6; k++) {
            double a = k * 1.05;
            int px = (int)(x + std::cos(a) * t * 30), py = (int)(y + std::sin(a) * t * 30);
            l.sombrear(px, py, t < 0.12 ? Glifo::BLOQUE : Glifo::MEDIO, Color(255, 220, 120));
        }
    }
};
class TextoFlotanteHD : public Entidad {
private:
    std::wstring texto;
    double t;
    Color color;
public:
    TextoFlotanteHD(double px, double py, const std::wstring& s, Color c) : Entidad(px, py), texto(s), t(0), color(c) {}
    void actualizar(double dt) override { t += dt; y -= 12 * dt; if (t > 0.8) desactivar(); }
    void dibujar(Lienzo& l) const override;
};
class NidoHD {
private:
    int c, r;
    int total, salidos;
    bool destruido;
public:
    NidoHD() : c(0), r(0), total(10), salidos(0), destruido(false) {}
    void ubicar(int nc, int nr, int cuantos) { c = nc; r = nr; total = cuantos; salidos = 0; destruido = false; }
    bool quedan() const { return salidos < total; }
    void soltar() { salidos++; }
    void destruir() { destruido = true; }
    bool estaDestruido() const { return destruido; }
    double centroX() const { return c * MapaHD::CELDA + MapaHD::CELDA; }
    double piesY() const { return (r + 2) * MapaHD::CELDA - 2.0; }
    int getC() const { return c; }
    int getR() const { return r; }
    void dibujar(Lienzo& l, double t) const {
        auto& b = BancoSprites::instancia();
        int quedan = total - salidos;
        const int ox[3] = { -12, 12, 0 }, oy[3] = { -10, -10, 0 };
        for (int i = 0; i < 3; i++) {
            int fase = destruido ? 5 : (quedan > (2 - i) * 3 ? 1 : (quedan > 0 ? 2 : 3));
            const Sprite* h = b.obtener(SpritesHD::H + "Cy-Bugs/ciclo_de_vida/cybug_ciclo_de_vida_0" + std::to_string(fase));
            int pulso = destruido ? 0 : ((int)(t * 3 + i)) % 2;
            h->dibujar(l, (int)centroX() + ox[i] - h->getAncho() / 2, (int)piesY() + oy[i] - h->getAlto() - pulso);
        }
        if (!destruido) for (int i = -18; i <= 18; i += 2) l.sombrear((int)centroX() + i, (int)piesY() + 1, Glifo::CLARO, Color(120, 255, 160));
    }
};
class JefeHD : public PersonajeAnimado {
private:
    double vida, vidaMax;
    double tiempo;
    double embestida;
    double ex, ey;
    double proximaCria, proximaEmbestida;
    bool atonto, lanzarCria;
    double xMin, xMax, yMin, yMax;
public:
    JefeHD(double px, double py, double vidaTotal, double x0, double x1, double y0, double y1)
        : PersonajeAnimado(px, py), vida(vidaTotal), vidaMax(vidaTotal), tiempo(0), embestida(0), ex(0), ey(0), proximaCria(3.0),
        proximaEmbestida(6.0), atonto(false), lanzarCria(false), xMin(x0), xMax(x1), yMin(y0), yMax(y1) {
        cambiarAnimacion(SpritesHD::anim("Cy-Bugs/reina_grande_2", "cybug_reina_grande_2_", 1, 3, 9));
    }
    void actualizar(double dt) override { PersonajeAnimado::actualizar(dt); tiempo += dt; }
    void perseguir(double objX, double objY, double dt) {
        if (atonto) return;
        double dx = objX - x, dy = objY - y, n = std::sqrt(dx * dx + dy * dy);
        if (embestida > 0) {
            embestida -= dt;
            x += ex * 120 * dt; y += ey * 120 * dt;
        }
        else if (n > 1) {
            x += dx / n * 17 * dt; y += dy / n * 17 * dt;
            espejo = dx > 0;
        }
        x = std::max(xMin, std::min(xMax, x));
        y = std::max(yMin, std::min(yMax, y));
        proximaCria -= dt;
        if (proximaCria <= 0) { proximaCria = Azar::real(3.0, 4.2); lanzarCria = true; }
        proximaEmbestida -= dt;
        if (proximaEmbestida <= 0 && n > 1) {
            proximaEmbestida = Azar::real(6.5, 9.0);
            embestida = 0.8; ex = dx / n; ey = dy / n; espejo = dx > 0;
        }
    }
    void rebotar(double desdeX, double desdeY) {
        double dx = x - desdeX, dy = y - desdeY, n = std::sqrt(dx * dx + dy * dy) + 0.01;
        x += dx / n * 24; y += dy / n * 24; embestida = 0;
        x = std::max(xMin, std::min(xMax, x));
        y = std::max(yMin, std::min(yMax, y));
    }
    void dibujar(Lienzo& l) const override {
        const Sprite* s = animacion.actual();
        if (!s) return;
        double alto = atonto ? 0 : 10 + std::sin(tiempo * 3) * 3;
        for (int i = -16; i <= 16; i++) l.sombrear((int)x + i, (int)y, Glifo::MEDIO, Colores::NEGRO);
        s->dibujar(l, (int)x - s->getAncho() / 2, (int)(y - alto) - s->getAlto() + 2, espejo);
    }
    void herir(double dano) { vida -= dano; if (vida < 0) vida = 0; }
    void atontar() { atonto = true; embestida = 0; cambiarAnimacion(SpritesHD::anim("Cy-Bugs/reina_grande_2", "cybug_reina_grande_2_", 4, 4, 1)); }
    void recuperar() { vida = vidaMax; atonto = false; cambiarAnimacion(SpritesHD::anim("Cy-Bugs/reina_grande_2", "cybug_reina_grande_2_", 1, 3, 12)); }
    void irHacia(double tx2, double ty2, double dt, double velocidad) {
        double dx = tx2 - x, dy = ty2 - y, n = std::sqrt(dx * dx + dy * dy);
        if (n > 1) { x += dx / n * std::min(n, velocidad * dt); y += dy / n * std::min(n, velocidad * dt); espejo = dx > 0; }
    }
    bool consumirCria() { bool c = lanzarCria; lanzarCria = false; return c && !atonto; }
    bool embistiendo() const { return embestida > 0; }
    double fraccionVida() const { return vida / vidaMax; }
    bool derrotado() const { return vida <= 0; }
    double centroY() const { return y - 30; }
};
