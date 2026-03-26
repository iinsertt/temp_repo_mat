#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>

using namespace std;

const double EPS = 1e-9;

// Структура для хранения результата решения игры
struct GameSolution {
    bool hasSaddlePoint;
    double saddleValue;
    int saddleRow;
    int saddleCol;
    vector<double> p;  // вероятности для игрока A (строки)
    vector<double> q;  // вероятности для игрока B (столбцы)
    double gameValue;
};

class MatrixGame {
private:
    vector<vector<double>> payoff;
    int m;  // число строк (стратегии игрока A)
    int n;  // число столбцов (стратегии игрока B)

public:
    MatrixGame(const vector<vector<double>>& matrix) : payoff(matrix) {
        m = matrix.size();
        n = matrix[0].size();
    }

    // Поиск седловой точки
    pair<bool, tuple<double, int, int>> findSaddlePoint() {
        vector<double> rowMin(m);
        vector<double> colMax(n);
        
        // Минимумы в строках
        for (int i = 0; i < m; ++i) {
            rowMin[i] = *min_element(payoff[i].begin(), payoff[i].end());
        }
        
        // Максимумы в столбцах
        for (int j = 0; j < n; ++j) {
            double maxVal = payoff[0][j];
            for (int i = 1; i < m; ++i) {
                if (payoff[i][j] > maxVal) maxVal = payoff[i][j];
            }
            colMax[j] = maxVal;
        }
        
        double maxOfRowMin = *max_element(rowMin.begin(), rowMin.end());
        double minOfColMax = *min_element(colMax.begin(), colMax.end());
        
        // Ищем позицию седловой точки
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (abs(payoff[i][j] - maxOfRowMin) < EPS && 
                    abs(payoff[i][j] - minOfColMax) < EPS) {
                    return {true, {payoff[i][j], i, j}};
                }
            }
        }
        
        return {false, {0, -1, -1}};
    }

    // Решение игры 2x2 (частный случай)
    pair<vector<double>, vector<double>> solve2x2() {
        if (m != 2 || n != 2) {
            return {{}, {}};
        }
        
        double a = payoff[0][0];
        double b = payoff[0][1];
        double c = payoff[1][0];
        double d = payoff[1][1];
        
        double p1 = (d - c) / (a - b - c + d);
        double p2 = 1 - p1;
        double q1 = (d - b) / (a - b - c + d);
        double q2 = 1 - q1;
        double v = (a * d - b * c) / (a - b - c + d);
        
        return {{p1, p2}, {q1, q2}};
    }

    // Решение игры 2xn или mx2 (графическим методом)
    // Для простоты реализуем общий метод через линейное программирование
    // Используем метод, основанный на решении систем линейных уравнений
    
    void solveMixedStrategies() {
        // Для игрока A: ищем вероятности p_i, такие что
        // для каждого столбца j: sum_i p_i * a_ij >= v, и sum_i p_i = 1, v -> max
        
        // Для игрока B: ищем вероятности q_j, такие что
        // для каждой строки i: sum_j a_ij * q_j <= v, и sum_j q_j = 1, v -> min
        
        // Используем упрощенный подход для матриц 3x3
        if (m == 3 && n == 3) {
            solve3x3();
        } else {
            cout << "Для матриц другого размера требуется решение задачи линейного программирования" << endl;
        }
    }

    // Решение игры 3x3 методом сведения к системе линейных уравнений
    void solve3x3() {
        // Предполагаем, что все стратегии активны
        // Решаем систему: для игрока A
        // p1*a11 + p2*a21 + p3*a31 = v
        // p1*a12 + p2*a22 + p3*a32 = v
        // p1*a13 + p2*a23 + p3*a33 = v
        // p1 + p2 + p3 = 1
        
        vector<vector<double>> A(4, vector<double>(4, 0.0));
        vector<double> B(4, 0.0);
        
        // Уравнения для стратегий игрока A
        for (int j = 0; j < 3; ++j) {
            for (int i = 0; i < 3; ++i) {
                A[j][i] = payoff[i][j];
            }
            A[j][3] = -1.0;
        }
        
        // Уравнение нормировки
        for (int i = 0; i < 3; ++i) {
            A[3][i] = 1.0;
        }
        A[3][3] = 0.0;
        B[3] = 1.0;
        
        // Решаем систему линейных уравнений методом Гаусса
        vector<double> solution = gaussElimination(A, B);
        
        if (!solution.empty()) {
            vector<double> p(3);
            for (int i = 0; i < 3; ++i) {
                p[i] = solution[i];
            }
            double v = solution[3];
            
            // Проверяем неотрицательность вероятностей
            bool valid = true;
            for (double prob : p) {
                if (prob < -EPS) valid = false;
            }
            
            if (valid) {
                cout << "\nСмешанные стратегии для игрока A (строки):" << endl;
                for (int i = 0; i < 3; ++i) {
                    cout << "  p" << i+1 << " = " << fixed << setprecision(4) << p[i] << endl;
                }
                cout << "Цена игры v = " << v << endl;
                
                // Для игрока B решаем аналогичную систему
                vector<vector<double>> B_mat(4, vector<double>(4, 0.0));
                vector<double> B_rhs(4, 0.0);
                
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        B_mat[i][j] = payoff[i][j];
                    }
                    B_mat[i][3] = -1.0;
                }
                
                for (int j = 0; j < 3; ++j) {
                    B_mat[3][j] = 1.0;
                }
                B_mat[3][3] = 0.0;
                B_rhs[3] = 1.0;
                
                vector<double> q_sol = gaussElimination(B_mat, B_rhs);
                
                if (!q_sol.empty()) {
                    vector<double> q(3);
                    for (int i = 0; i < 3; ++i) {
                        q[i] = q_sol[i];
                    }
                    
                    cout << "\nСмешанные стратегии для игрока B (столбцы):" << endl;
                    for (int j = 0; j < 3; ++j) {
                        cout << "  q" << j+1 << " = " << fixed << setprecision(4) << q[j] << endl;
                    }
                }
            }
        }
    }

    // Метод Гаусса для решения СЛАУ
    vector<double> gaussElimination(vector<vector<double>> A, vector<double> B) {
        int n = A.size();
        
        // Прямой ход
        for (int i = 0; i < n; ++i) {
            // Поиск максимального элемента в столбце
            int maxRow = i;
            for (int k = i + 1; k < n; ++k) {
                if (abs(A[k][i]) > abs(A[maxRow][i])) {
                    maxRow = k;
                }
            }
            
            // Перестановка строк
            swap(A[i], A[maxRow]);
            swap(B[i], B[maxRow]);
            
            if (abs(A[i][i]) < EPS) {
                continue; // Система может иметь множество решений
            }
            
            // Нормировка строки
            double pivot = A[i][i];
            for (int j = i; j < n; ++j) {
                A[i][j] /= pivot;
            }
            B[i] /= pivot;
            
            // Исключение в других строках
            for (int k = 0; k < n; ++k) {
                if (k != i && abs(A[k][i]) > EPS) {
                    double factor = A[k][i];
                    for (int j = i; j < n; ++j) {
                        A[k][j] -= factor * A[i][j];
                    }
                    B[k] -= factor * B[i];
                }
            }
        }
        
        return B;
    }

    // Проверка, является ли стратегия оптимальной
    bool isOptimalStrategy(const vector<double>& p, const vector<double>& q) {
        double v = 0;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                v += p[i] * payoff[i][j] * q[j];
            }
        }
        
        // Проверка: для всех чистых стратегий игрока A
        for (int i = 0; i < m; ++i) {
            double expected = 0;
            for (int j = 0; j < n; ++j) {
                expected += payoff[i][j] * q[j];
            }
            if (expected > v + EPS) return false;
        }
        
        // Проверка: для всех чистых стратегий игрока B
        for (int j = 0; j < n; ++j) {
            double expected = 0;
            for (int i = 0; i < m; ++i) {
                expected += p[i] * payoff[i][j];
            }
            if (expected < v - EPS) return false;
        }
        
        return true;
    }

    void solve() {
        cout << "\nМатрица выигрышей:" << endl;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                cout << setw(6) << payoff[i][j];
            }
            cout << endl;
        }
        
        // Поиск седловой точки
        auto [hasSaddle, saddleInfo] = findSaddlePoint();
        auto [saddleValue, saddleRow, saddleCol] = saddleInfo;
        
        if (hasSaddle) {
            cout << "\nСедловая точка найдена!" << endl;
            cout << "Значение седловой точки: " << saddleValue << endl;
            cout << "Позиция: строка " << saddleRow + 1 << ", столбец " << saddleCol + 1 << endl;
            cout << "\nОптимальные чистые стратегии:" << endl;
            cout << "Игрок A выбирает стратегию " << saddleRow + 1 << endl;
            cout << "Игрок B выбирает стратегию " << saddleCol + 1 << endl;
            cout << "Цена игры v = " << saddleValue << endl;
        } else {
            cout << "\nСедловая точка отсутствует." << endl;
            cout << "Используем смешанные стратегии." << endl;
            
            // Вычисление минимакса и максимина
            vector<double> rowMin(m);
            vector<double> colMax(n);
            
            for (int i = 0; i < m; ++i) {
                rowMin[i] = *min_element(payoff[i].begin(), payoff[i].end());
            }
            for (int j = 0; j < n; ++j) {
                double maxVal = payoff[0][j];
                for (int i = 1; i < m; ++i) {
                    if (payoff[i][j] > maxVal) maxVal = payoff[i][j];
                }
                colMax[j] = maxVal;
            }
            
            double maximin = *max_element(rowMin.begin(), rowMin.end());
            double minimax = *min_element(colMax.begin(), colMax.end());
            
            cout << "\nНижняя цена игры (максимин): " << maximin << endl;
            cout << "Верхняя цена игры (минимакс): " << minimax << endl;
            cout << "maximin ≠ minimax, что подтверждает отсутствие седловой точки." << endl;
            
            // Решение смешанных стратегий
            solveMixedStrategies();
        }
    }
};

