#pragma once
#include "Cinematica.h"
#include "Personajes.h"
#include <vector>
#include <memory>
#include <algorithm>
#include "../Entidades/Actor.h"
#include "../Motor/Azar.h"
class HeroeCentral : public Entidad {
private:
    std::string prefijo;
    int cuadros;
    double vel, pausa, t, proximoCambio;
    int dir;
public:
    HeroeCentral(const std::string& p, int n, double px, double py)
        : Entidad(px, py), prefijo(p), cuadros(n), vel(Azar::real(12, 26)), pausa(0), t(Azar::real(0, 3)),
        proximoCambio(Azar::real(2, 6)), dir(Azar::probabilidad(0.5) ? 1 : -1) {}
    void actualizar(double dt) override {
        t += dt;
        proximoCambio -= dt;
        if (pausa > 0) { pausa -= dt; return; }
        x += dir * vel * dt;
        if (proximoCambio <= 0) {
            proximoCambio = Azar::real(2, 6);
            int r = Azar::entero(0, 2);
            if (r == 0) pausa = Azar::real(0.6, 2.0);
            else if (r == 1) dir = -dir;
            vel = Azar::real(12, 26);
        }
        if (x < -120) dir = 1;
        if (x > 360) dir = -1;
    }
    void dibujar(Lienzo& l) const override {
        int c = pausa > 0 ? 1 : 1 + ((int)(t * (4 + vel / 6))) % cuadros;
        char num[8];
        sprintf_s(num, "%02d", c);
        const Sprite* s = BancoSprites::instancia().obtener(prefijo + num);
        s->dibujar(l, (int)x - s->getAncho() / 2, (int)y - s->getAlto(), dir < 0);
    }
};
class EscenaCentral : public EscenaCinematica {
private:
    int toma;
    Actor actorRalph;
    double fundido;
    double pantallaSonic;
    double gente;
    std::vector<std::unique_ptr<HeroeCentral>> heroes;
    static const int SUELO = 84;
    static const int PUERTA_X = 184;
    const std::string GCS = "12_Escenarios_y_entornos_IA/Escenario2_Estacion_GCS/";
    void dibujarEstacion(Lienzo& l) {
        auto& b = BancoSprites::instancia();
        l.degradadoVertical(0, l.getAlto(), Color(36, 40, 58), Color(14, 14, 22));
        for (int y = l.yMin(); y < SUELO; y++)
            for (int x = l.xMin(); x < l.xMax(); x++)
                if ((x % 40 == 0) || (y % 30 == 0)) l.pixel(x, y, Color(54, 60, 84));
        for (int y = SUELO; y < l.yMax(); y++)
            for (int x = l.xMin(); x < l.xMax(); x++)
                l.pixel(x, y, ((x / 12 + y / 6) % 2) ? Color(70, 58, 40) : Color(84, 70, 48));
        for (int x = l.xMin(); x < l.xMax(); x += 3) l.sombrear(x, SUELO + 2, Glifo::CLARO, Color(255, 230, 160));
        for (auto& h : heroes) if (h->getY() < SUELO - 2) h->dibujar(l);
        if (toma == 0) {
            b.obtener(GCS + "tablero_departure")->dibujar(l, 4, -4);
            b.obtener(GCS + "cartel_anden_pacman_azul")->dibujar(l, 152, -7);
            b.obtener(GCS + "maquina_boletos_2")->dibujar(l, 208, SUELO - 53);
            b.obtener(GCS + "barrera_con_persona")->dibujar(l, 124, SUELO - 49);
            if (pantallaSonic > 0) {
                int x0 = 6, y0 = -2, w = 85, h = 42;
                l.rectangulo(x0 - 2, y0 - 2, w + 4, h + 4, Color(20, 20, 30));
                l.rectangulo(x0, y0, w, h, Color(30, 90, 200));
                for (int y = y0; y < y0 + h; y += 2) for (int x = x0; x < x0 + w; x++) l.sombrear(x, y, Glifo::CLARO, Color(120, 200, 255));
                char id[96];
                sprintf_s(id, "05_Sonic_Advance_3/fila_01/sonic_fila_01_%02d", 1 + ((int)(gente * 6)) % 8);
                const Sprite* s = b.obtener(id);
                s->dibujar(l, x0 + w / 2 - s->getAncho() / 2, y0 + h - s->getAlto() - 2);
            }
        }
        else {
            const Sprite* puerta = b.obtener(GCS + "puerta_embarque_circular");
            puerta->dibujar(l, PUERTA_X - puerta->getAncho() / 2, SUELO - 72);
            b.obtener("04_Heros_Duty/UI/logo_heros_duty")->dibujarReducido(l, 10, -4, 2);
            int cx = 104, cy = 50;
            l.rectangulo(cx - 28, cy - 10, 56, 36, Color(20, 24, 34));
            l.marco(cx - 28, cy - 10, 56, 36, Interfaz::DORADO);
            bool brilla = ((int)(gente * 4)) % 2 == 0;
            Color oro(255, 205, 60), oroOsc(190, 130, 20), cinta(200, 30, 40);
            for (int j = 0; j < 6; j++) { l.pixel(cx - 3 + j / 2, cy - 7 + j, cinta); l.pixel(cx + 3 - j / 2, cy - 7 + j, cinta); }
            for (int j = -5; j <= 5; j++) for (int i = -5; i <= 5; i++) if (i * i + j * j <= 25) l.pixel(cx + i, cy + 4 + j, (i + j) < 0 ? oro : oroOsc);
            if (brilla) { l.sombrear(cx - 8, cy, Glifo::MEDIO, oro); l.sombrear(cx + 8, cy + 6, Glifo::MEDIO, oro); }
            const std::wstring l1 = L"\x00A1MEDALLA AL", l2 = L"GANADOR!";
            FuenteMini::texto(l, cx - FuenteMini::medir(l1) / 2, cy + 12, l1, oro);
            FuenteMini::texto(l, cx - FuenteMini::medir(l2) / 2, cy + 19, l2, oro);
        }
        std::vector<const Entidad*> orden;
        for (auto& h : heroes) if (h->getY() >= SUELO - 2) orden.push_back(h.get());
        orden.push_back(&actorRalph);
        std::stable_sort(orden.begin(), orden.end(), [](const Entidad* a, const Entidad* b) { return a->getY() < b->getY(); });
        for (auto* e : orden) e->dibujar(l);
    }
    void crearHeroes() {
        struct D { const char* p; int n; };
        const D lista[] = {
            { "07_Famosos_16bit_12-18_IA/Ryu_(etiquetado_como_Chun-Li)/caminar/ryu_(etiquetado_como_chun-li)_caminar_", 4 },
            { "09_Anime_16bit_IA/Naruto_Uzumaki/caminar_version_2/naruto_uzumaki_caminar_version_2_", 4 },
            { "09_Anime_16bit_IA/Sasuke_Uchiha/caminar_version_2/sasuke_uchiha_caminar_version_2_", 4 },
            { "09_Anime_16bit_IA/Son_Goku/caminar_version_2/son_goku_caminar_version_2_", 4 },
            { "09_Anime_16bit_IA/Hinata_Hyuga/caminar/hinata_hyuga_caminar_", 4 },
            { "07_Famosos_16bit_12-18_IA/Bomberman/caminar/bomberman_caminar_", 4 },
            { "07_Famosos_16bit_12-18_IA/Chun-Li/caminar/chun-li_caminar_", 2 },
            { "07_Famosos_16bit_12-18_IA/Cloud_Strife/caminar/cloud_strife_caminar_", 4 },
            { "07_Famosos_16bit_12-18_IA/Crono/caminar/crono_caminar_", 4 },
            { "07_Famosos_16bit_12-18_IA/Arthur/caminar/arthur_caminar_", 4 },
            { "07_Famosos_16bit_12-18_IA/Terra_Branford/caminar/terra_branford_caminar_", 4 },
            { "07_Famosos_16bit_12-18_IA/Gex/caminar/gex_caminar_", 4 },
            { "09_Anime_16bit_IA/Vegeta/caminar_version_2/vegeta_caminar_version_2_", 4 },
            { "09_Anime_16bit_IA/Bulma_Briefs/caminar/bulma_briefs_caminar_", 4 },
            { "08_Famosos_16bit_1-8_IA/Link/caminar/link_caminar_", 4 },
            { "08_Famosos_16bit_1-8_IA/Mega_Man/caminar/mega_man_caminar_", 4 },
            { "08_Famosos_16bit_1-8_IA/Samus/caminar/samus_caminar_", 4 },
            { "08_Famosos_16bit_1-8_IA/Kirby/caminar/kirby_caminar_", 3 },
            { "08_Famosos_16bit_1-8_IA/Ness/caminar/ness_caminar_", 3 },
            { "08_Famosos_16bit_1-8_IA/Simon_Belmont/caminar/simon_belmont_caminar_", 4 },
            { "08_Famosos_16bit_1-8_IA/Scorpion/caminar/scorpion_caminar_", 4 },
            { "08_Famosos_16bit_1-8_IA/Donkey_Kong/caminar/donkey_kong_caminar_", 3 } };
        int n = (int)(sizeof(lista) / sizeof(lista[0]));
        for (int i = 0; i < n; i++) {
            bool grande = i >= 14;
            double px = i < 6 ? 20 + i * 40 + Azar::real(-8, 8) : (grande ? (Azar::probabilidad(0.5) ? Azar::real(-110, -40) : Azar::real(280, 350)) : Azar::real(-110, 350));
            double py = grande ? SUELO - 5 - (i % 3) : SUELO - 2 + (i * 5) % 11;
            heroes.push_back(std::make_unique<HeroeCentral>(lista[i].p, lista[i].n, px, py));
        }
    }
protected:
    void actualizarEscenario(double dt) override {
        actorRalph.actualizar(dt);
        for (auto& h : heroes) h->actualizar(dt);
        gente += dt;
        if (pantallaSonic > 0) pantallaSonic -= dt;
        if (fundido > 0) fundido -= dt * 1.5;
    }
    void dibujarEscenario(Lienzo& l) override {
        dibujarEstacion(l);
        if (fundido > 0) Efectos::fundido(l, (float)fundido);
    }
    void alTerminar() override { ctx.irA(IdEscena::NIVEL2_HEROS); }
public:
    explicit EscenaCentral(Contexto& c)
        : EscenaCinematica(c, IdEscena::CINE_CENTRAL), toma(0),
        actorRalph(-30, SUELO, Personajes::ralphParado(), Personajes::ralphCamina()), fundido(1.0), pantallaSonic(0), gente(0) {
        using namespace Personajes;
        crearHeroes();
        Retrato surge = Retrato::recortado(SPR(GCS + "barrera_con_persona"), 0, 19);
        const Color C_SURGE(120, 200, 255);
        hacer([this] { actorRalph.caminarA(98, 26); });
        esperarQue([this] { return actorRalph.llego(); });
        decir("surge", L"SURGE PROTECTOR", L"Nombre.", surge, C_SURGE, L"surge_nombre.wav");
        decir("ralph", L"RALPH", L"Ralph el Demoledor.", ralphArcade(), C_RALPH, L"ralph_nombre.wav");
        decir("surge", L"SURGE PROTECTOR", L"\x00BFTransporta frutas con usted?", surge, C_SURGE, L"surge_frutas.wav");
        decir("ralph", L"RALPH", L"No, no, ni una cereza.", ralphArcade(), C_RALPH, L"ralph_frutas.wav");
        decir("surge", L"SURGE PROTECTOR", L"\x00BF" L"Algo que declarar?", surge, C_SURGE, L"surge_declarar.wav");
        decir("ralph", L"RALPH", L"Que te odio.", ralphArcade(), C_RALPH, L"ralph_odio.wav");
        decir("surge", L"SURGE PROTECTOR", L"Eso dicen todos. Proceda.", surge, C_SURGE, L"surge_proceda.wav");
        hacer([this] { pantallaSonic = 14.0; });
        decir("sonic", L"SONIC (PANTALLA)", L"Si sales de tu juego, cu\x00ED" L"date. Y hagas lo que hagas, que no te eliminen: fuera de tu propio juego ya no te regenerar\x00E1s. Nunca. \x00A1Game Over!",
            Retrato(SPR("05_Sonic_Advance_3/fila_01/sonic_fila_01_01")), Color(80, 160, 255), L"sonic_aviso.wav");
        hacer([this] { pantallaSonic = 0; actorRalph.caminarA(270, 30); });
        esperarQue([this] { return actorRalph.llego(); });
        hacer([this] { toma = 1; fundido = 1.0; actorRalph.ubicar(-30, SUELO); actorRalph.caminarA(52, 28); });
        esperarQue([this] { return actorRalph.llego(); });
        esperar(0.6);
        decir("ralph", L"RALPH", L"\x00BFHero's Duty? ... \x00BFUna medalla para el ganador?", ralphArcade(), C_RALPH);
        decir("ralph", L"RALPH", L"Si consigo esa medalla, nadie podr\x00E1 decir que Ralph no es un h\x00E9roe. \x00A1" L"Esa medalla me la llevo a casa!", ralphArcade(), C_RALPH);
        hacer([this] { actorRalph.caminarA(PUERTA_X, 30); });
        esperarQue([this] { return actorRalph.llego(); });
        hacer([this] { actorRalph.mostrar(false); ctx.audio.efecto(L"puerta.wav"); });
        esperar(1.0);
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"central.wav"); }
};
