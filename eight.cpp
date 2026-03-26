#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <random>
#include <algorithm>

using namespace std;

const double EPS = 1e-9;

// ==================== Задание 1: СМО с отказами ====================
pair<double, double> erlangCalc(int n, double alpha) {
    // Вычисление P0 и Pотк
    double sum_p0 = 0.0;
    double fact = 1.0;
    
    for (int k = 0; k <= n; ++k) {
        if (k > 0) fact *= k;
        sum_p0 += pow(alpha, k) / fact;
    }
    
    double p0 = 1.0 / sum_p0;
    
    // Вычисление факториала n
    double fact_n = 1.0;
    for (int i = 1; i <= n; ++i) fact_n *= i;
    
    double p_otk = p0 * (pow(alpha, n) / fact_n);
    
    return {p0, p_otk};
}

void task1() {
    cout << string(60, '=') << endl;
    cout << "Задание 1: СМО с отказами" << endl;
    cout << string(60, '=') << endl;
    
    double lambda_val = 16.0;  // заявок в час
    double t_service_min = 10.0; // минут
    double t_service_hour = t_service_min / 60.0;
    double mu = 1.0 / t_service_hour;
    double alpha = lambda_val / mu;
    
    int n = 2;
    
    cout << fixed << setprecision(3);
    cout << "Исходные данные: λ = " << lambda_val << " заявок/ч, среднее время обслуживания = "
         << t_service_min << " мин → μ = " << mu << " заявок/ч" << endl;
    cout << "Нагрузка α = λ/μ = " << alpha << endl;
    
    auto [p0, p_otk] = erlangCalc(n, alpha);
    double p_obs = 1.0 - p_otk;
    double A = lambda_val * p_obs;
    double M = A / mu;
    
    cout << fixed << setprecision(4);
    cout << "\nХарактеристики при 2 бухгалтерах:" << endl;
    cout << "  Вероятность простоя каналов P0 = " << p0 << endl;
    cout << "  Вероятность отказа Pотк = " << p_otk << endl;
    cout << "  Вероятность обслуживания Pобс = " << p_obs << endl;
    cout << "  Абсолютная пропускная способность A = " << A << " заявок/ч" << endl;
    cout << "  Среднее число занятых бухгалтеров M = " << M << endl;
    
    vector<int> n_values;
    vector<double> p_otk_list, p_obs_list;
    
    for (int n_cur = 1; n_cur <= 7; ++n_cur) {
        auto [_, p_otk_cur] = erlangCalc(n_cur, alpha);
        n_values.push_back(n_cur);
        p_otk_list.push_back(p_otk_cur);
        p_obs_list.push_back(1.0 - p_otk_cur);
    }
    
    cout << "\nПодбор числа бухгалтеров:" << endl;
    cout << "  n   Pотк       Pобс" << endl;
    for (size_t i = 0; i < n_values.size(); ++i) {
        cout << " " << setw(2) << n_values[i] << "   " 
             << fixed << setprecision(4) << p_otk_list[i] << "   " 
             << 1.0 - p_otk_list[i] << endl;
    }
    
    for (size_t i = 0; i < n_values.size(); ++i) {
        if (p_obs_list[i] > 0.85) {
            cout << "\nМинимальное число бухгалтеров для Pобс > 85%: n = " << n_values[i] << endl;
            break;
        }
    }
    
    // Вывод данных для построения графика
    cout << "\nДанные для построения графика:" << endl;
    cout << "n_values = [";
    for (size_t i = 0; i < n_values.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << n_values[i];
    }
    cout << "]" << endl;
    
    cout << "p_obs = [";
    for (size_t i = 0; i < p_obs_list.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << p_obs_list[i];
    }
    cout << "]" << endl;
    
    cout << "\nДля построения графика используйте Python:" << endl;
    cout << "----------------------------------------" << endl;
    cout << "import matplotlib.pyplot as plt" << endl;
    cout << "n_values = ["; 
    for (size_t i = 0; i < n_values.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << n_values[i];
    }
    cout << "]" << endl;
    cout << "p_obs = [";
    for (size_t i = 0; i < p_obs_list.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << p_obs_list[i];
    }
    cout << "]" << endl;
    cout << "plt.plot(n_values, p_obs, 'bo-', label='Pобс')" << endl;
    cout << "plt.axhline(y=0.85, color='r', linestyle='--', label='85%')" << endl;
    cout << "plt.xlabel('Число бухгалтеров n')" << endl;
    cout << "plt.ylabel('Вероятность обслуживания')" << endl;
    cout << "plt.title('Зависимость вероятности обслуживания от числа бухгалтеров')" << endl;
    cout << "plt.grid(True)" << endl;
    cout << "plt.legend()" << endl;
    cout << "plt.show()" << endl;
}

