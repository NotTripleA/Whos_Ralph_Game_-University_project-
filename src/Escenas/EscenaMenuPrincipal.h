#pragma once
#include "Escena.h"
#include "FondoMenu.h"
#include "InterfazMenu.h"
class EscenaMenuPrincipal : public Escena {
private:
    ListaOpciones opciones;
    DialogoConfirmacion salir;
    static int ultimaSeleccion;
public:
    explicit EscenaMenuPrincipal(Contexto& c)
        : Escena(c),
        opciones({ L"JUGAR HISTORIA", L"JUGAR MINIJUEGOS", L"INSTRUCCIONES", L"CR\x00C9" L"DITOS", L"SALIR" }, ultimaSeleccion),
        salir(L"\x00BF" L"EST\x00C1S SEGURO QUE", L"QUIERES SALIR DEL JUEGO?") {}
    void entrar() override { ctx.audio.reproducirMusica(L"menu.wav"); }
    void actualizar(double dt) override {
        ctx.fondo.actualizar(dt);
        if (salir.estaAbierto()) {
            int r = salir.manejar(ctx, dt);
            if (r == 1) ctx.irA(IdEscena::SALIR_DEL_JUEGO);
            return;
        }
        opciones.actualizar(dt);
        if (ctx.teclado.cancelar()) { ctx.audio.efecto(L"menu_atras.wav"); salir.abrir(); return; }
        int elegida = opciones.manejar(ctx);
        ultimaSeleccion = opciones.getSeleccion();
        switch (elegida) {
        case 0:
            if (Progreso::hayPartida()) ctx.irA(IdEscena::HISTORIA_MENU);
            else { ctx.progreso.reiniciar(); ctx.irA(IdEscena::PROLOGO); }
            break;
        case 1: ctx.irA(IdEscena::MINIJUEGOS); break;
        case 2: ctx.irA(IdEscena::INSTRUCCIONES); break;
        case 3: ctx.irA(IdEscena::CREDITOS); break;
        case 4: salir.abrir(); break;
        }
    }
    void dibujar(Lienzo& l) override {
        int w = opciones.anchoMaximo() + 52, px = (l.getAncho() - w) / 2;
        ctx.fondo.marcarPanel(px, px + w);
        ctx.fondo.dibujar(l, false);
        Interfaz::titulo(l, 5);
        Interfaz::panel(l, px, 36, w, 68);
        ctx.fondo.dibujarDesfile(l);
        opciones.dibujarVertical(l, ctx.fuentes, 44, 12);
        Interfaz::ayuda(l, L"Use WASD o \x2191\x2193\x2190\x2192 para cambiar de opci\x00F3n",
            L"X o ENTER para seleccionar la opci\x00F3n");
        if (salir.estaAbierto()) salir.dibujar(l, ctx.fuentes);
    }
};
int EscenaMenuPrincipal::ultimaSeleccion = 0;
