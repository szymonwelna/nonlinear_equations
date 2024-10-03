#include <iostream>
#include <vector>
#include <cmath>
#include <functional>

// Funkcja implementująca metodę połowienia
double bisection(std::function<double(double)> f, double& x0, double& x1, int max_iter, double eps, double& wynik_przyb, int& iteracje, int& status) {
    if (max_iter < 1 || x0 >= x1) {
        status = 1;
        return 0;
    }

    double znak_x0 = f(x0); // Zmienne służące do sprawdzenia, czy na granicach przedziału są dwa różne znaki
    double znak_x1 = f(x1);

    if (znak_x0 * znak_x1 >= 0) {
        status = 2;
        return 0;
    }

    status = 3;
    iteracje = 0;
    double mid, znak_mid, w1, w2; // mid to środek przedziału x0 x1, w1 i w2 służą do porównania, czy przedział jest wystarczająco mały w porównaniu do eps

    do {
        iteracje++;
        mid = (x0 + x1) / 2;
        w1 = fabs(x1);
        w2 = fabs(x0);
        if (w1 < w2) w1 = w2;
        if (w1 == 0) {
            status = 0;
        } else if ((x1 - x0) / w1 < eps) {
            status = 0;
        } else {
            znak_mid = f(mid);
            if (znak_mid == 0) {
                status = 0;
            } else {
                if (znak_x0 * znak_mid < 0) {
                    x1 = mid;
                } else {
                    x0 = mid;
                }
                znak_x0 = f(x0);
                znak_x1 = f(x1);
            }
        }
    } while (iteracje != max_iter && status != 0);

    if (status == 0 || status == 3) {
        wynik_przyb = znak_mid;
        return mid;
    }

    return 0;
}

// Przykładowe użycie zmodyfikowanej funkcji bisection
int main() {
    // Przykładowa funkcja
    auto f = [](double x) -> double {
        return x * x - 2; // Funkcja f(x) = x^2 - 2
    };

    double x0 = 0.0, x1 = 2.0; // Przedział początkowy
    int max_iter = 100; // Maksymalna liczba iteracji
    double eps = 1e-6; // Dokładność
    double wynik_przyb; // Wartość funkcji w przybliżonym pierwiastku
    int iteracje, status; // Liczba iteracji, status

    double root = bisection(f, x0, x1, max_iter, eps, wynik_przyb, iteracje, status);

    std::cout << "Approximate root: " << root << std::endl;
    std::cout << "Function value at root: " << wynik_przyb << std::endl;
    std::cout << "Number of iterations: " << iteracje << std::endl;
    std::cout << "Status: " << status << std::endl;

    return 0;
}
