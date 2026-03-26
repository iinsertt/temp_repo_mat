#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <iomanip>
#include <algorithm>

using namespace std;

const double EPS = 1e-9;

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

// Находит пересечение двух прямых, заданных в виде (a, b, c): a*x + b*y = c
Point findIntersection(const tuple<double, double, double>& line1, 
                       const tuple<double, double, double>& line2) {
    double a1 = get<0>(line1), b1 = get<1>(line1), c1 = get<2>(line1);
    double a2 = get<0>(line2), b2 = get<1>(line2), c2 = get<2>(line2);
    
    double det = a1 * b2 - a2 * b1;
    if (abs(det) < EPS) {
        return Point(NAN, NAN);
    }
    
    double x = (c1 * b2 - c2 * b1) / det;
    double y = (a1 * c2 - a2 * c1) / det;
    return Point(x, y);
}

// Проверяет, принадлежит ли точка области допустимых решений
bool isFeasible(double x, double y, const vector<tuple<double, double, double, int>>& constraints) {
    for (const auto& cons : constraints) {
        double a = get<0>(cons), b = get<1>(cons), c = get<2>(cons);
        int sign = get<3>(cons);
        
        double val = a * x + b * y;
        
        if (sign == -1) { // <=
            if (val - c > EPS) return false;
        } else if (sign == 1) { // >=
            if (c - val > EPS) return false;
        } else { // ==
            if (abs(val - c) > EPS) return false;
        }
    }
    return true;
}

// Решает задачу ЛП с двумя переменными
tuple<double, double, double, vector<Point>> solveLP2D(const vector<double>& c, 
                                                        const vector<tuple<double, double, double, int>>& constraints) {
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
            Point p = findIntersection(lines[i], lines[j]);
            
            if (!isnan(p.x) && !isnan(p.y)) {
                if (isFeasible(p.x, p.y, constraints)) {
                    // Проверяем, нет ли уже такой точки
                    bool isDuplicate = false;
                    for (const auto& v : vertices) {
                        if (abs(p.x - v.x) < 1e-6 && abs(p.y - v.y) < 1e-6) {
                            isDuplicate = true;
                            break;
                        }
                    }
                    if (!isDuplicate) {
                        vertices.push_back(p);
                    }
                }
            }
        }
    }
    
    if (vertices.empty()) {
        return {NAN, NAN, NAN, {}};
    }
    
    // Находим оптимальное решение (максимизация)
    double bestVal = -numeric_limits<double>::infinity();
    Point bestPoint;
    
    for (const auto& p : vertices) {
        double val = c[0] * p.x + c[1] * p.y;
        if (val > bestVal) {
            bestVal = val;
            bestPoint = p;
        }
    }
    
    return {bestPoint.x, bestPoint.y, bestVal, vertices};
}

// Выводит точки с фиксированной точностью
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
    
    auto [x1_opt, x2_opt, val_opt, verts_a] = solveLP2D({0, 0}, cons_a);
    cout << "Угловые точки области:" << endl;
    printPoints(verts_a);
    
    // Задание 1б
    cout << "\nб) x1 - x2 ≤ 3, x1 + x2 ≤ 9, -x1 + x2 ≥ 3, x1 + x2 ≥ 1.5, x1 ≥0, x2 ≥0" << endl;
    vector<tuple<double, double, double, int>> cons_b = {
        {1, -1, 3, -1},
        {1, 1, 9, -1},
        {-1, 1, 3, 1},
        {1, 1, 1.5, 1}
    };
    
    auto [x1_opt2, x2_opt2, val_opt2, verts_b] = solveLP2D({0, 0}, cons_b);
    cout << "Угловые точки области:" << endl;
    printPoints(verts_b);
    
    cout << "\nПримечание: Для построения графиков в C++ требуется дополнительная библиотека." << endl;
    cout << "Графики можно построить с помощью matplotlib-cpp или сохранить данные в файл." << endl;
    
    // ========== ЗАДАНИЕ 2 ==========
    cout << "\n2. Задача про мангалы" << endl;
    cout << "x1 – кол-во угольных мангалов, x2 – кол-во газовых" << endl;
    cout << "Целевая функция: max Z = x1 + x2 (максимизация суммарного выпуска)" << endl;
    cout << "Ограничения:" << endl;
    cout << "  5*x1 + 8*x2 ≤ 2600  (производство)" << endl;
    cout << "  0.8*x1 + 1.2*x2 ≤ 400 (сборка)" << endl;
    cout << "  0.5*x1 + 0.5*x2 ≤ 200 (упаковка)" << endl;
    cout << "  x1 ≥ 300, x2 ≥ 300 (контракт)" << endl;
    cout << "  x1 ≥ 0, x2 ≥ 0" << endl;
    cout << "\nТак как прибыль не указана, найдём максимально возможный выпуск (максимизируем x1+x2)." << endl;
    
    vector<double> c_m = {1, 1};
    vector<tuple<double, double, double, int>> cons_m = {
        {5, 8, 2600, -1},
        {0.8, 1.2, 400, -1},
        {0.5, 0.5, 200, -1},
        {1, 0, 300, 1},
        {0, 1, 300, 1}
    };
    
    auto [x1_opt_m, x2_opt_m, val_opt_m, verts_m] = solveLP2D(c_m, cons_m);
    
    if (!isnan(x1_opt_m)) {
        cout << fixed << setprecision(2);
        cout << "\nОптимальное решение: x1 = " << x1_opt_m << ", x2 = " << x2_opt_m << endl;
        cout << "Максимальный суммарный выпуск: " << val_opt_m << " шт." << endl;
        
        // Выводим все угловые точки для анализа
        cout << "\nУгловые точки области допустимых решений:" << endl;
        printPoints(verts_m);
    } else {
        cout << "Область допустимых решений пуста" << endl;
    }
    
    // Дополнительный анализ: проверка выполнения ограничений
    if (!isnan(x1_opt_m)) {
        cout << "\nПроверка ограничений для оптимального решения:" << endl;
        cout << "  5*" << x1_opt_m << " + 8*" << x2_opt_m << " = " << (5*x1_opt_m + 8*x2_opt_m) 
             << " ≤ 2600" << endl;
        cout << "  0.8*" << x1_opt_m << " + 1.2*" << x2_opt_m << " = " << (0.8*x1_opt_m + 1.2*x2_opt_m) 
             << " ≤ 400" << endl;
        cout << "  0.5*" << x1_opt_m << " + 0.5*" << x2_opt_m << " = " << (0.5*x1_opt_m + 0.5*x2_opt_m) 
             << " ≤ 200" << endl;
        cout << "  x1 = " << x1_opt_m << " ≥ 300" << endl;
        cout << "  x2 = " << x2_opt_m << " ≥ 300" << endl;
    }
    
    return 0;
}
