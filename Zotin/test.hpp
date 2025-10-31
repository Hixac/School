#pragma once

#include <omp.h>

#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>

struct unit_test {
    std::string name;
    std::function<void()> func;
    unsigned int threads;
    double time;
};

class test {
    public:
        test() = default;

        template <typename function_type, typename... Args>
        void add_function(std::string name, function_type func, unsigned int threads, Args&&... args) {
            if (threads <= 0) 
                throw std::invalid_argument("Negative or zero number of threads");
            auto binded_func = [func, &args...]() mutable { func(std::forward<Args>(args)...); };
            auto unit = unit_test { name, binded_func, threads, 0 };
            m_units.push_back(unit);
        }

        void do_tests() {
            for (auto &unit : m_units) {
                omp_set_num_threads(unit.threads);
                unit.time = measure_average_time(unit);
            }
        }

        const auto &get_units() { return m_units; }
        inline void set_iteration_count(const unsigned int new_count) { iteration_count = new_count; }

    private:
        double measure_average_time(unit_test &unit) {
            std::vector<double> measures;
            double average = 0, median = 0;

            preliminary(unit, measures, average, median);
            double sd = standart_differentiate(measures, average);

            double new_average = 0;
            unsigned int new_size = 0;
            for (auto measure : measures) {
                if (median - sd <= measure && measure <= median + sd) {
                    new_average += measure;
                    new_size += 1;
                }
            }
            if (new_size == 0) new_size = 0;
            return new_average / static_cast<double>(new_size);
        }

        double standart_differentiate(std::vector<double> measures, double average) {
            double sd = 0;
            for (auto measure : measures) {
                sd += std::pow(measure - average, 2);
            }
            sd /= std::sqrt(static_cast<double>(iteration_count) - 1.0);

            return sd;
        }

        void preliminary(unit_test &unit, std::vector<double> &measures, double &average, double &median) {
            for (int i = 0; i < iteration_count; ++i) {
                double start_time = omp_get_wtime();
                unit.func();
                double end_time = omp_get_wtime();

                double time = (end_time - start_time) * 1000;
                average += time;

                measures.push_back(time);
            }
            std::sort(measures.begin(), measures.end());

            if (measures.size() % 2 != 0) {
                median = measures[measures.size() / 2];
            } else {
                auto temp = measures.size() / 2;
                median = measures[temp] + measures[temp - 1];
                median /= 2.0;
            }
            average /= static_cast<double>(iteration_count);
        }

        unsigned int iteration_count = 15;
        std::vector<unit_test> m_units;
};


