#pragma once
#include <cmath>
#include "PersonajeAnimado.h"
#include "../Motor/Azar.h"
namespace RutasSprites {
    const std::string RALPH = "01_Fix-It_Felix_Jr/Ralph/";
    const std::string FELIX = "01_Fix-It_Felix_Jr/Felix/";
    const std::string VANELLOPE = "!02_Vanellope_y_personajes/Vanellope/";
    const std::string PATOS = "01_Fix-It_Felix_Jr/Patos/";
    const std::string OBJETOS = "01_Fix-It_Felix_Jr/Objetos/";
    const std::string EFECTOS = "01_Fix-It_Felix_Jr/Efectos/";
}
class RalphMenu : public PersonajeAnimado {
private:
    enum Estado { CAMINAR, PUNOS, GOLPEAR, PARADO };
    Estado estado;
    double tiempoEstado;
    double duracion;
    double minX, maxX;
    int direccion;
    int ultimoCuadro;
    bool golpe;
    void ir(Estado e) {
        estado = e;
        tiempoEstado = 0;
        duracion = Azar::real(2.0, 4.0);
        ultimoCuadro = -1;
        auto& b = BancoSprites::instancia();
        using namespace RutasSprites;
        switch (e) {
        case CAMINAR: cambiarAnimacion(Animacion(b.secuencia(RALPH + "fila_3/ralph_f3_", 1, 8), 8)); break;
        case PUNOS: cambiarAnimacion(Animacion(b.lista(RALPH + "fila_1/ralph_f1_", { 4, 5 }), 4)); break;
        case GOLPEAR: cambiarAnimacion(Animacion(b.lista(RALPH + "fila_2/ralph_f2_", { 8, 9 }), 6)); break;
        case PARADO: cambiarAnimacion(Animacion(b.lista(RALPH + "fila_1/ralph_f1_", { 1 }), 1)); break;
        }
    }
public:
    RalphMenu(double px, double suelo, double xMin, double xMax)
        : PersonajeAnimado(px, suelo), estado(PARADO), tiempoEstado(0), duracion(0), minX(xMin), maxX(xMax), direccion(-1),
        ultimoCuadro(-1), golpe(false) {
        ir(PARADO);
    }
    void actualizar(double dt) override {
        PersonajeAnimado::actualizar(dt);
        tiempoEstado += dt;
        switch (estado) {
        case PARADO:
            if (tiempoEstado > 1.2) ir(CAMINAR);
            break;
        case CAMINAR:
            x += direccion * 14 * dt;
            if (x < minX) { x = minX; direccion = 1; }
            if (x > maxX) { x = maxX; direccion = -1; }
            if (tiempoEstado > duracion) ir(PUNOS);
            break;
        case PUNOS:
            if (tiempoEstado > 1.0) ir(GOLPEAR);
            break;
        case GOLPEAR:
            if (animacion.getIndice() != ultimoCuadro) {
                ultimoCuadro = animacion.getIndice();
                if (ultimoCuadro == 1) golpe = true;
            }
            if (tiempoEstado > 2.0) ir(PARADO);
            break;
        }
    }
    bool consumirGolpe() {
        bool g = golpe;
        golpe = false;
        return g;
    }
};
class FelixMenu : public PersonajeAnimado {
private:
    enum Estado { CAMINAR, MARTILLAR };
    Estado estado;
    double tiempoEstado;
    double duracion;
    double minX, maxX;
    int direccion;
    double destello;
    void ir(Estado e) {
        estado = e;
        tiempoEstado = 0;
        duracion = Azar::real(2.0, 4.0);
        auto& b = BancoSprites::instancia();
        using namespace RutasSprites;
        if (e == CAMINAR) cambiarAnimacion(Animacion(b.secuencia(FELIX + "fila_1/felix_f1_", 1, 4), 8));
        else cambiarAnimacion(Animacion(b.lista(FELIX + "fila_1/felix_f1_", { 9, 5 }), 5));
    }
public:
    FelixMenu(double px, double suelo, double xMin, double xMax)
        : PersonajeAnimado(px, suelo), estado(CAMINAR), tiempoEstado(0), duracion(0), minX(xMin), maxX(xMax), direccion(1), destello(0) {
        ir(CAMINAR);
    }
    void actualizar(double dt) override {
        PersonajeAnimado::actualizar(dt);
        tiempoEstado += dt;
        if (destello > 0) destello -= dt;
        if (estado == CAMINAR) {
            x += direccion * 20 * dt;
            if (x < minX) { x = minX; direccion = 1; }
            if (x > maxX) { x = maxX; direccion = -1; }
            espejo = direccion < 0;
            if (tiempoEstado > duracion) ir(MARTILLAR);
        }
        else {
            if (animacion.getIndice() == 1 && destello <= 0) destello = 0.15;
            if (tiempoEstado > 1.6) ir(CAMINAR);
        }
    }
    void dibujar(Lienzo& l) const override {
        PersonajeAnimado::dibujar(l);
        if (estado == MARTILLAR && destello > 0) {
            const Sprite* d = SPR(RutasSprites::EFECTOS + "destello/destello_01");
            int dx = espejo ? (int)x - 26 : (int)x + 10;
            d->dibujar(l, dx, (int)y - 10, espejo);
        }
    }
};
class VanellopeMenu : public PersonajeAnimado {
private:
    int direccion;
    double espera;
    double glitch;
    double proximoGlitch;
    double anchoPantalla;
    bool corriendo;
    void empezarCarrera() {
        corriendo = true;
        auto& b = BancoSprites::instancia();
        using namespace RutasSprites;
        if (direccion > 0) {
            x = -10;
            cambiarAnimacion(Animacion(b.secuencia(VANELLOPE + "fila_2/vanellope_f2_", 6, 13), 12));
        }
        else {
            x = anchoPantalla + 10;
            cambiarAnimacion(Animacion(b.secuencia(VANELLOPE + "fila_1/vanellope_f1_", 6, 13), 12));
        }
        proximoGlitch = Azar::real(0.8, 2.0);
    }
public:
    VanellopeMenu(double suelo, double ancho, int dir = 1, double esperaInicial = 4.0)
        : PersonajeAnimado(-20, suelo), direccion(dir), espera(esperaInicial), glitch(0), proximoGlitch(1), anchoPantalla(ancho), corriendo(false) {}
    bool estaCorriendo() const { return corriendo; }
    void actualizar(double dt) override {
        if (!corriendo) {
            espera -= dt;
            if (espera <= 0) empezarCarrera();
            return;
        }
        PersonajeAnimado::actualizar(dt);
        x += direccion * 42 * dt;
        proximoGlitch -= dt;
        if (glitch > 0) glitch -= dt;
        if (proximoGlitch <= 0) {
            glitch = 0.3;
            x += direccion * Azar::entero(8, 16);
            proximoGlitch = Azar::real(1.0, 2.2);
        }
        if (x < -30 || x > anchoPantalla + 30) {
            corriendo = false;
            espera = Azar::real(3.0, 6.0);
            direccion = -direccion;
        }
    }
    void dibujar(Lienzo& l) const override {
        if (!corriendo) return;
        const Sprite* s = animacion.actual();
        if (!s) return;
        int sx = (int)x - s->getAncho() / 2, sy = (int)y - s->getAlto();
        if (glitch > 0) {
            Color cian(60, 230, 255), azul(40, 90, 255);
            s->dibujarSombreado(l, sx - direccion * 9, sy, Glifo::CLARO, azul);
            s->dibujarSombreado(l, sx - direccion * 6, sy - 1, Glifo::MEDIO, cian);
            s->dibujarSombreado(l, sx - direccion * 3, sy, Glifo::OSCURO, cian);
            if ((int)(glitch * 40) % 2 == 0) { s->dibujarSilueta(l, sx, sy, cian); return; }
        }
        s->dibujar(l, sx, sy);
    }
};
class PatoMenu : public PersonajeAnimado {
private:
    double base;
    double tiempo;
    double espera;
    double anchoPantalla;
    bool volando;
public:
    PatoMenu(double ancho, double esperaInicial)
        : PersonajeAnimado(-20, 40), base(40), tiempo(0), espera(esperaInicial), anchoPantalla(ancho), volando(false) {}
    void actualizar(double dt) override {
        if (!volando) {
            espera -= dt;
            if (espera <= 0) {
                volando = true;
                x = -16;
                base = Azar::real(36, 70);
                int fila = Azar::entero(1, 3);
                char pre[64];
                sprintf_s(pre, "fila_%d/pato_f%d_", fila, fila);
                cambiarAnimacion(Animacion(BancoSprites::instancia().secuencia(RutasSprites::PATOS + pre, 1, 2), 6));
            }
            return;
        }
        PersonajeAnimado::actualizar(dt);
        tiempo += dt;
        x += 28 * dt;
        y = base + std::sin(tiempo * 3.0) * 4;
        if (x > anchoPantalla + 20) { volando = false; espera = Azar::real(4.0, 9.0); }
    }
    void dibujar(Lienzo& l) const override {
        if (volando) PersonajeAnimado::dibujar(l);
    }
};
class Ladrillo : public Entidad {
private:
    double vx, vy;
    double suelo;
    double tiempoRoto;
    bool roto;
    const Sprite* sprite;
    const Sprite* escombro;
public:
    Ladrillo(double px, double py, double sueloY)
        : Entidad(px, py), vx(Azar::real(-55, -15)), vy(Azar::real(-75, -45)), suelo(sueloY), tiempoRoto(0), roto(false) {
        sprite = SPR(RutasSprites::OBJETOS + (Azar::probabilidad(0.5) ? "ladrillo/ladrillo_01" : "ladrillo/ladrillo_02"));
        escombro = SPR(RutasSprites::EFECTOS + (Azar::probabilidad(0.5) ? "escombros/escombro_01" : "escombros/escombro_02"));
    }
    void actualizar(double dt) override {
        if (roto) {
            tiempoRoto += dt;
            if (tiempoRoto > 0.5) desactivar();
            return;
        }
        vy += 140 * dt;
        x += vx * dt;
        y += vy * dt;
        if (y >= suelo) { y = suelo; roto = true; }
    }
    void dibujar(Lienzo& l) const override {
        const Sprite* s = roto ? escombro : sprite;
        s->dibujar(l, (int)x - s->getAncho() / 2, (int)y - s->getAlto());
    }
};
