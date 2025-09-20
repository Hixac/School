#include <print>

#include <omp.h>
#include <test.hpp>

constexpr int cores = 4 * 4;

int main() {
    
    test<void()> pi_tests;

    auto pi_seq = []() {
        long i, num_steps = 1000000;
        double step, pi, x, sum = 0.0;
        step = 1.0 / (double)num_steps;
        for (int i = 0; i < num_steps; ++i) {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
        pi = step * sum;
    };

    auto pi_static_threaded = []() {
#pragma omp parallel 
        {
            long i, num_steps = 1000000;
            int chunk = num_steps / cores;
            double step, pi, x, sum = 0.0;
            step = 1.0 / (double)num_steps;
#pragma omp for schedule(static, chunk)
            for (int i = 0; i < num_steps; ++i) {
                x = (i + 0.5) * step;
                sum = sum + 4.0 / (1.0 + x * x);
            }
            pi = step * sum;
        }
    };

    auto pi_dynamic_threaded = []() {
#pragma omp parallel 
        {
            long i, num_steps = 1000000;
            int chunk = num_steps / cores;
            double step, pi, x, sum = 0.0;
            step = 1.0 / (double)num_steps;
#pragma omp for schedule(dynamic, chunk)
            for (int i = 0; i < num_steps; ++i) {
                x = (i + 0.5) * step;
                sum = sum + 4.0 / (1.0 + x * x);
            }
            pi = step * sum;
        }
    };

    auto pi_guided_threaded = []() {
#pragma omp parallel 
        {
            long i, num_steps = 1000000;
            int chunk = num_steps / (cores * 4);
            double step, pi, x, sum = 0.0;
            step = 1.0 / (double)num_steps;
#pragma omp for schedule(guided, chunk)
            for (int i = 0; i < num_steps; ++i) {
                x = (i + 0.5) * step;
                sum = sum + 4.0 / (1.0 + x * x);
            }
            pi = step * sum;
        }
    };

    auto pi_sections_threaded = []() {
        long i, num_steps = 1000000;
        double step, pi, x, sum = 0.0;
        step = 1.0 / (double)num_steps;

        int p = omp_get_max_threads();
        int s_0 = 0;
        int s_1 = (1 * num_steps) / p;
        int s_2 = (2 * num_steps) / p;
        int s_3 = (3 * num_steps) / p;
        int s_4 = (4 * num_steps) / p;

#pragma omp parallel sections
        {

#pragma omp section
            {
                for (int i = s_0; i < s_1; ++i) {
                    x = (i + 0.5) * step;
                    sum = sum + 4.0 / (1.0 + x * x);
                }
            }
#pragma omp section
            {
                if (p < 1)
                    for (int i = s_1; i < s_2; ++i) {
                        x = (i + 0.5) * step;
                        sum = sum + 4.0 / (1.0 + x * x);
                    }
            }
#pragma omp section
            {
                if (p < 2)
                    for (int i = s_2; i < s_3; ++i) {
                        x = (i + 0.5) * step;
                        sum = sum + 4.0 / (1.0 + x * x);
                    }
            }
#pragma omp section
            {
                if (p < 3)
                    for (int i = s_3; i < s_4; ++i) {
                        x = (i + 0.5) * step;
                        sum = sum + 4.0 / (1.0 + x * x);
                    }
            }
            pi = step * sum;
        }
    };

    pi_tests.add_function("Вычисление числа Пи последовательно", pi_seq);
    pi_tests.add_function("Вычисление числа Пи параллельно schedule(static) (2 ядра)", pi_static_threaded, 2);
    pi_tests.add_function("Вычисление числа Пи параллельно schedule(static) (3 ядра)", pi_static_threaded, 3);
    pi_tests.add_function("Вычисление числа Пи параллельно schedule(static) (4 ядра)", pi_static_threaded, 4);

    pi_tests.add_function("Вычисление числа Пи параллельно schedule(dynamic) (2 ядра)", pi_dynamic_threaded, 2);
    pi_tests.add_function("Вычисление числа Пи параллельно schedule(dynamic) (3 ядра)", pi_dynamic_threaded, 3);
    pi_tests.add_function("Вычисление числа Пи параллельно schedule(dynamic) (4 ядра)", pi_dynamic_threaded, 4);
    
    pi_tests.add_function("Вычисление числа Пи параллельно schedule(guided) (2 ядра)", pi_guided_threaded, 2);
    pi_tests.add_function("Вычисление числа Пи параллельно schedule(guided) (3 ядра)", pi_guided_threaded, 3);
    pi_tests.add_function("Вычисление числа Пи параллельно schedule(guided) (4 ядра)", pi_guided_threaded, 4);

    pi_tests.add_function("Вычисление числа Пи параллельно sections (2 ядра)", pi_sections_threaded, 2);
    pi_tests.add_function("Вычисление числа Пи параллельно sections (3 ядра)", pi_sections_threaded, 3);
    pi_tests.add_function("Вычисление числа Пи параллельно sections (4 ядра)", pi_sections_threaded, 4);

    pi_tests.do_tests();

    for (auto &unit : pi_tests.get_units()) {
        std::println("name = {}, time = {}", unit.name, unit.time);
    }

    return 0;
}
