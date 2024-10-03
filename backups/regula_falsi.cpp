#include <iostream>
#include <functional>
#include <cmath>

double regulafalsi(std::function<double(double)> f, double& x0, double& x1, int max_iter, double eps, double& wynik_przyb, int& status) {
    double fx0 = f(x0); // Wartość funkcji w x0
    double fx1 = f(x1); // Wartość funkcji w x1

    if (x0 >= x1) {
        status = 1; // Błąd: nieprawidłowy przedział
        return 0;
    }

    if (fx0 * fx1 > 0) {
        status = 2; // Błąd: brak zmiany znaku na końcach przedziału
        return 0;
    }

    status = 3; // Domyślny status: kolejne przybliżenie
    int iteracje = 0;
    double x, fx; // Przybliżone rozwiązanie i wartość funkcji w tym punkcie

    while (iteracje < max_iter) {
        x = x1 - fx1 * (x1 - x0) / (fx1 - fx0);
        fx = f(x);

        if (fabs(fx) < eps || fabs(x1 - x0) < eps) {
            status = 0; // Sukces
            wynik_przyb = fx;
            return x;
        }

        iteracje++;

        if (fx0 * fx < 0) {
            x1 = x;
            fx1 = fx;
        } else {
            x0 = x;
            fx0 = fx;
        }
    }

    // Jeśli osiągnięto maksymalną liczbę iteracji
    status = 4; // Osiągnięto maksymalną liczbę iteracji bez zbieżności
    wynik_przyb = fx;
    return x;
}

// Przykładowe użycie funkcji regulafalsi
int main() {
    // Przykładowa funkcja
    auto f = [](double x) -> double {
        return x * x - 2; // Funkcja f(x) = x^2 - 2
    };

    double x0 = 0.0, x1 = 2.0; // Przedział początkowy
    int max_iter = 100; // Maksymalna liczba iteracji
    double eps = 1e-6; // Dokładność
    double wynik_przyb; // Wartość funkcji w przybliżonym pierwiastku
    int status; // Status

    double root = regulafalsi(f, x0, x1, max_iter, eps, wynik_przyb, status);

    std::cout << "Approximate root: " << root << std::endl;
    std::cout << "Function value at root: " << wynik_przyb << std::endl;
    std::cout << "Status: " << status << std::endl;

    return 0;
}
