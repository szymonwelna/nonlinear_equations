#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

// Funkcja implementująca metodę siecznych
double secant_double(std::function<double(double)> f, double x0, double x1, int max_iter, double eps, double& wynik_przyb, int& iteracje, int& status) {
    // Sprawdzenie początkowych warunków
    if (fabs(x1 - x0) < eps) {
        status = 2; // Błąd: początkowe punkty są zbyt blisko siebie lub identyczne
        return 0;
    }

    double f_x0 = f(x0); // Wartość funkcji w x0
    double f_x1 = f(x1); // Wartość funkcji w x1

    iteracje = 0;
    status = 3; // Domyślnie zakładamy, że nie osiągnęliśmy jeszcze zbieżności

    double x2, f_x2;

    while (iteracje < max_iter) {
        iteracje++;

        // Obliczanie kolejnego przybliżenia x2 za pomocą metody siecznych
        if (fabs(f_x1 - f_x0) < eps) {
            status = 1; // Błąd: podział przez zero lub zbyt mała różnica wartości funkcji
            return 0;
        }
        x2 = x1 - f_x1 * (x1 - x0) / (f_x1 - f_x0);
        f_x2 = f(x2);

        // Sprawdzenie, czy osiągnięto zbieżność
        if (fabs(f_x2) < eps || fabs(x2 - x1) < eps) {
            status = 0; // Sukces
            wynik_przyb = f_x2;
            return x2;
        }

        // Aktualizacja wartości dla następnej iteracji
        x0 = x1;
        f_x0 = f_x1;
        x1 = x2;
        f_x1 = f_x2;
    }

    // Jeśli pętla zakończyła się, ponieważ osiągnęliśmy maksymalną liczbę iteracji bez zbieżności
    status = 4; // Osiągnięto maksymalną liczbę iteracji bez zbieżności
    wynik_przyb = f_x2;
    return x2;
}

// Przykładowe użycie zmodyfikowanej funkcji secantMethod
int main() {
    // Przykładowa funkcja
    auto f = [](double x) -> double {
        return x * x - 2; // Funkcja f(x) = x^2 - 2
    };

    double x0 = 1.0, x1 = 2.0; // Przedział początkowy
    int max_iter = 100; // Maksymalna liczba iteracji
    double eps = 1e-6; // Dokładność
    double wynik_przyb; // Wartość funkcji w przybliżonym pierwiastku
    int iteracje, status; // Liczba iteracji, status

    double root = secant_double(f, x0, x1, max_iter, eps, wynik_przyb, iteracje, status);

    std::cout << "Approximate root: " << root << std::endl;
    std::cout << "Function value at root: " << wynik_przyb << std::endl;
    std::cout << "Number of iterations: " << iteracje << std::endl;
    std::cout << "Status: " << status << std::endl;

    return 0;
}
