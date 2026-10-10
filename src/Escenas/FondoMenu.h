#pragma once
#include <vector>
#include <memory>
#include "../Motor/Efectos.h"
#include "../Entidades/ActoresMenu.h"
#include "../Entidades/DesfileMenu.h"
#include "EscenarioNeon.h"
class FondoMenu {
private:
    int ancho;
    int alto;
    int bordeIzq;
    int bordeDer;
    EscenarioNeon escenario;
    RalphMenu* ralph;
    std::vector<std::unique_ptr<Entidad>> curiosos;
    std::vector<std::unique_ptr<Entidad>> cielo;
    std::vector<std::unique_ptr<Entidad>> actores;
    std::vector<std::unique_ptr<Entidad>> ladrillos;
    DirectorDesfile desfile;
public:
    static const int SUELO = 116;
    FondoMenu(int w, int h) : ancho(w), alto(h), bordeIzq(0), bordeDer(w), ralph(nullptr), desfile(SUELO, w) {
        curiosos.push_back(std::make_unique<NicelanderAsomado>(&bordeIzq, -1, 3.0));
        curiosos.push_back(std::make_unique<NicelanderAsomado>(&bordeDer, 1, 5.5));
        cielo.push_back(std::make_unique<PatoMenu>(w, 2.0));
        cielo.push_back(std::make_unique<PatoMenu>(w, 7.5));
        cielo.push_back(std::make_unique<CyBugVolador>(w, 5.0));
        actores.push_back(std::make_unique<FelixMenu>(30, SUELO, 16, 70));
        auto r = std::make_unique<RalphMenu>(206, SUELO, 190, 214);
        ralph = r.get();
        actores.push_back(std::move(r));
    }
    void marcarPanel(int izquierda, int derecha) { bordeIzq = izquierda; bordeDer = derecha; }
    void actualizar(double dt) {
        escenario.actualizar(dt);
        for (auto& c : curiosos) c->actualizar(dt);
        for (auto& c : cielo) c->actualizar(dt);
        for (auto& a : actores) a->actualizar(dt);
        desfile.actualizar(dt);
        if (ralph->consumirGolpe())
            ladrillos.push_back(std::make_unique<Ladrillo>(ralph->getX() - 12, SUELO - 50, SUELO));
        for (auto& b : ladrillos) b->actualizar(dt);
        for (size_t i = 0; i < ladrillos.size();) {
            if (!ladrillos[i]->estaActiva()) ladrillos.erase(ladrillos.begin() + i);
            else i++;
        }
    }
    void dibujar(Lienzo& l, bool conDesfile = true) const {
        escenario.dibujar(l);
        for (const auto& c : cielo) c->dibujar(l);
        for (const auto& c : curiosos) c->dibujar(l);
        for (const auto& a : actores) a->dibujar(l);
        for (const auto& b : ladrillos) b->dibujar(l);
        if (conDesfile) desfile.dibujar(l);
    }
    void dibujarDesfile(Lienzo& l) const { desfile.dibujar(l); }
};
