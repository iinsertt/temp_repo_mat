#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <tuple>

using namespace std;

const double EPS = 1e-9;

// Функция для вывода матрицы
void printMatrix(const vector<vector<double>>& matrix, const string& title = "") {
    if (!title.empty()) {
        cout << title << endl;
    }
    for (const auto& row : matrix) {
        for (double val : row) {
            cout << fixed << setprecision(2) << setw(8) << val;
        }
        cout << endl;
    }
}

// Функция для вывода вектора
void printVector(const vector<double>& vec, const string& title = "") {
    if (!title.empty()) {
        cout << title;
    }
    for (double val : vec) {
        cout << fixed << setprecision(2) << setw(8) << val;
    }
    cout << endl;
}

// Поиск цикла для перераспределения
vector<pair<int, int>> findCycle(const vector<vector<double>>& x, int i0, int j0) {
    int m = x.size();
    int n = x[0].size();
    
    // Собираем заполненные клетки
    vector<pair<int, int>> filled;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (x[i][j] > EPS) {
                filled.emplace_back(i, j);
            }
        }
    }
    
    vector<pair<int, int>> path;
    path.emplace_back(i0, j0);
    
    // DFS для поиска цикла
    function<bool(int, int, char)> dfs = [&](int i, int j, char prevDirection) -> bool {
        if (path.size() > 1 && i == i0 && j == j0) {
            return true;
        }
        
        // Движение по горизонтали
        if (prevDirection != 'h') {
            for (int j2 = 0; j2 < n; ++j2) {
                if (j2 == j) continue;
                if (x[i][j2] > EPS) {
                    auto cell = make_pair(i, j2);
                    if (find(path.begin(), path.end(), cell) == path.end()) {
                        path.push_back(cell);
                        if (dfs(i, j2, 'h')) return true;
                        path.pop_back();
                    }
                }
            }
        }
        
        // Движение по вертикали
        if (prevDirection != 'v') {
            for (int i2 = 0; i2 < m; ++i2) {
                if (i2 == i) continue;
                if (x[i2][j] > EPS) {
                    auto cell = make_pair(i2, j);
                    if (find(path.begin(), path.end(), cell) == path.end()) {
                        path.push_back(cell);
                        if (dfs(i2, j, 'v')) return true;
                        path.pop_back();
                    }
                }
            }
        }
        
        return false;
    };
    
    dfs(i0, j0, '\0');
    return path;
}

