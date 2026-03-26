#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <functional>
#include <fstream>

using namespace std;

const double EPS = 1e-8;
const int MAX_ITER = 100;

// Класс для решения краевой задачи методом стрельбы
class ShootingMethod {
private:
    double T;           // правый конец интервала
    double a, b;        // краевые условия: y(0)=a, y(T)=b
    int n;              // количество шагов для метода Рунге-Кутты
    bool shootLeft;     // true - стреляем влево, false - стреляем вправо
    
    // Функция, задающая дифференциальное уравнение: y'' = f(t, y, y')
    function<double(double, double, double)> f;
    
public:
    ShootingMethod(double T_, double a_, double b_, int n_, bool shootLeft_,
                   function<double(double, double, double)> f_)
        : T(T_), a(a_), b(b_), n(n_), shootLeft(shootLeft_), f(f_) {}
    
    // Метод Рунге-Кутты 4-го порядка для решения системы ОДУ
    // Возвращает вектор пар (t, y)
    vector<pair<double, double>> rungeKutta(double alpha) {
        double h = T / n;
        vector<double> t(n + 1);
        vector<double> y(n + 1);
        vector<double> z(n + 1); // y' = z
        
        // Начальные условия
        t[0] = shootLeft ? T : 0;
        
        if (shootLeft) {
            // Стреляем влево: y(T)=b, y'(T)=alpha
            y[0] = b;
            z[0] = alpha;
        } else {
            // Стреляем вправо: y(0)=a, y'(0)=alpha
            y[0] = a;
            z[0] = alpha;
        }
        
        // Метод Рунге-Кутты 4-го порядка
        for (int i = 0; i < n; ++i) {
            double k1y = h * z[i];
            double k1z = h * f(t[i], y[i], z[i]);
            
            double k2y = h * (z[i] + k1z / 2);
            double k2z = h * f(t[i] + h/2, y[i] + k1y/2, z[i] + k1z/2);
            
            double k3y = h * (z[i] + k2z / 2);
            double k3z = h * f(t[i] + h/2, y[i] + k2y/2, z[i] + k2z/2);
            
            double k4y = h * (z[i] + k3z);
            double k4z = h * f(t[i] + h, y[i] + k3y, z[i] + k3z);
            
            y[i+1] = y[i] + (k1y + 2*k2y + 2*k3y + k4y) / 6;
            z[i+1] = z[i] + (k1z + 2*k2z + 2*k3z + k4z) / 6;
            t[i+1] = t[i] + h;
        }
        
        // Формируем результат
        vector<pair<double, double>> result(n + 1);
        for (int i = 0; i <= n; ++i) {
            result[i] = {t[i], y[i]};
        }
        return result;
    }
    
    // Функция невязки: F(alpha) = y(T) - b (или y(0) - a для стрельбы влево)
    double residual(double alpha) {
        auto solution = rungeKutta(alpha);
        if (shootLeft) {
            return solution[n].second - a;  // y(0) - a
        } else {
            return solution[n].second - b;  // y(T) - b
        }
    }
    
    // Метод секущих для решения нелинейного уравнения
    double secantMethod(double alpha0, double alpha1) {
        double f0 = residual(alpha0);
        double f1 = residual(alpha1);
        
        for (int iter = 0; iter < MAX_ITER; ++iter) {
            if (abs(f1 - f0) < EPS) break;
            
            double alpha2 = alpha1 - f1 * (alpha1 - alpha0) / (f1 - f0);
            double f2 = residual(alpha2);
            
            if (abs(f2) < EPS) {
                return alpha2;
            }
            
            alpha0 = alpha1;
            f0 = f1;
            alpha1 = alpha2;
            f1 = f2;
        }
        
        return alpha1;
    }
    
    // Основной метод решения краевой задачи
    vector<pair<double, double>> solve(double alpha0, double alpha1) {
        double alpha_opt = secantMethod(alpha0, alpha1);
        cout << "Оптимальный параметр alpha = " << alpha_opt << endl;
        return rungeKutta(alpha_opt);
    }
    
