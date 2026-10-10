#pragma once
#include <vector>
#include <memory>
#include <cmath>
#include "ActoresMenu.h"
class EventoDesfile {
protected:
    double suelo;
    double ancho;
    double tiempo;
public:
    EventoDesfile(double s, double w) : suelo(s), ancho(w), tiempo(0) {}
    virtual ~EventoDesfile() {}
    virtual void actualizar(double dt) = 0;
    virtual void dibujar(Lienzo& l) const = 0;
    virtual bool terminado() const = 0;
};
namespace RutasDesfile {
    const std::string PACMAN = "06_Pac-Man_arcade/";
    const std::string SUGAR = "03_Sugar_Rush/";
    const std::string HEROS = "04_Heros_Duty/";
}
inline void dibujarAbajoCentro(Lienzo& l, const Sprite* s, double x, double y, bool espejo = false) {
    if (s) s->dibujar(l, (int)x - s->getAncho() / 2, (int)y - s->getAlto(), espejo);
}
class EventoPacMan : public EventoDesfile {
private:
    struct Fantasma {
        std::string nombre;
        double x;
        bool comido;
        double tiempoPuntaje;
        double xOjos;
    };
    double xPac;
    bool poder;
    double tiempoPoder;
    double xPildora;
    int comidos;
    std::vector<Fantasma> fantasmas;
    Animacion pacDer, pacIzq;
public:
    EventoPacMan(double s, double w) : EventoDesfile(s, w), xPac(-12), poder(false), tiempoPoder(0), xPildora(w * 0.62), comidos(0) {
        auto& b = BancoSprites::instancia();
        pacDer = Animacion(b.secuencia(RutasDesfile::PACMAN + "PacMan/mover_derecha/pacman_derecha_", 1, 3), 14);
        pacIzq = Animacion(b.secuencia(RutasDesfile::PACMAN + "PacMan/mover_izquierda/pacman_izquierda_", 1, 3), 14);
        const char* nombres[4] = { "Blinky_rojo/blinky", "Pinky_rosa/pinky", "Inky_celeste/inky", "Clyde_naranja/clyde" };
        for (int i = 0; i < 4; i++) fantasmas.push_back({ nombres[i], xPac - 22.0 * (i + 1) - 6, false, 0, 0 });
    }
    void actualizar(double dt) override {
        tiempo += dt;
        pacDer.actualizar(dt);
        pacIzq.actualizar(dt);
        if (!poder) {
            xPac += 46 * dt;
            for (auto& f : fantasmas) f.x += 46 * dt;
            if (xPac >= xPildora) { poder = true; tiempoPoder = 0; }
            return;
        }
        tiempoPoder += dt;
        xPac -= 52 * dt;
        for (auto& f : fantasmas) {
            if (!f.comido) {
                f.x -= 34 * dt;
                if (std::fabs(f.x - xPac) < 6) { f.comido = true; f.xOjos = f.x; comidos++; f.tiempoPuntaje = 0.7; }
            }
            else {
                f.xOjos += 90 * dt;
                if (f.tiempoPuntaje > 0) f.tiempoPuntaje -= dt;
            }
        }
    }
    void dibujar(Lienzo& l) const override {
        auto& b = BancoSprites::instancia();
        bool par = ((int)(tiempo * 8)) % 2 == 0;
        if (!poder) {
            if (par) {
                const int px = (int)xPildora, py = (int)suelo - 9;
                for (int j = -2; j <= 2; j++)
                    for (int i = -2; i <= 2; i++)
                        if (i * i + j * j <= 5) l.pixel(px + i, py + j, Color(255, 184, 174));
            }
            for (const auto& f : fantasmas)
                dibujarAbajoCentro(l, b.obtener(RutasDesfile::PACMAN + "Fantasmas/" + f.nombre.substr(0, f.nombre.find('/')) + "/derecha/" +
                    f.nombre.substr(f.nombre.find('/') + 1) + (par ? "_derecha_01" : "_derecha_02")), f.x, suelo);
            dibujarAbajoCentro(l, pacDer.actual(), xPac, suelo);
            return;
        }
        int valores[4] = { 200, 400, 800, 1600 };
        int k = 0;
        for (const auto& f : fantasmas) {
            if (!f.comido) {
                bool parpadeo = tiempoPoder > 2.2 && par;
                std::string id = parpadeo ? "Fantasmas/Asustado_parpadeo_blanco/asustado_blanco_0" : "Fantasmas/Asustado_azul/asustado_azul_0";
                dibujarAbajoCentro(l, b.obtener(RutasDesfile::PACMAN + id + (par ? "1" : "2")), f.x, suelo);
            }
            else {
                if (f.tiempoPuntaje > 0) {
                    const Sprite* p = b.obtener(RutasDesfile::PACMAN + "Puntajes/fantasma_" + std::to_string(valores[k]));
                    dibujarAbajoCentro(l, p, f.x, suelo - 4);
                }
                dibujarAbajoCentro(l, b.obtener(RutasDesfile::PACMAN + "Fantasmas/Ojos_regresando/ojos_derecha"), f.xOjos, suelo);
                k++;
            }
        }
        dibujarAbajoCentro(l, pacIzq.actual(), xPac, suelo);
    }
    bool terminado() const override {
        if (!poder) return false;
        if (xPac > -20) return false;
        for (const auto& f : fantasmas)
            if ((!f.comido && f.x > -20) || (f.comido && f.xOjos < ancho + 20)) return false;
        return true;
    }
};
class EventoCarrera : public EventoDesfile {
private:
    struct Kart {
        Animacion anim;
        double x;
        double velocidad;
        bool glitchea;
        double glitch;
        double proximoGlitch;
    };
    std::vector<Kart> karts;
public:
    EventoCarrera(double s, double w) : EventoDesfile(s, w) {
        auto& b = BancoSprites::instancia();
        std::string c = RutasDesfile::SUGAR + "Corredores/";
        karts.push_back({ Animacion(b.secuencia(c + "Taffyta/kart_lado/taffyta_kart_lado_", 1, 4), 10), -40, Azar::real(105, 120), false, 0, 0 });
        karts.push_back({ Animacion({ b.obtener(c + "Rancis/kart_lado/rancis_kart_lado_01") }, 1), -85, Azar::real(95, 112), false, 0, 0 });
        karts.push_back({ Animacion({ b.obtener(c + "Snowanna/kart_lado/snowanna_kart_lado_01") }, 1), -125, Azar::real(95, 112), false, 0, 0 });
        karts.push_back({ Animacion({ b.obtener(c + "Crumbelina/kart_lado/crumbelina_kart_lado_01") }, 1), -165, Azar::real(95, 110), false, 0, 0 });
        karts.push_back({ Animacion(b.secuencia(c + "Vanellope/kart_lado/vanellope_kart_lado_", 1, 4), 12), -215, 150, true, 0, 0.9 });
    }
    void actualizar(double dt) override {
        tiempo += dt;
        for (auto& k : karts) {
            k.anim.actualizar(dt);
            k.x += k.velocidad * dt;
            if (k.glitchea) {
                if (k.glitch > 0) k.glitch -= dt;
                k.proximoGlitch -= dt;
                if (k.proximoGlitch <= 0) { k.glitch = 0.25; k.x += 18; k.proximoGlitch = Azar::real(0.6, 1.2); }
            }
        }
    }
    void dibujar(Lienzo& l) const override {
        const Color polvo(250, 205, 225);
        for (const auto& k : karts) {
            const Sprite* s = k.anim.actual();
            int atras = (int)k.x - s->getAncho() / 2;
            for (int d = 1; d <= 18; d++) {
                wchar_t g = d < 5 ? Glifo::OSCURO : d < 11 ? Glifo::MEDIO : Glifo::CLARO;
                int salto = (int)(std::sin(tiempo * 20 + d) * 1.5);
                if ((d + (int)(tiempo * 30)) % 3 != 0) l.sombrear(atras - d, (int)suelo - 2 - (d % 3) + salto, g, polvo);
            }
            int bote = ((int)(tiempo * 14 + k.x) % 2 == 0) ? 0 : 1;
            if (k.glitch > 0) {
                Color cian(60, 230, 255);
                s->dibujarSombreado(l, atras - 10, (int)suelo - s->getAlto(), Glifo::CLARO, cian);
                s->dibujarSombreado(l, atras - 5, (int)suelo - s->getAlto(), Glifo::MEDIO, cian);
            }
            s->dibujar(l, atras, (int)suelo - s->getAlto() - bote);
        }
    }
    bool terminado() const override {
        for (const auto& k : karts) if (k.x < ancho + 60) return false;
        return true;
    }
};
class EventoCalhoun : public EventoDesfile {
private:
    Animacion calhoun;
    Animacion bichos;
    double x;
public:
    EventoCalhoun(double s, double w) : EventoDesfile(s, w), x(-90) {
        auto& b = BancoSprites::instancia();
        calhoun = Animacion(b.secuencia(RutasDesfile::HEROS + "Calhoun/caminar/calhoun_caminar_", 1, 7), 9);
        bichos = Animacion(b.secuencia(RutasDesfile::HEROS + "Cy-Bugs/explorador_chico/cybug_explorador_chico_", 1, 5), 12);
    }
    void actualizar(double dt) override {
        tiempo += dt;
        calhoun.actualizar(dt);
        bichos.actualizar(dt);
        x += 30 * dt;
    }
    void dibujar(Lienzo& l) const override {
        for (int i = 0; i < 3; i++) {
            double bx = x + 46 + i * 30 + std::sin(tiempo * 9 + i) * 3;
            double by = suelo - std::fabs(std::sin(tiempo * 7 + i * 1.7)) * 4;
            dibujarAbajoCentro(l, bichos.actual(), bx, by, true);
        }
        dibujarAbajoCentro(l, calhoun.actual(), x, suelo + 1);
    }
    bool terminado() const override { return x > ancho + 40; }
};
class EventoTurbo : public EventoDesfile {
private:
    Animacion caminar;
    Animacion celebrar;
    double x;
    double pausa;
    bool celebro;
public:
    EventoTurbo(double s, double w) : EventoDesfile(s, w), x(-20), pausa(0), celebro(false) {
        auto& b = BancoSprites::instancia();
        caminar = Animacion(b.secuencia(RutasDesfile::SUGAR + "Turbo/arcade_caminar/turbo_caminar_", 1, 4), 8);
        celebrar = Animacion(b.secuencia(RutasDesfile::SUGAR + "Turbo/arcade_celebrar/turbo_celebrar_", 1, 4), 6);
    }
    void actualizar(double dt) override {
        tiempo += dt;
        if (!celebro && x >= ancho * 0.45) {
            pausa += dt;
            celebrar.actualizar(dt);
            if (pausa > 2.4) celebro = true;
            return;
        }
        caminar.actualizar(dt);
        x += 32 * dt;
    }
    void dibujar(Lienzo& l) const override {
        bool festejando = !celebro && pausa > 0;
        dibujarAbajoCentro(l, festejando ? celebrar.actual() : caminar.actual(), x, suelo);
        if (festejando && ((int)(pausa * 4)) % 2 == 0)
            l.texto((int)x - 8, (int)suelo - 42, L"\x00A1TURBO-T\x00C1STICO!", Color(255, 230, 60));
    }
    bool terminado() const override { return x > ancho + 30; }
};
class EventoVanellope : public EventoDesfile {
private:
    VanellopeMenu vanellope;
    bool arranco;
public:
    EventoVanellope(double s, double w, int direccion) : EventoDesfile(s, w), vanellope(s + 1, w, direccion, 0.0), arranco(false) {}
    void actualizar(double dt) override {
        vanellope.actualizar(dt);
        if (vanellope.estaCorriendo()) arranco = true;
    }
    void dibujar(Lienzo& l) const override { vanellope.dibujar(l); }
    bool terminado() const override { return arranco && !vanellope.estaCorriendo(); }
};
class DirectorDesfile {
private:
    double suelo;
    double ancho;
    std::unique_ptr<EventoDesfile> actual;
    double espera;
    int ultimo;
    int vanellopeDir;
    std::unique_ptr<EventoDesfile> crear(int tipo) {
        switch (tipo) {
        case 0: return std::make_unique<EventoPacMan>(suelo, ancho);
        case 1: return std::make_unique<EventoCarrera>(suelo + 1, ancho);
        case 2: return std::make_unique<EventoCalhoun>(suelo, ancho);
        case 3: return std::make_unique<EventoTurbo>(suelo, ancho);
        default: vanellopeDir = -vanellopeDir; return std::make_unique<EventoVanellope>(suelo, ancho, vanellopeDir);
        }
    }
public:
    DirectorDesfile(double s, double w) : suelo(s), ancho(w), espera(1.5), ultimo(-1), vanellopeDir(-1) {}
    void actualizar(double dt) {
        if (actual) {
            actual->actualizar(dt);
            if (actual->terminado()) { actual.reset(); espera = Azar::real(1.0, 2.5); }
            return;
        }
        espera -= dt;
        if (espera <= 0) {
            int tipo;
            do tipo = Azar::entero(0, 4); while (tipo == ultimo);
            ultimo = tipo;
            actual = crear(tipo);
        }
    }
    void dibujar(Lienzo& l) const { if (actual) actual->dibujar(l); }
};
class CyBugVolador : public PersonajeAnimado {
private:
    double base;
    double espera;
    double tiempo;
    double ancho;
    int direccion;
    bool volando;
public:
    CyBugVolador(double w, double esperaInicial)
        : PersonajeAnimado(-30, 50), base(50), espera(esperaInicial), tiempo(0), ancho(w), direccion(1), volando(false) {
        cambiarAnimacion(Animacion(BancoSprites::instancia().secuencia(RutasDesfile::HEROS + "Cy-Bugs/volador_1/cybug_volador_1_", 1, 5), 12));
    }
    void actualizar(double dt) override {
        if (!volando) {
            espera -= dt;
            if (espera <= 0) {
                volando = true;
                direccion = Azar::probabilidad(0.5) ? 1 : -1;
                x = direccion > 0 ? -30 : ancho + 30;
                base = Azar::real(38, 62);
                espera = 0;
            }
            return;
        }
        PersonajeAnimado::actualizar(dt);
        tiempo += dt;
        x += direccion * 38 * dt;
        y = base + std::sin(tiempo * 2.3) * 7;
        espejo = direccion < 0;
        if (x < -40 || x > ancho + 40) { volando = false; espera = Azar::real(9.0, 16.0); }
    }
    void dibujar(Lienzo& l) const override {
        if (volando) PersonajeAnimado::dibujar(l);
    }
};
class NicelanderAsomado : public Entidad {
private:
    const int* borde;
    int lado;
    double tiempo;
    double espera;
    int cara;
    int altura;
public:
    NicelanderAsomado(const int* bordePanel, int ladoAsomo, double esperaInicial)
        : Entidad(0, 0), borde(bordePanel), lado(ladoAsomo), tiempo(-1), espera(esperaInicial), cara(1), altura(60) {}
    void actualizar(double dt) override {
        if (tiempo < 0) {
            espera -= dt;
            if (espera <= 0) { tiempo = 0; cara = Azar::entero(1, 13); altura = Azar::entero(44, 84); }
            return;
        }
        tiempo += dt;
        if (tiempo > 3.4) { tiempo = -1; espera = Azar::real(2.5, 6.0); }
    }
    void dibujar(Lienzo& l) const override {
        if (tiempo < 0) return;
        double asomo = tiempo < 0.5 ? tiempo / 0.5 : tiempo > 2.9 ? (3.4 - tiempo) / 0.5 : 1.0;
        int salto = (tiempo > 1.2 && tiempo < 1.5) ? -2 : 0;
        char id[64];
        sprintf_s(id, "01_Fix-It_Felix_Jr/Nicelanders/cara_%02d", cara);
        const Sprite* s = SPR(id);
        int sx = lado < 0 ? *borde - (int)(asomo * 13) : *borde - 16 + (int)(asomo * 13);
        s->dibujar(l, sx, altura + salto, lado > 0);
    }
};