// Функция для выбора варианта
vector<vector<double>> getMatrix(int variant) {
    switch(variant) {
        case 1:
            return {{1, 3, -2}, {0, -1, 4}, {3, 2, -1}};
        case 2:
            return {{4, 1, 0}, {-2, 3, 2}, {1, -3, 5}};
        case 3:
            return {{5, -1, 2}, {0, 3, -2}, {-2, 4, 1}};
        case 4:
            return {{2, 4, 1}, {3, -1, 0}, {-1, 2, 2}};
        case 5:
            return {{3, -2, 1}, {0, 2, 4}, {2, -1, -3}};
        case 6:
            return {{-1, 3, 2}, {4, 0, -2}, {1, 2, -1}};
        case 7:
            return {{3, 1, 0}, {-2, 4, 2}, {1, -3, 5}};
        case 8:
            return {{0, -1, 4}, {2, 3, -2}, {-2, 1, 5}};
        case 9:
            return {{1, 2, 3}, {4, 0, -1}, {-2, 5, 0}};
        case 10:
            return {{2, 3, 4}, {-1, 0, 5}, {1, -3, 2}};
        default:
            return {{3, -1, 2}, {2, 0, -2}, {-1, 4, 1}}; // пример из задания
    }
}

int main() {
    cout << string(60, '=') << endl;
    cout << "Лабораторная работа №5. Решение матричных игр" << endl;
    cout << string(60, '=') << endl;
    
    int choice;
    cout << "\nВыберите вариант (1-10): ";
    cin >> choice;
    
    auto matrix = getMatrix(choice);
    
    MatrixGame game(matrix);
    game.solve();
    
    // Контрольные вопросы
    cout << "\n" << string(60, '=') << endl;
    cout << "Контрольные вопросы" << endl;
    cout << string(60, '=') << endl;
    
    cout << "\n1. Что такое седловая точка и как она помогает в решении матричных игр?" << endl;
    cout << "   Седловая точка - это элемент матрицы, который является одновременно" << endl;
    cout << "   минимальным в своей строке и максимальным в своем столбце." << endl;
    cout << "   Если седловая точка существует, она дает оптимальное решение в чистых" << endl;
    cout << "   стратегиях, и ни одному игроку не выгодно отклоняться от выбранной стратегии." << endl;
    
    cout << "\n2. Какой принцип лежит в основе метода минимакса?" << endl;
    cout << "   Принцип минимакса заключается в том, что игрок A стремится максимизировать" << endl;
    cout << "   свой минимальный выигрыш, а игрок B - минимизировать свой максимальный проигрыш." << endl;
    cout << "   Это гарантирует каждому игроку определенный уровень выигрыша независимо от действий противника." << endl;
    
    cout << "\n3. В каких случаях игроки прибегают к смешанным стратегиям?" << endl;
    cout << "   К смешанным стратегиям прибегают, когда в игре отсутствует седловая точка," << endl;
    cout << "   то есть когда нижняя цена игры не равна верхней (maximin ≠ minimax)." << endl;
    cout << "   Смешанные стратегии позволяют найти оптимальное вероятностное распределение" << endl;
    cout << "   между доступными чистыми стратегиями." << endl;
    
    cout << "\n4. Как определить, оптимальна ли стратегия в матричной игре?" << endl;
    cout << "   Стратегия оптимальна, если: для игрока A - при ее использовании он получает" << endl;
    cout << "   выигрыш не меньше цены игры против любой стратегии противника;" << endl;
    cout << "   для игрока B - при ее использовании он проигрывает не больше цены игры" << endl;
    cout << "   против любой стратегии противника. Для смешанных стратегий это проверяется" << endl;
    cout << "   через систему неравенств, вытекающих из условия равновесия." << endl;
    
    cout << "\n5. Объясните, что такое равновесие Нэша в контексте матричных игр." << endl;
    cout << "   Равновесие Нэша - это набор стратегий (чистых или смешанных), при котором" << endl;
    cout << "   ни одному из игроков невыгодно отклоняться от своей стратегии в одиночку," << endl;
    cout << "   если остальные придерживаются своих стратегий. В матричных играх с нулевой" << endl;
    cout << "   суммой равновесие Нэша соответствует оптимальным стратегиям, найденным" << endl;
    cout << "   методом минимакса, и цене игры." << endl;
    
    return 0;
}
