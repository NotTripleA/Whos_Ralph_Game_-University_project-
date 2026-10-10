#pragma once
#define NOMINMAX
#include <windows.h>
#include <string>
#include <vector>
#include "Lienzo.h"
class Consola {
private:
    HANDLE hSalida;
    HANDLE hEntrada;
    int columnas;
    int filas;
    int offX;
    int offY;
    std::vector<Celda> anterior;
    bool forzarTodo;
    unsigned cuadros;
    std::string buffer;
    Color fgActual, bgActual;
    bool fgValido, bgValido;
    static void anadirUtf8(std::string& s, wchar_t c) {
        unsigned int u = (unsigned int)c;
        if (u < 0x80) s += (char)u;
        else if (u < 0x800) { s += (char)(0xC0 | (u >> 6)); s += (char)(0x80 | (u & 0x3F)); }
        else { s += (char)(0xE0 | (u >> 12)); s += (char)(0x80 | ((u >> 6) & 0x3F)); s += (char)(0x80 | (u & 0x3F)); }
    }
    void anadirNumero(int n) {
        char t[8];
        int len = 0;
        if (n == 0) t[len++] = '0';
        while (n > 0) { t[len++] = (char)('0' + n % 10); n /= 10; }
        while (len > 0) buffer += t[--len];
    }
    void ponerFrente(const Color& c) {
        if (fgValido && fgActual == c) return;
        buffer += "\x1b[38;2;";
        anadirNumero(c.r); buffer += ';'; anadirNumero(c.g); buffer += ';'; anadirNumero(c.b); buffer += 'm';
        fgActual = c; fgValido = true;
    }
    void ponerFondo(const Color& c) {
        if (bgValido && bgActual == c) return;
        buffer += "\x1b[48;2;";
        anadirNumero(c.r); buffer += ';'; anadirNumero(c.g); buffer += ';'; anadirNumero(c.b); buffer += 'm';
        bgActual = c; bgValido = true;
    }
    void escribir(const std::string& s) {
        DWORD escritos = 0;
        size_t pos = 0;
        while (pos < s.size()) {
            DWORD trozo = (DWORD)(s.size() - pos > 1 << 20 ? 1 << 20 : s.size() - pos);
            WriteConsoleA(hSalida, s.data() + pos, trozo, &escritos, NULL);
            pos += trozo;
        }
    }
public:
    Consola() : hSalida(NULL), hEntrada(NULL), columnas(0), filas(0), offX(0), offY(0),
        forzarTodo(true), cuadros(0), fgValido(false), bgValido(false) {}
    static bool necesitaConhost(int argc, wchar_t** argv) {
        for (int i = 1; i < argc; i++) {
            std::wstring a = argv[i];
            if (a == L"--conhost" || a == L"--sin-relanzar") return false;
        }
        HWND h = GetConsoleWindow();
        if (h == NULL) return true;
        wchar_t clase[64] = {};
        GetClassNameW(h, clase, 64);
        return std::wstring(clase) != L"ConsoleWindowClass";
    }
    static bool relanzarEnConhost(int argc = 0, wchar_t** argv = nullptr) {
        wchar_t exe[MAX_PATH];
        GetModuleFileNameW(NULL, exe, MAX_PATH);
        std::wstring cmd = L"conhost.exe \"" + std::wstring(exe) + L"\" --conhost";
        for (int i = 1; i < argc; i++) cmd += L" \"" + std::wstring(argv[i]) + L"\"";
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi = {};
        if (!CreateProcessW(NULL, &cmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) return false;
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return true;
    }
    void iniciar(const wchar_t* titulo, int anchoLienzo, int altoLienzo) {
        hSalida = GetStdHandle(STD_OUTPUT_HANDLE);
        hEntrada = GetStdHandle(STD_INPUT_HANDLE);
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        SetConsoleTitleW(titulo);
        CONSOLE_FONT_INFOEX f = {};
        f.cbSize = sizeof(f);
        f.dwFontSize.X = 8;
        f.dwFontSize.Y = 8;
        f.FontFamily = FF_DONTCARE;
        f.FontWeight = FW_NORMAL;
        wcscpy_s(f.FaceName, L"Terminal");
        SetCurrentConsoleFontEx(hSalida, FALSE, &f);
        CONSOLE_FONT_INFOEX real = {};
        real.cbSize = sizeof(real);
        if (GetCurrentConsoleFontEx(hSalida, FALSE, &real) && real.dwFontSize.Y != 8) {
            f.dwFontSize.X = 0;
            f.FontFamily = FF_MODERN | FIXED_PITCH;
            wcscpy_s(f.FaceName, L"Consolas");
            SetCurrentConsoleFontEx(hSalida, FALSE, &f);
        }
        SetConsoleMode(hSalida, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
        SetConsoleMode(hEntrada, ENABLE_EXTENDED_FLAGS);
        COORD dim = {};
        bool completa = SetConsoleDisplayMode(hSalida, CONSOLE_FULLSCREEN_MODE, &dim) != 0;
        if (completa) Sleep(150);
        COORD mayor = GetLargestConsoleWindowSize(hSalida);
        columnas = mayor.X > 0 ? mayor.X : anchoLienzo;
        filas = mayor.Y > 0 ? mayor.Y : altoLienzo;
        SMALL_RECT minimo = { 0, 0, 1, 1 };
        SetConsoleWindowInfo(hSalida, TRUE, &minimo);
        SetConsoleScreenBufferSize(hSalida, COORD{ (SHORT)columnas, (SHORT)filas });
        SMALL_RECT r = { 0, 0, (SHORT)(columnas - 1), (SHORT)(filas - 1) };
        SetConsoleWindowInfo(hSalida, TRUE, &r);
        HWND ventana = GetConsoleWindow();
        if (ventana && !completa) {
            ShowWindow(ventana, SW_MAXIMIZE);
            LONG estilo = GetWindowLongW(ventana, GWL_STYLE);
            estilo &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);
            SetWindowLongW(ventana, GWL_STYLE, estilo);
            SetWindowPos(ventana, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
        if (ventana) {
            ShowScrollBar(ventana, SB_BOTH, FALSE);
            SetForegroundWindow(ventana);
        }
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(hSalida, &info)) {
            columnas = info.srWindow.Right - info.srWindow.Left + 1;
            filas = info.srWindow.Bottom - info.srWindow.Top + 1;
        }
        CONSOLE_CURSOR_INFO cursor = { 1, FALSE };
        SetConsoleCursorInfo(hSalida, &cursor);
        escribir("\x1b[?25l\x1b[?7l\x1b[48;2;0;0;0m\x1b[2J");
        forzarTodo = true;
    }
    void redibujarTodo() {
        forzarTodo = true;
        escribir("\x1b[48;2;0;0;0m\x1b[2J");
        bgValido = false;
        fgValido = false;
    }
    void presentar(const Lienzo& l) {
        buffer = "\x1b[?25l";
        if (++cuadros % 15 == 0) {
            HWND ventana = GetConsoleWindow();
            if (ventana) ShowScrollBar(ventana, SB_BOTH, FALSE);
        }
        const int w = l.getAnchoFisico();
        const int h = l.getAltoFisico();
        if ((int)anterior.size() != w * h) { anterior.assign(w * h, Celda()); forzarTodo = true; }
        offX = (columnas - w) / 2;
        offY = (filas - h) / 2;
        for (int y = 0; y < h; y++) {
            int sy = y + offY;
            if (sy < 0 || sy >= filas) continue;
            int x = 0;
            while (x < w) {
                const Celda& c = l.celda(x, y);
                if (!forzarTodo && c == anterior[y * w + x]) { x++; continue; }
                int sx = x + offX;
                if (sx < 0 || sx >= columnas) { x++; continue; }
                buffer += "\x1b[";
                anadirNumero(sy + 1); buffer += ';'; anadirNumero(sx + 1); buffer += 'H';
                while (x < w && x + offX < columnas) {
                    const Celda& k = l.celda(x, y);
                    if (!forzarTodo && k == anterior[y * w + x]) {
                        int fin = x;
                        while (fin < w && fin - x < 6 && l.celda(fin, y) == anterior[y * w + fin]) fin++;
                        if (fin - x >= 6 || fin >= w) break;
                    }
                    if (k.glifo == Glifo::BLOQUE) ponerFrente(k.fg);
                    else if (k.glifo == Glifo::VACIO) ponerFondo(k.bg);
                    else { ponerFrente(k.fg); ponerFondo(k.bg); }
                    anadirUtf8(buffer, k.glifo);
                    anterior[y * w + x] = k;
                    x++;
                }
            }
        }
        forzarTodo = false;
        if (!buffer.empty()) escribir(buffer);
    }
    void restaurar() {
        escribir("\x1b[0m\x1b[?25h\x1b[2J");
    }
    HANDLE getEntrada() const { return hEntrada; }
    int getColumnas() const { return columnas; }
    int getFilas() const { return filas; }
};
