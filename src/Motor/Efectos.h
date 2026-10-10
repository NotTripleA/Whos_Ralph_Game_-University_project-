#pragma once
#include <vector>
#include <cmath>
#include <random>
#include "Lienzo.h"
class CampoEstrellas {
private:
    struct Estrella {
        int x, y;
        float fase;
        float velocidad;
        Color color;
    };
    std::vector<Estrella> estrellas;
    double tiempo;
public:
    CampoEstrellas(int cantidad, int ancho, int altoMaximo, unsigned semilla) : tiempo(0) {
        std::mt19937 rnd(semilla);
        const Color tonos[] = { Color(255, 255, 255), Color(255, 240, 170), Color(170, 210, 255), Color(255, 190, 220) };
        for (int i = 0; i < cantidad; i++) {
            Estrella e;
            e.x = (int)(rnd() % ancho);
            e.y = (int)(rnd() % altoMaximo);
            e.fase = (float)(rnd() % 628) / 100.0f;
            e.velocidad = 1.0f + (float)(rnd() % 300) / 100.0f;
            e.color = tonos[rnd() % 4];
            estrellas.push_back(e);
        }
    }
    void actualizar(double dt) { tiempo += dt; }
    void dibujar(Lienzo& l) const {
        for (const auto& e : estrellas) {
            float b = 0.5f + 0.5f * std::sin((float)tiempo * e.velocidad + e.fase);
            if (b < 0.15f) continue;
            wchar_t g = b < 0.4f ? Glifo::CLARO : b < 0.65f ? Glifo::MEDIO : b < 0.9f ? Glifo::OSCURO : Glifo::BLOQUE;
            if (g == Glifo::BLOQUE) {
                l.pixel(e.x, e.y, e.color);
                l.sombrear(e.x - 1, e.y, Glifo::CLARO, e.color);
                l.sombrear(e.x + 1, e.y, Glifo::CLARO, e.color);
                l.sombrear(e.x, e.y - 1, Glifo::CLARO, e.color);
                l.sombrear(e.x, e.y + 1, Glifo::CLARO, e.color);
            }
            else l.sombrear(e.x, e.y, g, e.color);
        }
    }
};
namespace Efectos {
    inline void fundido(Lienzo& l, float nivel) {
        if (nivel <= 0.05f) return;
        wchar_t g = nivel < 0.3f ? Glifo::CLARO : nivel < 0.55f ? Glifo::MEDIO : nivel < 0.8f ? Glifo::OSCURO : Glifo::BLOQUE;
        l.cubrirTodo(g, Colores::NEGRO);
    }
    inline void tecla(Lienzo& l, int x, int y, int ancho, bool resaltada = false) {
        Color borde(30, 30, 40), cara = resaltada ? Color(255, 236, 140) : Color(228, 228, 236),
            luz = resaltada ? Color(255, 250, 210) : Color(255, 255, 255), lado = resaltada ? Color(190, 150, 40) : Color(140, 140, 160);
        int alto = 11;
        l.rectangulo(x + 1, y, ancho - 2, alto, borde);
        l.rectangulo(x, y + 1, ancho, alto - 2, borde);
        l.rectangulo(x + 1, y + 1, ancho - 2, alto - 3, cara);
        l.rectangulo(x + 1, y + alto - 3, ancho - 2, 2, lado);
        for (int i = x + 2; i < x + ancho - 2; i++) l.pixel(i, y + 1, luz);
    }
}
