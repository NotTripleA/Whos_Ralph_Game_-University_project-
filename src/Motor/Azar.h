#pragma once
#include <random>
#include <ctime>
class Azar {
private:
    static std::mt19937& motor() {
        static std::mt19937 m((unsigned)time(nullptr));
        return m;
    }
public:
    static int entero(int a, int b) { return std::uniform_int_distribution<int>(a, b)(motor()); }
    static double real(double a, double b) { return std::uniform_real_distribution<double>(a, b)(motor()); }
    static bool probabilidad(double p) { return real(0, 1) < p; }
};
