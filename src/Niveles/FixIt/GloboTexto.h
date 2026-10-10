#pragma once
#include <string>
#include "../../Motor/FuenteMini.h"
class GloboTexto {
private:
    std::wstring texto;
    int x, y;
    double tiempo;
    double duracion;
public:
    GloboTexto() : x(0), y(0), tiempo(0), duracion(0) {}
    void mostrar(const std::wstring& t, int px, int py, double segundos) { texto = t; x = px; y = py; tiempo = 0; duracion = segundos; }
    void ocultar() { duracion = 0; }
    bool visible() const { return tiempo < duracion; }
    void actualizar(double dt) { tiempo += dt; }
    void dibujar(Lienzo& l) const {
        if (!visible()) return;
        int w = FuenteMini::medir(texto) + 8, h = 11;
        int bx = x - w / 2, by = y - h - 4;
        if (bx < 2) bx = 2;
        if (bx + w > 238) bx = 238 - w;
        Color blanco(250, 250, 250), borde(20, 20, 20);
        l.rectangulo(bx + 1, by, w - 2, h, blanco);
        l.rectangulo(bx, by + 1, w, h - 2, blanco);
        for (int i = bx + 1; i < bx + w - 1; i++) { l.pixel(i, by - 1, borde); l.pixel(i, by + h, borde); }
        for (int j = by + 1; j < by + h - 1; j++) { l.pixel(bx - 1, j, borde); l.pixel(bx + w, j, borde); }
        l.pixel(x, by + h, blanco); l.pixel(x - 1, by + h, blanco); l.pixel(x, by + h + 1, blanco); l.pixel(x, by + h + 2, borde);
        l.pixel(x - 2, by + h, borde); l.pixel(x - 1, by + h + 1, borde); l.pixel(x + 1, by + h, borde); l.pixel(x + 1, by + h + 1, borde);
        FuenteMini::texto(l, bx + 4, by + 2, texto, Color(10, 10, 10));
    }
};
