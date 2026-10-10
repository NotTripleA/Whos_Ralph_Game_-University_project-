#pragma once
#include <string>
#include <vector>
#include "Lienzo.h"
class FuenteMini {
private:
    static const char* glifo(wchar_t c) {
        switch (c) {
        case L'A': return ".#.#.#####.##.#";
        case L'B': return "##.#.###.#.###.";
        case L'C': return ".###..#..#...##";
        case L'D': return "##.#.##.##.###.";
        case L'E': return "####..##.#..###";
        case L'F': return "####..##.#..#..";
        case L'G': return ".###..#.##.#.##";
        case L'H': return "#.##.#####.##.#";
        case L'I': return "###.#..#..#.###";
        case L'J': return "..#..#..##.#.#.";
        case L'K': return "#.##.###.#.##.#";
        case L'L': return "#..#..#..#..###";
        case L'M': return "#.########.##.#";
        case L'N': return "##.#.##.##.##.#";
        case L'O': return ".#.#.##.##.#.#.";
        case L'P': return "##.#.###.#..#..";
        case L'Q': return ".#.#.##.###..##";
        case L'R': return "##.#.###.#.##.#";
        case L'S': return ".###...#...###.";
        case L'T': return "###.#..#..#..#.";
        case L'U': return "#.##.##.##.####";
        case L'V': return "#.##.##.##.#.#.";
        case L'W': return "#.##.########.#";
        case L'X': return "#.##.#.#.#.##.#";
        case L'Y': return "#.##.#.#..#..#.";
        case L'Z': return "###..#.#.#..###";
        case L'0': return "####.##.##.####";
        case L'1': return ".#.##..#..#.###";
        case L'2': return "##...#.#.#..###";
        case L'3': return "##...#.#...###.";
        case L'4': return "#.##.####..#..#";
        case L'5': return "####..##...###.";
        case L'6': return ".###..####.####";
        case L'7': return "###..#.#..#..#.";
        case L'8': return "####.#####.####";
        case L'9': return "####.####..###.";
        case L'.': return "............#..";
        case L',': return ".........#..#..";
        case L'!': return ".#..#..#.....#.";
        case L'?': return "##...#.#.....#.";
        case L'\x00A1': return ".#.....#..#..#.";
        case L'\x00BF': return ".#.....#.#...##";
        case L':': return "....#.....#....";
        case L';': return "....#.....#.#..";
        case L'-': return "......###......";
        case L'+': return "....#.###.#....";
        case L'\'': return ".#..#..........";
        case L'"': return "#.##.#.........";
        case L'(': return "..#.#..#..#...#";
        case L')': return "#...#..#..#.#..";
        case L'[': return "##.#..#..#..##.";
        case L']': return ".##..#..#..#.##";
        case L'/': return "..#..#.#.#..#..";
        case L'%': return "#.#..#.#.#..#.#";
        case L'|': return ".#..#..#..#..#.";
        case L'*': return "....#.#.#.#....";
        case L'=': return "...###...###...";
        case L'>': return "#...#...#.#.#..";
        case L'<': return "..#.#.#...#...#";
        case L'_': return "............###";
        case L'#': return "#.####.####.#.#";
        case L'\x2191': return ".#.###.#..#..#.";
        case L'\x2193': return ".#..#..#.###.#.";
        case L'\x2190': return "..#.#####.##..#";
        case L'\x2192': return "#..##.#####.#..";
        case L'\x25BC': return "...###.#.......";
        case L'\x2665': return "#.#####.#..#...";
        }
        return nullptr;
    }
    static wchar_t base(wchar_t c, int& marca) {
        marca = 0;
        switch (c) {
        case L'\x00E1': case L'\x00C1': marca = 1; return L'A';
        case L'\x00E9': case L'\x00C9': marca = 1; return L'E';
        case L'\x00ED': case L'\x00CD': marca = 1; return L'I';
        case L'\x00F3': case L'\x00D3': marca = 1; return L'O';
        case L'\x00FA': case L'\x00DA': marca = 1; return L'U';
        case L'\x00FC': case L'\x00DC': marca = 3; return L'U';
        case L'\x00F1': case L'\x00D1': marca = 2; return L'N';
        }
        if (c >= L'a' && c <= L'z') return (wchar_t)(c - L'a' + L'A');
        return c;
    }
public:
    static const int ANCHO = 4;
    static const int LINEA = 8;
    static void caracter(Lienzo& l, int x, int y, wchar_t ch, Color c) {
        int marca;
        wchar_t b = base(ch, marca);
        const char* g = glifo(b);
        int y0 = y + 2;
        if (g) {
            std::string m(g);
            while (m.size() < 15) m += '.';
            for (int j = 0; j < 5; j++)
                for (int i = 0; i < 3; i++)
                    if (m[j * 3 + i] == '#') l.pixel(x + i, y0 + j, c);
        }
        if (marca == 1) { l.pixel(x + 2, y, c); l.pixel(x + 1, y + 1, c); }
        if (marca == 2) { l.pixel(x + 1, y, c); l.pixel(x + 2, y, c); l.pixel(x, y + 1, c); l.pixel(x + 1, y + 1, c); }
        if (marca == 3) { l.pixel(x, y + 1, c); l.pixel(x + 2, y + 1, c); }
    }
    static void texto(Lienzo& l, int x, int y, const std::wstring& t, Color c) {
        for (size_t i = 0; i < t.size(); i++) caracter(l, x + (int)i * ANCHO, y, t[i], c);
    }
    static void textoConSombra(Lienzo& l, int x, int y, const std::wstring& t, Color c, Color sombra = Colores::NEGRO) {
        texto(l, x + 1, y + 1, t, sombra);
        texto(l, x, y, t, c);
    }
    static int medir(const std::wstring& t) { return (int)t.size() * ANCHO - 1; }
    static void centrado(Lienzo& l, int y, const std::wstring& t, Color c, bool sombra = true) {
        int x = (l.getAncho() - medir(t)) / 2;
        if (sombra) textoConSombra(l, x, y, t, c); else texto(l, x, y, t, c);
    }
    static std::vector<std::wstring> partir(const std::wstring& t, int columnas) {
        std::vector<std::wstring> lineas;
        std::wstring actual, palabra;
        auto volcar = [&]() {
            if (palabra.empty()) return;
            if (!actual.empty() && (int)(actual.size() + 1 + palabra.size()) > columnas) { lineas.push_back(actual); actual.clear(); }
            if (!actual.empty()) actual += L' ';
            actual += palabra;
            palabra.clear();
        };
        for (wchar_t c : t) {
            if (c == L'\n') { volcar(); lineas.push_back(actual); actual.clear(); }
            else if (c == L' ') volcar();
            else palabra += c;
        }
        volcar();
        if (!actual.empty()) lineas.push_back(actual);
        return lineas;
    }
};
