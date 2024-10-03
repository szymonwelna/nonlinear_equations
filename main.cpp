#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <dlfcn.h>
#include <dirent.h>
#include <cstring>
#include "interval.h"

using namespace std;
using namespace interval_arithmetic;

// Funkcja wyświetlająca odpowiedni komunikat w oparciu o status zakończenia obliczeń
void return_status(int &status) {
    if (status == 0) {
        std::cout << "Sukces - Zbieżność osiągnięta." << endl;
    } else if (status == 1) {
        std::cout << "Błąd inicjalizacji lub podział przez zero." << endl;
    } else if (status == 2) {
        std::cout << "Brak zmiany znaku na końcach przedziału." << endl;
    } else if (status == 3) {
        std::cout << "BŁĄD FUNKCJA POWINNA WCIĄŻ SIĘ WYKONYWAĆ" << endl;
    } else if (status == 4) {
        std::cout << "Osiągnięto maksymalną liczbę iteracji bez zbieżności." << endl;
    } else if (status == 5) {
        std::cout << "Punkty początkowe są zbyt blisko siebie by wykorzystać metodę siecznych." << endl;
    } else {
        std::cout << "Błąd statusu." << endl;
    }
    return;
}

// Funkcja służąca wypisaniu przedziału
string interval_to_string(Interval<double> value) {
    string result = "", l = "", r = "";

    value = value.Projection();
    value.IEndsToStrings(l, r);
    result = "[" + l + "; " + r + "]";
    return result;
}

// Funkcja pomocnicza do konwersji double na Interval<double>
Interval<double> make_interval(double value) {
    return Interval<double>(value, value);
}

// Funkcja pomocnicza do konwersji double na najbliższą jej dokładną reprezentację maszynową w postaci przedziału
Interval<double> make_machine_interval(double value) {
    // Użycie std::nextafter do uzyskania najbliższych wartości maszynowych
    double lower_bound = std::nextafter(value, -std::numeric_limits<double>::infinity());
    double upper_bound = std::nextafter(value, std::numeric_limits<double>::infinity());

    // Zwrócenie przedziału utworzonego na podstawie tych wartości
    return Interval<double>(lower_bound, upper_bound);
}


// Funkcja do wyświetlania wszystkich plików .so w bieżącym folderze
std::vector<std::string> show_so_files(const std::string& directory) {
    std::vector<std::string> so_files;
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string file_name = ent->d_name;
            if (file_name.length() > 3 && file_name.substr(file_name.length() - 3) == ".so") {
                std::cout << so_files.size() + 1 << ": " << file_name << std::endl;
                so_files.push_back(directory + "/" + file_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Nie udało się otworzyć katalogu." << std::endl;
    }

    return so_files;
}

// Funkcja do załadowania funkcji matematycznej z wybranego pliku .so
std::function<double(double)> load_math_function(const std::string& so_file) {
    // Załaduj bibliotekę .so
    void* handle = dlopen(so_file.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Nie udało się załadować biblioteki: " << dlerror() << std::endl;
        return nullptr;
    }

    // Wyzeruj błędy
    dlerror();

    // Pobierz wskaźnik do funkcji
    typedef double (*math_func_t)(double);
    math_func_t math_function = (math_func_t) dlsym(handle, "math_function");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Nie udało się załadować funkcji: " << dlsym_error << std::endl;
        dlclose(handle);
        return nullptr;
    }

    // Zwróć std::function, który przechowuje wskaźnik do funkcji
    return std::function<double(double)>([=](double x) {
        return math_function(x);
    });
}

// Funkcja, która wyświetla pliki .so, pozwala użytkownikowi wybrać jeden i zwraca funkcję matematyczną
std::function<double(double)> select_and_load_function(const std::string& directory) {
    std::vector<std::string> so_files = show_so_files(directory);

    if (so_files.empty()) {
        std::cout << "Brak dostępnych plików .so w katalogu." << std::endl;
        return nullptr;
    }

    std::cout << "Wybierz numer pliku, który chcesz załadować: ";
    int choice;
    std::cin >> choice;

    if (choice < 1 || choice > so_files.size()) {
        std::cout << "Nieprawidłowy wybór." << std::endl;
        return nullptr;
    }

    std::string selected_file = so_files[choice - 1];
    return load_math_function(selected_file);
}



