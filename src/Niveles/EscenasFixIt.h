#pragma once
#include <memory>
#include "../Historia/EscenaHistoria.h"
#include "../Escenas/InterfazMenu.h"
#include "FixIt/JuegoFixIt.h"
class EscenaNivel1 : public EscenaHistoria {
private:
    std::unique_ptr<JuegoFixIt> juego;
protected:
    void entrarHistoria() override { juego = std::make_unique<JuegoFixIt>(ctx, JuegoFixIt::HISTORIA); }
    void actualizarHistoria(double dt) override {
        juego->actualizar(dt);
        if (juego->gano()) { ctx.progreso.sumarPuntos(juego->getPuntos()); ctx.irA(IdEscena::CINE_REUNION); }
        else if (juego->perdio()) { ctx.reintentar = IdEscena::NIVEL1_FIXIT; ctx.irA(IdEscena::PERDISTE); }
    }
    void dibujarHistoria(Lienzo& l) override { juego->dibujar(l); }
public:
    explicit EscenaNivel1(Contexto& c) : EscenaHistoria(c, IdEscena::NIVEL1_FIXIT) {}
};
class EscenaMiniFixIt : public Escena {
private:
    std::unique_ptr<JuegoFixIt> juego;
    DialogoConfirmacion salir;
public:
    explicit EscenaMiniFixIt(Contexto& c) : Escena(c), salir(L"\x00BFSALIR DEL MINIJUEGO?", L"SE GUARDA TU R\x00C9" L"CORD") {}
    void entrar() override { juego = std::make_unique<JuegoFixIt>(ctx, JuegoFixIt::INFINITO); }
    void actualizar(double dt) override {
        if (salir.estaAbierto()) {
            int r = salir.manejar(ctx, dt);
            if (r == 1) ctx.irA(IdEscena::MINIJUEGOS);
            else if (r == 0) ctx.audio.reanudarMusica();
            return;
        }
        if (ctx.teclado.cancelar()) { ctx.audio.efecto(L"menu_atras.wav"); ctx.audio.pausarMusica(); salir.abrir(); return; }
        juego->actualizar(dt);
        if (juego->perdio()) ctx.irA(IdEscena::MINIJUEGOS);
    }
    void dibujar(Lienzo& l) override {
        juego->dibujar(l);
        if (salir.estaAbierto()) salir.dibujar(l, ctx.fuentes);
    }
};
