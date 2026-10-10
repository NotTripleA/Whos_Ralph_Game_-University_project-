#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "Lienzo.h"
class Sprite {
private:
    std::string nombre;
    int ancho;
    int alto;
    std::vector<Color> paleta;
    std::vector<int16_t> pixeles;
    static int hexDigito(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return 0;
    }
public:
    Sprite() : ancho(0), alto(0) {}
    bool cargar(const std::wstring& ruta) {
        std::ifstream f{ std::filesystem::path(ruta) };
        if (!f) return false;
        std::string linea;
        int simbolo[128];
        for (int& s : simbolo) s = -1;
        int colores = 0;
        while (std::getline(f, linea)) {
            if (!linea.empty() && linea.back() == '\r') linea.pop_back();
            if (linea.rfind("SPRITE ", 0) == 0) nombre = linea.substr(7);
            else if (linea.rfind("SIZE ", 0) == 0) sscanf_s(linea.c_str() + 5, "%d %d", &ancho, &alto);
            else if (linea.rfind("COLORS ", 0) == 0) {
                colores = atoi(linea.c_str() + 7);
                for (int i = 0; i < colores && std::getline(f, linea); i++) {
                    unsigned char s = (unsigned char)linea[0];
                    const char* h = linea.c_str() + 2;
                    Color c(hexDigito(h[0]) * 16 + hexDigito(h[1]), hexDigito(h[2]) * 16 + hexDigito(h[3]),
                        hexDigito(h[4]) * 16 + hexDigito(h[5]));
                    if (s < 128) simbolo[s] = (int)paleta.size();
                    paleta.push_back(c);
                }
            }
            else if (linea == "PIXELS") {
                pixeles.assign(ancho * alto, -1);
                for (int y = 0; y < alto && std::getline(f, linea); y++)
                    for (int x = 0; x < ancho && x < (int)linea.size(); x++) {
                        unsigned char s = (unsigned char)linea[x];
                        if (s != '.' && s < 128) pixeles[y * ancho + x] = (int16_t)simbolo[s];
                    }
            }
        }
        return ancho > 0 && alto > 0 && !pixeles.empty();
    }
    void crearFaltante(const std::string& n) {
        nombre = n;
        ancho = alto = 8;
        paleta = { Color(255, 0, 255), Color(0, 0, 0) };
        pixeles.assign(64, 0);
        for (int i = 0; i < 64; i++) pixeles[i] = (int16_t)(((i % 8) / 2 + (i / 8) / 2) % 2);
    }
    const std::string& getNombre() const { return nombre; }
    int getAncho() const { return ancho; }
    int getAlto() const { return alto; }
    bool opaco(int x, int y) const { return x >= 0 && y >= 0 && x < ancho && y < alto && pixeles[y * ancho + x] >= 0; }
    Color color(int x, int y) const { return paleta[pixeles[y * ancho + x]]; }
    void dibujar(Lienzo& l, int x, int y, bool espejo = false) const {
        for (int j = 0; j < alto; j++)
            for (int i = 0; i < ancho; i++) {
                int16_t p = pixeles[j * ancho + i];
                if (p >= 0) l.pixel(x + (espejo ? ancho - 1 - i : i), y + j, paleta[p]);
            }
    }
    void dibujarParte(Lienzo& l, int x, int y, int sx, int sy, int sw, int sh) const {
        for (int j = 0; j < sh; j++)
            for (int i = 0; i < sw; i++) {
                int px = sx + i, py = sy + j;
                if (opaco(px, py)) l.pixel(x + i, y + j, color(px, py));
            }
    }
    void dibujarReducido(Lienzo& l, int x, int y, int f) const {
        for (int j = 0; j * f < alto; j++)
            for (int i = 0; i * f < ancho; i++) {
                int r = 0, g = 0, b = 0, n = 0;
                for (int v = 0; v < f; v++)
                    for (int u = 0; u < f; u++)
                        if (opaco(i * f + u, j * f + v)) { Color c = color(i * f + u, j * f + v); r += c.r; g += c.g; b += c.b; n++; }
                if (n * 2 >= f * f) l.pixel(x + i, y + j, Color(r / n, g / n, b / n));
            }
    }
    void dibujarSilueta(Lienzo& l, int x, int y, Color c, bool espejo = false) const {
        for (int j = 0; j < alto; j++)
            for (int i = 0; i < ancho; i++)
                if (pixeles[j * ancho + i] >= 0) l.pixel(x + (espejo ? ancho - 1 - i : i), y + j, c);
    }
    void dibujarSombreado(Lienzo& l, int x, int y, wchar_t glifo, Color c, bool espejo = false) const {
        for (int j = 0; j < alto; j++)
            for (int i = 0; i < ancho; i++)
                if (pixeles[j * ancho + i] >= 0) l.sombrear(x + (espejo ? ancho - 1 - i : i), y + j, glifo, c);
    }
};
