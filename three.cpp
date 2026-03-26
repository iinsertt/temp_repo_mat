#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <fstream>

using namespace std;

// Функция для сохранения данных в файл для последующего построения графика
void saveToFile(const vector<double>& x, const vector<double>& u, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return;
    }
    
    file << "# x\tu(x,t)" << endl;
    for (size_t i = 0; i < x.size(); ++i) {
        file << x[i] << "\t" << u[i] << endl;
    }
    file.close();
    cout << "Данные сохранены в файл: " << filename << endl;
}

int main() {
    // Параметры задачи
    const double L = 10.0;
    const double T = 2.0;
    const int Nx = 200;
    const int Nt = 2000;
    const double a = 1.0;
    
    // Вычисление шагов
    const double h = 2.0 * L / Nx;
    const double tau = T / Nt;
    const double sigma = a * tau / (h * h);
    
    // Проверка устойчивости
    cout << fixed << setprecision(3);
    if (sigma > 0.5) {
        cout << "sigma = " << sigma << " > 0.5, схема неустойчива" << endl;
    } else {
        cout << "sigma = " << sigma << " <= 0.5, схема устойчива" << endl;
    }
    
    // Создание сетки по x
    vector<double> x(Nx + 1);
    for (int i = 0; i <= Nx; ++i) {
        x[i] = -L + i * h;
    }
    
    // Начальное условие: u(x,0) = exp(-x^2)
    vector<double> u(Nx + 1);
    for (int i = 0; i <= Nx; ++i) {
        u[i] = exp(-x[i] * x[i]);
    }
    
    // Вектор для нового временного слоя
    vector<double> u_new = u;
    
    // Основной цикл по времени
    for (int j = 0; j < Nt; ++j) {
        // Явная схема: u_new[i] = u[i] + sigma * (u[i-1] - 2*u[i] + u[i+1])
        for (int i = 1; i < Nx; ++i) {
            u_new[i] = u[i] + sigma * (u[i-1] - 2.0 * u[i] + u[i+1]);
        }
        
        // Граничные условия (Неймана или Дирихле)
        // Для простоты используем нулевые граничные условия
        u_new[0] = u_new[1];     // dudx = 0 на левой границе
        u_new[Nx] = u_new[Nx-1]; // dudx = 0 на правой границе
        
        // Обновляем решение
        u = u_new;
        
        // Вывод прогресса (опционально)
        if ((j + 1) % (Nt / 10) == 0 || j == 0) {
            cout << "Выполнено " << (j + 1) * 100 / Nt << "% (t = " 
                 << (j + 1) * tau << ")" << endl;
        }
    }
    
    // Вывод результатов в консоль (первые и последние несколько точек)
    cout << "\nРезультаты при t = " << T << ":" << endl;
    cout << "x\t\tu(x,t)" << endl;
    cout << "-----------------------------" << endl;
    for (int i = 0; i <= Nx; i += Nx / 10) {
        cout << fixed << setprecision(4) << x[i] << "\t\t" << u[i] << endl;
    }
    
    // Сохранение данных в файл для построения графика
    saveToFile(x, u, "heat_equation_results.txt");
    
    // Дополнительный вывод статистики
    double u_max = u[0];
    double u_min = u[0];
    double u_sum = 0.0;
    for (int i = 0; i <= Nx; ++i) {
        if (u[i] > u_max) u_max = u[i];
        if (u[i] < u_min) u_min = u[i];
        u_sum += u[i];
    }
    double u_mean = u_sum / (Nx + 1);
    
    cout << "\nСтатистика решения:" << endl;
    cout << "  Максимальное значение: " << fixed << setprecision(6) << u_max << endl;
    cout << "  Минимальное значение: " << u_min << endl;
    cout << "  Среднее значение: " << u_mean << endl;
    
    cout << "\nДля построения графика выполните в Python:" << endl;
    cout << "----------------------------------------" << endl;
    cout << "import numpy as np" << endl;
    cout << "import matplotlib.pyplot as plt" << endl;
    cout << endl;
    cout << "data = np.loadtxt('heat_equation_results.txt', skiprows=1)" << endl;
    cout << "x = data[:, 0]" << endl;
    cout << "u = data[:, 1]" << endl;
    cout << endl;
    cout << "plt.plot(x, u, label=f't = " << T << "')" << endl;
    cout << "plt.xlabel('x')" << endl;
    cout << "plt.ylabel('u(x,t)')" << endl;
    cout << "plt.title('Явная схема для уравнения теплопроводности')" << endl;
    cout << "plt.grid()" << endl;
    cout << "plt.legend()" << endl;
    cout << "plt.show()" << endl;
    
    return 0;
}
