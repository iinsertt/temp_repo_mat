#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

const double EPS = 1e-9;

// Структура для хранения точки
struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

// Функция для поиска вершин области допустимых решений
vector<Point> findVertices(const vector<tuple<double, double, double, int>>& constraints) {
    vector<tuple<double, double, double>> lines;
    
    // Добавляем прямые из ограничений
    for (const auto& cons : constraints) {
        lines.emplace_back(get<0>(cons), get<1>(cons), get<2>(cons));
    }
    
    // Добавляем ограничения неотрицательности
    lines.emplace_back(1, 0, 0);
    lines.emplace_back(0, 1, 0);
    
    vector<Point> vertices;
    
    // Находим все точки пересечения
    for (size_t i = 0; i < lines.size(); ++i) {
        for (size_t j = i + 1; j < lines.size(); ++j) {
            double a1 = get<0>(lines[i]), b1 = get<1>(lines[i]), c1 = get<2>(lines[i]);
            double a2 = get<0>(lines[j]), b2 = get<1>(lines[j]), c2 = get<2>(lines[j]);
            
            double det = a1 * b2 - a2 * b1;
            if (abs(det) < EPS) continue;
            
            double x = (c1 * b2 - c2 * b1) / det;
            double y = (a1 * c2 - a2 * c1) / det;
            
            // Проверяем принадлежность области
            bool feasible = true;
            for (const auto& cons : constraints) {
                double a = get<0>(cons), b = get<1>(cons), c = get<2>(cons);
                int sign = get<3>(cons);
                double val = a * x + b * y;
                
                if (sign == -1) { // <=
                    if (val > c + EPS) feasible = false;
                } else { // >=
                    if (val < c - EPS) feasible = false;
                }
            }
            
            if (feasible && x >= -EPS && y >= -EPS) {
                // Проверяем на дубликаты
                bool duplicate = false;
                for (const auto& v : vertices) {
                    if (abs(x - v.x) < 1e-6 && abs(y - v.y) < 1e-6) {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate) {
                    vertices.emplace_back(x, y);
                }
            }
        }
    }
    
    return vertices;
}

// Реализация симплекс-метода
vector<double> simplex(const vector<double>& c, const vector<vector<double>>& A, const vector<double>& b) {
    int m = A.size();    // количество ограничений
    int n = c.size();    // количество переменных
    
    // Создаем симплекс-таблицу: (m+1) строк, (n + m + 1) столбцов
    vector<vector<double>> tableau(m + 1, vector<double>(n + m + 1, 0.0));
    
    // Заполняем ограничения
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            tableau[i][j] = A[i][j];
        }
        tableau[i][n + i] = 1.0;  // базисные переменные
        tableau[i][n + m] = b[i]; // правая часть
    }
    
    // Заполняем целевую функцию (минимизация)
    for (int j = 0; j < n; ++j) {
        tableau[m][j] = -c[j];
    }
    
    // Основной цикл симплекс-метода
    while (true) {
        // Поиск входящей переменной (наиболее отрицательный коэффициент в строке цели)
        int col = -1;
        for (int j = 0; j < n + m; ++j) {
            if (tableau[m][j] < -EPS) {
                if (col == -1 || tableau[m][j] < tableau[m][col]) {
                    col = j;
                }
            }
        }
        
        if (col == -1) break; // Оптимальное решение найдено
        
        // Поиск выходящей переменной (минимальное отношение)
        int row = -1;
        double minRatio = numeric_limits<double>::infinity();
        
        for (int i = 0; i < m; ++i) {
            if (tableau[i][col] > EPS) {
                double ratio = tableau[i][n + m] / tableau[i][col];
                if (ratio < minRatio) {
                    minRatio = ratio;
                    row = i;
                }
            }
        }
        
        if (row == -1) {
            return {}; // Задача неограничена
        }
        
        // Нормируем строку с pivot-элементом
        double pivot = tableau[row][col];
        for (int j = 0; j <= n + m; ++j) {
            tableau[row][j] /= pivot;
        }
        
        // Обновляем остальные строки
        for (int i = 0; i <= m; ++i) {
            if (i != row && abs(tableau[i][col]) > EPS) {
                double factor = tableau[i][col];
                for (int j = 0; j <= n + m; ++j) {
                    tableau[i][j] -= factor * tableau[row][j];
                }
            }
        }
    }
    
    // Извлекаем решение
    vector<double> x(n, 0.0);
    for (int i = 0; i < m; ++i) {
        int basicCol = -1;
        for (int j = 0; j < n; ++j) {
            if (abs(tableau[i][j] - 1.0) < EPS) {
                bool isBasic = true;
                for (int k = 0; k < m; ++k) {
                    if (k != i && abs(tableau[k][j]) > EPS) {
                        isBasic = false;
                        break;
                    }
                }
                if (isBasic) {
                    basicCol = j;
                    break;
                }
            }
        }
        if (basicCol != -1) {
            x[basicCol] = tableau[i][n + m];
        }
    }
    
    return x;
}

