#pragma once
#include <vector>
#include "Escena.h"
#include "FondoMenu.h"
#include "InterfazMenu.h"
class EscenaCreditos : public Escena {
private:
    enum Tipo { TITULO, ENCABEZADO, NOMBRE, CHICO, ESPACIO };
    struct Linea {
        Tipo tipo;
        std::wstring texto;
    };
    std::vector<Linea> lineas;
    double desplazamiento;
    int altoTotal;
    static constexpr double VELOCIDAD = 11.0;
    int altoDe(const Linea& ln) const {
        switch (ln.tipo) {
        case TITULO: return SPR("Juego/titulo_grande")->getAlto() + 6;
        case ENCABEZADO: return 13;
        case NOMBRE: return 11;
        case CHICO: return 3;
        default: return 9;
        }
    }
public:
    explicit EscenaCreditos(Contexto& c) : Escena(c), desplazamiento(0), altoTotal(0) {
        lineas = {
            { TITULO, L"" }, { ESPACIO, L"" },
            { ENCABEZADO, L"DEVELOPING TEAM" }, { NOMBRE, L"AUNOMO PRODUCTIONS" }, { ESPACIO, L"" },
            { ENCABEZADO, L"DESIGN" }, { NOMBRE, L"THE WALT DISNEY STUDIOS" }, { ESPACIO, L"" },
            { ENCABEZADO, L"VOICES" }, { NOMBRE, L"THE WALT DISNEY STUDIOS" }, { NOMBRE, L"SPANISH" }, { ESPACIO, L"" },
            { NOMBRE, L"TODOS LOS CR\x00C9" L"DITOS DE LA" }, { NOMBRE, L"PEL\x00CD" L"CULA RALPH EL DEMOLEDOR" },
            { NOMBRE, L"CORRESPONDIENTES" }, { ESPACIO, L"" }, { ESPACIO, L"" },
            { ENCABEZADO, L"SPRITES" }, { NOMBRE, L"FIX-IT FELIX JR.: 125SCRATCH" },
            { CHICO, L"\"Sprites by 125scratch, from my remake Fix-It Felix Classic\"" },
            { CHICO, L"Hojas de Sugar Rush, Hero's Duty y escenarios generadas con IA" },
            { CHICO, L"y corregidas cuadro por cuadro por el equipo" }, { ESPACIO, L"" },
            { ENCABEZADO, L"CURSO" }, { NOMBRE, L"ALGORITMOS - 1ACC0265" }, { NOMBRE, L"UPC 2026-20" }, { ESPACIO, L"" },
            { CHICO, L"Personajes y juegos \x00A9" L"Disney - Ralph el Demoledor. Uso acad\x00E9mico." },
            { ESPACIO, L"" }, { ESPACIO, L"" }, { ESPACIO, L"" },
            { ENCABEZADO, L"\x00A1GRACIAS POR JUGAR!" },
        };
        for (const auto& ln : lineas) altoTotal += altoDe(ln);
    }
    void entrar() override {
        ctx.audio.reproducirMusica(L"cr\x00E9" L"ditos.wav");
        desplazamiento = 0;
    }
    void actualizar(double dt) override {
        ctx.fondo.actualizar(dt);
        desplazamiento += VELOCIDAD * dt;
        if (ctx.teclado.cancelar()) {
            ctx.audio.efecto(L"menu_atras.wav");
            ctx.irA(IdEscena::MENU_PRINCIPAL);
            return;
        }
        if (desplazamiento > altoTotal + 104) ctx.irA(IdEscena::MENU_PRINCIPAL);
    }
    void dibujar(Lienzo& l) override {
        const int arriba = 2, abajo = 104;
        int w = 232, x0 = (l.getAncho() - w) / 2;
        ctx.fondo.marcarPanel(x0, x0 + w);
        ctx.fondo.dibujar(l);
        Interfaz::panel(l, x0, arriba - 2, w, abajo - arriba + 4);
        Lienzo& capa = l;
        l.recortar(x0 + 2, arriba, w - 4, abajo - arriba);
        int y = abajo - (int)desplazamiento;
        for (const auto& ln : lineas) {
            int h = altoDe(ln);
            if (y + h > arriba - 30 && y < abajo + 10) {
                switch (ln.tipo) {
                case TITULO: {
                    const Sprite* t = SPR("Juego/titulo_grande");
                    t->dibujar(capa, (l.getAncho() - t->getAncho()) / 2, y);
                    break;
                }
                case ENCABEZADO: ctx.fuentes.amarilla.dibujarCentrado(capa, y + 3, ln.texto); break;
                case NOMBRE: ctx.fuentes.blanca.dibujarCentrado(capa, y + 1, ln.texto); break;
                case CHICO: capa.textoCentrado(y + 1, ln.texto, Color(200, 220, 255)); break;
                default: break;
                }
            }
            y += h;
        }
        l.quitarRecorte();
        const wchar_t degr[3] = { Glifo::OSCURO, Glifo::MEDIO, Glifo::CLARO };
        for (int i = 0; i < 3; i++)
            for (int xx = x0 + 2; xx < x0 + w - 2; xx++) {
                l.sombrear(xx, arriba + i, degr[i], Interfaz::PANEL);
                l.sombrear(xx, abajo - 1 - i, degr[i], Interfaz::PANEL);
            }
        int ancho = Interfaz::anchoTecla(L"ESC") + 6 + FuentePixel::medir(L"SALTAR");
        int ix = (l.getAncho() - ancho) / 2, iy = 107;
        l.panelTranslucido(ix - 6, iy - 3, ancho + 12, 17, Colores::NEGRO, Glifo::OSCURO);
        Interfaz::teclaConTexto(l, ctx.fuentes, ix, iy, L"ESC");
        ctx.fuentes.blanca.dibujarConSombra(l, ix + Interfaz::anchoTecla(L"ESC") + 6, iy + 1, L"SALTAR");
    }
};
