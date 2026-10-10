#pragma once
#include "../Motor/Lienzo.h"
class Entidad {
protected:
    double x;
    double y;
    bool activa;
public:
    Entidad(double px, double py) : x(px), y(py), activa(true) {}
    virtual ~Entidad() {}
    virtual void actualizar(double dt) = 0;
    virtual void dibujar(Lienzo& l) const = 0;
    double getX() const { return x; }
    double getY() const { return y; }
    bool estaActiva() const { return activa; }
    void desactivar() { activa = false; }
};
