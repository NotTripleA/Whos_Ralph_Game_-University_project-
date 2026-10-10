#pragma once
#include <vector>
#include "Sprite.h"
class Animacion {
private:
    std::vector<const Sprite*> cuadros;
    double segundosPorCuadro;
    double tiempo;
    int indice;
    bool enBucle;
    bool terminada;
public:
    Animacion() : segundosPorCuadro(0.1), tiempo(0), indice(0), enBucle(true), terminada(false) {}
    Animacion(const std::vector<const Sprite*>& c, double cuadrosPorSegundo, bool bucle = true)
        : cuadros(c), segundosPorCuadro(1.0 / cuadrosPorSegundo), tiempo(0), indice(0), enBucle(bucle), terminada(false) {}
    void actualizar(double dt) {
        if (cuadros.empty() || terminada) return;
        tiempo += dt;
        while (tiempo >= segundosPorCuadro) {
            tiempo -= segundosPorCuadro;
            if (indice + 1 < (int)cuadros.size()) indice++;
            else if (enBucle) indice = 0;
            else { terminada = true; break; }
        }
    }
    void reiniciar() { indice = 0; tiempo = 0; terminada = false; }
    bool haTerminado() const { return terminada; }
    int getIndice() const { return indice; }
    int cantidad() const { return (int)cuadros.size(); }
    const Sprite* actual() const { return cuadros.empty() ? nullptr : cuadros[indice]; }
};
