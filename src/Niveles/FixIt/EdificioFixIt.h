#pragma once
#include <string>
#include "../../Motor/BancoSprites.h"
#include "../../Motor/Azar.h"
class EdificioFixIt {
public:
    static const int PISOS = 6;
    static const int COLUMNAS = 5;
    static const int X = 8;
    static const int ALTO_PISO = 56;
    static const int DANO_MAX = 3;
    static const int FILAS_VISTA = 135;
    static const int ALTO_RALPH = 47;
private:
    int dano[PISOS][COLUMNAS][2];
    std::string material[PISOS];
public:
    EdificioFixIt() {
        for (int p = 0; p < PISOS; p++)
            for (int c = 0; c < COLUMNAS; c++) { dano[p][c][0] = 0; dano[p][c][1] = 0; }
        dano[0][2][0] = dano[0][2][1] = -1;
        dano[1][2][1] = -1;
        pintar(0);
    }
    static int topePiso(int p) { return 8 + (PISOS - 1 - p) * ALTO_PISO; }
    static int centroVentana(int c) { return X + 48 + 32 * c; }
    static int piesFelix(int p) { return topePiso(p) + 43; }
    static int pisoInferior(int etapa) { return 2 * etapa; }
    static int pisoSuperior(int etapa) { return 2 * etapa + 1; }
    static int piesRalph(int etapa) { return topePiso(pisoSuperior(etapa)) + 11; }
    static int vistaEtapa(int etapa) { return piesRalph(etapa) - ALTO_RALPH; }
    static int suelo() { return topePiso(0) + ALTO_PISO + 16; }
    static int etapas() { return PISOS / 2; }
    bool hayVidrio(int p, int c, int k) const { return dano[p][c][k] >= 0; }
    bool hayVentana(int p, int c) const { return hayVidrio(p, c, 0) || hayVidrio(p, c, 1); }
    void pintar(int variante) {
        const char* m[4] = { "ladrillo_rojo", "crema", "ladrillo_verde", "ladrillo_morado" };
        for (int p = 2; p < PISOS; p++) material[p] = m[variante % 4];
    }
    void romper(int estabilidadObjetivo) {
        for (int p = 0; p < PISOS; p++)
            for (int c = 0; c < COLUMNAS; c++)
                for (int k = 0; k < 2; k++)
                    if (hayVidrio(p, c, k)) dano[p][c][k] = Azar::entero(0, DANO_MAX);
        for (int intento = 0; intento < 2000 && estabilidad() > estabilidadObjetivo + 1; intento++) {
            int p = Azar::entero(0, PISOS - 1), c = Azar::entero(0, COLUMNAS - 1), k = Azar::entero(0, 1);
            if (hayVidrio(p, c, k) && dano[p][c][k] < DANO_MAX) dano[p][c][k]++;
        }
        for (int intento = 0; intento < 2000 && estabilidad() < estabilidadObjetivo - 1; intento++) {
            int p = Azar::entero(0, PISOS - 1), c = Azar::entero(0, COLUMNAS - 1), k = Azar::entero(0, 1);
            if (hayVidrio(p, c, k) && dano[p][c][k] > 0) dano[p][c][k]--;
        }
        for (int p = 0; p < PISOS; p++) {
            int rotas = 0;
            for (int c = 0; c < COLUMNAS; c++) if (!ventanaSana(p, c)) rotas++;
            while (rotas < 2) {
                int c = Azar::entero(0, COLUMNAS - 1);
                if (hayVidrio(p, c, 0) && ventanaSana(p, c)) { dano[p][c][0] = DANO_MAX; rotas++; }
            }
        }
    }
    void danar(int p, int c) {
        if (p < 0 || p >= PISOS || c < 0 || c >= COLUMNAS) return;
        int k = Azar::entero(0, 1);
        if (!hayVidrio(p, c, k)) k = 1 - k;
        if (hayVidrio(p, c, k) && dano[p][c][k] < DANO_MAX) dano[p][c][k]++;
    }
    int reparar(int p, int c) {
        if (!hayVentana(p, c) || ventanaSana(p, c)) return 0;
        int k = (hayVidrio(p, c, 1) && dano[p][c][1] > 0) ? 1 : 0;
        dano[p][c][k]--;
        return ventanaSana(p, c) ? 2 : 1;
    }
    bool ventanaSana(int p, int c) const {
        for (int k = 0; k < 2; k++) if (hayVidrio(p, c, k) && dano[p][c][k] > 0) return false;
        return true;
    }
    bool etapaReparada(int e) const {
        for (int c = 0; c < COLUMNAS; c++) if (!ventanaSana(e, c)) return false;
        return true;
    }
    int ventanasRotas(int e) const {
        int n = 0;
        for (int c = 0; c < COLUMNAS; c++) if (!ventanaSana(e, c)) n++;
        return n;
    }
    int estabilidad() const {
        int total = 0, malo = 0;
        for (int p = 0; p < PISOS; p++)
            for (int c = 0; c < COLUMNAS; c++)
                for (int k = 0; k < 2; k++)
                    if (hayVidrio(p, c, k)) { total += DANO_MAX; malo += dano[p][c][k]; }
        return total ? (int)(100.0 * (total - malo) / total + 0.5) : 100;
    }
    void dibujar(Lienzo& l) const {
        auto& b = BancoSprites::instancia();
        const std::string sec = "01_Fix-It_Felix_Jr/Edificio/secciones/";
        b.obtener(sec + "cornisa_techo")->dibujar(l, X, 0);
        for (int p = PISOS - 1; p >= 2; p--) {
            b.obtener(sec + "piso_" + material[p])->dibujar(l, X, topePiso(p));
            b.obtener(sec + "franja_" + material[p])->dibujar(l, X, topePiso(p) + 48);
        }
        b.obtener(sec + "planta_baja_entrada")->dibujar(l, X, topePiso(1));
        b.obtener(sec + "base_escalones")->dibujar(l, X, topePiso(0) + ALTO_PISO);
        for (int p = 0; p < PISOS; p++)
            for (int c = 0; c < COLUMNAS; c++)
                for (int k = 0; k < 2; k++) {
                    if (!hayVidrio(p, c, k) || dano[p][c][k] == 0) continue;
                    char id[96];
                    sprintf_s(id, "01_Fix-It_Felix_Jr/Edificio/ventanas/ventana/ventana_%02d", dano[p][c][k] + 1);
                    b.obtener(id)->dibujar(l, X + 40 + 32 * c, topePiso(p) + 8 + 16 * k);
                }
    }
};
