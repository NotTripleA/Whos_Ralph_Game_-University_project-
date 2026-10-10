#pragma once
#include "Cinematica.h"
#include "../Motor/BancoSprites.h"
#include "../Motor/Efectos.h"
class EscenaPrologo : public EscenaCinematica {
private:
    int vineta;
    double tiempoVineta;
    CampoEstrellas estrellas;
    Animacion ralphGolpe, ralphPunos, felixMartillo, felixMedalla;
    double salida;
    static const int SUELO = 80;
    void cambiarVineta(int v) { vineta = v; tiempoVineta = 0; }
    void dibujarSuelo(Lienzo& l, Color c) const {
        for (int x = 0; x < l.getAncho(); x++) { l.pixel(x, SUELO, c); l.sombrear(x, SUELO + 1, Glifo::MEDIO, c); }
    }
protected:
    void actualizarEscenario(double dt) override {
        tiempoVineta += dt;
        estrellas.actualizar(dt);
        ralphGolpe.actualizar(dt);
        ralphPunos.actualizar(dt);
        felixMartillo.actualizar(dt);
        felixMedalla.actualizar(dt);
        if (salida >= 0) salida += dt;
    }
    void dibujarEscenario(Lienzo& l) override {
        l.limpiar(Color(4, 4, 14));
        estrellas.dibujar(l);
        auto& b = BancoSprites::instancia();
        switch (vineta) {
        case 0: {
            const Sprite* logo = b.obtener("01_Fix-It_Felix_Jr/UI/logo_fix_it_felix_jr");
            int y = (int)(-96 + (tiempoVineta < 1.2 ? tiempoVineta / 1.2 : 1.0) * 90);
            logo->dibujar(l, (l.getAncho() - logo->getAncho()) / 2, y);
            if (tiempoVineta > 1.4 && ((int)(tiempoVineta * 2)) % 2 == 0) FuenteMini::centrado(l, 72, L"30 A\x00D1OS DE ARCADE", Colores::AMARILLO);
            break;
        }
        case 1: {
            dibujarSuelo(l, Color(40, 120, 40));
            const Sprite* r = ralphGolpe.actual();
            r->dibujar(l, 60 - r->getAncho() / 2, SUELO - r->getAlto());
            for (int i = 0; i < 3; i++) {
                double t = std::fmod(tiempoVineta * 1.3 + i * 0.33, 1.0);
                int bx = 80 + i * 14 + (int)(t * 30), by = SUELO - 50 + (int)(-30 * t + 70 * t * t);
                b.obtener(i % 2 ? "01_Fix-It_Felix_Jr/Objetos/ladrillo/ladrillo_01" : "01_Fix-It_Felix_Jr/Objetos/ladrillo/ladrillo_02")->dibujar(l, bx, by);
            }
            int estado = 4 - ((int)(tiempoVineta * 1.5) % 4);
            char id[96];
            sprintf_s(id, "01_Fix-It_Felix_Jr/Edificio/ventanas/ventana/ventana_%02d", estado < 1 ? 1 : estado);
            b.obtener(id)->dibujar(l, 182, SUELO - 40);
            const Sprite* f = felixMartillo.actual();
            f->dibujar(l, 168 - f->getAncho() / 2, SUELO - f->getAlto());
            if (felixMartillo.getIndice() == 1) b.obtener("01_Fix-It_Felix_Jr/Efectos/destello/destello_01")->dibujar(l, 178, SUELO - 22);
            break;
        }
        case 2: {
            dibujarSuelo(l, Color(40, 120, 40));
            const Sprite* f = felixMedalla.actual();
            f->dibujar(l, 70 - f->getAncho() / 2, SUELO - f->getAlto());
            const int posiciones[4] = { 14, 32, 96, 114 };
            for (int i = 0; i < 4; i++) {
                char id[64];
                sprintf_s(id, "01_Fix-It_Felix_Jr/Nicelanders/cara_%02d", 1 + i * 3);
                int salto = ((int)(tiempoVineta * 4) + i) % 2 ? 3 : 0;
                b.obtener(id)->dibujar(l, posiciones[i], SUELO - 16 - salto);
            }
            l.rectangulo(150, SUELO - 2, 70, 4, Color(90, 60, 30));
            const Sprite* lodo = b.obtener("01_Fix-It_Felix_Jr/Ralph/fila_5/ralph_f5_08");
            lodo->dibujar(l, 185 - lodo->getAncho() / 2, SUELO + 2 - lodo->getAlto());
            break;
        }
        case 3: {
            const Sprite* dump = b.obtener("01_Fix-It_Felix_Jr/Escenario/basurero_dump");
            dump->dibujar(l, -34, SUELO - dump->getAlto() + 1);
            dibujarSuelo(l, Color(96, 62, 34));
            const Sprite* r = b.obtener("01_Fix-It_Felix_Jr/Ralph/fila_3/ralph_f3_01");
            r->dibujar(l, 186 - r->getAncho() / 2, SUELO - r->getAlto());
            if (((int)(tiempoVineta * 2)) % 2 == 0) FuenteMini::caracter(l, 204, 22, L'?', Colores::BLANCO);
            if (((int)(tiempoVineta * 2 + 1)) % 2 == 0) FuenteMini::caracter(l, 210, 16, L'?', Colores::BLANCO);
            break;
        }
        case 4: {
            if (((int)(tiempoVineta * 6)) % 2 == 0)
                for (int y = 0; y < SUELO; y++) for (int x = 0; x < l.getAncho(); x++) if ((x + y) % 7 == 0) l.sombrear(x, y, Glifo::CLARO, Color(200, 0, 0));
            const Sprite* r = ralphPunos.actual();
            int temblor = ((int)(tiempoVineta * 20)) % 2;
            r->dibujar(l, 120 - r->getAncho() / 2 + temblor, SUELO - r->getAlto());
            break;
        }
        default:
            break;
        }
        if (salida > 0) Efectos::fundido(l, (float)(salida / 0.8));
    }
    void alTerminar() override { ctx.irA(IdEscena::NIVEL1_FIXIT); }
public:
    explicit EscenaPrologo(Contexto& c) : EscenaCinematica(c, IdEscena::PROLOGO), vineta(0), tiempoVineta(0), estrellas(80, 240, 80, 33), salida(-1) {
        auto& b = BancoSprites::instancia();
        ralphGolpe = Animacion(b.lista("01_Fix-It_Felix_Jr/Ralph/fila_2/ralph_f2_", { 8, 9 }), 5);
        ralphPunos = Animacion(b.lista("01_Fix-It_Felix_Jr/Ralph/fila_1/ralph_f1_", { 4, 5 }), 4);
        felixMartillo = Animacion(b.lista("01_Fix-It_Felix_Jr/Felix/fila_1/felix_f1_", { 9, 5 }), 3);
        felixMedalla = Animacion(b.lista("01_Fix-It_Felix_Jr/Felix/fila_1/felix_f1_", { 12, 13, 14, 13 }), 4);
        esperar(0.6);
        narrar(L"Litwak's Family Fun Center. Hace 30 a\x00F1os se encendi\x00F3 por primera vez la m\x00E1quina de FIX-IT FELIX JR.");
        hacer([this] { cambiarVineta(1); });
        narrar(L"Cada d\x00ED" L"a, RALPH destruye el edificio de Niceland... y cada d\x00ED" L"a, FELIX lo repara con su martillo dorado.");
        hacer([this] { cambiarVineta(2); });
        narrar(L"F\x00E9lix recibe medallas, pasteles y aplausos. Ralph, en cambio, siempre termina arrojado al lodo.");
        hacer([this] { cambiarVineta(3); });
        narrar(L"Despu\x00E9s de 30 a\x00F1os de arcade, Ralph empieza a dudar de su rol en el juego.");
        hacer([this] { cambiarVineta(4); ctx.audio.efecto(L"menu_error.wav"); });
        narrar(L"Un d\x00ED" L"a, algo colm\x00F3 su paciencia y quiso hacer un cambio en su vida...");
        hacer([this] { cambiarVineta(5); });
        narrar(L"...solo que hab\x00ED" L"a un problema.");
        hacer([this] { salida = 0.001; });
        esperar(0.9);
    }
    void entrarHistoria() override { ctx.audio.reproducirMusica(L"prologo.wav"); }
};