// Funkcja implementująca metodę połowienia
double bisection_double(std::function<double(double)> f, double& x0, double& x1, int max_iter, double eps, double& wynik_przyb, int& iteracje, int& status) {
    if (max_iter < 1 || x0 >= x1) {
        status = 1; // Błąd inicjalizacji
        return 0;
    }

    double f_x0 = f(x0);
    double f_x1 = f(x1);

    if (f_x0 * f_x1 >= 0) {
        status = 2; // Brak zmiany znaku
        return 0;
    }

    status = 3; // Proces iteracyjny trwa
    iteracje = 0;
    double mid, f_mid, w1, w2;

    do {
        iteracje++;
        mid = (x0 + x1) / 2;
        w1 = fabs(x1);
        w2 = fabs(x0);
        if (w1 < w2) w1 = w2;
        if (w1 == 0) {
            status = 0; // Sukces
        } else if ((x1 - x0) / w1 < eps) {
            status = 0; // Sukces
        } else {
            f_mid = f(mid);
            if (f_mid == 0) {
                status = 0; // Sukces
            } else {
                if (f_x0 * f_mid < 0) {
                    x1 = mid;
                } else {
                    x0 = mid;
                }
                f_x0 = f(x0);
                f_x1 = f(x1);
            }
        }
    } while (iteracje < max_iter && status != 0);

    if (status == 0) {
        wynik_przyb = f_mid;
        return mid;
    } else if (status == 3) {
        status = 4;
        wynik_przyb = f_mid;
        return mid;
    }

    status = 4; // Brak zbieżności
    return 0;
}

// Funkcja implementująca metodę połowienia dla arytmetyki przedziałowej
Interval<double> bisection_interval(std::function<double(double)> f, Interval<double>& x0, Interval<double>& x1, int max_iter, Interval<double> eps, Interval<double>& wynik_przyb, int& iteracje, int& status) {
    // Sprawdzenie początkowych warunków
    if (f(x0.a) * f(x1.b) > 0) {
        status = 2; // Brak zmiany znaku na końcach przedziału
        return make_interval(0.0);
    }

    iteracje = 0;
    status = 3; // Proces iteracyjny trwa

    Interval<double> mid, f_mid;

    while (iteracje < max_iter) {
        iteracje++;

        // Obliczanie punktu środkowego
        mid = (x0 + x1) / make_interval(2.0);

        // Obliczenie wartości funkcji w punkcie środkowym
        double f_mid_a = f(mid.a);
        double f_mid_b = f(mid.b);
        f_mid.a = std::min(f_mid_a, f_mid_b);
        f_mid.b = std::max(f_mid_a, f_mid_b);

        // Sprawdzenie zbieżności
        if (IAbs(f_mid).a < eps.a || IAbs(x1 - x0).a < eps.a) {
            status = 0; // Sukces
            wynik_przyb = mid;
            return mid;
        }

        // Decyzja, który przedział wybrać
        if (f(x0.a) * f_mid_a < 0) {
            x1 = mid;
        } else {
            x0 = mid;
        }
    }

    // Jeśli osiągnęliśmy maksymalną liczbę iteracji bez zbieżności
    status = 4; // Brak zbieżności
    wynik_przyb = mid;
    return mid;
}

// Funkcja implementująca metodę regula-falsi
double regulafalsi_double(std::function<double(double)> f, double& x0, double& x1, int max_iter, double eps, double& wynik_przyb, int& iteracje, int& status) {
    double fx0 = f(x0);
    double fx1 = f(x1);

    if (x0 >= x1) {
        status = 1; // Błąd inicjalizacji
        return 0;
    }

    if (fx0 * fx1 > 0) {
        status = 2; // Brak zmiany znaku
        return 0;
    }

    status = 3; // Proces iteracyjny trwa
    iteracje = 0;
    double x, fx;

    while (iteracje < max_iter) {
        iteracje++;

        if (fabs(fx1 - fx0) < eps) {
            status = 3; // Podział przez zero
            return 0;
        }

        x = x1 - fx1 * (x1 - x0) / (fx1 - fx0);
        fx = f(x);

        if (fabs(fx) < eps || fabs(x1 - x0) < eps) {
            status = 0; // Sukces
            wynik_przyb = fx;
            return x;
        }

        if (fx0 * fx < 0) {
            x1 = x;
            fx1 = fx;
        } else {
            x0 = x;
            fx0 = fx;
        }
    }

    status = 4; // Brak zbieżności
    wynik_przyb = fx;
    return x;
}