    // Вывод таблицы значений
    void printTable(const vector<pair<double, double>>& solution) {
        cout << fixed << setprecision(6);
        cout << "\n" << string(50, '=') << endl;
        cout << "t\t\ty(t)" << endl;
        cout << string(50, '=') << endl;
        
        for (const auto& point : solution) {
            cout << point.first << "\t\t" << point.second << endl;
        }
        cout << string(50, '=') << endl;
    }
    
    // Сохранение в файл для построения графика
    void saveToFile(const vector<pair<double, double>>& solution, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Ошибка: не удалось открыть файл " << filename << endl;
            return;
        }
        
        file << "# t\ty(t)" << endl;
        for (const auto& point : solution) {
            file << point.first << "\t" << point.second << endl;
        }
        file.close();
        cout << "\nДанные сохранены в файл: " << filename << endl;
    }
};

// Варианты задач
void variant1() {
    cout << "\nВариант 1: x^2*y'' - y' - x*y = 3*x^2, [1,2], y(1)=2, y(2)=9" << endl;
    cout << "Приводим к виду: y'' = (y' + x*y + 3*x^2) / x^2" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return (yp + x*y + 3*x*x) / (x*x);
    };
    
    ShootingMethod solver(2.0, 2.0, 9.0, 100, false, f);
    auto solution = solver.solve(0.0, 5.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant1.txt");
}

void variant2() {
    cout << "\nВариант 2: -y'' + x^2*y = 9/(4+x^2)*cos(3x), [0,1], y(0)=1, y(1)=0" << endl;
    cout << "Приводим к виду: y'' = x^2*y - 9*cos(3x)/(4+x^2)" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return x*x*y - 9*cos(3*x)/(4 + x*x);
    };
    
    ShootingMethod solver(1.0, 1.0, 0.0, 100, false, f);
    auto solution = solver.solve(0.0, -1.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant2.txt");
}

void variant3() {
    cout << "\nВариант 3: x^2*y'' + x*y' - y = x^2, [1,3], y(1)=4/3, y(3)=3" << endl;
    cout << "Приводим к виду: y'' = (-x*y' + y + x^2) / x^2" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return (-x*yp + y + x*x) / (x*x);
    };
    
    ShootingMethod solver(3.0, 4.0/3.0, 3.0, 100, false, f);
    auto solution = solver.solve(0.0, 1.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant3.txt");
}

