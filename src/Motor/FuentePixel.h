#pragma once
#include <map>
#include <string>
#include "BancoSprites.h"
class FuentePixel {
private:
    std::map<wchar_t, const Sprite*> glifos;
    Color color;
    static const char* mascaraExtra(wchar_t c) {
        switch (c) {
        case L'?': return ".aaaaa.." "aa...aa." ".....aa." "...aaa.." "...aa..." "........" "...aa..." "........";
        case L'\x00BF': return "..aa...." "........" "..aa...." ".aaa...." "aa......" "aa...aa." ".aaaaa.." "........";
        case L'\x00A1': return "....a..." "........" "...a...." "..aa...." ".aaa...." ".aaa...." ".aa....." "........";
        case L':': return "........" "..aa...." "..aa...." "........" "..aa...." "..aa...." "........" "........";
        case L',': return "........" "........" "........" "........" "........" ".aa....." ".aa....." "aa......";
        case L'[': return ".aaaa..." ".aa....." ".aa....." ".aa....." ".aa....." ".aa....." ".aaaa..." "........";
        case L']': return ".aaaa..." "...aa..." "...aa..." "...aa..." "...aa..." "...aa..." ".aaaa..." "........";
        case L'/': return "......aa" ".....aa." "....aa.." "...aa..." "..aa...." ".aa....." "aa......" "........";
        case L'(': return "...aa..." "..aa...." ".aa....." ".aa....." ".aa....." "..aa...." "...aa..." "........";
        case L')': return ".aa....." "..aa...." "...aa..." "...aa..." "...aa..." "..aa...." ".aa....." "........";
        case L'\x2191': return "...a...." "..aaa..." ".aaaaa.." "aaaaaaa." "..aaa..." "..aaa..." "..aaa..." "........";
        case L'\x2193': return "..aaa..." "..aaa..." "..aaa..." "aaaaaaa." ".aaaaa.." "..aaa..." "...a...." "........";
        case L'\x2190': return "...a...." "..aa...." ".aaaaaaa" "aaaaaaaa" ".aaaaaaa" "..aa...." "...a...." "........";
        case L'\x2192': return "....a..." "....aa.." "aaaaaaa." "aaaaaaaa" "aaaaaaa." "....aa.." "....a..." "........";
        }
        return nullptr;
    }
    static wchar_t base(wchar_t c, int& marca) {
        marca = 0;
        switch (c) {
        case L'\x00C1': case L'\x00E1': marca = 1; return L'A';
        case L'\x00C9': case L'\x00E9': marca = 1; return L'E';
        case L'\x00CD': case L'\x00ED': marca = 1; return L'I';
        case L'\x00D3': case L'\x00F3': marca = 1; return L'O';
        case L'\x00DA': case L'\x00FA': marca = 1; return L'U';
        case L'\x00DC': case L'\x00FC': marca = 3; return L'U';
        case L'\x00D1': case L'\x00F1': marca = 2; return L'N';
        }
        if (c >= L'a' && c <= L'z') return (wchar_t)(c - L'a' + L'A');
        return c;
    }
public:
    explicit FuentePixel(const std::string& carpeta) {
        std::string ruta = "01_Fix-It_Felix_Jr/UI/fuente_" + carpeta + "/";
        for (char c = '0'; c <= '9'; c++) glifos[(wchar_t)c] = SPR(ruta + c);
        for (char c = 'A'; c <= 'Z'; c++) glifos[(wchar_t)c] = SPR(ruta + c);
        glifos[L'\''] = SPR(ruta + "apostrofe");
        glifos[L'\x00A9'] = SPR(ruta + "copyright");
        glifos[L'!'] = SPR(ruta + "exclamacion");
        glifos[L'-'] = SPR(ruta + "guion");
        glifos[L'.'] = SPR(ruta + "punto");
        const Sprite* a = glifos[L'A'];
        color = Colores::BLANCO;
        for (int y = 0; y < a->getAlto(); y++)
            for (int x = 0; x < a->getAncho(); x++)
                if (a->opaco(x, y)) { color = a->color(x, y); y = 99; break; }
    }
    Color getColor() const { return color; }
    static int medir(const std::wstring& t) { return (int)t.size() * 8; }
    void dibujarCaracter(Lienzo& l, int x, int y, wchar_t ch, const Color* tinte = nullptr) const {
        int marca;
        wchar_t b = base(ch, marca);
        Color c = tinte ? *tinte : color;
        auto it = glifos.find(b);
        if (it != glifos.end()) {
            if (tinte) it->second->dibujarSilueta(l, x, y, c);
            else it->second->dibujar(l, x, y);
        }
        else if (const char* m = mascaraExtra(b)) {
            for (int j = 0; j < 8; j++)
                for (int i = 0; i < 8; i++)
                    if (m[j * 8 + i] == 'a') l.pixel(x + i, y + j, c);
        }
        if (marca == 1) { l.pixel(x + 4, y - 3, c); l.pixel(x + 5, y - 3, c); l.pixel(x + 3, y - 2, c); l.pixel(x + 4, y - 2, c); }
        if (marca == 2) {
            l.pixel(x + 2, y - 3, c); l.pixel(x + 3, y - 3, c); l.pixel(x + 6, y - 3, c);
            l.pixel(x + 1, y - 2, c); l.pixel(x + 4, y - 2, c); l.pixel(x + 5, y - 2, c);
        }
        if (marca == 3) { l.pixel(x + 1, y - 2, c); l.pixel(x + 2, y - 2, c); l.pixel(x + 4, y - 2, c); l.pixel(x + 5, y - 2, c); }
    }
    void dibujar(Lienzo& l, int x, int y, const std::wstring& t) const {
        for (size_t i = 0; i < t.size(); i++) dibujarCaracter(l, x + (int)i * 8, y, t[i]);
    }
    void dibujarConSombra(Lienzo& l, int x, int y, const std::wstring& t, Color sombra = Colores::NEGRO) const {
        for (size_t i = 0; i < t.size(); i++) dibujarCaracter(l, x + (int)i * 8 + 1, y + 1, t[i], &sombra);
        dibujar(l, x, y, t);
    }
    void dibujarCentrado(Lienzo& l, int y, const std::wstring& t, bool sombra = true) const {
        int x = (l.getAncho() - medir(t)) / 2;
        if (sombra) dibujarConSombra(l, x, y, t); else dibujar(l, x, y, t);
    }
};
