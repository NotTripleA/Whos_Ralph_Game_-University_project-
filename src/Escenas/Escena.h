#pragma once
#include <string>
#include "../Motor/Lienzo.h"
#include "../Motor/Teclado.h"
#include "../Motor/Audio.h"
#include "../Motor/FuentePixel.h"
#include "../Historia/Progreso.h"
class FondoMenu;
enum class IdEscena {
    NINGUNA,
    PRE_MENU,
    MENU_PRINCIPAL,
    MINIJUEGOS,
    INSTRUCCIONES,
    CREDITOS,
    HISTORIA_MENU,
    PROLOGO,
    NIVEL1_FIXIT,
    CINE_REUNION,
    CINE_BADANON,
    CINE_REGRESO,
    NIVEL1_FIN,
    CINE_CENTRAL,
    NIVEL2_HEROS,
    CINE_FINAL2,
    NIVEL2_FIN,
    CINE_SUGAR_LLEGADA,
    CINE_SUGAR_CARRO,
    CINE_SUGAR_INICIO,
    NIVEL3_CARRERA,
    CINE_TURBO,
    NIVEL_SECRETO,
    CINE_FINAL,
    PERDISTE,
    GANASTE,
    MINI_FIX_IT,
    MINI_HEROS_DUTY,
    MINI_SUGAR_RUSH,
    SALIR_DEL_JUEGO
};
namespace PuntosHistoria {
    inline std::string nombre(IdEscena id) {
        switch (id) {
        case IdEscena::PROLOGO: return "PROLOGO";
        case IdEscena::NIVEL1_FIXIT: return "NIVEL1_FIXIT";
        case IdEscena::CINE_REUNION: return "CINE_REUNION";
        case IdEscena::CINE_BADANON: return "CINE_BADANON";
        case IdEscena::CINE_REGRESO: return "CINE_REGRESO";
        case IdEscena::NIVEL1_FIN: return "NIVEL1_FIN";
        case IdEscena::CINE_CENTRAL: return "CINE_CENTRAL";
        case IdEscena::NIVEL2_HEROS: return "NIVEL2_HEROS";
        case IdEscena::CINE_FINAL2: return "CINE_FINAL2";
        case IdEscena::NIVEL2_FIN: return "NIVEL2_FIN";
        case IdEscena::CINE_SUGAR_LLEGADA: return "CINE_SUGAR_LLEGADA";
        case IdEscena::CINE_SUGAR_CARRO: return "CINE_SUGAR_CARRO";
        case IdEscena::CINE_SUGAR_INICIO: return "CINE_SUGAR_INICIO";
        case IdEscena::NIVEL3_CARRERA: return "NIVEL3_CARRERA";
        case IdEscena::CINE_TURBO: return "CINE_TURBO";
        case IdEscena::NIVEL_SECRETO: return "NIVEL_SECRETO";
        case IdEscena::CINE_FINAL: return "CINE_FINAL";
        default: return "PROLOGO";
        }
    }
    inline IdEscena escena(const std::string& n) {
        const IdEscena todas[] = { IdEscena::PROLOGO, IdEscena::NIVEL1_FIXIT, IdEscena::CINE_REUNION, IdEscena::CINE_BADANON,
            IdEscena::CINE_REGRESO, IdEscena::NIVEL1_FIN, IdEscena::CINE_CENTRAL, IdEscena::NIVEL2_HEROS, IdEscena::CINE_FINAL2,
            IdEscena::NIVEL2_FIN, IdEscena::CINE_SUGAR_LLEGADA, IdEscena::CINE_SUGAR_CARRO, IdEscena::CINE_SUGAR_INICIO,
            IdEscena::NIVEL3_CARRERA, IdEscena::CINE_TURBO, IdEscena::NIVEL_SECRETO, IdEscena::CINE_FINAL };
        if (n == "NIVEL2") return IdEscena::CINE_CENTRAL;
        if (n == "NIVEL3") return IdEscena::CINE_SUGAR_LLEGADA;
        for (IdEscena id : todas) if (nombre(id) == n) return id;
        return IdEscena::PROLOGO;
    }
}
struct Fuentes {
    FuentePixel blanca;
    FuentePixel amarilla;
    FuentePixel roja;
    FuentePixel azul;
    Fuentes() : blanca("blanca"), amarilla("amarilla"), roja("roja"), azul("azul") {}
};
struct Contexto {
    Teclado& teclado;
    Audio& audio;
    Fuentes& fuentes;
    FondoMenu& fondo;
    Progreso& progreso;
    IdEscena pedido;
    IdEscena reintentar;
    Contexto(Teclado& t, Audio& a, Fuentes& f, FondoMenu& fm, Progreso& p)
        : teclado(t), audio(a), fuentes(f), fondo(fm), progreso(p), pedido(IdEscena::NINGUNA), reintentar(IdEscena::PROLOGO) {}
    void irA(IdEscena id) { pedido = id; }
};
class Escena {
protected:
    Contexto& ctx;
public:
    explicit Escena(Contexto& c) : ctx(c) {}
    virtual ~Escena() {}
    virtual void entrar() {}
    virtual void actualizar(double dt) = 0;
    virtual void dibujar(Lienzo& l) = 0;
};
