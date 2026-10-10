#pragma once
#include <cmath>
#include "../../Entidades/PersonajeAnimado.h"
#include "../../Motor/Azar.h"
#include "EdificioFixIt.h"
namespace SpritesFixIt {
    const std::string F = "01_Fix-It_Felix_Jr/";
    inline Animacion felix(std::initializer_list<int> cuadros, double fps, bool bucle = true) {
        return Animacion(BancoSprites::instancia().lista(F + "Felix/fila_1/felix_f1_", std::vector<int>(cuadros)), fps, bucle);
    }
    inline Animacion felixCae(std::initializer_list<int> cuadros, double fps, bool bucle = true) {
        return Animacion(BancoSprites::instancia().lista(F + "Felix/fila_2/felix_f2_", std::vector<int>(cuadros)), fps, bucle);
    }
    inline Animacion ralph(int fila, std::initializer_list<int> cuadros, double fps, bool bucle = true) {
        return Animacion(BancoSprites::instancia().lista(F + "Ralph/fila_" + std::to_string(fila) + "/ralph_f" + std::to_string(fila) + "_", std::vector<int>(cuadros)), fps, bucle);
    }
}
class FelixJugador : public PersonajeAnimado {
public:
    enum Estado { PARADO, CAMINANDO, MARTILLANDO, SALTANDO, CAYENDO, TIRADO, CELEBRANDO };
private:
    Estado estado;
    double tEstado;
    int piso;
    int pisoDestino;
    double ySalto0, ySalto1;
    double invulnerable;
    double poder;
    double aturdido;
    bool golpeListo;
    void poner(Estado e) {
        estado = e;
        tEstado = 0;
        using namespace SpritesFixIt;
        switch (e) {
        case PARADO: cambiarAnimacion(felix({ 1 }, 1)); break;
        case CAMINANDO: cambiarAnimacion(felix({ 1, 2, 3, 4 }, 10)); break;
        case MARTILLANDO: cambiarAnimacion(felix({ 9, 5 }, 14, false)); break;
        case SALTANDO: cambiarAnimacion(felix({ 6 }, 1)); break;
        case CAYENDO: cambiarAnimacion(felixCae({ 1, 2, 3, 4 }, 12)); break;
        case TIRADO: cambiarAnimacion(felixCae({ 5, 6 }, 4)); break;
        case CELEBRANDO: cambiarAnimacion(felix({ 9, 10 }, 4)); break;
        }
    }
public:
    FelixJugador(double px, int pisoInicial)
        : PersonajeAnimado(px, EdificioFixIt::piesFelix(pisoInicial)), estado(PARADO), tEstado(0), piso(pisoInicial),
        pisoDestino(pisoInicial), ySalto0(0), ySalto1(0), invulnerable(0), poder(0), aturdido(0), golpeListo(false) {
        poner(PARADO);
    }
    void controlar(int dir, bool subir, bool bajar, bool martillar, int pisoMin, int pisoMax, double dt) {
        if (estado == CAYENDO || estado == TIRADO || estado == SALTANDO || estado == CELEBRANDO) return;
        if (aturdido > 0) return;
        if (martillar && estado != MARTILLANDO) { poner(MARTILLANDO); return; }
        if (estado == MARTILLANDO) return;
        if (subir) { saltarA(piso < pisoMax ? piso + 1 : piso); return; }
        if (bajar && piso > pisoMin) { saltarA(piso - 1); return; }
        if (dir != 0) {
            if (estado != CAMINANDO) poner(CAMINANDO);
            espejo = dir < 0;
            x += dir * 52.0 * dt;
            double xmin = EdificioFixIt::centroVentana(0) - 2, xmax = EdificioFixIt::centroVentana(EdificioFixIt::COLUMNAS - 1) + 2;
            if (x < xmin) x = xmin;
            if (x > xmax) x = xmax;
        }
        else if (estado == CAMINANDO) poner(PARADO);
    }
    void saltarA(int nuevoPiso) {
        pisoDestino = nuevoPiso;
        ySalto0 = y;
        ySalto1 = EdificioFixIt::piesFelix(nuevoPiso);
        poner(SALTANDO);
    }
    void actualizar(double dt) override {
        PersonajeAnimado::actualizar(dt);
        tEstado += dt;
        golpeListo = false;
        if (invulnerable > 0) invulnerable -= dt;
        if (poder > 0) poder -= dt;
        if (aturdido > 0) aturdido -= dt;
        switch (estado) {
        case MARTILLANDO:
            if (animacion.getIndice() == 1 && tEstado < 0.1) golpeListo = true;
            if (animacion.haTerminado() || tEstado > 0.22) poner(PARADO);
            break;
        case SALTANDO: {
            const double dur = 0.32;
            double k = tEstado / dur;
            if (k >= 1) { y = ySalto1; piso = pisoDestino; poner(PARADO); }
            else y = ySalto0 + (ySalto1 - ySalto0) * k - std::sin(k * 3.14159) * 12;
            break;
        }
        case CAYENDO:
            y += 70 * dt;
            if (y >= ySalto1) { y = ySalto1; poner(TIRADO); }
            break;
        case TIRADO:
            if (tEstado > 1.3) { piso = pisoDestino; invulnerable = 2.0; poner(PARADO); }
            break;
        default:
            break;
        }
    }
    void dibujar(Lienzo& l) const override {
        const Sprite* s = animacion.actual();
        if (!s) return;
        if (invulnerable > 0 && ((int)(invulnerable * 12)) % 2 == 0) return;
        int sx = (int)x - s->getAncho() / 2, sy = (int)y - s->getAlto();
        if (poder > 0 && ((int)(poder * 10)) % 2 == 0) s->dibujarSombreado(l, sx, sy, Glifo::MEDIO, Color(255, 220, 60), espejo);
        else s->dibujar(l, sx, sy, espejo);
        if (aturdido > 0) {
            for (int i = 0; i < 3; i++) {
                double a = aturdido * 8 + i * 2.1;
                l.pixel((int)(x + std::cos(a) * 8), (int)(y - 34 + std::sin(a) * 2), Colores::AMARILLO);
            }
        }
    }
    bool golpear(int pisoMin) {
        if (vulnerable() == false) return false;
        pisoDestino = pisoMin;
        ySalto1 = EdificioFixIt::piesFelix(pisoMin);
        poner(CAYENDO);
        return true;
    }
    void aturdir() { if (vulnerable()) aturdido = 1.0; }
    void comerPastel() { poder = 6.0; }
    void celebrar() { poner(CELEBRANDO); }
    void parar() { poner(PARADO); }
    void ubicar(double px, int p) { x = px; piso = p; pisoDestino = p; y = EdificioFixIt::piesFelix(p); poner(PARADO); }
    void moverY(double dy) { y += dy; }
    bool vulnerable() const { return invulnerable <= 0 && poder <= 0 && estado != CAYENDO && estado != TIRADO; }
    bool golpeDeMartillo() const { return golpeListo; }
    bool tieneMandos() const { return estado != CAYENDO && estado != TIRADO; }
    bool estaTirado() const { return estado == CAYENDO || estado == TIRADO; }
    bool conPoder() const { return poder > 0; }
    int getPiso() const { return piso; }
    Estado getEstado() const { return estado; }
};
class RalphFixIt : public PersonajeAnimado {
public:
    enum Estado { CAMINANDO, GOLPEANDO, ENOJADO, TREPANDO, QUIETO };
private:
    Estado estado;
    double tEstado;
    double objetivoX;
    double duracionGolpe;
    double velocidad;
    int ultimoCuadro;
    int golpesPendientes;
    void poner(Estado e) {
        estado = e;
        tEstado = 0;
        ultimoCuadro = -1;
        using namespace SpritesFixIt;
        switch (e) {
        case CAMINANDO: cambiarAnimacion(ralph(3, { 1, 2, 3, 4, 5, 6, 7, 8 }, 10)); break;
        case GOLPEANDO: cambiarAnimacion(ralph(2, { 1, 2 }, 6)); break;
        case ENOJADO: cambiarAnimacion(ralph(1, { 4, 5 }, 5)); break;
        case TREPANDO: cambiarAnimacion(ralph(2, { 3, 4 }, 6)); break;
        case QUIETO: cambiarAnimacion(ralph(1, { 1 }, 1)); break;
        }
    }
public:
    RalphFixIt(double px, double pies)
        : PersonajeAnimado(px, pies), estado(QUIETO), tEstado(0), objetivoX(px), duracionGolpe(2), velocidad(26), ultimoCuadro(-1), golpesPendientes(0) {
        poner(QUIETO);
    }
    void setDificultad(double vel, double golpe) { velocidad = vel; duracionGolpe = golpe; }
    void empezar() { elegirDestino(); }
    void quieto() { poner(QUIETO); }
    void enojado() { poner(ENOJADO); }
    void trepar() { poner(TREPANDO); }
    void ubicar(double px, double py) { x = px; y = py; }
    void moverY(double dy) { y += dy; }
    void elegirDestino() {
        objetivoX = EdificioFixIt::centroVentana(Azar::entero(0, EdificioFixIt::COLUMNAS - 1));
        poner(CAMINANDO);
    }
    void actualizar(double dt) override {
        PersonajeAnimado::actualizar(dt);
        tEstado += dt;
        if (estado == CAMINANDO) {
            double paso = velocidad * dt;
            if (std::fabs(objetivoX - x) <= paso) { x = objetivoX; poner(GOLPEANDO); }
            else x += objetivoX > x ? paso : -paso;
        }
        else if (estado == GOLPEANDO) {
            if (animacion.getIndice() != ultimoCuadro) { ultimoCuadro = animacion.getIndice(); if (ultimoCuadro == 1) golpesPendientes++; }
            if (tEstado > duracionGolpe) elegirDestino();
        }
    }
    bool consumirGolpe() {
        if (golpesPendientes <= 0) return false;
        golpesPendientes--;
        return true;
    }
    Estado getEstado() const { return estado; }
    int columna() const {
        int mejor = 0;
        for (int c = 1; c < EdificioFixIt::COLUMNAS; c++)
            if (std::fabs(EdificioFixIt::centroVentana(c) - x) < std::fabs(EdificioFixIt::centroVentana(mejor) - x)) mejor = c;
        return mejor;
    }
};
class LadrilloFixIt : public Entidad {
private:
    double vy;
    double suelo;
    double tiempoRoto;
    bool roto;
    const Sprite* sprite;
    const Sprite* escombro;
public:
    LadrilloFixIt(double px, double py, double sueloY)
        : Entidad(px, py), vy(20), suelo(sueloY), tiempoRoto(0), roto(false) {
        sprite = SPR(SpritesFixIt::F + (Azar::probabilidad(0.5) ? "Objetos/ladrillo/ladrillo_01" : "Objetos/ladrillo/ladrillo_02"));
        escombro = SPR(SpritesFixIt::F + (Azar::probabilidad(0.5) ? "Efectos/escombros/escombro_01" : "Efectos/escombros/escombro_02"));
    }
    void actualizar(double dt) override {
        if (roto) { tiempoRoto += dt; if (tiempoRoto > 0.5) desactivar(); return; }
        vy += 150 * dt;
        if (vy > 95) vy = 95;
        y += vy * dt;
        if (y >= suelo) { y = suelo; roto = true; }
    }
    void dibujar(Lienzo& l) const override {
        const Sprite* s = roto ? escombro : sprite;
        s->dibujar(l, (int)x - s->getAncho() / 2, (int)y - s->getAlto());
    }
    bool estaRoto() const { return roto; }
    void romper() { roto = true; }
};
class PatoFixIt : public PersonajeAnimado {
private:
    double velocidad;
    double limite;
public:
    PatoFixIt(double px, double py, int dir, double lim) : PersonajeAnimado(px, py), velocidad(32.0 * dir), limite(lim) {
        int fila = Azar::entero(1, 3);
        std::string pre = SpritesFixIt::F + "Patos/fila_" + std::to_string(fila) + "/pato_f" + std::to_string(fila) + "_";
        cambiarAnimacion(Animacion(BancoSprites::instancia().secuencia(pre, 1, 2), 7));
        espejo = dir < 0;
    }
    void actualizar(double dt) override {
        PersonajeAnimado::actualizar(dt);
        x += velocidad * dt;
        if (x < -20 || x > limite + 20) desactivar();
    }
};
class PastelFixIt : public Entidad {
private:
    double tiempo;
    int piso, columna;
public:
    PastelFixIt(int p, int c) : Entidad(EdificioFixIt::centroVentana(c), EdificioFixIt::piesFelix(p)), tiempo(0), piso(p), columna(c) {}
    void actualizar(double dt) override { tiempo += dt; if (tiempo > 9.0) desactivar(); }
    void dibujar(Lienzo& l) const override {
        if (tiempo < 1.2) {
            char id[64];
            sprintf_s(id, "Nicelanders/cara_%02d", 1 + (piso * 5 + columna) % 13);
            SPR(SpritesFixIt::F + id)->dibujar(l, (int)x - 8, EdificioFixIt::topePiso(piso) + 18);
            return;
        }
        if (tiempo > 7.5 && ((int)(tiempo * 8)) % 2) return;
        const Sprite* s = SPR(SpritesFixIt::F + "Objetos/pastel/pastel_0" + std::to_string(1 + ((int)(tiempo * 3)) % 2));
        s->dibujar(l, (int)x - 8, (int)y - 16);
    }
    bool listo() const { return tiempo >= 1.2; }
    int getPiso() const { return piso; }
};
class PuntajeFlotante : public Entidad {
private:
    const Sprite* sprite;
    double tiempo;
public:
    PuntajeFlotante(double px, double py, int valor) : Entidad(px, py), tiempo(0) {
        sprite = SPR(SpritesFixIt::F + "UI/puntos/puntos_" + std::to_string(valor));
    }
    void actualizar(double dt) override { tiempo += dt; y -= 14 * dt; if (tiempo > 0.9) desactivar(); }
    void dibujar(Lienzo& l) const override {
        if (tiempo > 0.6 && ((int)(tiempo * 16)) % 2) return;
        sprite->dibujar(l, (int)x - 8, (int)y - 16);
    }
};
class VidrioCayendo : public Entidad {
private:
    double vx, vy;
    const Sprite* sprite;
public:
    VidrioCayendo(double px, double py) : Entidad(px, py), vx(Azar::real(-20, 20)), vy(Azar::real(-20, 0)) {
        char id[96];
        sprintf_s(id, "01_Fix-It_Felix_Jr/Objetos/vidrio_roto/vidrio_%02d", Azar::entero(1, 3));
        sprite = SPR(id);
    }
    void actualizar(double dt) override { vy += 160 * dt; x += vx * dt; y += vy * dt; if (vy > 200) desactivar(); }
    void dibujar(Lienzo& l) const override { sprite->dibujar(l, (int)x - 8, (int)y - 8); }
};
