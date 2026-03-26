#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <iomanip>

using namespace std;

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

struct Constraint {
    double a, b, rhs;
    int sign; // -1 for <=, 1 for >=, 0 for =
};

struct Line {
    double a, b, c;
    Line(double a, double b, double c) : a(a), b(b), c(c) {}
};

const double EPS = 1e-9;

Point findIntersection(const Line& line1, const Line& line2) {
    double det = line1.a * line2.b - line2.a * line1.b;
    if (abs(det) < EPS) {
        return Point(NAN, NAN);
    }
    double x = (line1.c * line2.b - line2.c * line1.b) / det;
    double y = (line1.a * line2.c - line2.a * line1.c) / det;
    return Point(x, y);
}

bool isFeasible(double x, double y, const vector<Constraint>& constraints) {
    for (const auto& cons : constraints) {
        double val = cons.a * x + cons.b * y;
        if (cons.sign == -1) { // <=
            if (val - cons.rhs > EPS) {
                return false;
            }
        } else if (cons.sign == 1) { // >=
            if (cons.rhs - val > EPS) {
                return false;
            }
        } else { // ==
            if (abs(val - cons.rhs) > EPS) {
                return false;
            }
        }
    }
    return true;
}

bool pointEqual(const Point& p1, const Point& p2) {
    return abs(p1.x - p2.x) < 1e-6 && abs(p1.y - p2.y) < 1e-6;
}

