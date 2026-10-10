#pragma once
#include "PersonajeAnimado.h"
class Actor : public PersonajeAnimado {
private:
    Animacion parado;
    Animacion caminando;
    double objetivoX;
    double velocidad;
    bool visible;
    bool moviendo;
public:
    Actor(double px, double py, const Animacion& quieto, const Animacion& camina)
        : PersonajeAnimado(px, py), parado(quieto), caminando(camina), objetivoX(px), velocidad(0), visible(true), moviendo(false) {
        cambiarAnimacion(parado);
    }
    void caminarA(double x2, double vel) {
        objetivoX = x2;
        velocidad = vel;
        moviendo = true;
        cambiarAnimacion(caminando);
    }
    void actualizar(double dt) override {
        PersonajeAnimado::actualizar(dt);
        if (!moviendo) return;
        double paso = velocidad * dt;
        if (std::fabs(objetivoX - x) <= paso) {
            x = objetivoX;
            moviendo = false;
            cambiarAnimacion(parado);
        }
        else x += (objetivoX > x ? paso : -paso);
    }
    void dibujar(Lienzo& l) const override { if (visible) PersonajeAnimado::dibujar(l); }
    bool llego() const { return !moviendo; }
    void ponerAnimacion(const Animacion& a) { cambiarAnimacion(a); }
    void ponerParado(const Animacion& a) { parado = a; if (!moviendo) cambiarAnimacion(parado); }
    void mostrar(bool v) { visible = v; }
    bool estaVisible() const { return visible; }
    void mirar(bool haciaIzquierda) { espejo = haciaIzquierda; }
    void ubicar(double px, double py) { x = px; y = py; objetivoX = px; }
    void mover(double dx, double dy) { x += dx; y += dy; objetivoX = x; }
    const Animacion& getAnimacion() const { return animacion; }
};
