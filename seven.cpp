#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

const double INF = 1e9;
const double EPS = 1e-9;

// ==================== ЗАДАЧА 1: Распределение средств ====================
pair<double, vector<pair<double, double>>> solveInvestment(double S0, int n_years = 4) {
    double step = 1.0;
    int max_S = static_cast<int>(S0);
    
    vector<int> S_values;
    for (int i = 0; i <= max_S; ++i) {
        S_values.push_back(i);
    }
    
    map<int, double> f_next;
    for (int s : S_values) {
        f_next[s] = 0.0;
    }
    
    map<int, map<int, double>> best_x;
    
    // Динамическое программирование по годам
    for (int year = n_years - 1; year >= 0; --year) {
        map<int, double> f_curr;
        map<int, double> best_x_year;
        
        for (int s : S_values) {
            double best_income = -INF;
            double best_x_val = 0;
            
            // Перебираем x от 0 до s
            for (int x = 0; x <= s; ++x) {
                double y = s - x;
                double income_now = 5.0 * x + 3.0 * y;
                double s_next = 0.1 * x + 0.5 * y;
                
                int s_next_int = static_cast<int>(round(s_next));
                if (s_next_int > max_S) s_next_int = max_S;
                if (s_next_int < 0) s_next_int = 0;
                
                double total_income = income_now + f_next[s_next_int];
                if (total_income > best_income + EPS) {
                    best_income = total_income;
                    best_x_val = x;
                }
            }
            f_curr[s] = best_income;
            best_x_year[s] = best_x_val;
        }
        f_next = f_curr;
        best_x[year] = best_x_year;
    }
    
    // Восстановление плана
    double s_current = S0;
    vector<pair<double, double>> plan;
    
    for (int year = 0; year < n_years; ++year) {
        int s_int = static_cast<int>(round(s_current));
        double x_opt = best_x[year][s_int];
        double y_opt = s_current - x_opt;
        plan.emplace_back(x_opt, y_opt);
        
        double s_next = 0.1 * x_opt + 0.5 * y_opt;
        s_current = s_next;
    }
    
    return {f_next[static_cast<int>(S0)], plan};
}

// ==================== ЗАДАЧА 2: Алгоритм Дейкстры ====================
map<int, map<int, double>> buildGraphVariant3() {
    map<int, map<int, double>> graph;
    
    graph[1] = {{2, 2}, {3, 5}};
    graph[2] = {{1, 2}, {4, 3}, {5, 6}};
    graph[3] = {{1, 5}, {4, 4}, {6, 7}};
    graph[4] = {{2, 3}, {3, 4}, {5, 2}, {7, 5}};
    graph[5] = {{2, 6}, {4, 2}, {8, 3}};
    graph[6] = {{3, 7}, {7, 4}, {9, 6}};
    graph[7] = {{4, 5}, {6, 4}, {8, 3}, {10, 7}};
    graph[8] = {{5, 3}, {7, 3}, {10, 5}};
    graph[9] = {{6, 6}, {10, 4}};
    graph[10] = {{7, 7}, {8, 5}, {9, 4}};
    
    return graph;
}

pair<double, vector<int>> dijkstra(const map<int, map<int, double>>& graph, int start, int end) {
    map<int, double> distances;
    map<int, int> previous;
    
    // Инициализация
    for (const auto& vertex : graph) {
        distances[vertex.first] = INF;
        previous[vertex.first] = -1;
    }
    distances[start] = 0;
    
    // Очередь с приоритетом (расстояние, вершина)
    using pii = pair<double, int>;
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    pq.push({0, start});
    
    while (!pq.empty()) {
        double cur_dist = pq.top().first;
        int cur = pq.top().second;
        pq.pop();
        
        if (cur_dist > distances[cur] + EPS) continue;
        if (cur == end) break;
        
        // Проверяем, есть ли вершина в графе
        if (graph.find(cur) == graph.end()) continue;
        
        for (const auto& neighbor : graph.at(cur)) {
            int next = neighbor.first;
            double weight = neighbor.second;
            double new_dist = cur_dist + weight;
            
            if (new_dist < distances[next] - EPS) {
                distances[next] = new_dist;
                previous[next] = cur;
                pq.push({new_dist, next});
            }
        }
    }
    
    // Восстановление пути
    vector<int> path;
    int cur = end;
    while (cur != -1) {
        path.push_back(cur);
        cur = previous[cur];
    }
    reverse(path.begin(), path.end());
    
    return {distances[end], path};
}

// Альтернативная версия с использованием map для всех вершин
pair<double, vector<int>> dijkstraComplete(const map<int, map<int, double>>& graph, int start, int end) {
    // Находим все вершины
    set<int> all_vertices;
    for (const auto& [v, neighbors] : graph) {
        all_vertices.insert(v);
        for (const auto& [neighbor, _] : neighbors) {
            all_vertices.insert(neighbor);
        }
    }
    
    map<int, double> distances;
    map<int, int> previous;
    
    for (int v : all_vertices) {
        distances[v] = INF;
        previous[v] = -1;
    }
    distances[start] = 0;
    
    using pii = pair<double, int>;
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    pq.push({0, start});
    
    while (!pq.empty()) {
        double cur_dist = pq.top().first;
        int cur = pq.top().second;
        pq.pop();
        
        if (cur_dist > distances[cur] + EPS) continue;
        
        if (graph.find(cur) != graph.end()) {
            for (const auto& [next, weight] : graph.at(cur)) {
                double new_dist = cur_dist + weight;
                if (new_dist < distances[next] - EPS) {
                    distances[next] = new_dist;
                    previous[next] = cur;
                    pq.push({new_dist, next});
                }
            }
        }
    }
    
    // Восстановление пути
    vector<int> path;
    int cur = end;
    while (cur != -1) {
        path.push_back(cur);
        cur = previous[cur];
    }
    reverse(path.begin(), path.end());
    
    return {distances[end], path};
}

int main() {
    cout << fixed << setprecision(2);
    cout << string(60, '=') << endl;
    cout << "ЗАДАЧА 1: Распределение средств между предприятиями" << endl;
    cout << string(60, '=') << endl;
    
    double S0 = 100.0;
    auto [max_income, distribution] = solveInvestment(S0);
    
    cout << "Начальная сумма: " << S0 << endl;
    cout << "Максимальный доход за 4 года: " << max_income << endl;
    cout << "Распределение по годам (x_A, x_B):" << endl;
    
    int year_num = 1;
    for (const auto& [x, y] : distribution) {
        cout << "  Год " << year_num++ << ": A = " << x << ", B = " << y << endl;
    }
    
    cout << "\n" << string(60, '=') << endl;
    cout << "ЗАДАЧА 2: Кратчайший путь от 1 до 10 в графе (алгоритм Дейкстры)" << endl;
    cout << string(60, '=') << endl;
    
    auto graph = buildGraphVariant3();
    
    cout << "Граф (ребра с весами):" << endl;
    for (const auto& [v, neighbors] : graph) {
        cout << "  " << v << ": {";
        bool first = true;
        for (const auto& [neighbor, weight] : neighbors) {
            if (!first) cout << ", ";
            cout << neighbor << ": " << weight;
            first = false;
        }
        cout << "}" << endl;
    }
    
    auto [distance, path] = dijkstra(graph, 1, 10);
    
    cout << "\nКратчайшее расстояние от 1 до 10: " << distance << endl;
    cout << "Путь: ";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) cout << " -> ";
        cout << path[i];
    }
    cout << endl;
    
    return 0;
}
