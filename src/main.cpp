#include "Juego.h"
int wmain(int argc, wchar_t** argv) {
    if (Consola::necesitaConhost(argc, argv) && Consola::relanzarEnConhost(argc, argv)) return 0;
    IdEscena inicio = IdEscena::PRE_MENU;
    for (int i = 1; i + 1 < argc; i++) {
        if (std::wstring(argv[i]) != L"--escena") continue;
        std::string nombre;
        for (const wchar_t* p = argv[i + 1]; *p; p++) nombre += (char)*p;
        if (nombre == "MENU") inicio = IdEscena::MENU_PRINCIPAL;
        else if (nombre == "MINI_FIX_IT") inicio = IdEscena::MINI_FIX_IT;
        else if (nombre == "MINI_HEROS_DUTY") inicio = IdEscena::MINI_HEROS_DUTY;
        else if (nombre == "MINI_SUGAR_RUSH") inicio = IdEscena::MINI_SUGAR_RUSH;
        else inicio = PuntosHistoria::escena(nombre);
    }
    Juego juego;
    juego.ejecutar(inicio);
    return 0;
}