int main() {
    cout << "Транспортная задача (вариант 2)" << endl;
    
    // Исходные данные
    vector<double> a = {180, 80, 40};  // запасы
    vector<double> b = {100, 100, 200}; // потребности
    vector<vector<double>> C = {        // матрица тарифов
        {1, 3, 2},
        {4, 6, 1},
        {2, 3, 5}
    };
    
    cout << "Запасы: ";
    printVector(a);
    cout << "Потребности: ";
    printVector(b);
    cout << "Матрица тарифов:" << endl;
    printMatrix(C);
    cout << endl;
    
    // Проверка на закрытость задачи
    double sum_a = 0, sum_b = 0;
    for (double val : a) sum_a += val;
    for (double val : b) sum_b += val;
    
    int m = a.size();
    int n = b.size();
    
    if (abs(sum_a - sum_b) > EPS) {
        cout << "Задача открытая, приводим к закрытой" << endl;
        if (sum_a > sum_b) {
            // Добавляем фиктивного потребителя
            b.push_back(sum_a - sum_b);
            for (int i = 0; i < m; ++i) {
                C[i].push_back(0);
            }
            n++;
        } else {
            // Добавляем фиктивного поставщика
            a.push_back(sum_b - sum_a);
            vector<double> newRow(n, 0);
            C.push_back(newRow);
            m++;
        }
    } else {
        cout << "Задача закрытая" << endl;
    }
    cout << endl;
    
    // Начальный опорный план методом минимального элемента
    vector<vector<double>> x(m, vector<double>(n, 0));
    vector<double> a_copy = a;
    vector<double> b_copy = b;
    
    while (true) {
        double min_cost = numeric_limits<double>::infinity();
        int min_i = -1, min_j = -1;
        
        // Поиск клетки с минимальным тарифом
        for (int i = 0; i < m; ++i) {
            if (a_copy[i] < EPS) continue;
            for (int j = 0; j < n; ++j) {
                if (b_copy[j] < EPS) continue;
                if (C[i][j] < min_cost - EPS) {
                    min_cost = C[i][j];
                    min_i = i;
                    min_j = j;
                }
            }
        }
        
        if (min_i == -1 || min_j == -1) break;
        
        double qty = min(a_copy[min_i], b_copy[min_j]);
        x[min_i][min_j] = qty;
        a_copy[min_i] -= qty;
        b_copy[min_j] -= qty;
    }
    
    cout << "Начальный опорный план (метод минимального элемента):" << endl;
    printMatrix(x);
    
    // Оптимизация методом потенциалов
    while (true) {
        // Вычисление потенциалов
        vector<double> u(m, numeric_limits<double>::quiet_NaN());
        vector<double> v(n, numeric_limits<double>::quiet_NaN());
        u[0] = 0;
        
        bool changed = true;
        while (changed) {
            changed = false;
            for (int i = 0; i < m; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (x[i][j] > EPS) {
                        if (!isnan(u[i]) && isnan(v[j])) {
                            v[j] = C[i][j] - u[i];
                            changed = true;
                        }
                        if (!isnan(v[j]) && isnan(u[i])) {
                            u[i] = C[i][j] - v[j];
                            changed = true;
                        }
                    }
                }
            }
        }
        
        // Проверка на оптимальность
        bool optimal = true;
        double min_delta = numeric_limits<double>::infinity();
        int enter_i = -1, enter_j = -1;
        
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (x[i][j] < EPS && !isnan(u[i]) && !isnan(v[j])) {
                    double delta = C[i][j] - (u[i] + v[j]);
                    if (delta < -EPS) {
                        optimal = false;
                        if (delta < min_delta) {
                            min_delta = delta;
                            enter_i = i;
                            enter_j = j;
                        }
                    }
                }
            }
        }
        
        if (optimal) break;
        
        // Поиск цикла и перераспределение
        auto cycle = findCycle(x, enter_i, enter_j);
        
        // Находим минимальную перевозку в нечетных вершинах цикла
        double theta = numeric_limits<double>::infinity();
        for (size_t k = 1; k < cycle.size(); k += 2) {
            int i = cycle[k].first;
            int j = cycle[k].second;
            if (x[i][j] < theta) {
                theta = x[i][j];
            }
        }
        
        // Перераспределение по циклу
        for (size_t k = 0; k < cycle.size(); ++k) {
            int i = cycle[k].first;
            int j = cycle[k].second;
            if (k % 2 == 0) {
                x[i][j] += theta;
            } else {
                x[i][j] -= theta;
            }
        }
    }
    
    cout << "\nОптимальный план перевозок:" << endl;
    printMatrix(x);
    
    // Вычисление общей стоимости
    double total_cost = 0;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            total_cost += x[i][j] * C[i][j];
        }
    }
    cout << fixed << setprecision(2);
    cout << "Минимальная стоимость перевозок: " << total_cost << endl;
    
    // Проверка выполнения ограничений
    cout << "\nПроверка:" << endl;
    
    vector<double> a_plan(m, 0);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            a_plan[i] += x[i][j];
        }
    }
    cout << "Запасы поставщиков (план): ";
    printVector(a_plan);
    cout << "Запасы по условию: ";
    printVector(a);
    
    vector<double> b_plan(n, 0);
    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < m; ++i) {
            b_plan[j] += x[i][j];
        }
    }
    cout << "Потребности потребителей (план): ";
    printVector(b_plan);
    cout << "Потребности по условию: ";
    printVector(b);
    
    return 0;
}
