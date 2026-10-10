#pragma once
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include "Rutas.h"
#pragma comment(lib, "winmm.lib")
class Audio {
private:
    std::wstring musicaActual;
    bool abierta;
    bool repetirManual;
    bool enBucle;
    long desde, hasta;
    bool pausada;
    bool vozAbierta;
    DWORD ultimoChequeo;
    int volumenPuesto;
    static const int VOLUMEN_MUSICA = 330;
    static const int VOLUMEN_CON_VOZ = 140;
    void ponerVolumen(int v) {
        if (!abierta || v == volumenPuesto) return;
        std::wstring cmd = L"setaudio musica volume to " + std::to_wstring(v);
        mciSendStringW(cmd.c_str(), NULL, 0, NULL);
        volumenPuesto = v;
    }
    static std::wstring sinTildes(std::wstring s) {
        for (auto& c : s) {
            if (c == L'\x00E1') c = L'a'; else if (c == L'\x00E9') c = L'e'; else if (c == L'\x00ED') c = L'i';
            else if (c == L'\x00F3') c = L'o'; else if (c == L'\x00FA') c = L'u';
        }
        return s;
    }
    static std::wstring buscar(const std::vector<std::wstring>& carpetas, const std::wstring& archivo) {
        std::vector<std::wstring> nombres = { archivo };
        if (sinTildes(archivo) != archivo) nombres.push_back(sinTildes(archivo));
        for (size_t i = 0, n = nombres.size(); i < n; i++) {
            std::wstring s = nombres[i];
            if (s.size() > 4 && s.compare(s.size() - 4, 4, L".wav") == 0) nombres.push_back(s.substr(0, s.size() - 4) + L".mp3");
            else if (s.size() > 4 && s.compare(s.size() - 4, 4, L".mp3") == 0) nombres.push_back(s.substr(0, s.size() - 4) + L".wav");
        }
        for (const auto& carpeta : carpetas)
            for (const auto& n : nombres) {
                std::wstring ruta = Rutas::en(carpeta + L"\\" + n);
                if (Rutas::existeArchivo(ruta)) return ruta;
            }
        return L"";
    }
    static std::vector<std::wstring> carpetasMusica() { return { L"M\x00FAsica", L"Musica", L"..\\Soundtrack\\M\x00FAsica", L"Sonidos" }; }
    static std::vector<std::wstring> carpetasVoz() { return { L"Dialogos de voz", L"..\\Soundtrack\\Dialogos de voz" }; }
    static long consultar(const wchar_t* cmd) {
        wchar_t r[64] = {};
        if (mciSendStringW(cmd, r, 64, NULL) != 0) return -1;
        return _wtol(r);
    }
    void tocarSegmento() {
        std::wstring cmd = L"play musica from " + std::to_wstring(desde);
        if (hasta >= 0) cmd += L" to " + std::to_wstring(hasta);
        mciSendStringW(cmd.c_str(), NULL, 0, NULL);
    }
public:
    Audio() : abierta(false), repetirManual(false), enBucle(true), desde(0), hasta(-1), pausada(false), vozAbierta(false), ultimoChequeo(0), volumenPuesto(-1) {}
    ~Audio() { detenerMusica(); detenerVoz(); }
    static bool existeMusica(const std::wstring& archivo) { return !buscar(carpetasMusica(), archivo).empty(); }
    static bool existeVoz(const std::wstring& archivo) { return !buscar(carpetasVoz(), archivo).empty(); }
    void reproducirMusica(const std::wstring& archivo, bool bucle = true) {
        if (archivo == musicaActual && desde == 0 && hasta < 0) return;
        reproducirSegmento(archivo, 0, -1, bucle);
    }
    void reproducirSegmento(const std::wstring& archivo, long desdeMs, long hastaMs, bool bucle) {
        detenerMusica();
        musicaActual = archivo;
        desde = desdeMs;
        hasta = hastaMs;
        enBucle = bucle;
        std::wstring ruta = buscar(carpetasMusica(), archivo);
        if (ruta.empty()) return;
        std::wstring abrir = L"open \"" + ruta + L"\" type mpegvideo alias musica";
        if (mciSendStringW(abrir.c_str(), NULL, 0, NULL) != 0) return;
        mciSendStringW(L"set musica time format milliseconds", NULL, 0, NULL);
        abierta = true;
        volumenPuesto = -1;
        ponerVolumen(vozSonando() ? VOLUMEN_CON_VOZ : VOLUMEN_MUSICA);
        if (bucle && desde == 0 && hasta < 0) {
            repetirManual = mciSendStringW(L"play musica repeat", NULL, 0, NULL) != 0;
            if (repetirManual) mciSendStringW(L"play musica", NULL, 0, NULL);
        }
        else {
            repetirManual = bucle;
            tocarSegmento();
        }
    }
    void detenerMusica() {
        if (abierta) mciSendStringW(L"close musica", NULL, 0, NULL);
        abierta = false;
        pausada = false;
        musicaActual.clear();
        desde = 0;
        hasta = -1;
    }
    void pausarMusica() {
        if (vozAbierta) mciSendStringW(L"pause voz", NULL, 0, NULL);
        if (abierta && !pausada) { mciSendStringW(L"pause musica", NULL, 0, NULL); pausada = true; }
    }
    void reanudarMusica() {
        if (vozAbierta) mciSendStringW(L"resume voz", NULL, 0, NULL);
        if (abierta && pausada) { mciSendStringW(L"resume musica", NULL, 0, NULL); pausada = false; }
    }
    bool musicaTerminada() const {
        if (!abierta) return true;
        if (pausada) return false;
        wchar_t estado[32] = {};
        mciSendStringW(L"status musica mode", estado, 32, NULL);
        if (std::wstring(estado) == L"stopped") return true;
        if (hasta >= 0) return consultar(L"status musica position") >= hasta;
        return false;
    }
    const std::wstring& getMusicaActual() const { return musicaActual; }
    void actualizar() {
        if (!abierta || pausada) return;
        DWORD ahora = GetTickCount();
        if (ahora - ultimoChequeo < 50) return;
        ultimoChequeo = ahora;
        ponerVolumen(vozSonando() ? VOLUMEN_CON_VOZ : VOLUMEN_MUSICA);
        if (!repetirManual) return;
        wchar_t estado[32] = {};
        mciSendStringW(L"status musica mode", estado, 32, NULL);
        bool fin = std::wstring(estado) == L"stopped";
        if (!fin && hasta >= 0) fin = consultar(L"status musica position") >= hasta - 30;
        if (fin) tocarSegmento();
    }
    bool reproducirVoz(const std::wstring& archivo) {
        detenerVoz();
        std::wstring ruta = buscar(carpetasVoz(), archivo);
        if (ruta.empty()) return false;
        std::wstring abrir = L"open \"" + ruta + L"\" type mpegvideo alias voz";
        if (mciSendStringW(abrir.c_str(), NULL, 0, NULL) != 0) return false;
        vozAbierta = true;
        mciSendStringW(L"play voz", NULL, 0, NULL);
        ponerVolumen(VOLUMEN_CON_VOZ);
        return true;
    }
    bool vozSonando() const {
        if (!vozAbierta) return false;
        wchar_t estado[32] = {};
        mciSendStringW(L"status voz mode", estado, 32, NULL);
        return std::wstring(estado) == L"playing";
    }
    void detenerVoz() {
        if (vozAbierta) mciSendStringW(L"close voz", NULL, 0, NULL);
        vozAbierta = false;
    }
    void efecto(const std::wstring& archivo) {
        std::wstring ruta = buscar({ L"Sonidos", L"..\\Soundtrack\\Sonidos" }, archivo);
        if (!ruta.empty()) PlaySoundW(ruta.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }
};
