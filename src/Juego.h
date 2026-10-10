#pragma once
#define NOMINMAX
#include <windows.h>
#include <memory>
#include <string>
#include "Motor/Consola.h"
#include "Motor/Teclado.h"
#include "Motor/Audio.h"
#include "Motor/Lienzo.h"
#include "Motor/Efectos.h"
#include "Escenas/Escena.h"
#include "Escenas/FondoMenu.h"
#include "Escenas/EscenaPreMenu.h"
#include "Escenas/EscenaMenuPrincipal.h"
#include "Escenas/EscenaMinijuegos.h"
#include "Escenas/EscenaInstrucciones.h"
#include "Escenas/EscenaCreditos.h"
#include "Escenas/EscenaEnConstruccion.h"
#include "Historia/Progreso.h"
#include "Historia/EscenaPrologo.h"
#include "Historia/EscenaReunion.h"
#include "Historia/EscenaBadAnon.h"
#include "Historia/EscenaRegreso.h"
#include "Historia/PantallasHistoria.h"
#include "Historia/EscenaCentral.h"
#include "Historia/EscenasSugar.h"
#include "Historia/EscenaFinales.h"
#include "Niveles/EscenasFixIt.h"
#include "Niveles/EscenasHerosDuty.h"
#include "Niveles/EscenasSugarRush.h"
#include "Niveles/EscenaTurbo.h"
#pragma comment(lib, "winmm.lib")
class Juego {
private:
    static const int ANCHO = 240;
    static const int ALTO = 120;
    static constexpr double FPS = 30.0;
    enum Fase { NORMAL, SALIENDO, ENTRANDO };
    Consola consola;
    Teclado teclado;
    Audio audio;
    Lienzo lienzo;
    Progreso progreso;
    std::unique_ptr<Fuentes> fuentes;
    std::unique_ptr<FondoMenu> fondo;
    std::unique_ptr<Contexto> ctx;
    std::unique_ptr<Escena> escena;
    IdEscena actual;
    IdEscena siguiente;
    Fase fase;
    double tiempoFase;
    double duracionFase;
    bool corriendo;
    bool mostrarFps;
    std::unique_ptr<Escena> crear(IdEscena id) {
        Contexto& c = *ctx;
        switch (id) {
        case IdEscena::PRE_MENU: return std::make_unique<EscenaPreMenu>(c);
        case IdEscena::MENU_PRINCIPAL: return std::make_unique<EscenaMenuPrincipal>(c);
        case IdEscena::MINIJUEGOS: return std::make_unique<EscenaMinijuegos>(c);
        case IdEscena::INSTRUCCIONES: return std::make_unique<EscenaInstrucciones>(c);
        case IdEscena::CREDITOS: return std::make_unique<EscenaCreditos>(c);
        case IdEscena::HISTORIA_MENU: return std::make_unique<EscenaMenuHistoria>(c);
        case IdEscena::PROLOGO: return std::make_unique<EscenaPrologo>(c);
        case IdEscena::NIVEL1_FIXIT: return std::make_unique<EscenaNivel1>(c);
        case IdEscena::CINE_REUNION: return std::make_unique<EscenaReunion>(c);
        case IdEscena::CINE_BADANON: return std::make_unique<EscenaBadAnon>(c);
        case IdEscena::CINE_REGRESO: return std::make_unique<EscenaRegreso>(c);
        case IdEscena::NIVEL1_FIN: return std::make_unique<EscenaFinNivel>(c, 1);
        case IdEscena::CINE_CENTRAL: return std::make_unique<EscenaCentral>(c);
        case IdEscena::NIVEL2_HEROS: return std::make_unique<EscenaNivel2>(c);
        case IdEscena::CINE_FINAL2: return std::make_unique<EscenaFinal2>(c);
        case IdEscena::NIVEL2_FIN: return std::make_unique<EscenaFinNivel>(c, 2);
        case IdEscena::NIVEL3_CARRERA: return std::make_unique<EscenaNivel3>(c);
        case IdEscena::CINE_SUGAR_LLEGADA: return std::make_unique<EscenaLlegadaSugar>(c);
        case IdEscena::CINE_SUGAR_CARRO: return std::make_unique<EscenaCarroSugar>(c);
        case IdEscena::CINE_SUGAR_INICIO: return std::make_unique<EscenaInicioSugar>(c);
        case IdEscena::CINE_TURBO: return std::make_unique<EscenaRevelacionTurbo>(c);
        case IdEscena::NIVEL_SECRETO: return std::make_unique<EscenaBatallaTurbo>(c);
        case IdEscena::CINE_FINAL: return std::make_unique<EscenaFinales>(c);
        case IdEscena::PERDISTE: return std::make_unique<EscenaPerdiste>(c);
        case IdEscena::GANASTE: return std::make_unique<EscenaGanaste>(c);
        case IdEscena::MINI_FIX_IT: return std::make_unique<EscenaMiniFixIt>(c);
        case IdEscena::MINI_HEROS_DUTY: return std::make_unique<EscenaMiniHerosDuty>(c);
        case IdEscena::MINI_SUGAR_RUSH: return std::make_unique<EscenaMiniSugarRush>(c);
        default: return std::make_unique<EscenaMenuPrincipal>(c);
        }
    }
    void cambiarA(IdEscena id) {
        escena = crear(id);
        actual = id;
        escena->entrar();
    }
    double duracionTransicion(IdEscena desde, IdEscena hacia) const {
        if (desde == IdEscena::PRE_MENU || hacia == IdEscena::SALIR_DEL_JUEGO) return 0.45;
        return 0.18;
    }
public:
    Juego() : lienzo(ANCHO, ALTO), actual(IdEscena::NINGUNA), siguiente(IdEscena::NINGUNA), fase(ENTRANDO),
        tiempoFase(0), duracionFase(0.45), corriendo(true), mostrarFps(false) {}
    void ejecutar(IdEscena inicio = IdEscena::PRE_MENU) {
        consola.iniciar(L"WHO'S RALPH?", ANCHO, ALTO);
        lienzo = Lienzo(ANCHO, ALTO, consola.getColumnas(), consola.getFilas());
        teclado.iniciar(consola.getEntrada());
        fuentes = std::make_unique<Fuentes>();
        fondo = std::make_unique<FondoMenu>(ANCHO, ALTO);
        ctx = std::make_unique<Contexto>(teclado, audio, *fuentes, *fondo, progreso);
        if (inicio != IdEscena::PRE_MENU && inicio != IdEscena::MENU_PRINCIPAL && inicio != IdEscena::MINI_FIX_IT && inicio != IdEscena::MINI_HEROS_DUTY &&
            inicio != IdEscena::MINI_SUGAR_RUSH) progreso.cargar();
        cambiarA(inicio);
        timeBeginPeriod(1);
        LARGE_INTEGER frecuencia, antes, ahora;
        QueryPerformanceFrequency(&frecuencia);
        QueryPerformanceCounter(&antes);
        double fpsMedido = FPS;
        while (corriendo) {
            QueryPerformanceCounter(&ahora);
            double dt = (double)(ahora.QuadPart - antes.QuadPart) / (double)frecuencia.QuadPart;
            antes = ahora;
            if (dt > 0.1) dt = 0.1;
            fpsMedido = fpsMedido * 0.9 + (dt > 0 ? 1.0 / dt : FPS) * 0.1;
            teclado.actualizar();
            audio.actualizar();
            if (teclado.presionada(VK_F3)) mostrarFps = !mostrarFps;
            if (fase == NORMAL) {
                escena->actualizar(dt);
                if (ctx->pedido != IdEscena::NINGUNA) {
                    siguiente = ctx->pedido;
                    ctx->pedido = IdEscena::NINGUNA;
                    duracionFase = duracionTransicion(actual, siguiente);
                    fase = SALIENDO;
                    tiempoFase = 0;
                }
            }
            else {
                tiempoFase += dt;
                fondo->actualizar(dt);
                if (fase == SALIENDO && tiempoFase >= duracionFase) {
                    if (siguiente == IdEscena::SALIR_DEL_JUEGO) corriendo = false;
                    else { cambiarA(siguiente); fase = ENTRANDO; tiempoFase = 0; }
                }
                else if (fase == ENTRANDO && tiempoFase >= duracionFase) fase = NORMAL;
            }
            escena->dibujar(lienzo);
            double nivel = fase == SALIENDO ? tiempoFase / duracionFase : fase == ENTRANDO ? 1.0 - tiempoFase / duracionFase : 0.0;
            Efectos::fundido(lienzo, (float)nivel);
            if (mostrarFps) lienzo.texto(lienzo.xMin() + 1, lienzo.yMin(), L"FPS " + std::to_wstring((int)(fpsMedido + 0.5)) + L" " + std::to_wstring(consola.getColumnas()) + L"x" + std::to_wstring(consola.getFilas()),Colores::AMARILLO, Colores::NEGRO);
            consola.presentar(lienzo);
            LARGE_INTEGER fin;
            QueryPerformanceCounter(&fin);
            double usado = (double)(fin.QuadPart - ahora.QuadPart) / (double)frecuencia.QuadPart;
            double resta = 1.0 / FPS - usado;
            if (resta > 0) Sleep((DWORD)(resta * 1000));
        }
        audio.detenerMusica();
        timeEndPeriod(1);
        consola.restaurar();
    }
};
