#pragma once
#include <vector>
#include <cmath>
#include "../Motor/Lienzo.h"
#include "../Motor/Azar.h"
#include "../Motor/Efectos.h"
class EscenarioNeon {
private:
    struct Particula {
        double x, y, vx, vy, vida, vidaMax;
        Color color;
    };
    struct Cohete {
        double x, y, objetivoY;
        Color color;
    };
    struct Edificio {
        int x, ancho, alto;
        std::vector<int> ventanas;
    };
    std::vector<Particula> chispas;
    std::vector<Cohete> cohetes;
    std::vector<Edificio> ciudad;
    CampoEstrellas estrellas;
    double tiempo;
    double proximoCohete;
public:
    static const int HORIZONTE = 98;
    EscenarioNeon() : estrellas(110, 240, 92, 2026), tiempo(0), proximoCohete(0.8) {
        int x = -12;
        while (x < 252) {
            Edificio e;
            e.x = x;
            e.ancho = Azar::entero(10, 22);
            e.alto = Azar::entero(10, 30);
            for (int j = 0; j < (e.alto - 4) / 4; j++)
                for (int i = 0; i < (e.ancho - 4) / 4; i++) {
                    int r = Azar::entero(0, 9);
                    e.ventanas.push_back(r < 4 ? 0 : (r < 8 ? 1 : 2));
                }
            ciudad.push_back(e);
            x += e.ancho + Azar::entero(0, 3);
        }
    }
    void actualizar(double dt) {
        tiempo += dt;
        estrellas.actualizar(dt);
        proximoCohete -= dt;
        if (proximoCohete <= 0) {
            proximoCohete = Azar::real(0.9, 2.2);
            const Color tonos[6] = { Color(255, 214, 64), Color(255, 80, 160), Color(80, 230, 255), Color(120, 255, 120), Color(255, 140, 40), Color(255, 255, 255) };
            double lado = Azar::probabilidad(0.5) ? Azar::real(8, 70) : Azar::real(170, 232);
            if (Azar::probabilidad(0.3)) lado = Azar::real(40, 200);
            cohetes.push_back({ lado, (double)HORIZONTE, Azar::real(-6, 28), tonos[Azar::entero(0, 5)] });
        }
        for (size_t i = 0; i < cohetes.size();) {
            Cohete& c = cohetes[i];
            c.y -= 110 * dt;
            if (c.y <= c.objetivoY) {
                int n = 30;
                for (int k = 0; k < n; k++) {
                    double a = k * 6.2832 / n + Azar::real(-0.1, 0.1), v = Azar::real(14, 34);
                    chispas.push_back({ c.x, c.y, std::cos(a) * v, std::sin(a) * v * 0.8, 0, Azar::real(1.0, 1.6), c.color });
                }
                cohetes.erase(cohetes.begin() + i);
            }
            else i++;
        }
        for (size_t i = 0; i < chispas.size();) {
            Particula& p = chispas[i];
            p.vida += dt;
            p.vy += 18 * dt;
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            if (p.vida >= p.vidaMax) chispas.erase(chispas.begin() + i);
            else i++;
        }
    }
    void dibujar(Lienzo& l) const {
        int x0 = l.xMin(), x1 = l.xMax(), y0 = l.yMin(), y1 = l.yMax();
        const Color c0(5, 2, 24), c1(30, 8, 66), c2(120, 22, 100);
        for (int y = y0; y < HORIZONTE; y++) {
            double k = (double)(y - y0) / (HORIZONTE - y0);
            Color c = k < 0.6 ? c0.mezclar(c1, (float)(k / 0.6)) : c1.mezclar(c2, (float)((k - 0.6) / 0.4));
            for (int x = x0; x < x1; x++) l.pixel(x, y, c);
        }
        estrellas.dibujar(l);
        for (int r = 0; r < 2; r++) {
            double bx = r == 0 ? 26 : 214;
            double angulo = (r == 0 ? -0.35 : 0.35) + std::sin(tiempo * 0.7 + r * 2.1) * 0.45;
            for (int y = HORIZONTE - 1; y > y0; y--) {
                double d = HORIZONTE - y;
                double cx = bx + std::tan(angulo) * d;
                double ancho = 1.5 + d * 0.09;
                for (int x = (int)(cx - ancho); x <= (int)(cx + ancho); x++) {
                    double borde = std::fabs(x - cx) / ancho;
                    l.sombrear(x, y, borde < 0.45 ? Glifo::MEDIO : Glifo::CLARO, Color(255, 246, 200));
                }
            }
        }
        for (const auto& c : cohetes) {
            l.pixel((int)c.x, (int)c.y, Colores::BLANCO);
            l.sombrear((int)c.x, (int)c.y + 1, Glifo::OSCURO, c.color);
            l.sombrear((int)c.x, (int)c.y + 2, Glifo::MEDIO, c.color);
            l.sombrear((int)c.x, (int)c.y + 3, Glifo::CLARO, c.color);
        }
        for (const auto& p : chispas) {
            double f = 1.0 - p.vida / p.vidaMax;
            if (f > 0.7) l.pixel((int)p.x, (int)p.y, p.color);
            else l.sombrear((int)p.x, (int)p.y, f > 0.45 ? Glifo::OSCURO : f > 0.2 ? Glifo::MEDIO : Glifo::CLARO, p.color);
        }
        for (const auto& e : ciudad) {
            int top = HORIZONTE - e.alto;
            l.rectangulo(e.x, top, e.ancho, e.alto, Color(18, 8, 38));
            for (int i = e.x; i < e.x + e.ancho; i++) l.pixel(i, top, Color(90, 40, 160));
            int k = 0;
            for (int j = 0; j < (e.alto - 4) / 4; j++)
                for (int i = 0; i < (e.ancho - 4) / 4; i++, k++) {
                    int v = e.ventanas[k];
                    if (v == 0) continue;
                    bool parpadea = ((k * 7 + e.x) % 23 == 0) && ((int)(tiempo * 2 + k) % 3 == 0);
                    if (parpadea) continue;
                    Color cv = v == 1 ? Color(255, 214, 120) : Color(110, 220, 255);
                    l.pixel(e.x + 2 + i * 4, top + 3 + j * 4, cv);
                    l.pixel(e.x + 3 + i * 4, top + 3 + j * 4, cv);
                }
        }
        for (int x = x0; x < x1; x++) {
            l.sombrear(x, HORIZONTE - 1, Glifo::MEDIO, Color(255, 60, 170));
            l.pixel(x, HORIZONTE, Color(255, 90, 200));
        }
        const Color piso(14, 2, 26), linea(255, 50, 200), lineaC(60, 230, 255);
        for (int y = HORIZONTE + 1; y < y1; y++)
            for (int x = x0; x < x1; x++) l.pixel(x, y, piso);
        double avance = std::fmod(tiempo * 0.5, 1.0);
        for (int n = 0; n < 9; n++) {
            double t = (n + avance) / 9.0;
            int y = HORIZONTE + 1 + (int)(t * t * (y1 - HORIZONTE));
            for (int x = x0; x < x1; x++) {
                l.pixel(x, y, linea);
                l.sombrear(x, y - 1, Glifo::CLARO, linea);
            }
        }
        for (int i = -14; i <= 14; i++) {
            double xs = 120 + i * 9.0, xb = 120 + i * 40.0;
            for (int y = HORIZONTE + 1; y < y1; y++) {
                double k = (double)(y - HORIZONTE) / (y1 - HORIZONTE);
                l.pixel((int)(xs + (xb - xs) * k), y, lineaC);
            }
        }
        int paso = (int)(tiempo * 10);
        int n = 0;
        auto foco = [&](int x, int y) {
            bool on = (n + paso) % 3 == 0;
            Color c = on ? Color(255, 236, 110) : Color(90, 66, 20);
            l.pixel(x, y, c); l.pixel(x + 1, y, c); l.pixel(x, y + 1, c); l.pixel(x + 1, y + 1, c);
            if (on) { l.sombrear(x - 1, y, Glifo::CLARO, c); l.sombrear(x + 2, y + 1, Glifo::CLARO, c); }
            n++;
        };
        for (int x = x0 + 1; x < x1 - 2; x += 5) foco(x, y0 + 1);
        for (int y = y0 + 6; y < y1 - 8; y += 5) foco(x1 - 3, y);
        for (int y = y1 - 9; y > y0 + 5; y -= 5) foco(x0 + 1, y);
    }
};
