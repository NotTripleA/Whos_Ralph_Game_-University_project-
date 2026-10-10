#pragma once
#include <vector>
#include <string>
#include "Color.h"
namespace Glifo {
    const wchar_t BLOQUE = L'\x2588';
    const wchar_t OSCURO = L'\x2593';
    const wchar_t MEDIO = L'\x2592';
    const wchar_t CLARO = L'\x2591';
    const wchar_t VACIO = L' ';
}
struct Celda {
    wchar_t glifo;
    Color fg;
    Color bg;
    Celda() : glifo(Glifo::VACIO) {}
    Celda(wchar_t g, Color f, Color b) : glifo(g), fg(f), bg(b) {}
    bool operator==(const Celda& o) const {
        if (glifo != o.glifo) return false;
        if (glifo == Glifo::BLOQUE) return fg == o.fg;
        if (glifo == Glifo::VACIO) return bg == o.bg;
        return fg == o.fg && bg == o.bg;
    }
    bool operator!=(const Celda& o) const { return !(*this == o); }
};
class Lienzo {
private:
    int anchoF, altoF;
    int ancho, alto;
    int ox, oy;
    std::vector<Celda> celdas;
    int rx0, ry0, rx1, ry1;
    int camX, camY;
    int fx(int x) const { return x - camX + ox; }
    int fy(int y) const { return y - camY + oy; }
    bool dentroF(int x, int y) const { return x >= rx0 && y >= ry0 && x < rx1 && y < ry1; }
    Color colorBaseF(int x, int y) const {
        if (!dentroF(x, y)) return Colores::NEGRO;
        const Celda& k = celdas[y * anchoF + x];
        switch (k.glifo) {
        case Glifo::BLOQUE: return k.fg;
        case Glifo::OSCURO: return k.bg.mezclar(k.fg, 0.75f);
        case Glifo::MEDIO: return k.bg.mezclar(k.fg, 0.5f);
        case Glifo::CLARO: return k.bg.mezclar(k.fg, 0.25f);
        default: return k.bg;
        }
    }
public:
    Lienzo(int w, int h, int pantallaW = 0, int pantallaH = 0)
        : anchoF(pantallaW > w ? pantallaW : w), altoF(pantallaH > h ? pantallaH : h), ancho(w), alto(h),
        ox(0), oy(0), rx0(0), ry0(0), rx1(0), ry1(0), camX(0), camY(0) {
        ox = (anchoF - ancho) / 2;
        oy = (altoF - alto) / 2;
        celdas.assign(anchoF * altoF, Celda());
        quitarRecorte();
    }
    void camara(int x, int y) { camX = x; camY = y; }
    void sinCamara() { camX = 0; camY = 0; }
    int getAncho() const { return ancho; }
    int getAlto() const { return alto; }
    int getAnchoFisico() const { return anchoF; }
    int getAltoFisico() const { return altoF; }
    int xMin() const { return -ox; }
    int xMax() const { return anchoF - ox; }
    int yMin() const { return -oy; }
    int yMax() const { return altoF - oy; }
    bool dentro(int x, int y) const { return dentroF(fx(x), fy(y)); }
    void recortar(int x, int y, int w, int h) {
        int x0 = x + ox, y0 = y + oy;
        rx0 = x0 < 0 ? 0 : x0; ry0 = y0 < 0 ? 0 : y0;
        rx1 = x0 + w > anchoF ? anchoF : x0 + w; ry1 = y0 + h > altoF ? altoF : y0 + h;
    }
    void quitarRecorte() { rx0 = 0; ry0 = 0; rx1 = anchoF; ry1 = altoF; }
    const Celda& celda(int x, int y) const { return celdas[y * anchoF + x]; }
    void limpiar(Color c) {
        for (auto& k : celdas) k = Celda(Glifo::BLOQUE, c, c);
    }
    Color colorBase(int x, int y) const { return colorBaseF(fx(x), fy(y)); }
    void pixel(int x, int y, Color c) {
        x = fx(x); y = fy(y);
        if (dentroF(x, y)) celdas[y * anchoF + x] = Celda(Glifo::BLOQUE, c, c);
    }
    void sombrear(int x, int y, wchar_t glifo, Color c) {
        x = fx(x); y = fy(y);
        if (!dentroF(x, y)) return;
        Color base = colorBaseF(x, y);
        celdas[y * anchoF + x] = Celda(glifo, c, base);
    }
    void oscurecer(int x, int y, float f) {
        x = fx(x); y = fy(y);
        if (!dentroF(x, y)) return;
        Celda& k = celdas[y * anchoF + x];
        k.fg = k.fg.oscurecer(f);
        k.bg = k.bg.oscurecer(f);
    }
    void cubrirTodo(wchar_t glifo, Color c) {
        for (int y = 0; y < altoF; y++)
            for (int x = 0; x < anchoF; x++) {
                if (glifo == Glifo::BLOQUE) celdas[y * anchoF + x] = Celda(Glifo::BLOQUE, c, c);
                else celdas[y * anchoF + x] = Celda(glifo, c, colorBaseF(x, y));
            }
    }
    void rectangulo(int x, int y, int w, int h, Color c) {
        for (int j = y; j < y + h; j++)
            for (int i = x; i < x + w; i++) pixel(i, j, c);
    }
    void marco(int x, int y, int w, int h, Color c) {
        for (int i = x; i < x + w; i++) { pixel(i, y, c); pixel(i, y + h - 1, c); }
        for (int j = y; j < y + h; j++) { pixel(x, j, c); pixel(x + w - 1, j, c); }
    }
    void panelTranslucido(int x, int y, int w, int h, Color c, wchar_t glifo = Glifo::MEDIO) {
        for (int j = y; j < y + h; j++)
            for (int i = x; i < x + w; i++) sombrear(i, j, glifo, c);
    }
    void degradadoVertical(int y0, int y1, Color arriba, Color abajo) {
        int a = y0 + oy, b = y1 + oy;
        if (y0 <= 0) a = 0;
        if (y1 >= alto) b = altoF;
        for (int y = a; y < b; y++) {
            Color c = arriba.mezclar(abajo, (float)(y - a) / (float)(b - a - 1 > 0 ? b - a - 1 : 1));
            for (int x = 0; x < anchoF; x++) if (dentroF(x, y)) celdas[y * anchoF + x] = Celda(Glifo::BLOQUE, c, c);
        }
    }
    void texto(int x, int y, const std::wstring& s, Color fg) {
        x = fx(x); y = fy(y);
        for (size_t i = 0; i < s.size(); i++) {
            int cx = x + (int)i;
            if (!dentroF(cx, y)) continue;
            Color base = colorBaseF(cx, y);
            celdas[y * anchoF + cx] = Celda(s[i], fg, base);
        }
    }
    void texto(int x, int y, const std::wstring& s, Color fg, Color bg) {
        x = fx(x); y = fy(y);
        for (size_t i = 0; i < s.size(); i++) {
            int cx = x + (int)i;
            if (dentroF(cx, y)) celdas[y * anchoF + cx] = Celda(s[i], fg, bg);
        }
    }
    void textoCentrado(int y, const std::wstring& s, Color fg) {
        texto((ancho - (int)s.size()) / 2, y, s, fg);
    }
};