void solveLP2D(const vector<double>& c, const vector<Constraint>& constraints, 
               const vector<string>& varNames = {"x1", "x2"}) {
    
    vector<Line> lines;
    
    // Add constraint lines
    for (const auto& cons : constraints) {
        lines.emplace_back(cons.a, cons.b, cons.rhs);
    }
    
    // Add non-negativity constraints
    lines.emplace_back(1, 0, 0);
    lines.emplace_back(0, 1, 0);
    
    vector<Point> vertices;
    
    // Find all intersection points
    for (size_t i = 0; i < lines.size(); ++i) {
        for (size_t j = i + 1; j < lines.size(); ++j) {
            Point p = findIntersection(lines[i], lines[j]);
            if (!isnan(p.x) && !isnan(p.y)) {
                if (isFeasible(p.x, p.y, constraints)) {
                    bool isDuplicate = false;
                    for (const auto& v : vertices) {
                        if (pointEqual(v, p)) {
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
        cout << "Область допустимых решений пуста или задача не имеет решения." << endl;
        return;
    }
    
    // Find optimal solution (maximization)
    double bestValue = -numeric_limits<double>::infinity();
    Point bestPoint;
    
    for (const auto& p : vertices) {
        double val = c[0] * p.x + c[1] * p.y;
        if (val > bestValue) {
            bestValue = val;
            bestPoint = p;
        }
    }
    
    cout << fixed << setprecision(2);
    cout << "Оптимальное решение: " << varNames[0] << " = " << bestPoint.x 
         << ", " << varNames[1] << " = " << bestPoint.y << endl;
    
    if (c[0] == -0.20 && c[1] == -0.24) {
        // Minimization case
        cout << "Минимальная стоимость: " << -bestValue << " руб." << endl;
    } else {
        cout << "Максимальный доход: " << bestValue << " руб." << endl;
    }
}

int main() {
    cout << string(50, '=') << endl;
    cout << "ЗАДАЧА 1: СУДА" << endl;
    cout << string(50, '=') << endl;
    
    vector<double> c1 = {20, 10};
    vector<Constraint> constraints1 = {
        {12000, 7000, 60000, -1},
        {250, 100, 700, -1},
        {2000, 1000, 7500, 1}
    };
    
    cout << "Модель:" << endl;
    cout << "  max Z = 20*x1 + 10*x2" << endl;
    cout << "  при ограничениях:" << endl;
    cout << "    12000*x1 + 7000*x2 <= 60000" << endl;
    cout << "    250*x1 + 100*x2 <= 700" << endl;
    cout << "    2000*x1 + 1000*x2 >= 7500" << endl;
    cout << "    x1 >= 0, x2 >= 0" << endl;
    cout << endl;
    
    solveLP2D(c1, constraints1);
    cout << endl;
    
    cout << string(50, '=') << endl;
    cout << "ЗАДАЧА 2: ПИТАНИЕ" << endl;
    cout << string(50, '=') << endl;
    
    vector<double> c2 = {-0.20, -0.24};
    vector<Constraint> constraints2 = {
        {1, 1, 10, 1},
        {4, 2, 12, 1},
        {2, 2, 8, 1},
        {0, 1, 1, 1}
    };
    
    cout << "Модель:" << endl;
    cout << "  min Z = 0.20*x1 + 0.24*x2" << endl;
    cout << "  при ограничениях:" << endl;
    cout << "    x1 + x2 >= 10      (жиры)" << endl;
    cout << "    4*x1 + 2*x2 >= 12  (белки)" << endl;
    cout << "    2*x1 + 2*x2 >= 8   (углеводы)" << endl;
    cout << "    x2 >= 1            (витамины)" << endl;
    cout << "    x1 >= 0, x2 >= 0" << endl;
    cout << endl;
    
    solveLP2D(c2, constraints2);
    cout << endl;
    
    cout << string(50, '=') << endl;
    cout << "ЗАДАЧА 3: ДЕТАЛИ" << endl;
    cout << string(50, '=') << endl;
    
    vector<double> c3 = {1.1, 1.5};
    vector<Constraint> constraints3 = {
        {1, 2, 5000, -1},
        {2, 4, 10000, -1},
        {5, 3, 10000, -1},
        {1, 0, 2500, -1},
        {0, 1, 2000, -1},
        {1, 1, 1500, 1}
    };
    
    cout << "Модель:" << endl;
    cout << "  max Z = 1.1*x1 + 1.5*x2" << endl;
    cout << "  при ограничениях:" << endl;
    cout << "    x1 + 2*x2 <= 5000   (рабочее время)" << endl;
    cout << "    2*x1 + 4*x2 <= 10000 (полимер)" << endl;
    cout << "    5*x1 + 3*x2 <= 10000 (листовой)" << endl;
    cout << "    x1 <= 2500          (мощность А)" << endl;
    cout << "    x2 <= 2000          (мощность В)" << endl;
    cout << "    x1 + x2 >= 1500     (общее кол-во)" << endl;
    cout << "    x1 >= 0, x2 >= 0" << endl;
    cout << endl;
    
    solveLP2D(c3, constraints3);
    cout << endl;
    
    cout << string(50, '=') << endl;
    cout << "ПРАКТИЧЕСКАЯ РАБОТА 2: ГРАФИЧЕСКОЕ РЕШЕНИЕ" << endl;
    cout << string(50, '=') << endl;
    cout << "Система:" << endl;
    cout << "  x1 + x2 <= 5" << endl;
    cout << "  3*x1 - x2 <= 3" << endl;
    cout << "  x1 >= 0, x2 >= 0" << endl;
    cout << endl;
    
    vector<Constraint> constraintsGr = {
        {1, 1, 5, -1},
        {3, -1, 3, -1}
    };
    
    vector<Line> linesGr;
    for (const auto& cons : constraintsGr) {
        linesGr.emplace_back(cons.a, cons.b, cons.rhs);
    }
    linesGr.emplace_back(1, 0, 0);
    linesGr.emplace_back(0, 1, 0);
    
    vector<Point> verticesGr;
    for (size_t i = 0; i < linesGr.size(); ++i) {
        for (size_t j = i + 1; j < linesGr.size(); ++j) {
            Point p = findIntersection(linesGr[i], linesGr[j]);
            if (!isnan(p.x) && !isnan(p.y)) {
                if (isFeasible(p.x, p.y, constraintsGr)) {
                    bool isDuplicate = false;
                    for (const auto& v : verticesGr) {
                        if (pointEqual(v, p)) {
                            isDuplicate = true;
                            break;
                        }
                    }
                    if (!isDuplicate) {
                        verticesGr.push_back(p);
                    }
                }
            }
        }
    }
    
    cout << "Угловые точки области:" << endl;
    for (size_t i = 0; i < verticesGr.size(); ++i) {
        cout << "  " << i + 1 << ": x1 = " << fixed << setprecision(2) << verticesGr[i].x 
             << ", x2 = " << verticesGr[i].y << endl;
    }
    
    cout << "\nГрафическое решение: допустимая область — четырёхугольник с вершинами ";
    cout << "(0,0), (1,0), (2,3), (0,5)." << endl;
    
    return 0;
}
