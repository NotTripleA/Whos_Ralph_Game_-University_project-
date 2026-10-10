#pragma once
#include <map>
#include "Cinematica.h"
#include "Personajes.h"
class EscenaBadAnon : public EscenaCinematica {
private:
    struct Villano {
        std::string id;
        std::string carpeta;
        std::string base;
        std::string habla;
        int x;
        int pies;
        bool espejo;
    };
    std::vector<Villano> villanos;
    double sorpresa;
    double fundido;
    double brillo;
    static const int PIES = 84;
protected:
    void actualizarEscenario(double dt) override {
        if (sorpresa > 0) sorpresa -= dt;
        if (fundido > 0) fundido -= dt * 1.5;
        brillo += dt;
    }
    void dibujarEscenario(Lienzo& l) override {
        auto& b = BancoSprites::instancia();
        const std::string esc = "12_Escenarios_y_entornos_IA/Escenario1_Sala_Bad-Anon/";
        const Sprite* sala = b.obtener(esc + "sala_con_podio");
        int sx = (l.getAncho() - sala->getAncho()) / 2, sy = -10;
        l.limpiar(Color(20, 20, 26));
        for (int k = 0; k < 2; k++) {
            sala->dibujarParte(l, sx - 28 - k * 28, sy, 0, 0, 28, sala->getAlto());
            sala->dibujarParte(l, sx + sala->getAncho() + k * 28, sy, sala->getAncho() - 28, 0, 28, sala->getAlto());
        }
        sala->dibujar(l, sx, sy);
        b.obtener(esc + "cartel_rules")->dibujar(l, 6, 6);
        b.obtener(esc + "sillas_apiladas")->dibujar(l, 196, 12);
        for (const auto& v : villanos) {
            bool habla = (hablante == v.id || (hablante == "todos" && v.id != "ralph")) && ((int)(tiempoEscena * 7)) % 2 == 0;
            const Sprite* s = v.id == "ralph" ? b.obtener(Personajes::FIXIT + "Ralph/fila_1/ralph_f1_0" + std::string(habla ? "3" : "1"))
                                              : b.obtener(Personajes::BADANON + v.carpeta + "/" + (habla ? v.habla : v.base));
            int salto = sorpresa > 0 && v.id != "ralph" ? (int)(std::sin(sorpresa * 20) * 3) - 3 : 0;
            int flota = v.id == "clyde" ? (int)(std::sin(tiempoEscena * 2.5) * 2) : 0;
            s->dibujar(l, v.x - s->getAncho() / 2, v.pies - s->getAlto() + salto + flota, v.espejo);
        }
        if (brillo < 3.2) {
            int a = brillo < 0.4 ? (int)(brillo / 0.4 * 30) : brillo > 2.8 ? (int)((3.2 - brillo) / 0.4 * 30) : 30;
            int y0 = 40 - a / 2;
            l.rectangulo(20, y0, 200, a, Colores::NEGRO);
            if (a >= 30) {
                l.marco(20, y0, 200, a, Interfaz::DORADO);
                FuenteMini::centrado(l, y0 + 6, L"ASOCIACI\x00D3N DE VILLANOS AN\x00D3NIMOS", Colores::BLANCO);
                ctx.fuentes.roja.dibujarCentrado(l, y0 + 17, L"BAD-ANON");
            }
        }
        if (fundido > 0) Efectos::fundido(l, (float)fundido);
    }
    void alTerminar() override { ctx.irA(IdEscena::CINE_REGRESO); }
public:
    explicit EscenaBadAnon(Contexto& c) : EscenaCinematica(c, IdEscena::CINE_BADANON), sorpresa(0), fundido(1.0), brillo(0) {
        villanos = {
            { "satan", "Satan_(Satans_Hollow)", "base/satan_(satans_hollow)_base_01", "hablando/satan_(satans_hollow)_hablando_01", 26, PIES, false },
            { "bowser", "Bowser", "base/bowser_base_01", "hablando/bowser_hablando_01", 62, PIES, false },
            { "clyde", "Clyde", "base/clyde_base_01", "hablando/clyde_hablando_01", 96, PIES - 6, false },
            { "ralph", "Ralph", "base/ralph_base_01", "hablando/ralph_hablando_01", 150, PIES, true },
            { "bison", "M_Bison", "base_2_(estaba_en_fila_Eggman)/m_bison_base_2_(estaba_en_fila_eggman)_01", "hablando/m_bison_hablando_01", 190, PIES, true },
            { "eggman", "Dr_Eggman", "base_2_(estaba_en_fila_Bowser)/dr_eggman_base_2_(estaba_en_fila_bowser)_01", "hablando/dr_eggman_hablando_01", 224, PIES, true },
        };
        using namespace Personajes;
        esperar(3.3);
        decir("ralph", L"RALPH", L"Chicos, \x00BFustedes piensan que es malo ser malo?", ralphBadAnon(), C_RALPH);
        decir("bowser", L"BOWSER", L"No, ser malo no es malo, es lo que somos. Ser malo es bueno para nosotros, no es ning\x00FAn problema, es nuestra funci\x00F3n.", bowser(), C_BOWSER);
        decir("bison", L"M. BISON", L"S\x00ED, por ejemplo, yo mato a mis enemigos del juego, pero eso no quita que salgamos un d\x00ED" L"a que otro a tomar unas bebidas.", bison(), C_BISON);
        decir("eggman", L"DR. EGGMAN", L"El tema es ser malo para ser bueno en lo que hacemos.", eggman(), C_EGGMAN);
        decir("clyde", L"CLYDE", L"\x00BFQuieres comentarnos algo, Ralph?", clyde(), C_CLYDE);
        decir("ralph", L"RALPH", L"Ya no quiero ser malo...", ralphBadAnon(), C_RALPH);
        hacer([this] { sorpresa = 0.8; ctx.audio.efecto(L"menu_error.wav"); });
        decir("todos", L"TODOS", L"\x00A1\x00BFQU\x00C9?!", Retrato(), C_TODOS);
        decir("ralph", L"RALPH", L"Pi\x00E9nsenlo, chicos: soy malo, pero nunca me felicitan por serlo, mientras que al bueno le dan una medalla. \x00BF" L"C\x00F3mo debo sentirme con eso?", ralphBadAnon(), C_RALPH);
        decir("clyde", L"CLYDE", L"Mira, Ralph, aqu\x00ED todos somos malos y ninguno es felicitado por serlo, pero eso no quita que seamos personas.", clyde(), C_CLYDE);
        decir("satan", L"SATAN", L"Aqu\x00ED hay un lema que creo que conoces bien.", satan(), C_SATAN);
        elegir(L"\x00BFQU\x00C9 HAR\x00C1 RALPH?", {
            L"ACEPTACI\x00D3N: dictas el lema de la asociaci\x00F3n",
            L"REBELI\x00D3N: \x00A1Voy a demostrar que yo tambi\x00E9n puedo ganar una medalla de bueno!" },
            [this](int r) {
                using namespace Personajes;
                ctx.progreso.decidir("badanon", r + 1);
                if (r == 0) {
                    decir("ralph", L"RALPH", L"Soy malo, y eso es bueno. Nunca ser\x00E9 bueno, y eso no es malo. No hay nadie que quisiera ser m\x00E1s que yo.", ralphBadAnon(), C_RALPH, L"lema_badanon.wav");
                    decir("todos", L"TODOS", L"\x00A1" L"Bien dicho, Ralph!", Retrato(), C_TODOS);
                    hacer([this] { decisionTomada(L"LOS VILLANOS RECORDAR\x00C1N ESTO...", 1, 1); });
                }
                else {
                    decir("ralph", L"RALPH", L"\x00A1Voy a demostrar que yo tambi\x00E9n puedo ganar una medalla de bueno!", ralphBadAnon(), C_RALPH);
                    hacer([this] { sorpresa = 0.6; });
                    decir("todos", L"TODOS", L"...", Retrato(), C_TODOS);
                    hacer([this] { decisionTomada(L"LOS VILLANOS RECORDAR\x00C1N ESTO...", 0, -1); });
                }
            });
        esperar(1.2);
        decir("clyde", L"CLYDE", L"Ralph, recuerda: en esta asociaci\x00F3n sabes que tienes a m\x00E1s como t\x00FA.", clyde(), C_CLYDE);
        esperar(0.8);
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"badanon.wav"); }
};
