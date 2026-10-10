#pragma once
#include <memory>
#include "../Historia/EscenaHistoria.h"
#include "../Escenas/InterfazMenu.h"
#include "SugarRush/CarreraSugar.h"
class EscenaNivel3 : public EscenaHistoria {
private:
    std::unique_ptr<CarreraSugar> carrera;
    bool eventoAplicado;
protected:
    void entrarHistoria() override { carrera = std::make_unique<CarreraSugar>(ctx, CarreraSugar::HISTORIA); }
    void actualizarHistoria(double dt) override {
        carrera->actualizar(dt);
        int e = carrera->getEleccionEvento();
        if (e != 0 && !eventoAplicado) {
            eventoAplicado = true;
            ctx.progreso.decidir("obstaculo", e);
            if (e == 1) decisionTomada(L"VANELLOPE RECORDAR\x00C1 ESTO...", 1, 0);
            else decisionTomada(L"VANELLOPE RECORDAR\x00C1 ESTO...", 0, 0);
        }
        if (carrera->termino()) {
            ctx.progreso.sumarPuntos(carrera->getPuntos());
            ctx.progreso.completarNivel(3);
            ctx.irA(IdEscena::CINE_TURBO);
        }
        else if (carrera->perdio()) { ctx.reintentar = IdEscena::NIVEL3_CARRERA; ctx.irA(IdEscena::PERDISTE); }
    }
    void dibujarHistoria(Lienzo& l) override { carrera->dibujar(l); }
public:
    explicit EscenaNivel3(Contexto& c) : EscenaHistoria(c, IdEscena::NIVEL3_CARRERA), eventoAplicado(false) {}
};
class EscenaMiniSugarRush : public Escena {
private:
    std::unique_ptr<CarreraSugar> carrera;
    DialogoConfirmacion salir;
public:
    explicit EscenaMiniSugarRush(Contexto& c) : Escena(c), salir(L"\x00BFSALIR DEL MINIJUEGO?", L"SE GUARDA TU R\x00C9" L"CORD") {}
    void entrar() override { carrera = std::make_unique<CarreraSugar>(ctx, CarreraSugar::INFINITO); }
    void actualizar(double dt) override {
        if (salir.estaAbierto()) {
            int r = salir.manejar(ctx, dt);
            if (r == 1) ctx.irA(IdEscena::MINIJUEGOS);
            else if (r == 0) ctx.audio.reanudarMusica();
            return;
        }
        if (ctx.teclado.cancelar()) {
            ctx.audio.efecto(L"menu_atras.wav");
            if (carrera->enMeta()) { ctx.irA(IdEscena::MINIJUEGOS); return; }
            ctx.audio.pausarMusica();
            salir.abrir();
            return;
        }
        carrera->actualizar(dt);
        if (carrera->perdio()) ctx.irA(IdEscena::MINIJUEGOS);
    }
    void dibujar(Lienzo& l) override {
        carrera->dibujar(l);
        if (salir.estaAbierto()) salir.dibujar(l, ctx.fuentes);
    }
};
