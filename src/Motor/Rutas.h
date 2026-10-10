#pragma once
#define NOMINMAX
#include <windows.h>
#include <string>
class Rutas {
private:
    static bool existeCarpeta(const std::wstring& p) {
        DWORD a = GetFileAttributesW(p.c_str());
        return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
    }
    static std::wstring buscarBase() {
        wchar_t exe[MAX_PATH];
        GetModuleFileNameW(NULL, exe, MAX_PATH);
        std::wstring dir = exe;
        for (int nivel = 0; nivel < 5; nivel++) {
            size_t p = dir.find_last_of(L"\\/");
            if (p == std::wstring::npos) break;
            dir = dir.substr(0, p);
            if (existeCarpeta(dir + L"\\Recursos\\Sprites")) return dir;
        }
        wchar_t actual[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, actual);
        return actual;
    }
public:
    static const std::wstring& base() {
        static std::wstring b = buscarBase();
        return b;
    }
    static std::wstring en(const std::wstring& relativa) { return base() + L"\\" + relativa; }
    static bool existeArchivo(const std::wstring& p) {
        DWORD a = GetFileAttributesW(p.c_str());
        return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
    }
};
