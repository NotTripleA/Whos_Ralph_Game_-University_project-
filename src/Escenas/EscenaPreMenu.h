#pragma once
#include <cmath>
#include "Escena.h"
#include "InterfazMenu.h"
#include "../Motor/Efectos.h"
class EscenaPreMenu : public Escena {
private:
    CampoEstrellas estrellas;
    double tiempo;
    double tiempoSalida;
    bool saliendo;
    static const int TITULO_Y = 18;
    static constexpr double CAIDA = 0.9;
    int posicionTitulo() const {
        if (tiempo >= CAIDA) return TITULO_Y;
        double t = tiempo / CAIDA;
        double rebote = t < 0.7 ? (t / 0.7) * (t / 0.7) : 1.0 - std::sin((t - 0.7) / 0.3 * 3.14159) * 0.08;
        return (int)(-50 + (TITULO_Y + 50) * rebote);
    }
public:
    explicit EscenaPreMenu(Contexto& c) : Escena(c), estrellas(110, 240, 120, 7), tiempo(0), tiempoSalida(0), saliendo(false) {}
    void entrar() override {
        ctx.audio.reproducirMusica(L"pre-menu.wav");
        tiempo = 0;
        saliendo = false;
    }
    void actualizar(double dt) override {
        tiempo += dt;
        estrellas.actualizar(dt);
        if (saliendo) {
            tiempoSalida += dt;
            if (tiempoSalida > 0.9) ctx.irA(IdEscena::MENU_PRINCIPAL);
            return;
        }
        if (tiempo > CAIDA && ctx.teclado.presionada(VK_RETURN)) {
            saliendo = true;
            tiempoSalida = 0;
            ctx.audio.detenerMusica();
            ctx.audio.efecto(L"inicio.wav");
        }
        else if (tiempo <= CAIDA && ctx.teclado.presionada(VK_RETURN)) tiempo = CAIDA;
    }
    void dibujar(Lienzo& l) override {
        l.limpiar(Color(4, 4, 14));
        estrellas.dibujar(l);
        int temblor = 0;
        if (tiempo > CAIDA && tiempo < CAIDA + 0.35) temblor = ((int)(tiempo * 40) % 2) ? 1 : -1;
        const Sprite* titulo = SPR("Juego/titulo_grande");
        int tx = (l.getAncho() - titulo->getAncho()) / 2 + temblor;
        titulo->dibujar(l, tx, posicionTitulo());
        if (tiempo > CAIDA && tiempo < CAIDA + 0.6) {
            double k = (tiempo - CAIDA) / 0.6;
            wchar_t g = k < 0.33 ? Glifo::OSCURO : k < 0.66 ? Glifo::MEDIO : Glifo::CLARO;
            int base = TITULO_Y + titulo->getAlto();
            for (int x = tx - 6; x < tx + titulo->getAncho() + 6; x++)
                for (int y = base - 2; y < base + 3; y++)
                    if ((x * 7 + y * 3) % 5 < 2) l.sombrear(x, y + (int)(k * 3), g, Color(200, 170, 140));
        }
        if (tiempo > CAIDA) {
            bool visible = saliendo ? ((int)(tiempoSalida * 16) % 2 == 0) : ((int)(tiempo * 2.2) % 2 == 0);
            if (visible) {
                ctx.fuentes.amarilla.dibujarCentrado(l, 80, L"PRESIONA [ENTER]");
                ctx.fuentes.blanca.dibujarCentrado(l, 92, L"PARA CONTINUAR");
            }
        }
        l.textoCentrado(l.yMax() - 4, L"\x00A9The Walt Disney Studios", Color(190, 190, 205));
    }
};