// Вспомогательная функция для вывода точек
void printPoints(const vector<Point>& points, const string& title = "") {
    if (!title.empty()) {
        cout << title << endl;
    }
    for (const auto& p : points) {
        cout << fixed << setprecision(2) << "  (" << p.x << ", " << p.y << ")" << endl;
    }
}

int main() {
    cout << "ВАРИАНТ 2\n" << endl;
    
    // ========== ЗАДАНИЕ 1 ==========
    cout << "1. Графическое решение систем неравенств" << endl;
    
    // Задание 1а
    cout << "а) x1 + x2 ≤ 5, 3x1 - x2 ≤ 3, x1 ≥ 0, x2 ≥ 0" << endl;
    vector<tuple<double, double, double, int>> cons_a = {
        {1, 1, 5, -1},
        {3, -1, 3, -1}
    };
    
    vector<Point> verts_a = findVertices(cons_a);
    cout << "Угловые точки:" << endl;
    printPoints(verts_a);
    
    // Задание 1б
    cout << "\nб) x1 - x2 ≤ 3, x1 + x2 ≤ 9, -x1 + x2 ≥ 3, x1 + x2 ≥ 1.5, x1 ≥0, x2 ≥0" << endl;
    vector<tuple<double, double, double, int>> cons_b = {
        {1, -1, 3, -1},
        {1, 1, 9, -1},
        {-1, 1, 3, 1},
        {1, 1, 1.5, 1}
    };
    
    vector<Point> verts_b = findVertices(cons_b);
    cout << "Угловые точки:" << endl;
    printPoints(verts_b);
    
    cout << "\nПримечание: Для построения графиков используйте Python или сохраните данные в файл." << endl;
    
    // ========== ЗАДАНИЕ 2 ==========
    cout << "\n2. Задача про мангалы" << endl;
    cout << "Математическая модель:" << endl;
    cout << "x1 – количество угольных мангалов" << endl;
    cout << "x2 – количество газовых мангалов" << endl;
    cout << "Целевая функция: max Z = x1 + x2 (суммарный выпуск)" << endl;
    cout << "Ограничения:" << endl;
    cout << "  5*x1 + 8*x2 ≤ 2600  (производство)" << endl;
    cout << "  0.8*x1 + 1.2*x2 ≤ 400 (сборка)" << endl;
    cout << "  0.5*x1 + 0.5*x2 ≤ 200 (упаковка)" << endl;
    cout << "  x1 ≥ 300, x2 ≥ 300" << endl;
    cout << "  x1, x2 ≥ 0" << endl;
    
    // Матрица ограничений (приводим все к виду ≤)
    vector<vector<double>> A = {
        {5, 8},      // 5*x1 + 8*x2 ≤ 2600
        {0.8, 1.2},  // 0.8*x1 + 1.2*x2 ≤ 400
        {0.5, 0.5},  // 0.5*x1 + 0.5*x2 ≤ 200
        {-1, 0},     // -x1 ≤ -300  (x1 ≥ 300)
        {0, -1}      // -x2 ≤ -300  (x2 ≥ 300)
    };
    
    vector<double> b = {2600, 400, 200, -300, -300};
    vector<double> c = {-1, -1}; // для минимизации -Z
    
    vector<double> x_opt = simplex(c, A, b);
    
    if (!x_opt.empty()) {
        cout << fixed << setprecision(2);
        cout << "\nРешение симплекс-методом:" << endl;
        cout << "x1 = " << x_opt[0] << ", x2 = " << x_opt[1] << endl;
        cout << "Суммарный выпуск Z = " << x_opt[0] + x_opt[1] << endl;
        
        // Проверка выполнения ограничений
        cout << "\nПроверка ограничений:" << endl;
        cout << "  5*" << x_opt[0] << " + 8*" << x_opt[1] << " = " 
             << (5*x_opt[0] + 8*x_opt[1]) << " ≤ 2600" << endl;
        cout << "  0.8*" << x_opt[0] << " + 1.2*" << x_opt[1] << " = " 
             << (0.8*x_opt[0] + 1.2*x_opt[1]) << " ≤ 400" << endl;
        cout << "  0.5*" << x_opt[0] << " + 0.5*" << x_opt[1] << " = " 
             << (0.5*x_opt[0] + 0.5*x_opt[1]) << " ≤ 200" << endl;
        cout << "  x1 = " << x_opt[0] << " ≥ 300" << endl;
        cout << "  x2 = " << x_opt[1] << " ≥ 300" << endl;
    } else {
        cout << "Задача не имеет допустимого решения" << endl;
    }
    
    // Дополнительно: находим угловые точки для задачи про мангалы
    cout << "\nУгловые точки области допустимых решений задачи про мангалы:" << endl;
    vector<tuple<double, double, double, int>> cons_m = {
        {5, 8, 2600, -1},
        {0.8, 1.2, 400, -1},
        {0.5, 0.5, 200, -1},
        {1, 0, 300, 1},
        {0, 1, 300, 1}
    };
    
    vector<Point> verts_m = findVertices(cons_m);
    printPoints(verts_m);
    
    return 0;
}