// ==================== Задание 2: Имитационное моделирование ====================
void task2() {
    cout << "\n" << string(60, '=') << endl;
    cout << "Задание 2: Имитационное моделирование" << endl;
    cout << string(60, '=') << endl;
    
    double lambda_pois = 1.8;
    double mu_exp = 0.5;
    
    // Инициализация генераторов случайных чисел
    random_device rd;
    mt19937 gen(rd());
    
    // Экспоненциальное распределение (методом обратного преобразования)
    uniform_real_distribution<> uniform_dist(0.0, 1.0);
    exponential_distribution<> exp_dist(mu_exp);
    poisson_distribution<> pois_dist(lambda_pois);
    
    const int N = 15;
    vector<double> x(N);
    vector<int> y(N);
    
    cout << "\nСмоделированные значения:" << endl;
    cout << "  №    X (длительность обслуживания, ед.вр.)    Y (число клиентов)" << endl;
    
    double sum_x = 0.0;
    double sum_y = 0.0;
    
    for (int i = 0; i < N; ++i) {
        // Метод обратного преобразования для экспоненциального распределения
        double u = uniform_dist(gen);
        x[i] = -log(1.0 - u) / mu_exp;
        
        // Пуассоновское распределение
        y[i] = pois_dist(gen);
        
        sum_x += x[i];
        sum_y += y[i];
        
        cout << setw(3) << i+1 << "   " 
             << fixed << setprecision(4) << x[i] 
             << "                              " 
             << setw(2) << y[i] << endl;
    }
    
    double mean_x = sum_x / N;
    double mean_y = sum_y / N;
    double theoretical_mean_x = 1.0 / mu_exp;
    
    cout << fixed << setprecision(2);
    cout << "\nИнтерпретация:" << endl;
    cout << "  - Значения X означают время обслуживания одного клиента (в тех же единицах времени, что и интенсивность)." << endl;
    cout << "    Среднее теоретическое время обслуживания = 1/μ = " << theoretical_mean_x << "." << endl;
    cout << "    В выборке среднее = " << mean_x << "." << endl;
    cout << "  - Значения Y означают число клиентов, поступивших за единицу времени (например, за час)." << endl;
    cout << "    Среднее теоретическое = λ = " << lambda_pois << ". В выборке среднее = " << mean_y << "." << endl;
    cout << "  - Эти числа можно использовать для имитационного моделирования работы парикмахерской:";
    cout << " для каждого часа генерируется число клиентов, и для каждого клиента — время обслуживания." << endl;
    
    // Дополнительная статистика
    cout << "\nДополнительная статистика:" << endl;
    
    sort(x.begin(), x.end());
    double median_x = (x[N/2] + x[N/2 - 1]) / 2.0;
    
    sort(y.begin(), y.end());
    double median_y = (y[N/2] + y[N/2 - 1]) / 2.0;
    
    cout << "  Медиана X: " << median_x << endl;
    cout << "  Медиана Y: " << median_y << endl;
    
    // Вычисление дисперсии
    double var_x = 0.0, var_y = 0.0;
    for (int i = 0; i < N; ++i) {
        var_x += (x[i] - mean_x) * (x[i] - mean_x);
        var_y += (y[i] - mean_y) * (y[i] - mean_y);
    }
    var_x /= (N - 1);
    var_y /= (N - 1);
    
    cout << "  Дисперсия X: " << var_x << " (теоретическая: " << 1.0/(mu_exp*mu_exp) << ")" << endl;
    cout << "  Дисперсия Y: " << var_y << " (теоретическая: " << lambda_pois << ")" << endl;
}

int main() {
    cout << fixed << setprecision(4);
    
    task1();
    task2();
    
    return 0;
}
