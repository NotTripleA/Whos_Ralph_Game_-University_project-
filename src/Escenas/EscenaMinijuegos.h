#pragma once
#include "Escena.h"
#include "FondoMenu.h"
#include "InterfazMenu.h"
class EscenaMinijuegos : public Escena {
private:
    ListaOpciones opciones;
    static int ultimaSeleccion;
public:
    explicit EscenaMinijuegos(Contexto& c)
        : Escena(c), opciones({ L"FIX IT: FELIX JR.", L"HERO'S DUTY", L"SUGAR RUSH", L"ATR\x00C1S" }, ultimaSeleccion) {}
    void entrar() override { ctx.audio.reproducirMusica(L"menu.wav"); }
    void actualizar(double dt) override {
        ctx.fondo.actualizar(dt);
        opciones.actualizar(dt);
        if (ctx.teclado.cancelar()) { ctx.audio.efecto(L"menu_atras.wav"); ctx.irA(IdEscena::MENU_PRINCIPAL); return; }
        int elegida = opciones.manejar(ctx);
        ultimaSeleccion = opciones.getSeleccion();
        switch (elegida) {
        case 0: ctx.irA(IdEscena::MINI_FIX_IT); break;
        case 1: ctx.irA(IdEscena::MINI_HEROS_DUTY); break;
        case 2: ctx.irA(IdEscena::MINI_SUGAR_RUSH); break;
        case 3: ultimaSeleccion = 0; ctx.irA(IdEscena::MENU_PRINCIPAL); break;
        }
    }
    void dibujar(Lienzo& l) override {
        int w = opciones.anchoMaximo() + 52, px = (l.getAncho() - w) / 2;
        ctx.fondo.marcarPanel(px, px + w);
        ctx.fondo.dibujar(l, false);
        Interfaz::titulo(l, 5);
        Interfaz::panel(l, px, 34, w, 74);
        ctx.fondo.dibujarDesfile(l);
        ctx.fuentes.roja.dibujarCentrado(l, 42, L"MINIJUEGOS");
        opciones.dibujarVertical(l, ctx.fuentes, 58, 12);
        Interfaz::ayuda(l, L"Use WASD o \x2191\x2193\x2190\x2192 para cambiar de opci\x00F3n",
            L"X o ENTER para seleccionar  \x00B7  ESC para volver");
    }
};
int EscenaMinijuegos::ultimaSeleccion = 0;
