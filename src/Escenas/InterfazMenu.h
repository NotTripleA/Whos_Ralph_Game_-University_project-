#pragma once
#include <vector>
#include <string>
#include <cmath>
#include "Escena.h"
#include "../Motor/Efectos.h"
class ListaOpciones {
private:
    std::vector<std::wstring> opciones;
    int seleccion;
    double tiempo;
    void dibujarOpcion(Lienzo& l, Fuentes& f, int x, int y, int i) const {
        const std::wstring& t = opciones[i];
        if (i == seleccion) {
            f.amarilla.dibujarConSombra(l, x, y, t);
            const Sprite* martillo = SPR("01_Fix-It_Felix_Jr/Objetos/martillo_dorado_icono");
            int vaiven = (int)std::lround(std::sin(tiempo * 9.0) * 1.5);
            martillo->dibujar(l, x - 14 + vaiven, y - 5);
            martillo->dibujar(l, x + FuentePixel::medir(t) + 5 - vaiven, y - 5, true);
        }
        else f.blanca.dibujarConSombra(l, x, y, t);
    }
public:
    ListaOpciones(const std::vector<std::wstring>& o, int inicial = 0) : opciones(o), seleccion(inicial), tiempo(0) {}
    int getSeleccion() const { return seleccion; }
    void setSeleccion(int s) { seleccion = s; }
    int cantidad() const { return (int)opciones.size(); }
    int anchoMaximo() const {
        int m = 0;
        for (const auto& o : opciones) m = (std::max)(m, FuentePixel::medir(o));
        return m;
    }
    void actualizar(double dt) { tiempo += dt; }
    int manejar(Contexto& ctx) {
        int antes = seleccion;
        const Teclado& t = ctx.teclado;
        int n = (int)opciones.size();
        if (t.arriba() || t.izquierda()) seleccion = (seleccion + n - 1) % n;
        if (t.abajoP() || t.derecha()) seleccion = (seleccion + 1) % n;
        if (seleccion != antes) { ctx.audio.efecto(L"menu_mover.wav"); tiempo = 0; }
        if (t.aceptar()) { ctx.audio.efecto(L"menu_aceptar.wav"); return seleccion; }
        return -1;
    }
    void dibujarVertical(Lienzo& l, Fuentes& f, int y, int paso) const {
        for (int i = 0; i < (int)opciones.size(); i++)
            dibujarOpcion(l, f, (l.getAncho() - FuentePixel::medir(opciones[i])) / 2, y + i * paso, i);
    }
    void dibujarHorizontal(Lienzo& l, Fuentes& f, int cx, int y, int separacion) const {
        int total = 0;
        for (const auto& o : opciones) total += FuentePixel::medir(o);
        total += separacion * ((int)opciones.size() - 1);
        int x = cx - total / 2;
        for (int i = 0; i < (int)opciones.size(); i++) {
            dibujarOpcion(l, f, x, y, i);
            x += FuentePixel::medir(opciones[i]) + separacion;
        }
    }
};
namespace Interfaz {
    const Color PANEL(10, 10, 34);
    const Color DORADO(255, 208, 64);
    const Color DORADO_OSCURO(170, 110, 20);
    inline void panel(Lienzo& l, int x, int y, int w, int h, bool solido = false) {
        if (solido) l.rectangulo(x, y, w, h, PANEL);
        else l.panelTranslucido(x, y, w, h, PANEL, Glifo::OSCURO);
        l.marco(x, y, w, h, DORADO);
        l.marco(x + 1, y + 1, w - 2, h - 2, DORADO_OSCURO);
    }
    inline void titulo(Lienzo& l, int y) {
        const Sprite* t = SPR("Juego/titulo");
        t->dibujar(l, (l.getAncho() - t->getAncho()) / 2, y);
    }
    inline void teclaConTexto(Lienzo& l, Fuentes& f, int x, int y, const std::wstring& etiqueta, bool resaltada = false) {
        int ancho = FuentePixel::medir(etiqueta) + 6;
        Efectos::tecla(l, x, y, ancho, resaltada);
        Color tinta(28, 28, 40);
        for (size_t i = 0; i < etiqueta.size(); i++) f.blanca.dibujarCaracter(l, x + 3 + (int)i * 8, y + 1, etiqueta[i], &tinta);
    }
    inline int anchoTecla(const std::wstring& etiqueta) { return FuentePixel::medir(etiqueta) + 6; }
    inline void ayuda(Lienzo& l, const std::wstring& linea1, const std::wstring& linea2) {
        int y = l.yMax() - 4;
        l.panelTranslucido(l.xMin(), y - 1, l.xMax() - l.xMin(), 5, Colores::NEGRO, Glifo::OSCURO);
        l.textoCentrado(y, linea1, Color(235, 235, 245));
        l.textoCentrado(y + 2, linea2, Color(255, 220, 90));
    }
}
class DialogoConfirmacion {
private:
    std::wstring linea1;
    std::wstring linea2;
    ListaOpciones opciones;
    bool abierto;
public:
    DialogoConfirmacion(const std::wstring& l1, const std::wstring& l2)
        : linea1(l1), linea2(l2), opciones({ L"S\x00CD", L"NO" }, 1), abierto(false) {}
    void abrir() { abierto = true; opciones.setSeleccion(1); }
    void cerrar() { abierto = false; }
    bool estaAbierto() const { return abierto; }
    int manejar(Contexto& ctx, double dt) {
        opciones.actualizar(dt);
        if (ctx.teclado.cancelar()) { ctx.audio.efecto(L"menu_atras.wav"); abierto = false; return 0; }
        int r = opciones.manejar(ctx);
        if (r < 0) return -1;
        abierto = false;
        return r == 0 ? 1 : 0;
    }
    void dibujar(Lienzo& l, Fuentes& f) const {
        int w = 214, h = 56;
        int x = (l.getAncho() - w) / 2, y = (l.getAlto() - h) / 2;
        l.panelTranslucido(x + 3, y + 3, w, h, Colores::NEGRO, Glifo::MEDIO);
        Interfaz::panel(l, x, y, w, h, true);
        f.blanca.dibujarCentrado(l, y + 9, linea1);
        f.blanca.dibujarCentrado(l, y + 22, linea2);
        opciones.dibujarHorizontal(l, f, l.getAncho() / 2, y + 40, 48);
    }
};
