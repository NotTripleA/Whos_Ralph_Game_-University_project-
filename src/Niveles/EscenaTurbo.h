#pragma once
#include <memory>
#include "../Historia/EscenaHistoria.h"
#include "Turbo/BatallaTurbo.h"
class EscenaBatallaTurbo : public EscenaHistoria {
private:
    std::unique_ptr<Batalla> batalla;
protected:
    void entrarHistoria() override {
        batalla = std::make_unique<Batalla>(ctx);
        ctx.audio.reproducirMusica(L"turbo.wav");
    }
    void actualizarHistoria(double dt) override {
        batalla->actualizar(dt);
        if (batalla->gano()) { ctx.progreso.sumarPuntos(5000); ctx.irA(IdEscena::GANASTE); }
        else if (batalla->perdio()) { ctx.reintentar = IdEscena::NIVEL_SECRETO; ctx.irA(IdEscena::PERDISTE); }
    }
    void dibujarHistoria(Lienzo& l) override { batalla->dibujar(l); }
public:
    explicit EscenaBatallaTurbo(Contexto& c) : EscenaHistoria(c, IdEscena::NIVEL_SECRETO) {}
};
