#pragma once
#include <string>
#include "../Escenas/Escena.h"
#include "InterfazHistoria.h"
class EscenaHistoria : public Escena {
protected:
    IdEscena punto;
    Progreso inicio;
    MenuPausa pausa;
    Notificacion aviso;
    virtual void entrarHistoria() {}
    virtual void actualizarHistoria(double dt) = 0;
    virtual void dibujarHistoria(Lienzo& l) = 0;
    void decisionTomada(const std::wstring& mensaje, int reputacion, int empatia) {
        ctx.progreso.cambiarReputacion(reputacion);
        ctx.progreso.cambiarEmpatia(empatia);
        ctx.progreso.contarDecision(reputacion + empatia);
        auto signo = [](int v) { return (v >= 0 ? L"+" : L"") + std::to_wstring(v); };
        aviso.mostrar(mensaje,
            signo(reputacion) + L" REPUTACI\x00D3N   " + signo(empatia) + L" EMPAT\x00CD" L"A");
        ctx.audio.efecto(L"recordara.wav");
    }
public:
    EscenaHistoria(Contexto& c, IdEscena p) : Escena(c), punto(p) {}
    void entrar() override {
        ctx.progreso.setPunto(PuntosHistoria::nombre(punto));
        inicio = ctx.progreso;
        ctx.progreso.guardar();
        entrarHistoria();
    }
    void actualizar(double dt) override {
        if (pausa.estaAbierto()) {
            MenuPausa::Resultado r = pausa.manejar(ctx, dt);
            if (r == MenuPausa::CONTINUAR) ctx.audio.reanudarMusica();
            else if (r == MenuPausa::GUARDAR) { inicio.guardar(); pausa.avisarGuardado(); }
            else if (r == MenuPausa::SALIR) { ctx.audio.detenerVoz(); ctx.irA(IdEscena::MENU_PRINCIPAL); }
            return;
        }
        if (ctx.teclado.cancelar()) {
            ctx.audio.efecto(L"menu_atras.wav");
            ctx.audio.pausarMusica();
            pausa.abrir();
            return;
        }
        aviso.actualizar(dt);
        actualizarHistoria(dt);
    }
    void dibujar(Lienzo& l) override {
        dibujarHistoria(l);
        aviso.dibujar(l);
        pausa.dibujar(l, ctx.fuentes, ctx.progreso);
    }
};
