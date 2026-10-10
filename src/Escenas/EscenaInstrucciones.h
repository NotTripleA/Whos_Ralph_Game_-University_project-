#pragma once
#include <vector>
#include "Escena.h"
#include "FondoMenu.h"
#include "InterfazMenu.h"
#include "../Motor/FuenteMini.h"
class EscenaInstrucciones : public Escena {
private:
    struct Control {
        std::vector<std::wstring> teclas;
        std::wstring accion;
    };
    struct Pagina {
        std::wstring titulo;
        std::vector<Control> controles;
        std::vector<std::wstring> notas;
    };
    std::vector<Pagina> paginas;
    int pagina;
    double tiempo;
public:
    explicit EscenaInstrucciones(Contexto& c) : Escena(c), pagina(0), tiempo(0) {
        paginas.push_back({ L"INSTRUCCIONES", {
            { { L"W", L"\x2191" }, L"SALTAR / AVANZAR" },
            { { L"S", L"\x2193" }, L"BAJAR / AGACHARSE" },
            { { L"A", L"\x2190" }, L"MOVER IZQUIERDA" },
            { { L"D", L"\x2192" }, L"MOVER DERECHA" },
            { { L"X", L"ENTER" }, L"ACCI\x00D3N / ACEPTAR" },
            { { L"ESC" }, L"PAUSA / VOLVER" } }, {} });
        paginas.push_back({ L"NIVEL 1: FIX-IT", {
            { { L"A", L"\x2190" }, L"MOVER IZQUIERDA" },
            { { L"D", L"\x2192" }, L"MOVER DERECHA" },
            { { L"W", L"\x2191" }, L"SUBIR / SALTAR" },
            { { L"S", L"\x2193" }, L"BAJAR DE PISO" },
            { { L"X", L"ENTER" }, L"MARTILLAR" } }, {
            L"REPARA LOS 2 PISOS DEBAJO DE RALPH (3 ETAPAS)",
            L"LADRILLO: -1 VIDA   PASTEL: INVENCIBLE   SALTA LOS PATOS" } });
        paginas.push_back({ L"NIVEL 2: HERO'S DUTY", {
            { { L"\x2191", L"\x2193" }, L"CAMINAR ARRIBA/ABAJO" },
            { { L"\x2190", L"\x2192" }, L"CAMINAR A LOS LADOS" },
            { { L"ESPACIO" }, L"DISPARAR" },
            { { L"ESC" }, L"PAUSA" } }, {
            L"VISTA DESDE ARRIBA: DISPARAS HACIA DONDE MIRAS",
            L"DESTRUYE EL NIDO DE CADA PISO Y SUBE EN EL ASCENSOR",
            L"M\x00C1S INTEGRIDAD = M\x00C1S DA\x00D1O AL CY-BUG GIGANTE" } });
        paginas.push_back({ L"NIVEL 3: SUGAR RUSH", {
            { { L"W", L"\x2191" }, L"ACELERAR" },
            { { L"S", L"\x2193" }, L"FRENAR" },
            { { L"A", L"D" }, L"GIRAR" },
            { { L"ESPACIO" }, L"ACTIVAR GLITCH" } }, {
            L"CONTROLAS A VANELLOPE: 3 VUELTAS HASTA LA META",
            L"EL GLITCH ATRAVIESA OBST\x00C1" L"CULOS Y DA VELOCIDAD",
            L"CADA CHOQUE QUITA INTEGRIDAD AL VEH\x00CD" L"CULO" } });
        paginas.push_back({ L"NIVEL SECRETO: TURBO", {
            { { L"\x2191", L"\x2193" }, L"ELEGIR ACCI\x00D3N" },
            { { L"X", L"ENTER" }, L"CONFIRMAR" },
            { { L"ESPACIO" }, L"PRECISI\x00D3N / HABILIDAD" } }, {
            L"COMBATE POR TURNOS: TU EQUIPO DEPENDE DE TU REPUTACI\x00D3N",
            L"EN EL TURNO DE TURBO MUEVE EL CORAZ\x00D3N Y ESQUIVA",
            L"CADA PERSONAJE TIENE SU PROPIA HABILIDAD CON ESPACIO" } });
    }
    void entrar() override { ctx.audio.reproducirMusica(L"menu.wav"); }
    void actualizar(double dt) override {
        tiempo += dt;
        ctx.fondo.actualizar(dt);
        int n = (int)paginas.size();
        if (ctx.teclado.izquierda()) { pagina = (pagina + n - 1) % n; ctx.audio.efecto(L"menu_mover.wav"); tiempo = 0; }
        if (ctx.teclado.derecha()) { pagina = (pagina + 1) % n; ctx.audio.efecto(L"menu_mover.wav"); tiempo = 0; }
        if (ctx.teclado.cancelar() || ctx.teclado.aceptar()) {
            ctx.audio.efecto(L"menu_atras.wav");
            ctx.irA(IdEscena::MENU_PRINCIPAL);
        }
    }
    void dibujar(Lienzo& l) override {
        int w = 222, x0 = (l.getAncho() - w) / 2;
        ctx.fondo.marcarPanel(x0, x0 + w);
        ctx.fondo.dibujar(l);
        Interfaz::titulo(l, 2);
        Interfaz::panel(l, x0, 27, w, 87);
        const Pagina& p = paginas[pagina];
        ctx.fuentes.roja.dibujarCentrado(l, 31, p.titulo);
        if (((int)(tiempo * 3)) % 2 == 0) {
            ctx.fuentes.amarilla.dibujarCaracter(l, x0 + 4, 31, L'\x2190');
            ctx.fuentes.amarilla.dibujarCaracter(l, x0 + w - 12, 31, L'\x2192');
        }
        FuenteMini::texto(l, x0 + w - 18, 27 + 87 - 11, std::to_wstring(pagina + 1) + L"/" + std::to_wstring(paginas.size()), Color(160, 160, 180));
        int animada = (int)(tiempo * 1.5) % (int)p.controles.size();
        int paso = p.notas.empty() ? 12 : 11;
        int y = 42;
        for (int i = 0; i < (int)p.controles.size(); i++) {
            int x = x0 + 8;
            for (size_t k = 0; k < p.controles[i].teclas.size(); k++) {
                bool hundida = (i == animada) && ((int)(tiempo * 6) % 2 == 0);
                Interfaz::teclaConTexto(l, ctx.fuentes, x, y + (hundida ? 1 : 0), p.controles[i].teclas[k], i == animada);
                x += Interfaz::anchoTecla(p.controles[i].teclas[k]) + 3;
                if (k + 1 < p.controles[i].teclas.size()) x += 3;
            }
            ctx.fuentes.blanca.dibujarConSombra(l, x0 + 80, y + 1, p.controles[i].accion);
            y += paso;
        }
        for (size_t i = 0; i < p.notas.size(); i++) FuenteMini::centrado(l, y + 1 + (int)i * 8, p.notas[i], Colores::AMARILLO);
        Interfaz::ayuda(l, L"A/D o \x2190\x2192 para cambiar de p\x00E1gina",
            L"ESC o ENTER para volver al men\x00FA");
    }
};
