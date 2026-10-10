#pragma once
#include "Entidad.h"
#include "../Motor/Animacion.h"
#include "../Motor/BancoSprites.h"
class PersonajeAnimado : public Entidad {
protected:
    Animacion animacion;
    bool espejo;
    void cambiarAnimacion(const Animacion& a) { animacion = a; }
public:
    PersonajeAnimado(double px, double py) : Entidad(px, py), espejo(false) {}
    void actualizar(double dt) override { animacion.actualizar(dt); }
    void dibujar(Lienzo& l) const override {
        const Sprite* s = animacion.actual();
        if (!s) return;
        s->dibujar(l, (int)x - s->getAncho() / 2, (int)y - s->getAlto(), espejo);
    }
};
