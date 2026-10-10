#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include "EscenaHistoria.h"
class EscenaCinematica;
class Paso {
public:
    virtual ~Paso() {}
    virtual void iniciar(EscenaCinematica& e) {}
    virtual bool actualizar(EscenaCinematica& e, double dt) = 0;
    virtual bool instantaneo() const { return false; }
};
class EscenaCinematica : public EscenaHistoria {
protected:
    std::vector<std::unique_ptr<Paso>> guion;
    size_t indice;
    bool pasoIniciado;
    int puntoInsercion;
    CuadroDialogo dialogo;
    MenuEleccion eleccion;
    double tiempoEscena;
    std::string hablante;
    bool terminada;
    void agregar(std::unique_ptr<Paso> p) {
        if (puntoInsercion >= 0) guion.insert(guion.begin() + puntoInsercion++, std::move(p));
        else guion.push_back(std::move(p));
    }
    void decir(const std::string& id, const std::wstring& quien, const std::wstring& texto, const Retrato& r, Color c, const std::wstring& voz = L"");
    void narrar(const std::wstring& texto, const std::wstring& voz = L"");
    void esperar(double segundos);
    void hacer(std::function<void()> f);
    void esperarQue(std::function<bool()> f);
    void elegir(const std::wstring& titulo, const std::vector<std::wstring>& opciones, std::function<void(int)> alElegir);
    virtual void actualizarEscenario(double dt) {}
    virtual void dibujarEscenario(Lienzo& l) = 0;
    virtual void alTerminar() = 0;
    void actualizarHistoria(double dt) override {
        tiempoEscena += dt;
        actualizarEscenario(dt);
        dialogo.actualizar(dt, ctx.audio, !ctx.audio.vozSonando());
        for (int vueltas = 0; vueltas < 16 && !terminada; vueltas++) {
            if (indice >= guion.size()) { terminada = true; alTerminar(); break; }
            if (!pasoIniciado) { pasoIniciado = true; guion[indice]->iniciar(*this); }
            if (!guion[indice]->actualizar(*this, dt)) break;
            bool seguir = guion[indice]->instantaneo();
            indice++;
            pasoIniciado = false;
            dt = 0;
            if (!seguir) break;
        }
    }
    void dibujarHistoria(Lienzo& l) override {
        dibujarEscenario(l);
        dialogo.dibujar(l);
        eleccion.dibujar(l);
    }
public:
    EscenaCinematica(Contexto& c, IdEscena p)
        : EscenaHistoria(c, p), indice(0), pasoIniciado(false), puntoInsercion(-1), tiempoEscena(0), terminada(false) {}
    Contexto& getCtx() { return ctx; }
    CuadroDialogo& getDialogo() { return dialogo; }
    MenuEleccion& getEleccion() { return eleccion; }
    void setHablante(const std::string& h) { hablante = h; }
    void empezarInsercion() { puntoInsercion = (int)indice + 1; }
    void terminarInsercion() { puntoInsercion = -1; }
};
class PasoDialogo : public Paso {
private:
    std::string id;
    std::wstring quien, texto, voz;
    Retrato retrato;
    Color color;
public:
    PasoDialogo(const std::string& i, const std::wstring& q, const std::wstring& t, const Retrato& r, Color c, const std::wstring& v)
        : id(i), quien(q), texto(t), voz(v), retrato(r), color(c) {}
    void iniciar(EscenaCinematica& e) override {
        e.getDialogo().mostrar(quien, texto, retrato, color);
        e.setHablante(id);
        if (!voz.empty()) e.getCtx().audio.reproducirVoz(voz);
    }
    bool actualizar(EscenaCinematica& e, double dt) override {
        if (!e.getCtx().teclado.aceptar()) return false;
        if (!e.getDialogo().completo()) { e.getDialogo().completar(); return false; }
        e.getDialogo().ocultar();
        e.setHablante("");
        e.getCtx().audio.detenerVoz();
        return true;
    }
};
class PasoEspera : public Paso {
private:
    double total, t;
public:
    explicit PasoEspera(double s) : total(s), t(0) {}
    void iniciar(EscenaCinematica& e) override { t = 0; }
    bool actualizar(EscenaCinematica& e, double dt) override { t += dt; return t >= total; }
};
class PasoAccion : public Paso {
private:
    std::function<void()> f;
public:
    explicit PasoAccion(std::function<void()> fn) : f(fn) {}
    bool actualizar(EscenaCinematica& e, double dt) override { f(); return true; }
    bool instantaneo() const override { return true; }
};
class PasoCondicion : public Paso {
private:
    std::function<bool()> f;
public:
    explicit PasoCondicion(std::function<bool()> fn) : f(fn) {}
    bool actualizar(EscenaCinematica& e, double dt) override { return f(); }
};
class PasoEleccion : public Paso {
private:
    std::wstring titulo;
    std::vector<std::wstring> opciones;
    std::function<void(int)> alElegir;
public:
    PasoEleccion(const std::wstring& t, const std::vector<std::wstring>& o, std::function<void(int)> f) : titulo(t), opciones(o), alElegir(f) {}
    void iniciar(EscenaCinematica& e) override { e.getEleccion().mostrar(titulo, opciones); }
    bool actualizar(EscenaCinematica& e, double dt) override {
        int r = e.getEleccion().manejar(e.getCtx(), dt);
        if (r < 0) return false;
        e.empezarInsercion();
        alElegir(r);
        e.terminarInsercion();
        return true;
    }
};
inline void EscenaCinematica::decir(const std::string& id, const std::wstring& quien, const std::wstring& texto, const Retrato& r, Color c, const std::wstring& voz) {
    agregar(std::make_unique<PasoDialogo>(id, quien, texto, r, c, voz));
}
inline void EscenaCinematica::narrar(const std::wstring& texto, const std::wstring& voz) {
    agregar(std::make_unique<PasoDialogo>("", L"SISTEMA", texto, Retrato(), Color(120, 255, 140), voz));
}
inline void EscenaCinematica::esperar(double s) { agregar(std::make_unique<PasoEspera>(s)); }
inline void EscenaCinematica::hacer(std::function<void()> f) { agregar(std::make_unique<PasoAccion>(f)); }
inline void EscenaCinematica::esperarQue(std::function<bool()> f) { agregar(std::make_unique<PasoCondicion>(f)); }
inline void EscenaCinematica::elegir(const std::wstring& titulo, const std::vector<std::wstring>& opciones, std::function<void(int)> f) {
    agregar(std::make_unique<PasoEleccion>(titulo, opciones, f));
}
