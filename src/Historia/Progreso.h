#pragma once
#include <string>
#include <map>
#include <fstream>
#include <filesystem>
#include "../Motor/Rutas.h"
class Progreso {
private:
    int reputacion;
    int empatia;
    int nivelesCompletados;
    int puntuacion;
    int positivas, negativas;
    std::string punto;
    std::map<std::string, int> decisiones;
public:
    static const int NIVELES_TOTALES = 3;
    static const int FINALES_TOTALES = 3;
    Progreso() : reputacion(0), empatia(0), nivelesCompletados(0), puntuacion(0), positivas(0), negativas(0), punto("PROLOGO") {}
    void reiniciar() { *this = Progreso(); }
    int getReputacion() const { return reputacion; }
    int getEmpatia() const { return empatia; }
    int getNivelesCompletados() const { return nivelesCompletados; }
    int getPuntuacion() const { return puntuacion; }
    int getPositivas() const { return positivas; }
    int getNegativas() const { return negativas; }
    void sumarPuntos(int p) { puntuacion += p; }
    void contarDecision(int efecto) { if (efecto > 0) positivas++; else if (efecto < 0) negativas++; }
    bool escuadronSalvado() const { return decision("heros") == 1; }
    int aliadosDisponibles() const { return reputacion <= 0 ? 0 : (reputacion >= 4 ? 4 : reputacion); }
    static std::wstring rutaFinales() { return Rutas::en(L"Guardado\\finales.txt"); }
    static int finalesDescubiertos(bool* cuales = nullptr) {
        std::ifstream f{ std::filesystem::path(rutaFinales()) };
        bool v[FINALES_TOTALES] = { false, false, false };
        int n;
        while (f >> n) if (n >= 1 && n <= FINALES_TOTALES) v[n - 1] = true;
        int total = 0;
        for (int i = 0; i < FINALES_TOTALES; i++) { if (cuales) cuales[i] = v[i]; if (v[i]) total++; }
        return total;
    }
    static void descubrirFinal(int n) {
        bool v[FINALES_TOTALES];
        finalesDescubiertos(v);
        v[n - 1] = true;
        std::error_code ec;
        std::filesystem::create_directories(std::filesystem::path(Rutas::en(L"Guardado")), ec);
        std::ofstream f{ std::filesystem::path(rutaFinales()) };
        for (int i = 0; i < FINALES_TOTALES; i++) if (v[i]) f << (i + 1) << "\n";
    }
    const std::string& getPunto() const { return punto; }
    void setPunto(const std::string& p) { punto = p; }
    void cambiarReputacion(int d) { reputacion += d; }
    void cambiarEmpatia(int d) { empatia += d; }
    void completarNivel(int n) { if (n > nivelesCompletados) nivelesCompletados = n; }
    void decidir(const std::string& clave, int opcion) { decisiones[clave] = opcion; }
    int decision(const std::string& clave) const {
        auto it = decisiones.find(clave);
        return it == decisiones.end() ? 0 : it->second;
    }
    std::wstring etiquetaReputacion() const {
        if (reputacion <= -2) return L"TEMIDA";
        if (reputacion == -1) return L"DUDOSA";
        if (reputacion == 0) return L"PAC\x00CD" L"FICA";
        if (reputacion == 1) return L"AMIGABLE";
        if (reputacion == 2) return L"RESPETADA";
        return L"HEROICA";
    }
    std::wstring etiquetaEmpatia() const {
        if (empatia <= -2) return L"FR\x00CD" L"A";
        if (empatia == -1) return L"DISTANTE";
        if (empatia == 0) return L"NEUTRAL";
        if (empatia == 1) return L"C\x00C1" L"LIDA";
        return L"EMP\x00C1TICA";
    }
    static std::wstring rutaArchivo() { return Rutas::en(L"Guardado\\partida.txt"); }
    static bool hayPartida() { return Rutas::existeArchivo(rutaArchivo()); }
    bool guardar() const {
        std::error_code ec;
        std::filesystem::create_directories(std::filesystem::path(Rutas::en(L"Guardado")), ec);
        std::ofstream f{ std::filesystem::path(rutaArchivo()) };
        if (!f) return false;
        f << "# Partida guardada de WHO'S RALPH? (no editar a mano)\n";
        f << "punto=" << punto << "\n";
        f << "reputacion=" << reputacion << "\n";
        f << "empatia=" << empatia << "\n";
        f << "niveles=" << nivelesCompletados << "\n";
        f << "puntuacion=" << puntuacion << "\n";
        f << "positivas=" << positivas << "\n";
        f << "negativas=" << negativas << "\n";
        for (const auto& d : decisiones) f << "decision." << d.first << "=" << d.second << "\n";
        return true;
    }
    bool cargar() {
        std::ifstream f{ std::filesystem::path(rutaArchivo()) };
        if (!f) return false;
        reiniciar();
        std::string linea;
        while (std::getline(f, linea)) {
            if (!linea.empty() && linea.back() == '\r') linea.pop_back();
            size_t igual = linea.find('=');
            if (linea.empty() || linea[0] == '#' || igual == std::string::npos) continue;
            std::string clave = linea.substr(0, igual), valor = linea.substr(igual + 1);
            if (clave == "punto") punto = valor;
            else if (clave == "reputacion") reputacion = atoi(valor.c_str());
            else if (clave == "empatia") empatia = atoi(valor.c_str());
            else if (clave == "niveles") nivelesCompletados = atoi(valor.c_str());
            else if (clave == "puntuacion") puntuacion = atoi(valor.c_str());
            else if (clave == "positivas") positivas = atoi(valor.c_str());
            else if (clave == "negativas") negativas = atoi(valor.c_str());
            else if (clave.rfind("decision.", 0) == 0) decisiones[clave.substr(9)] = atoi(valor.c_str());
        }
        return true;
    }
};
