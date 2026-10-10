#pragma once
#define NOMINMAX
#include <windows.h>
#include <vector>
#include <algorithm>
class Teclado {
private:
    HANDLE hEntrada;
    bool abajo[256];
    std::vector<int> pulsadas;
public:
    Teclado() : hEntrada(NULL) { std::fill(abajo, abajo + 256, false); }
    void iniciar(HANDLE entrada) { hEntrada = entrada; FlushConsoleInputBuffer(hEntrada); }
    void actualizar() {
        pulsadas.clear();
        if (!hEntrada) return;
        DWORD pendientes = 0;
        GetNumberOfConsoleInputEvents(hEntrada, &pendientes);
        while (pendientes > 0) {
            INPUT_RECORD eventos[64];
            DWORD leidos = 0;
            ReadConsoleInputW(hEntrada, eventos, pendientes < 64 ? pendientes : 64, &leidos);
            if (leidos == 0) break;
            for (DWORD i = 0; i < leidos; i++) {
                if (eventos[i].EventType == KEY_EVENT) {
                    const KEY_EVENT_RECORD& k = eventos[i].Event.KeyEvent;
                    int vk = k.wVirtualKeyCode & 0xFF;
                    if (k.bKeyDown) { pulsadas.push_back(vk); abajo[vk] = true; }
                    else abajo[vk] = false;
                }
                else if (eventos[i].EventType == FOCUS_EVENT && !eventos[i].Event.FocusEvent.bSetFocus) {
                    std::fill(abajo, abajo + 256, false);
                }
            }
            pendientes = pendientes > leidos ? pendientes - leidos : 0;
        }
    }
    bool presionada(int vk) const { return std::find(pulsadas.begin(), pulsadas.end(), vk) != pulsadas.end(); }
    bool mantenida(int vk) const { return abajo[vk & 0xFF]; }
    bool algunaTecla() const { return !pulsadas.empty(); }
    bool arriba() const { return presionada('W') || presionada(VK_UP); }
    bool abajoP() const { return presionada('S') || presionada(VK_DOWN); }
    bool izquierda() const { return presionada('A') || presionada(VK_LEFT); }
    bool derecha() const { return presionada('D') || presionada(VK_RIGHT); }
    bool aceptar() const { return presionada('X') || presionada(VK_RETURN); }
    bool cancelar() const { return presionada(VK_ESCAPE); }
};
