#pragma once
#include <string>
#include <vector>
#include <cmath>
#include "../Motor/FuenteMini.h"
#include "../Motor/Efectos.h"
#include "../Escenas/InterfazMenu.h"
#include "Progreso.h"
inline void dibujarBarra(Lienzo& l, int x, int y, int ancho, int alto, double fraccion, Color lleno, Color vacio) {
    if (fraccion < 0) fraccion = 0;
    if (fraccion > 1) fraccion = 1;
    double llenas = fraccion * ancho;
    for (int i = 0; i < ancho; i++) {
        double resto = llenas - i;
        for (int j = 0; j < alto; j++) {
            if (resto >= 1) l.pixel(x + i, y + j, lleno);
            else if (resto > 0.66) { l.pixel(x + i, y + j, vacio); l.sombrear(x + i, y + j, Glifo::OSCURO, lleno); }
            else if (resto > 0.33) { l.pixel(x + i, y + j, vacio); l.sombrear(x + i, y + j, Glifo::MEDIO, lleno); }
            else if (resto > 0) { l.pixel(x + i, y + j, vacio); l.sombrear(x + i, y + j, Glifo::CLARO, lleno); }
            else l.pixel(x + i, y + j, vacio);
        }
    }
}
struct Retrato {
    const Sprite* normal;
    const Sprite* hablando;
    bool espejo;
    int recorteX, recorteY;
    Retrato() : normal(nullptr), hablando(nullptr), espejo(false), recorteX(-1), recorteY(-1) {}
    Retrato(const Sprite* n, const Sprite* h = nullptr, bool e = false) : normal(n), hablando(h), espejo(e), recorteX(-1), recorteY(-1) {}
    static Retrato recortado(const Sprite* s, int x, int y) { Retrato r(s); r.recorteX = x; r.recorteY = y; return r; }
};
inline void dibujarCara(Lienzo& l, const Sprite* s, int x, int y, int w, int h, bool espejo, int recX = -1, int recY = -1) {
    if (!s) return;
    if (recX >= 0) {
        for (int j = 0; j < h; j++)
            for (int i = 0; i < w; i++)
                if (s->opaco(recX + i, recY + j)) l.pixel(x + i, y + j, s->color(recX + i, recY + j));
        return;
    }
    int top = -1;
    for (int j = 0; j < s->getAlto() && top < 0; j++)
        for (int i = 0; i < s->getAncho(); i++) if (s->opaco(i, j)) { top = j; break; }
    if (top < 0) return;
    long suma = 0, n = 0;
    for (int j = top; j < top + 14 && j < s->getAlto(); j++)
        for (int i = 0; i < s->getAncho(); i++) if (s->opaco(i, j)) { suma += i; n++; }
    int cx = n ? (int)(suma / n) : s->getAncho() / 2;
    int sx = cx - w / 2, sy = top - 1;
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++) {
            int px = espejo ? s->getAncho() - 1 - (sx + i) : sx + i;
            int py = sy + j;
            if (s->opaco(px, py)) l.pixel(x + (espejo ? w - 1 - i : i), y + j, s->color(px, py));
        }
}
class CuadroDialogo {
private:
    std::wstring nombre;
    std::vector<std::wstring> lineas;
    Color colorNombre;
    Retrato retrato;
    double tiempo;
    int total;
    bool activo;
    int ultimoSonido;
    int visibles() const { int v = (int)(tiempo * 42.0); return v > total ? total : v; }
public:
    static const int Y = 83;
    static const int ALTO = 37;
    CuadroDialogo() : colorNombre(Colores::AMARILLO), tiempo(0), total(0), activo(false), ultimoSonido(0) {}
    void mostrar(const std::wstring& quien, const std::wstring& texto, const Retrato& r, Color c) {
        nombre = quien;
        retrato = r;
        colorNombre = c;
        int columnas = r.normal ? 47 : 56;
        lineas = FuenteMini::partir(texto, columnas);
        total = 0;
        for (const auto& ln : lineas) total += (int)ln.size();
        tiempo = 0;
        ultimoSonido = 0;
        activo = true;
    }
    void ocultar() { activo = false; }
    bool estaActivo() const { return activo; }
    bool completo() const { return visibles() >= total; }
    void completar() { tiempo = total / 42.0 + 0.01; }
    double getTiempo() const { return tiempo; }
    void actualizar(double dt, Audio& audio, bool sonidoLetras) {
        if (!activo) return;
        tiempo += dt;
        int v = visibles();
        if (sonidoLetras && v < total && v - ultimoSonido >= 3) { ultimoSonido = v; audio.efecto(L"texto.wav"); }
    }
    void dibujar(Lienzo& l) const {
        if (!activo) return;
        const int Y = l.yMax() - ALTO;
        int x0 = 2, w = l.getAncho() - 4;
        l.rectangulo(x0, Y, w, ALTO, Colores::NEGRO);
        l.marco(x0, Y, w, ALTO, Colores::BLANCO);
        l.marco(x0 + 1, Y + 1, w - 2, ALTO - 2, Colores::BLANCO);
        int tx = x0 + 6;
        if (retrato.normal) {
            l.marco(x0 + 4, Y + 4, 30, 29, Color(90, 90, 110));
            bool habla = !completo() && retrato.hablando && ((int)(tiempo * 7)) % 2 == 1;
            dibujarCara(l, habla ? retrato.hablando : retrato.normal, x0 + 5, Y + 5, 28, 27, retrato.espejo, retrato.recorteX, retrato.recorteY);
            tx = x0 + 38;
        }
        if (!nombre.empty()) {
            int nw = FuenteMini::medir(nombre) + 8;
            l.rectangulo(x0 + 4, Y - 9, nw, 10, Colores::NEGRO);
            l.marco(x0 + 4, Y - 9, nw, 10, Colores::BLANCO);
            FuenteMini::texto(l, x0 + 8, Y - 8, nombre, colorNombre);
        }
        int quedan = visibles();
        for (size_t i = 0; i < lineas.size() && i < 4; i++) {
            std::wstring parte = lineas[i].substr(0, (size_t)(quedan < 0 ? 0 : (quedan < (int)lineas[i].size() ? quedan : (int)lineas[i].size())));
            FuenteMini::texto(l, tx, Y + 4 + (int)i * 8, parte, Colores::BLANCO);
            quedan -= (int)lineas[i].size();
        }
        if (completo() && ((int)(tiempo * 3)) % 2 == 0) FuenteMini::caracter(l, x0 + w - 9, Y + ALTO - 10, L'\x25BC', Colores::AMARILLO);
    }
};
class MenuEleccion {
private:
    std::wstring titulo;
    std::vector<std::wstring> opciones;
    int seleccion;
    bool activo;
    double tiempo;
public:
    MenuEleccion() : seleccion(0), activo(false), tiempo(0) {}
    void mostrar(const std::wstring& t, const std::vector<std::wstring>& o) { titulo = t; opciones = o; seleccion = 0; activo = true; tiempo = 0; }
    bool estaActivo() const { return activo; }
    int manejar(Contexto& ctx, double dt) {
        if (!activo) return -1;
        tiempo += dt;
        int n = (int)opciones.size(), antes = seleccion;
        const Teclado& t = ctx.teclado;
        if (t.arriba() || t.izquierda()) seleccion = (seleccion + n - 1) % n;
        if (t.abajoP() || t.derecha()) seleccion = (seleccion + 1) % n;
        for (int i = 0; i < n && i < 9; i++) if (t.presionada('1' + i)) { seleccion = i; activo = false; ctx.audio.efecto(L"menu_aceptar.wav"); return i; }
        if (seleccion != antes) ctx.audio.efecto(L"menu_mover.wav");
        if (tiempo > 0.3 && t.aceptar()) { activo = false; ctx.audio.efecto(L"menu_aceptar.wav"); return seleccion; }
        return -1;
    }
    void dibujar(Lienzo& l) const {
        if (!activo) return;
        std::vector<std::vector<std::wstring>> lineas;
        int total = 0;
        for (int i = 0; i < (int)opciones.size(); i++) {
            lineas.push_back(FuenteMini::partir(L"[" + std::to_wstring(i + 1) + L"] " + opciones[i], 51));
            total += (int)lineas.back().size();
        }
        int alto = 14 + total * 8 + (int)opciones.size() * 3 + 2;
        int x0 = 2, w = l.getAncho() - 4, y0 = l.yMax() - alto;
        l.rectangulo(x0, y0, w, alto, Colores::NEGRO);
        l.marco(x0, y0, w, alto, Interfaz::DORADO);
        l.marco(x0 + 1, y0 + 1, w - 2, alto - 2, Interfaz::DORADO_OSCURO);
        FuenteMini::texto(l, x0 + 6, y0 + 4, titulo, Interfaz::DORADO);
        int y = y0 + 14;
        for (int i = 0; i < (int)opciones.size(); i++) {
            bool sel = i == seleccion;
            int h = (int)lineas[i].size() * 8;
            if (sel) {
                l.panelTranslucido(x0 + 3, y - 1, w - 6, h + 1, Color(60, 50, 0), Glifo::MEDIO);
                if (((int)(tiempo * 4)) % 2 == 0) FuenteMini::caracter(l, x0 + 5, y, L'>', Colores::AMARILLO);
            }
            for (size_t k = 0; k < lineas[i].size(); k++)
                FuenteMini::texto(l, x0 + 11 + (k > 0 ? 16 : 0), y + (int)k * 8, lineas[i][k], sel ? Colores::AMARILLO : Color(200, 200, 210));
            y += h + 3;
        }
    }
};
class Notificacion {
private:
    std::wstring texto;
    std::wstring detalle;
    double tiempo;
public:
    Notificacion() : tiempo(-1) {}
    void mostrar(const std::wstring& t, const std::wstring& d = L"") { texto = t; detalle = d; tiempo = 0; }
    void actualizar(double dt) { if (tiempo >= 0) { tiempo += dt; if (tiempo > 4.2) tiempo = -1; } }
    bool activa() const { return tiempo >= 0; }
    void dibujar(Lienzo& l) const {
        if (tiempo < 0) return;
        int w = (std::max)(FuenteMini::medir(texto), FuenteMini::medir(detalle)) + 22;
        int h = detalle.empty() ? 12 : 20;
        double entra = tiempo < 0.35 ? tiempo / 0.35 : tiempo > 3.8 ? (4.2 - tiempo) / 0.4 : 1.0;
        int x = -w + (int)(entra * (w + 3)), t0 = l.yMin();
        l.rectangulo(x, t0 + 3, w, h, Color(12, 12, 30));
        l.marco(x, t0 + 3, w, h, Interfaz::DORADO);
        Color c = ((int)(tiempo * 6)) % 2 ? Colores::AMARILLO : Colores::BLANCO;
        int ix = x + 6, iy = t0 + 8;
        l.pixel(ix, iy - 2, c); l.pixel(ix, iy - 1, c); l.pixel(ix - 2, iy, c); l.pixel(ix - 1, iy, c); l.pixel(ix, iy, c);
        l.pixel(ix + 1, iy, c); l.pixel(ix + 2, iy, c); l.pixel(ix, iy + 1, c); l.pixel(ix, iy + 2, c);
        FuenteMini::texto(l, x + 12, t0 + 5, texto, Colores::BLANCO);
        if (!detalle.empty()) FuenteMini::texto(l, x + 12, t0 + 13, detalle, Colores::AMARILLO);
        if (entra < 1) {
            wchar_t g = entra < 0.33 ? Glifo::OSCURO : entra < 0.66 ? Glifo::MEDIO : Glifo::CLARO;
            for (int j = t0 + 3; j < t0 + 3 + h; j++) for (int i = x; i < x + w; i++) l.sombrear(i, j, g, Colores::NEGRO);
        }
    }
};
class MenuPausa {
private:
    ListaOpciones opciones;
    bool abierto;
    double mensaje;
public:
    enum Resultado { NADA, CONTINUAR, GUARDAR, SALIR };
    MenuPausa() : opciones({ L"CONTINUAR", L"GUARDAR", L"SALIR" }), abierto(false), mensaje(0) {}
    void abrir() { abierto = true; opciones.setSeleccion(0); mensaje = 0; }
    void cerrar() { abierto = false; }
    bool estaAbierto() const { return abierto; }
    void avisarGuardado() { mensaje = 2.0; }
    Resultado manejar(Contexto& ctx, double dt) {
        opciones.actualizar(dt);
        if (mensaje > 0) mensaje -= dt;
        if (ctx.teclado.cancelar()) { ctx.audio.efecto(L"menu_atras.wav"); abierto = false; return CONTINUAR; }
        int r = opciones.manejar(ctx);
        if (r == 0) { abierto = false; return CONTINUAR; }
        if (r == 1) return GUARDAR;
        if (r == 2) { abierto = false; return SALIR; }
        return NADA;
    }
    void dibujar(Lienzo& l, Fuentes& f, const Progreso& p) const {
        if (!abierto) return;
        l.cubrirTodo(Glifo::MEDIO, Colores::NEGRO);
        int w = 176, h = 102, x0 = (l.getAncho() - w) / 2, y0 = 8;
        Interfaz::panel(l, x0, y0, w, h, true);
        f.amarilla.dibujarCentrado(l, y0 + 6, L"PAUSA");
        int sx = x0 + 10, sy = y0 + 20;
        FuenteMini::texto(l, sx, sy, L"REPUTACI\x00D3N:", Color(200, 200, 215));
        FuenteMini::texto(l, sx + 52, sy, p.etiquetaReputacion(), Colores::AMARILLO);
        FuenteMini::texto(l, sx, sy + 9, L"MEDIDOR DE EMPAT\x00CD" L"A:", Color(200, 200, 215));
        double fr = (p.getEmpatia() + 3) / 6.0;
        dibujarBarra(l, sx + 80, sy + 10, 40, 4, fr, Color(255, 120, 170), Color(50, 30, 50));
        FuenteMini::texto(l, sx + 124, sy + 9, p.etiquetaEmpatia(), Color(255, 150, 190));
        FuenteMini::texto(l, sx, sy + 18, L"NIVELES COMPLETADOS:", Color(200, 200, 215));
        FuenteMini::texto(l, sx + 84, sy + 18, std::to_wstring(p.getNivelesCompletados()) + L" / " + std::to_wstring(Progreso::NIVELES_TOTALES), Colores::AMARILLO);
        for (int i = 0; i < w - 20; i++) l.pixel(x0 + 10 + i, sy + 28, Interfaz::DORADO_OSCURO);
        opciones.dibujarVertical(l, f, y0 + 54, 13);
        if (mensaje > 0) FuenteMini::centrado(l, y0 + h - 11, L"PARTIDA GUARDADA", Color(120, 255, 140));
        else FuenteMini::centrado(l, y0 + h - 11, L"SE GUARDA EL INICIO DE ESTA ESCENA", Color(130, 130, 150), false);
    }
};
