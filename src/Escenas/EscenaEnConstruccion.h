#pragma once
#include "Escena.h"
#include "FondoMenu.h"
#include "InterfazMenu.h"
#include "../Motor/Animacion.h"
class EscenaEnConstruccion : public Escena {
private:
    std::wstring titulo;
    std::wstring subtitulo;
    IdEscena volverA;
    Animacion felix;
    double tiempo;
public:
    EscenaEnConstruccion(Contexto& c, const std::wstring& t, const std::wstring& s, IdEscena volver)
        : Escena(c), titulo(t), subtitulo(s), volverA(volver), tiempo(0) {
        felix = Animacion(BancoSprites::instancia().lista("01_Fix-It_Felix_Jr/Felix/fila_1/felix_f1_", { 9, 5 }), 4);
    }
    void entrar() override { ctx.audio.reproducirMusica(L"menu.wav"); }
    void actualizar(double dt) override {
        tiempo += dt;
        felix.actualizar(dt);
        ctx.fondo.actualizar(dt);
        if (ctx.teclado.cancelar() || ctx.teclado.aceptar()) {
            ctx.audio.efecto(L"menu_atras.wav");
            ctx.irA(volverA);
        }
    }
    void dibujar(Lienzo& l) override {
        int w = 200, x0 = (l.getAncho() - w) / 2;
        ctx.fondo.marcarPanel(x0, x0 + w);
        ctx.fondo.dibujar(l);
        Interfaz::titulo(l, 5);
        Interfaz::panel(l, x0, 34, w, 72);
        ctx.fuentes.amarilla.dibujarCentrado(l, 42, titulo);
        ctx.fuentes.blanca.dibujarCentrado(l, 54, subtitulo);
        const Sprite* s = felix.actual();
        s->dibujar(l, (l.getAncho() - s->getAncho()) / 2, 62);
        if ((int)(tiempo * 2) % 2 == 0) ctx.fuentes.roja.dibujarCentrado(l, 94, L"EN CONSTRUCCI\x00D3N");
        Interfaz::ayuda(l, L"Esta parte se habilitar\x00E1 cuando lleguen sus indicaciones",
            L"ESC o ENTER para volver");
    }
};