// Funkcja implementująca metodę regula-falsi dla przedziałów
Interval<double> regulafalsi_interval(std::function<double(double)> f, Interval<double>& x0, Interval<double>& x1, int max_iter, Interval<double> eps, Interval<double>& wynik_przyb, int& iteracje, int& status) {
    // Obliczanie wartości funkcji dla granic przedziałów
    double fx0_a = f(x0.a);
    double fx0_b = f(x0.b);
    double fx1_a = f(x1.a);
    double fx1_b = f(x1.b);

    // Tworzenie przedziałów na podstawie wyników funkcji
    Interval<double> fx0;
    fx0.a = std::min(fx0_a, fx0_b);
    fx0.b = std::max(fx0_a, fx0_b);
    Interval<double> fx1;
    fx1.a = std::min(fx1_a, fx1_b);
    fx1.b = std::max(fx1_a, fx1_b);

    if (x0.b >= x1.a) {
        status = 1; // Błąd inicjalizacji
        return make_interval(0.0);
    }

    if ((fx0 * fx1).a > 0) {
        status = 2; // Brak zmiany znaku
        return make_interval(0.0);
    }

    status = 3; // Proces iteracyjny trwa
    iteracje = 0;
    Interval<double> x, fx;

    while (iteracje < max_iter) {
        iteracje++;

        if (IAbs(fx1 - fx0).a < eps.a) {
            status = 3; // Podział przez zero
            return make_interval(0.0);
        }

        // Wyznaczanie nowej wartości x zgodnie z metodą regula falsi
        x = x1 - fx1 * (x1 - x0) / (fx1 - fx0);

        // Obliczanie funkcji dla nowego przedziału
        double fx_a = f(x.a);
        double fx_b = f(x.b);
        fx.a = std::min(fx_a, fx_b);
        fx.b = std::max(fx_a, fx_b);

        // Sprawdzenie warunków zbieżności
        if (IAbs(fx).a < eps.a || IAbs(x1 - x0).a < eps.a) {
            status = 0; // Sukces
            wynik_przyb = fx;
            return x;
        }

        // Aktualizacja przedziałów x0 i x1 na podstawie wyników
        if ((fx0 * fx).a < 0) {
            x1 = x;
            fx1 = fx;
        } else {
            x0 = x;
            fx0 = fx;
        }
    }

    status = 4; // Brak zbieżności
    wynik_przyb = fx;
    return x;
}