void variant4() {
    cout << "\nВариант 4: y'' - y' + y/exp(t) = 2*exp(t) + t + 1, [0,1], y(0)=0, y(1)=e" << endl;
    cout << "Приводим к виду: y'' = y' - y/exp(t) + 2*exp(t) + t + 1" << endl;
    
    auto f = [](double t, double y, double yp) -> double {
        return yp - y/exp(t) + 2*exp(t) + t + 1;
    };
    
    ShootingMethod solver(1.0, 0.0, exp(1.0), 100, false, f);
    auto solution = solver.solve(0.0, 2.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant4.txt");
}

void variant5() {
    cout << "\nВариант 5: y'' + y' + y/sin(x) = exp(x)*(3*cos(x)+sin(x)+1), [0.5,1.5], y(0.5)=0.8, y(1.5)=4.5" << endl;
    cout << "Приводим к виду: y'' = -y' - y/sin(x) + exp(x)*(3*cos(x)+sin(x)+1)" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return -yp - y/sin(x) + exp(x)*(3*cos(x) + sin(x) + 1);
    };
    
    ShootingMethod solver(1.5, 0.8, 4.5, 100, false, f);
    auto solution = solver.solve(0.0, 5.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant5.txt");
}

void variant6() {
    cout << "\nВариант 6: y'' + exp(x)*y' + y = 2*exp(x) + exp(2*x), [0,1], y(0)=1, y(1)=e" << endl;
    cout << "Приводим к виду: y'' = -exp(x)*y' - y + 2*exp(x) + exp(2*x)" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return -exp(x)*yp - y + 2*exp(x) + exp(2*x);
    };
    
    ShootingMethod solver(1.0, 1.0, exp(1.0), 100, false, f);
    auto solution = solver.solve(0.0, 2.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant6.txt");
}

void variant7() {
    cout << "\nВариант 7: -y'' + x^2*y = (16+x^2)*cos(4x), [0,1], y(0)=0, y(1)=0" << endl;
    cout << "Приводим к виду: y'' = x^2*y - (16+x^2)*cos(4x)" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return x*x*y - (16 + x*x)*cos(4*x);
    };
    
    ShootingMethod solver(1.0, 0.0, 0.0, 100, false, f);
    auto solution = solver.solve(0.0, 1.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant7.txt");
}

void variant8() {
    cout << "\nВариант 8: y'' + y' + y/x = 2*x*exp(x) + 4*exp(x), [1,2], y(1)=e, y(2)=2*e^2" << endl;
    cout << "Приводим к виду: y'' = -y' - y/x + 2*x*exp(x) + 4*exp(x)" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return -yp - y/x + 2*x*exp(x) + 4*exp(x);
    };
    
    ShootingMethod solver(2.0, exp(1.0), 2.0*exp(2.0), 100, false, f);
    auto solution = solver.solve(0.0, 10.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant8.txt");
}

void variant9() {
    cout << "\nВариант 9: y'' + y'*ln(x) - x*y + 2 = 0, [1,3], y(1)=1, y(3)=5" << endl;
    cout << "Приводим к виду: y'' = -y'*ln(x) + x*y - 2" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return -yp*log(x) + x*y - 2;
    };
    
    ShootingMethod solver(3.0, 1.0, 5.0, 100, false, f);
    auto solution = solver.solve(0.0, 2.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant9.txt");
}

void variant10() {
    cout << "\nВариант 10: y'' + y'/cos(x) + y/sin(x) - 2 = 2*x/cos(x) + x^2/sin(x), [0.2,1.2], y(0.2)=1, y(1.2)=2" << endl;
    cout << "Приводим к виду: y'' = -y'/cos(x) - y/sin(x) + 2 + 2*x/cos(x) + x^2/sin(x)" << endl;
    
    auto f = [](double x, double y, double yp) -> double {
        return -yp/cos(x) - y/sin(x) + 2 + 2*x/cos(x) + x*x/sin(x);
    };
    
    ShootingMethod solver(1.2, 1.0, 2.0, 100, false, f);
    auto solution = solver.solve(0.0, 1.0);
    solver.printTable(solution);
    solver.saveToFile(solution, "variant10.txt");
}

int main() {
    cout << "Лабораторная работа №5. Метод стрельбы для решения краевых задач" << endl;
    cout << string(60, '=') << endl;
    
    int choice;
    cout << "Выберите вариант (1-10): ";
    cin >> choice;
    
    switch(choice) {
        case 1: variant1(); break;
        case 2: variant2(); break;
        case 3: variant3(); break;
        case 4: variant4(); break;
        case 5: variant5(); break;
        case 6: variant6(); break;
        case 7: variant7(); break;
        case 8: variant8(); break;
        case 9: variant9(); break;
        case 10: variant10(); break;
        default: 
            cout << "Неверный выбор. Запускаем вариант 1 по умолчанию." << endl;
            variant1(); break;
    }
    
    cout << "\nДля построения графика используйте Python:" << endl;
    cout << "----------------------------------------" << endl;
    cout << "import numpy as np" << endl;
    cout << "import matplotlib.pyplot as plt" << endl;
    cout << endl;
    cout << "data = np.loadtxt('variant" << choice << ".txt', skiprows=1)" << endl;
    cout << "t, y = data[:, 0], data[:, 1]" << endl;
    cout << endl;
    cout << "plt.plot(t, y, 'b-', linewidth=2)" << endl;
    cout << "plt.xlabel('t')" << endl;
    cout << "plt.ylabel('y(t)')" << endl;
    cout << "plt.title('Решение краевой задачи (вариант " << choice << ")')" << endl;
    cout << "plt.grid(True)" << endl;
    cout << "plt.show()" << endl;
    
    return 0;
}
