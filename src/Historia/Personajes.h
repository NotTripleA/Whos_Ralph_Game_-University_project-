#pragma once
#include "InterfazHistoria.h"
#include "../Motor/BancoSprites.h"
#include "../Motor/Animacion.h"
namespace Personajes {
    const std::string FIXIT = "01_Fix-It_Felix_Jr/";
    const std::string BADANON = "11_Bad-Anon_villanos_IA/";
    const Color C_RALPH(255, 120, 90);
    const Color C_FELIX(110, 170, 255);
    const Color C_NICE(255, 170, 220);
    const Color C_BOWSER(255, 200, 60);
    const Color C_BISON(230, 70, 90);
    const Color C_EGGMAN(255, 140, 60);
    const Color C_CLYDE(255, 170, 60);
    const Color C_SATAN(255, 70, 70);
    const Color C_TODOS(220, 220, 220);
    inline Retrato ralphArcade() { return Retrato(SPR(FIXIT + "Ralph/fila_1/ralph_f1_01"), SPR(FIXIT + "Ralph/fila_1/ralph_f1_03")); }
    inline Retrato felix() { return Retrato(SPR(FIXIT + "Felix/fila_1/felix_f1_11"), SPR(FIXIT + "Felix/fila_1/felix_f1_07")); }
    inline Retrato nicelander(int n) {
        char id[64];
        sprintf_s(id, "Nicelanders/cara_%02d", n);
        return Retrato(SPR(FIXIT + id));
    }
    inline Retrato villano(const std::string& carpeta, const std::string& base, const std::string& habla) {
        return Retrato(SPR(BADANON + carpeta + "/" + base), SPR(BADANON + carpeta + "/" + habla));
    }
    inline Retrato ralphBadAnon() { return ralphArcade(); }
    inline Retrato bowser() { return villano("Bowser", "base/bowser_base_01", "hablando/bowser_hablando_01"); }
    inline Retrato bison() { return villano("M_Bison", "base/m_bison_base_01", "hablando/m_bison_hablando_01"); }
    inline Retrato eggman() { return villano("Dr_Eggman", "base/dr_eggman_base_01", "hablando/dr_eggman_hablando_01"); }
    inline Retrato clyde() { return villano("Clyde", "base/clyde_base_01", "hablando/clyde_hablando_01"); }
    inline Retrato satan() { return villano("Satan_(Satans_Hollow)", "base/satan_(satans_hollow)_base_01", "hablando/satan_(satans_hollow)_hablando_01"); }
    inline Animacion ralphParado() { return Animacion(BancoSprites::instancia().lista(FIXIT + "Ralph/fila_1/ralph_f1_", { 1 }), 1); }
    inline Animacion ralphCamina() { return Animacion(BancoSprites::instancia().secuencia(FIXIT + "Ralph/fila_3/ralph_f3_", 1, 8), 8); }
    inline Animacion ralphTriste() { return Animacion(BancoSprites::instancia().lista(FIXIT + "Ralph/fila_3/ralph_f3_", { 1 }), 1); }
    inline Animacion felixParado() { return Animacion(BancoSprites::instancia().lista(FIXIT + "Felix/fila_1/felix_f1_", { 11 }), 1); }
    inline Animacion felixCamina() { return Animacion(BancoSprites::instancia().secuencia(FIXIT + "Felix/fila_1/felix_f1_", 1, 4), 8); }
}
