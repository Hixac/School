#include <print>
#include <vector>
#include <string>
#include <format>
#include <omp.h>

#include <test.hpp>

int chunk(int size) {
    int den = omp_get_max_threads() * 4;
    if (size < den)
        return 1;
    return size / den;
}

struct matrix {
    std::vector<double> m;
    int rows, columns;

    matrix() = default;

    matrix(int rows, int columns) : rows(rows), columns(columns) {
        m.resize(rows * columns);
        m.shrink_to_fit();
    }

    double &operator()(int row, int column) {
        return m[row + column * rows];
    }

    double &operator[](int idx) {
        return m[idx];
    }

    void print() {
        for (int i = 0; i < columns; ++i) {
            for (int j = i * rows; j < (i + 1) * rows; ++j)
                std::print("{} ", m[j]);
            std::println();
        }
    }
};

int main() {

    auto fill_mat = [](matrix &m, int rows, int columns) {
        for (int i = 0; i < rows * columns; ++i)
            m[i] = std::sin(static_cast<double>(i) / 2.5) + std::cos(static_cast<double>(i) + 1.0);
    };

    auto fill_mat_for = [](matrix &m, int rows, int columns) {
#pragma omp parallel
        {
            int size = rows * columns;
#pragma omp for schedule(static, chunk(size))
            for (int i = 0; i < size; ++i)
                m[i] = std::sin(static_cast<double>(i) / 2.5) + std::cos(static_cast<double>(i) + 1.0);
        }
    };

    auto fill_mat_sections = [](matrix &m, int rows, int columns) {
        int p = omp_get_max_threads();
        int size = rows * columns;
        int s_0 = 0;
        int s_1 = (1 * size) / p;
        int s_2 = (2 * size) / p;
        int s_3 = (3 * size) / p;
        int s_4 = (4 * size) / p;
#pragma omp parallel sections
        {
#pragma omp section
            {
                for (int i = s_0; i < s_1; ++i)
                    m[i] = std::sin(static_cast<double>(i) / 2.5) + std::cos(static_cast<double>(i) + 1.0);
            }
#pragma omp section
            {
                if (p > 1)
                    for (int i = s_1; i < s_2; ++i)
                        m[i] = std::sin(static_cast<double>(i) / 2.5) + std::cos(static_cast<double>(i) + 1.0);
            }
#pragma omp section
            {
                if (p > 2)
                    for (int i = s_2; i < s_3; ++i)
                        m[i] = std::sin(static_cast<double>(i) / 2.5) + std::cos(static_cast<double>(i) + 1.0);
            }
#pragma omp section
            {
                if (p > 3)
                    for (int i = s_3; i < s_4; ++i)
                        m[i] = std::sin(static_cast<double>(i) / 2.5) + std::cos(static_cast<double>(i) + 1.0);
            }
        }
    };

    auto mul_mat = [](matrix &m1, matrix &m2, matrix &out) {
        auto temp_m = matrix(m2.columns, m2.rows);

        for (int i = 0; i < m2.columns; ++i)
            for (int j = 0; j < m2.rows; ++j)
                temp_m(j, i) = m2(i, j);

        for (int i = 0; i < m1.rows; ++i)
            for (int j = 0; j < m2.columns; ++j) {
                double cnt = 0;
                for (int k = 0; k < m2.rows; ++k)
                    cnt += m1(i, k) * temp_m(j, k);
                out(i, j) = cnt;
            }
    };

    auto mul_mat_for = [](matrix &m1, matrix &m2, matrix &out) {
        auto temp_m = matrix(m2.columns, m2.rows);
#pragma omp parallel 
        {
#pragma omp for schedule(static, chunk(m2.columns * m2.rows))
            for (int i = 0; i < m2.columns; ++i)
                for (int j = 0; j < m2.rows; ++j)
                    temp_m(j, i) = m2(i, j);

#pragma omp for schedule(static, chunk(m2.columns * m1.rows))
            for (int i = 0; i < m1.rows; ++i)
                for (int j = 0; j < m2.columns; ++j) {
                    double cnt = 0;
                    for (int k = 0; k < m2.rows; ++k)
                        cnt += m1(i, k) * temp_m(j, k);
                    out(i, j) = cnt;
                }
        }
    };

    test matrix_tests;

    int rows = 1000, columns = 1000;
    matrix m1 = matrix(rows, columns), m2 = matrix(rows, columns), m3 = matrix(rows, columns);

    matrix_tests.add_function("Заполнение матрицы (последовательно)", fill_mat, 1, m1, rows, columns);

    matrix_tests.add_function("Заполнение матрицы (for)", fill_mat_for, 2, m1, rows, columns);
    matrix_tests.add_function("Заполнение матрицы (for)", fill_mat_for, 3, m1, rows, columns);
    matrix_tests.add_function("Заполнение матрицы (for)", fill_mat_for, 4, m1, rows, columns);

    matrix_tests.add_function("Заполнение матрицы (sections)", fill_mat_for, 2, m2, rows, columns);
    matrix_tests.add_function("Заполнение матрицы (sections)", fill_mat_for, 3, m2, rows, columns);
    matrix_tests.add_function("Заполнение матрицы (sections)", fill_mat_for, 4, m2, rows, columns);

    matrix_tests.add_function("Перемножение матрциы (последовательно)", mul_mat, 1, m1, m2, m3);

    matrix_tests.add_function("Перемножение матрциы (for)", mul_mat_for, 2, m1, m2, m3);
    matrix_tests.add_function("Перемножение матрциы (for)", mul_mat_for, 3, m1, m2, m3);
    matrix_tests.add_function("Перемножение матрциы (for)", mul_mat_for, 4, m1, m2, m3);

    matrix_tests.do_tests();

    // omp_set_num_threads(2);
    // mul_mat_for(m1, m2, m3);

    for (auto &unit : matrix_tests.get_units()) {
        std::println("name = {}, time = {}", unit.name, unit.time);
    }

    return 0;
}
