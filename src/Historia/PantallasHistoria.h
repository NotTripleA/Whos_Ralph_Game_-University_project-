#pragma once
#include "EscenaHistoria.h"
#include "../Escenas/FondoMenu.h"
#include "../Escenas/InterfazMenu.h"
class EscenaMenuHistoria : public Escena {
private:
    ListaOpciones opciones;
public:
    explicit EscenaMenuHistoria(Contexto& c) : Escena(c), opciones({ L"CONTINUAR", L"NUEVA PARTIDA", L"ATR\x00C1S" }) {}
    void entrar() override { ctx.audio.reproducirMusica(L"menu.wav"); }
    void actualizar(double dt) override {
        ctx.fondo.actualizar(dt);
        opciones.actualizar(dt);
        if (ctx.teclado.cancelar()) { ctx.audio.efecto(L"menu_atras.wav"); ctx.irA(IdEscena::MENU_PRINCIPAL); return; }
        int r = opciones.manejar(ctx);
        if (r == 0) {
            ctx.progreso.cargar();
            ctx.irA(PuntosHistoria::escena(ctx.progreso.getPunto()));
        }
        else if (r == 1) {
            ctx.progreso.reiniciar();
            ctx.irA(IdEscena::PROLOGO);
        }
        else if (r == 2) ctx.irA(IdEscena::MENU_PRINCIPAL);
    }
    void dibujar(Lienzo& l) override {
        int w = opciones.anchoMaximo() + 60, px = (l.getAncho() - w) / 2;
        ctx.fondo.marcarPanel(px, px + w);
        ctx.fondo.dibujar(l, false);
        Interfaz::titulo(l, 5);
        Interfaz::panel(l, px, 34, w, 64);
        ctx.fondo.dibujarDesfile(l);
        ctx.fuentes.roja.dibujarCentrado(l, 40, L"MODO HISTORIA");
        opciones.dibujarVertical(l, ctx.fuentes, 56, 13);
        Interfaz::ayuda(l, L"Tu partida se guarda sola al empezar cada escena", L"X o ENTER para seleccionar  \x00B7  ESC para volver");
    }
};
class EscenaPerdiste : public Escena {
private:
    ListaOpciones opciones;
    double tiempo;
public:
    explicit EscenaPerdiste(Contexto& c) : Escena(c), opciones({ L"REINTENTAR", L"SALIR AL MEN\x00DA" }), tiempo(0) {}
    void entrar() override { ctx.audio.detenerMusica(); ctx.audio.detenerVoz(); ctx.audio.efecto(L"gameover.wav"); }
    void actualizar(double dt) override {
        tiempo += dt;
        opciones.actualizar(dt);
        if (tiempo < 1.0) return;
        int r = opciones.manejar(ctx);
        if (r == 0) ctx.irA(ctx.reintentar);
        else if (r == 1) ctx.irA(IdEscena::MENU_PRINCIPAL);
    }
    void dibujar(Lienzo& l) override {
        l.limpiar(Color(10, 0, 0));
        for (int y = l.yMin(); y < l.yMax(); y += 2)
            for (int x = l.xMin(); x < l.xMax(); x++) l.sombrear(x, y, Glifo::CLARO, Color(60, 0, 0));
        const std::wstring t = L"YOU LOST";
        int x0 = (l.getAncho() - (int)t.size() * 16) / 2;
        for (size_t i = 0; i < t.size(); i++) {
            Lienzo chico(8, 8);
            chico.limpiar(Color(1, 1, 1));
            ctx.fuentes.roja.dibujarCaracter(chico, 0, 0, t[i]);
            for (int y = 0; y < 8; y++)
                for (int x = 0; x < 8; x++) {
                    const Celda& k = chico.celda(x, y);
                    if (k.fg == Color(1, 1, 1)) continue;
                    l.rectangulo(x0 + (int)i * 16 + x * 2, 20 + y * 2, 2, 2, k.fg);
                }
        }
        FuenteMini::centrado(l, 46, L"No completaste los objetivos del nivel.", Color(230, 200, 200));
        Interfaz::panel(l, 60, 60, 120, 40, true);
        opciones.dibujarVertical(l, ctx.fuentes, 70, 14);
    }
};
class EscenaGanaste : public Escena {
private:
    double tiempo;
public:
    explicit EscenaGanaste(Contexto& c) : Escena(c), tiempo(0) {}
    void entrar() override { ctx.audio.detenerVoz(); ctx.audio.efecto(L"etapa.wav"); }
    void actualizar(double dt) override {
        tiempo += dt;
        if (tiempo > 1.0 && ctx.teclado.aceptar()) { ctx.audio.efecto(L"menu_aceptar.wav"); ctx.irA(IdEscena::CINE_FINAL); }
    }
    void dibujar(Lienzo& l) override {
        l.limpiar(Color(0, 10, 4));
        for (int y = l.yMin(); y < l.yMax(); y += 2)
            for (int x = l.xMin(); x < l.xMax(); x++) l.sombrear(x, y, Glifo::CLARO, Color(0, 60, 20));
        const std::wstring t = L"YOU WIN";
        int x0 = (l.getAncho() - (int)t.size() * 16) / 2;
        for (size_t i = 0; i < t.size(); i++) {
            Lienzo chico(8, 8);
            chico.limpiar(Color(1, 1, 1));
            ctx.fuentes.amarilla.dibujarCaracter(chico, 0, 0, t[i]);
            for (int y = 0; y < 8; y++)
                for (int x = 0; x < 8; x++) {
                    const Celda& k = chico.celda(x, y);
                    if (k.fg == Color(1, 1, 1)) continue;
                    l.rectangulo(x0 + (int)i * 16 + x * 2, 20 + y * 2, 2, 2, k.fg);
                }
        }
        FuenteMini::centrado(l, 46, L"Turbo fue derrotado. El arcade est\x00E1 a salvo.", Color(200, 240, 200));
        wchar_t num[24];
        swprintf_s(num, L"PUNTUACI\x00D3N: %07d", ctx.progreso.getPuntuacion());
        FuenteMini::centrado(l, 58, num, Colores::AMARILLO);
        for (int i = 0; i < 40; i++) {
            int px = (i * 53 + (int)(tiempo * 30 * (1 + i % 3))) % 240, py = (i * 31 + (int)(tiempo * 50)) % 130 - 7;
            l.sombrear(px, py, Glifo::BLOQUE, i % 3 == 0 ? Color(255, 120, 200) : i % 3 == 1 ? Color(120, 230, 255) : Color(255, 230, 90));
        }
        if (tiempo > 1.0 && ((int)(tiempo * 2)) % 2 == 0) FuenteMini::centrado(l, 80, L"PRESIONA X O ENTER PARA CONTINUAR", Colores::BLANCO);
    }
};
class EscenaFinNivel : public EscenaHistoria {
private:
    int nivel;
    double tiempo;
    IdEscena siguiente() const { return nivel == 1 ? IdEscena::CINE_CENTRAL : IdEscena::CINE_SUGAR_LLEGADA; }
protected:
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"menu.wav"); }
    void actualizarHistoria(double dt) override {
        tiempo += dt;
        ctx.fondo.actualizar(dt);
        if (tiempo > 1.0 && ctx.teclado.aceptar()) {
            ctx.audio.efecto(L"menu_aceptar.wav");
            ctx.progreso.setPunto(PuntosHistoria::nombre(siguiente()));
            ctx.progreso.guardar();
            ctx.irA(siguiente());
        }
    }
    void dibujarHistoria(Lienzo& l) override {
        int w = 220, x0 = (l.getAncho() - w) / 2;
        ctx.fondo.marcarPanel(x0, x0 + w);
        ctx.fondo.dibujar(l);
        Interfaz::panel(l, x0, 8, w, 102, true);
        ctx.fuentes.amarilla.dibujarCentrado(l, 14, nivel == 1 ? L"NIVEL 1 COMPLETADO" : L"NIVEL 2 COMPLETADO");
        FuenteMini::centrado(l, 26, nivel == 1 ? L"LA PERSPECTIVA DE FELIX: \x00BFQUI\x00C9N ES RALPH?" : L"LA PERSPECTIVA DE CALHOUN: \x00BFQUI\x00C9N ES RALPH?", Color(180, 200, 255));
        const Progreso& p = ctx.progreso;
        dibujarComunes(l, x0, p);
        int sx = x0 + 12, sy = 74;
        if (nivel == 1) {
            FuenteMini::texto(l, sx, sy, L"EN BAD-ANON RALPH ELIGI\x00D3:", Color(200, 200, 215));
            FuenteMini::texto(l, sx + 108, sy, p.decision("badanon") == 1 ? L"ACEPTACI\x00D3N" : L"REBELI\x00D3N", Color(120, 255, 140));
            FuenteMini::texto(l, sx, sy + 10, L"FELIX DECIDI\x00D3:", Color(200, 200, 215));
            FuenteMini::texto(l, sx + 64, sy + 10, p.decision("felix_invita") == 1 ? L"INVITAR A RALPH A LA FIESTA" : L"DARLE UN RESPIRO FUERA DEL JUEGO", Color(120, 255, 140));
        }
        else {
            bool escuadron = p.decision("heros") != 2;
            FuenteMini::texto(l, sx, sy, L"CALHOUN DECIDI\x00D3:", Color(200, 200, 215));
            FuenteMini::texto(l, sx + 68, sy, escuadron ? L"SALVAR AL ESCUADR\x00D3N" : L"SALVAR A RALPH", Color(120, 255, 140));
            FuenteMini::texto(l, sx, sy + 10, L"FINAL:", Color(200, 200, 215));
            FuenteMini::texto(l, sx + 28, sy + 10, escuadron ? L"HACER LO CORRECTO" : L"UN NUEVO AMIGO", Colores::AMARILLO);
        }
        if (((int)(tiempo * 2)) % 2 == 0) FuenteMini::centrado(l, 98, L"PRESIONA X O ENTER PARA CONTINUAR", Colores::BLANCO);
    }
    void dibujarComunes(Lienzo& l, int x0, const Progreso& p) {
        int sx = x0 + 12, sy = 40;
        FuenteMini::texto(l, sx, sy, L"REPUTACI\x00D3N:", Color(200, 200, 215));
        FuenteMini::texto(l, sx + 60, sy, p.etiquetaReputacion(), Colores::AMARILLO);
        FuenteMini::texto(l, sx, sy + 10, L"MEDIDOR DE EMPAT\x00CD" L"A:", Color(200, 200, 215));
        dibujarBarra(l, sx + 84, sy + 11, 50, 4, (p.getEmpatia() + 3) / 6.0, Color(255, 120, 170), Color(50, 30, 50));
        FuenteMini::texto(l, sx + 140, sy + 10, p.etiquetaEmpatia(), Color(255, 150, 190));
        FuenteMini::texto(l, sx, sy + 20, L"NIVELES COMPLETADOS:", Color(200, 200, 215));
        FuenteMini::texto(l, sx + 88, sy + 20, std::to_wstring(p.getNivelesCompletados()) + L" / 3", Colores::AMARILLO);
    }
public:
    EscenaFinNivel(Contexto& c, int n) : EscenaHistoria(c, n == 1 ? IdEscena::NIVEL1_FIN : IdEscena::NIVEL2_FIN), nivel(n), tiempo(0) {}
};
