#pragma once
#include <cstdint>
struct Color {
    uint8_t r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(int rr, int gg, int bb) : r((uint8_t)rr), g((uint8_t)gg), b((uint8_t)bb) {}
    static Color hex(uint32_t v) { return Color((v >> 16) & 255, (v >> 8) & 255, v & 255); }
    bool operator==(const Color& o) const { return r == o.r && g == o.g && b == o.b; }
    bool operator!=(const Color& o) const { return !(*this == o); }
    Color mezclar(const Color& o, float t) const {
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        return Color((int)(r + (o.r - r) * t), (int)(g + (o.g - g) * t), (int)(b + (o.b - b) * t));
    }
    Color oscurecer(float f) const { return mezclar(Color(0, 0, 0), f); }
    Color aclarar(float f) const { return mezclar(Color(255, 255, 255), f); }
};
namespace Colores {
    const Color NEGRO(0, 0, 0);
    const Color BLANCO(255, 255, 255);
    const Color AMARILLO(255, 255, 0);
    const Color ROJO(232, 40, 24);
    const Color GRIS(150, 150, 160);
    const Color CIELO_ALTO(8, 8, 28);
    const Color CIELO_BAJO(44, 22, 70);
}