// Funkcja implementująca metodę siecznych
double secant_double(std::function<double(double)> f, double x0, double x1, int max_iter, double eps, double& wynik_przyb, int& iteracje, int& status) {
    // Sprawdzenie początkowych warunków
    if (fabs(x1 - x0) < eps) {
        status = 5; // Punkty zbyt blisko siebie
        return 0;
    }

    double f_x0 = f(x0); // Wartość funkcji w x0
    double f_x1 = f(x1); // Wartość funkcji w x1

    iteracje = 0;
    status = 3; // Proces iteracyjny trwa

    double x2, f_x2;

    while (iteracje < max_iter) {
        iteracje++;

        // Obliczanie kolejnego przybliżenia x2 za pomocą metody siecznych
        if (fabs(f_x1 - f_x0) < eps) {
            status = 1; // Podział przez zero
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
    status = 4; // Brak zbieżności
    wynik_przyb = f_x2;
    return x2;
}

// Funkcja implementująca metodę siecznych dla arytmetyki przedziałowej
Interval<double> secant_interval(std::function<double(double)> f, Interval<double>& x0, Interval<double>& x1, int max_iter, Interval<double> eps, Interval<double>& wynik_przyb, int& iteracje, int& status) {
    // Sprawdzenie początkowych warunków
    if (IAbs(x1 - x0).a < eps.a) {
        status = 5; // Punkty zbyt blisko siebie
        return make_interval(0.0);
    }

    // Obliczenie wartości funkcji dla granic przedziałów x0 i x1
    double fx0_a = f(x0.a);
    double fx0_b = f(x0.b);
    double fx1_a = f(x1.a);
    double fx1_b = f(x1.b);

    // Utworzenie przedziałów na podstawie wyników funkcji
    Interval<double> fx0;
    fx0.a = std::min(fx0_a, fx0_b);
    fx0.b = std::max(fx0_a, fx0_b);
    Interval<double> fx1;
    fx1.a = std::min(fx1_a, fx1_b);
    fx1.b = std::max(fx1_a, fx1_b);

    iteracje = 0;
    status = 3; // Proces iteracyjny trwa

    Interval<double> x2, fx2;

    while (iteracje < max_iter) {
        iteracje++;

        // Sprawdzenie, czy różnica wartości funkcji jest wystarczająco duża, aby uniknąć podziału przez zero
        if (IAbs(fx1 - fx0).a < eps.a) {
            status = 1; // Podział przez zero
            return make_interval(0.0);
        }

        // Obliczanie kolejnego przybliżenia x2 za pomocą metody siecznych
        x2 = x1 - fx1 * (x1 - x0) / (fx1 - fx0);

        // Obliczenie wartości funkcji dla nowego przedziału x2
        double fx2_a = f(x2.a);
        double fx2_b = f(x2.b);
        fx2.a = std::min(fx2_a, fx2_b);
        fx2.b = std::max(fx2_a, fx2_b);

        // Sprawdzenie, czy osiągnięto zbieżność
        if (IAbs(fx2).a < eps.a || IAbs(x2 - x1).a < eps.a) {
            status = 0; // Sukces
            wynik_przyb = fx2;
            return x2;
        }

        // Aktualizacja wartości dla następnej iteracji
        x0 = x1;
        fx0 = fx1;
        x1 = x2;
        fx1 = fx2;
    }

    // Jeśli pętla zakończyła się, ponieważ osiągnęliśmy maksymalną liczbę iteracji bez zbieżności
    status = 4; // Brak zbieżności
    wynik_przyb = fx2;
    return x2;
}


void zmiennoprzecinkowa(int &met, std::function<double(double)>& f) {
    double eps = 1, wynik, x0, x1;
    int max_iter = 0;
    int iteracje = 0, status = 0;
    double wynik_przyb = 0;

    cout << "Podaj dolną granicę przedziału" << endl;
    cin >> x0;
    cout << endl;
    cout << "Podaj górną granicę przedziału" << endl;
    do {
        cin >> x1;
        cout << endl;
    } while (x1 <= x0);

    cout << "Podaj oczekiwaną dokładność obliczeń " << endl;
    do {
    cin >> eps;
    cout << endl;
    } while (eps < 0);

    cout << "Podaj maksymalną liczbę iteracji" << endl;
    do {
        cin >> max_iter;
        cout << endl;
    } while (max_iter <= 0);

    if (met == 1) {
        wynik = bisection_double(f, x0, x1, max_iter, eps, wynik_przyb, iteracje, status);
    } else if (met == 2) {
        wynik = regulafalsi_double(f, x0, x1, max_iter, eps, wynik_przyb, iteracje, status);
    } else {
        wynik = secant_double(f, x0, x1, max_iter, eps, wynik_przyb, iteracje, status);
    }

    // Wypisanie wyniku wywołania funkcji
    return_status(status);
    if (status == 0 || status == 4) {
        cout << "Przybliżony pierwiastek: " << wynik << endl;
        //std::cout << "Szerokość: " << wynik.GetWidth() << std::endl;
        cout << "Wartość funkcji dla pierwiastka: " << wynik_przyb << endl;
        cout << "Ilość iteracji: " << iteracje << endl;
    } else {
        cout << "Program zakończony błędem." << endl;
    }
    return;
}

void zw_precyzja(int &met, std::function<double(double)>& f) {
    double eps = 1.0, x0, x1;
    int max_iter = 0;
    int iteracje = 0, status = 0;
    

    cout << "Podaj dolną granicę przedziału" << endl;
    cin >> x0;
    cout << endl;
    cout << "Podaj górną granicę przedziału" << endl;
    do {
        cin >> x1;
        cout << endl;
    } while (x1 <= x0);

    // Deklaracja granic przedziału jako zmienne typu interval double
    Interval<double> x0_in = make_interval(x0);
    Interval<double> x1_in = make_interval(x1);

    cout << "Podaj oczekiwaną dokładność obliczeń " << endl;
    do {
    cin >> eps;
    cout << endl;
    } while (eps < 0);
    // Deklaracja dokładności maszynowej jako interval double
    Interval<double> eps_in = make_interval(eps);

    cout << "Podaj maksymalną liczbę iteracji" << endl;
    do {
        cin >> max_iter;
        cout << endl;
    } while (max_iter <= 0);


    Interval<double> wynik, wynik_przyb;
    if (met == 1) {
        wynik = bisection_interval(f, x0_in, x1_in, max_iter, eps_in, wynik_przyb, iteracje, status);
    } else if (met == 2) {
        wynik = regulafalsi_interval(f, x0_in, x1_in, max_iter, eps_in, wynik_przyb, iteracje, status);
    } else {
        wynik = secant_interval(f, x0_in, x1_in, max_iter, eps_in, wynik_przyb, iteracje, status);
    }

    // Wypisanie wyniku wywołania funkcji
    return_status(status);
    if (status == 0 || status == 4) {
        cout << "Przybliżony pierwiastek: " << interval_to_string(wynik) << endl;
        std::cout << "Szerokość: " << wynik.GetWidth() << std::endl;
        cout << "Wartość funkcji dla pierwiastka: " << interval_to_string(wynik_przyb) << endl;
        cout << "Ilość iteracji: " << iteracje << endl;
    } else {
        cout << "Program zakończony błędem." << endl;
    }
    return;
}

void przedziałowa(int &met, std::function<double(double)>& f) {
    double eps = 1.0, x0, x1;
    int max_iter = 0;
    int iteracje = 0, status = 0;
    

    cout << "Podaj dolną granicę przedziału" << endl;
    cin >> x0;
    cout << endl;
    cout << "Podaj górną granicę przedziału" << endl;
    do {
        cin >> x1;
        cout << endl;
    } while (x1 <= x0);

    // Deklaracja granic przedziału jako zmienne typu interval double
    Interval<double> x0_in = make_machine_interval(x0);
    Interval<double> x1_in = make_machine_interval(x1);

    cout << "Podaj oczekiwaną dokładność obliczeń " << endl;
    do {
    cin >> eps;
    cout << endl;
    } while (eps < 0);
    // Deklaracja dokładności maszynowej jako interval double
    Interval<double> eps_in = make_machine_interval(eps);

    cout << "Podaj maksymalną liczbę iteracji" << endl;
    do {
        cin >> max_iter;
        cout << endl;
    } while (max_iter <= 0);


    Interval<double> wynik, wynik_przyb;
    if (met == 1) {
        wynik = bisection_interval(f, x0_in, x1_in, max_iter, eps_in, wynik_przyb, iteracje, status);
    } else if (met == 2) {
        wynik = regulafalsi_interval(f, x0_in, x1_in, max_iter, eps_in, wynik_przyb, iteracje, status);
    } else {
        wynik = secant_interval(f, x0_in, x1_in, max_iter, eps_in, wynik_przyb, iteracje, status);
    }

    // Wypisanie wyniku wywołania funkcji
    return_status(status);
    if (status == 0 || status == 4) {
        cout << "Przybliżony pierwiastek: " << interval_to_string(wynik) << endl;
        std::cout << "Szerokość: " << wynik.GetWidth() << std::endl;
        cout << "Wartość funkcji dla pierwiastka: " << interval_to_string(wynik_przyb) << endl;
        cout << "Ilość iteracji: " << iteracje << endl;
    } else {
        cout << "Program zakończony błędem." << endl;
    }
    return;
}

int main() {
    interval_arithmetic::IAPrecision prec = interval_arithmetic::DOUBLE_PREC;
    interval_arithmetic::IAMode mode = interval_arithmetic::DINT_MODE;

    
    int arytm = 0, met = 0;
    int status = 0;

    do { 
        cout << "Wybierz arytmetykę:\n1 -> arytmetyka zmiennoprzecinkowa\n2 -> arytmetyka o zwiększonej precyzji\n3 -> arytmetyka przedziałowa" << endl;
        cin >> arytm;
        cout << endl;
    } while (arytm != 1 && arytm != 2 && arytm != 3);

    do {
        cout << "Wybierz metodę:\n1 -> metoda połowienia\n2 -> regula-falsi\n3 -> metoda siecznych" << endl;
        cin >> met;
        cout << endl;
    } while (met != 1 && met != 2 && met != 3);

    std::string directory = "."; // Bieżący katalog
    std::function<double(double)> f = select_and_load_function(directory);



    if (arytm == 1) {
        zmiennoprzecinkowa(met, f);
    } else if (arytm == 2) {
        zw_precyzja(met, f);
    } else {
        przedziałowa(met, f);
    }
    return 0;
}
