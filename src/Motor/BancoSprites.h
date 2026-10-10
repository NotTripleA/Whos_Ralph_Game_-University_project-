#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Sprite.h"
#include "Rutas.h"
class BancoSprites {
private:
    std::map<std::string, std::unique_ptr<Sprite>> cache;
    BancoSprites() {}
public:
    static BancoSprites& instancia() {
        static BancoSprites banco;
        return banco;
    }
    const Sprite* obtener(const std::string& id) {
        auto it = cache.find(id);
        if (it != cache.end()) return it->second.get();
        bool original = !id.empty() && id[0] == '!';
        std::string ruta = original ? id.substr(1) : id;
        std::wstring rel(ruta.begin(), ruta.end());
        for (auto& c : rel) if (c == L'/') c = L'\\';
        auto s = std::make_unique<Sprite>();
        bool ok = !original && s->cargar(Rutas::en(L"Recursos\\SpritesMejorados\\" + rel + L".txt"));
        if (!ok && !s->cargar(Rutas::en(L"Recursos\\Sprites\\" + rel + L".txt"))) {
            s->crearFaltante(ruta);
            std::ofstream log{ std::filesystem::path(Rutas::en(L"Recursos\\sprites_faltantes.txt")), std::ios::app };
            if (log) log << ruta << "\n";
        }
        const Sprite* p = s.get();
        cache[id] = std::move(s);
        return p;
    }
    std::vector<const Sprite*> secuencia(const std::string& prefijo, int desde, int hasta) {
        std::vector<const Sprite*> v;
        for (int i = desde; i <= hasta; i++) {
            char num[8];
            sprintf_s(num, "%02d", i);
            v.push_back(obtener(prefijo + num));
        }
        return v;
    }
    std::vector<const Sprite*> lista(const std::string& prefijo, const std::vector<int>& numeros) {
        std::vector<const Sprite*> v;
        for (int n : numeros) {
            char num[8];
            sprintf_s(num, "%02d", n);
            v.push_back(obtener(prefijo + num));
        }
        return v;
    }
};
inline const Sprite* SPR(const std::string& id) { return BancoSprites::instancia().obtener(id); }
