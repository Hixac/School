#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <omp.h>
#include <fstream>
#include <algorithm>
#include <random>
#include <cmath>
#include <stack>
#include <iomanip>

using namespace std;

template<typename T>
void bubbleSortClassic(T* arr, int n);

template<typename T>
void oddEvenSortSequential(T* arr, int n);

template<typename T>
void oddEvenSortParallel(T* arr, int n);

template<typename T>
void shellSortSequential(T* arr, int n);

template<typename T>
void shellSortParallel(T* arr, int n);

template<typename T>
void quickSortSequential(T* arr, int low, int high);

template<typename T>
void quickSortParallel(T* arr, int low, int high, int depth = 0);

template<typename T>
int partition(T* arr, int low, int high);

void generateIntArray(int* arr, int n);
void generateDoubleArray(double* arr, int n);

template<typename T>
void copyArray(T* source, T* destination, int n);

double AvgTrustedIntervalMED(double*& times, int cnt);

double MeasureFTime(std::function<double()> func, int expCNT);

#pragma region TimeMeasure

// Функция сортировки пузырьком
void bubbleSort(double* mas, int cnt) {
    bool exchanges;
    do {
        exchanges = false;
        for (int i = 0; i < cnt - 1; i++) {
            if (mas[i] > mas[i + 1]) {
                double temp = mas[i];
                mas[i] = mas[i + 1];
                mas[i + 1] = temp;
                exchanges = true;
            }
        }
    } while (exchanges);
}

// Функция расчёта среднеарифметического значения в доверительном интервале
double AvgTrustedIntervalMED(double*& times, int cnt) {
    double avg = 0;
    for (int i = 0; i < cnt; i++) {
        avg += times[i];
    }
    avg /= cnt;

    bubbleSort(times, cnt);
    double med = times[cnt / 2];

    double sd = 0, newAVg = 0;
    int newCnt = 0;
    for (int i = 0; i < cnt; i++) {
        sd += (times[i] - avg) * (times[i] - avg);
    }
    sd /= (cnt - 1.0);
    sd = sqrt(sd);

    for (int i = 0; i < cnt; i++) {
        if (med - sd <= times[i] && times[i] <= med + sd) {
            newAVg += times[i];
            newCnt++;
        }
    }
    if (newCnt == 0) newCnt = 1;
    return newAVg / newCnt;
}

// Функция рассчета времени
double MeasureFTime(std::function<double()> func, int expCNT) {
    double* times = new double[expCNT];
    for (int i = 0; i < expCNT; i++) {
        times[i] = func();
        std::cout << "+";
    }
    std::cout << std::endl;
    double avg_time = AvgTrustedIntervalMED(times, expCNT);
    delete[] times;
    return avg_time;
}

#pragma endregion

#pragma region SortingFunctions

// Классическая пузырьковая сортировка
template<typename T>
void bubbleSortClassic(T* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Чет-нечетная сортировка (последовательная)
template<typename T>
void oddEvenSortSequential(T* arr, int n) {
    bool isSorted = false;
    while (!isSorted) {
        isSorted = true;
        for (int i = 0; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
        for (int i = 1; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
    }
}

// Чет-нечетная сортировка (параллельная)
template<typename T>
void oddEvenSortParallel(T* arr, int n) {
    bool isSorted = false;
    while (!isSorted) {
        isSorted = true;
#pragma omp parallel for reduction(&&:isSorted)
        for (int i = 0; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
#pragma omp parallel for reduction(&&:isSorted)
        for (int i = 1; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
    }
}

// Сортировка Шелла (последовательная)
template<typename T>
void shellSortSequential(T* arr, int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            T temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

// Сортировка Шелла (параллельная)
template<typename T>
void shellSortParallel(T* arr, int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
#pragma omp parallel for
        for (int i = gap; i < n; i++) {
            T temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

// Функция для работы быстрой сортировки
template<typename T>
int partition(T* arr, int low, int high) {
    T pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// Быстрая сортировка (последовательная)
template<typename T>
void quickSortSequential(T* arr, int low, int high) {
    stack<pair<int, int>> stack;
    stack.push(make_pair(low, high));

    while (!stack.empty()) {
        low = stack.top().first;
        high = stack.top().second;
        stack.pop();

        if (low < high) {
            int pi = partition(arr, low, high);

            if (pi - 1 > low) {
                stack.push(make_pair(low, pi - 1));
            }
            if (pi + 1 < high) {
                stack.push(make_pair(pi + 1, high));
            }
        }
    }
}

// Быстрая сортировка (параллельная)
template<typename T>
void quickSortParallel(T* arr, int low, int high, int depth) {
    if (low < high) {
        int pi = partition(arr, low, high);
        if (depth < 2) {
#pragma omp parallel shared(arr)
            {
#pragma omp task
                quickSortParallel(arr, low, pi - 1, depth + 1);
#pragma omp task
                quickSortParallel(arr, pi + 1, high, depth + 1);
#pragma omp taskwait
            }
        }
        else {
            quickSortSequential(arr, low, pi - 1);
            quickSortSequential(arr, pi + 1, high);
        }
    }
}

#pragma endregion

#pragma region ArrayFunctions

// Генерация int массива
void generateIntArray(int* arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = sin(i / 5) + cos(i * 2);
    }
}

// Генерация массива double
void generateDoubleArray(double* arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = sin(i / 5.0) + cos(i * 2.0);
    }
}

// Функция копирования массива
template<typename T>
void copyArray(T* source, T* destination, int n) {
    for (int i = 0; i < n; i++) {
        destination[i] = source[i];
    }
}

#pragma endregion

// Функция для проведения экспериментального исследования
void test_sorting_functions(double** experiments_time_int, double** experiments_time_double,
    vector<string> function_names, int size, int iterations) {

    int* original_int = new int[size];
    int* test_int = new int[size];
    double* original_double = new double[size];
    double* test_double = new double[size];

    generateIntArray(original_int, size);
    generateDoubleArray(original_double, size);

    // Вектор функций для int массивов
    vector<function<double()>> intFunctions = {
        [&]() {
            copyArray(original_int, test_int, size);
            double t_start = omp_get_wtime();
            bubbleSortClassic(test_int, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_int, test_int, size);
            double t_start = omp_get_wtime();
            oddEvenSortSequential(test_int, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_int, test_int, size);
            double t_start = omp_get_wtime();
            oddEvenSortParallel(test_int, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_int, test_int, size);
            double t_start = omp_get_wtime();
            shellSortSequential(test_int, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_int, test_int, size);
            double t_start = omp_get_wtime();
            shellSortParallel(test_int, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_int, test_int, size);
            double t_start = omp_get_wtime();
            quickSortSequential(test_int, 0, size - 1);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_int, test_int, size);
            double t_start = omp_get_wtime();
            quickSortParallel(test_int, 0, size - 1, 0);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        }
    };

    // Вектор функций для double массивов
    vector<function<double()>> doubleFunctions = {
        [&]() {
            copyArray(original_double, test_double, size);
            double t_start = omp_get_wtime();
            bubbleSortClassic(test_double, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_double, test_double, size);
            double t_start = omp_get_wtime();
            oddEvenSortSequential(test_double, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_double, test_double, size);
            double t_start = omp_get_wtime();
            oddEvenSortParallel(test_double, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_double, test_double, size);
            double t_start = omp_get_wtime();
            shellSortSequential(test_double, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_double, test_double, size);
            double t_start = omp_get_wtime();
            shellSortParallel(test_double, size);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_double, test_double, size);
            double t_start = omp_get_wtime();
            quickSortSequential(test_double, 0, size - 1);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        },
        [&]() {
            copyArray(original_double, test_double, size);
            double t_start = omp_get_wtime();
            quickSortParallel(test_double, 0, size - 1, 0);
            double t_end = omp_get_wtime();
            return (t_end - t_start);
        }
    };

    size_t funct_cnt = min(intFunctions.size(), function_names.size());

    for (int threads = 2; threads <= 4; threads++) {
        omp_set_num_threads(threads);
        cout << "\nКоличество потоков: " << threads << endl;

        // Тестирование int массивов
        cout << "Целочисленные массивы:" << endl;
        for (int f = 0; f < funct_cnt; f++) {
            if (f == 0 || f == 1 || f == 3 || f == 5) {
                if (threads == 2) { // Последовательный алгоритм
                    cout << function_names[f] << " (int)\t";
                    experiments_time_int[f][0] = MeasureFTime(intFunctions[f], iterations);
                }
            }
            else {
                cout << function_names[f] << " (int) [потоков:" << threads << "]\t";
                experiments_time_int[f][threads - 2] = MeasureFTime(intFunctions[f], iterations);
            }
        }

        // Тестирование double массивов
        cout << "Double массивы:" << endl;
        for (int f = 0; f < funct_cnt; f++) {
            if (f == 0 || f == 1 || f == 3 || f == 5) {
                if (threads == 2) { // Последовательный алгоритм
                    cout << function_names[f] << " (double)\t";
                    experiments_time_double[f][0] = MeasureFTime(doubleFunctions[f], iterations);
                }
            }
            else {
                // Параллельные алгоритмы
                cout << function_names[f] << " (double) [потоков:" << threads << "]\t";
                experiments_time_double[f][threads - 2] = MeasureFTime(doubleFunctions[f], iterations);
            }
        }
    }

    delete[] original_int;
    delete[] test_int;
    delete[] original_double;
    delete[] test_double;
}

int main() {
    int iteration_cnt = 3;

    vector<string> function_names = {
        "Классический алгоритм пузырька",
        "Чет-нечетной перестановки (посл.)",
        "Чет-нечет перестановки (парал.)",
        "Шелла (посл.)",
        "Шелла (парал.)",
        "Быстрой сортировки (посл.)",
        "Быстрой сортировки (парал.)"
    };

    int nd_count = 3;
    int funct_cnt = 7;

    double*** experiments_time_int = new double** [nd_count];
    double*** experiments_time_double = new double** [nd_count];

    for (int nd = 0; nd < nd_count; nd++) {
        experiments_time_int[nd] = new double* [funct_cnt];
        experiments_time_double[nd] = new double* [funct_cnt];
        for (int f = 0; f < funct_cnt; f++) {
            experiments_time_int[nd][f] = new double[3]();
            experiments_time_double[nd][f] = new double[3]();
        }
    }

    int data_sizes[] = { 20000, 30000, 50000 };

    for (int nd = 0; nd < nd_count; nd++) {
        int nd_size = data_sizes[nd];
        cout << "\n----------------------------------------" << endl;
        cout << "ОБРАБОТКА НАБОРА ДАННЫХ: " << nd_size << " ЭЛЕМЕНТОВ" << endl;
        cout << "----------------------------------------" << endl;
        test_sorting_functions(experiments_time_int[nd], experiments_time_double[nd],
            function_names, nd_size, iteration_cnt);
    }

    ofstream fout("results.txt");

    fout << fixed << setprecision(6);
    cout << fixed << setprecision(6);

    cout << "\n\nРЕЗУЛЬТАТЫ ЭКСПЕРИМЕНТОВ" << endl;
    fout << "РЕЗУЛЬТАТЫ ЭКСПЕРИМЕНТОВ" << endl;

    for (int nd = 0; nd < nd_count; nd++) {
        int nd_size = data_sizes[nd];

        cout << "\n" << endl;
        cout << "РАЗМЕР МАССИВА: " << nd_size << " элементов" << endl;
        cout << "" << endl;

        cout << "\nINT" << endl;
        cout << function_names[0] << "\t" << experiments_time_int[nd][0][0] << " с" << endl;
        cout << function_names[1] << "\t" << experiments_time_int[nd][1][0] << " с" << endl;

        for (int th = 2; th <= 4; th++) {
            cout << function_names[2] << " (потоков:" << th << ")\t" << experiments_time_int[nd][2][th - 2] << " с" << endl;
        }

        cout << function_names[3] << "\t" << experiments_time_int[nd][3][0] << " с" << endl;

        for (int th = 2; th <= 4; th++) {
            cout << function_names[4] << " (потоков:" << th << ")\t" << experiments_time_int[nd][4][th - 2] << " с" << endl;
        }

        cout << function_names[5] << "\t" << experiments_time_int[nd][5][0] << " с" << endl;

        for (int th = 2; th <= 4; th++) {
            cout << function_names[6] << " (потоков:" << th << ")\t" << experiments_time_int[nd][6][th - 2] << " с" << endl;
        }

        cout << "\nDOUBLE" << endl;
        cout << function_names[0] << "\t" << experiments_time_double[nd][0][0] << " с" << endl;
        cout << function_names[1] << "\t" << experiments_time_double[nd][1][0] << " с" << endl;

        for (int th = 2; th <= 4; th++) {
            cout << function_names[2] << " (потоков:" << th << ")\t" << experiments_time_double[nd][2][th - 2] << " с" << endl;
        }

        cout << function_names[3] << "\t" << experiments_time_double[nd][3][0] << " с" << endl;

        for (int th = 2; th <= 4; th++) {
            cout << function_names[4] << " (потоков:" << th << ")\t" << experiments_time_double[nd][4][th - 2] << " с" << endl;
        }

        cout << function_names[5] << "\t" << experiments_time_double[nd][5][0] << " с" << endl;

        for (int th = 2; th <= 4; th++) {
            cout << function_names[6] << " (потоков:" << th << ")\t" << experiments_time_double[nd][6][th - 2] << " с" << endl;
        }

        fout << "\n" << nd_size << endl;
        fout << "int" << endl;
        fout << experiments_time_int[nd][0][0] << endl;
        fout << experiments_time_int[nd][1][0] << endl;

        for (int th = 2; th <= 4; th++) {
            fout << experiments_time_int[nd][2][th - 2] << endl;
        }

        fout << experiments_time_int[nd][3][0] << endl;

        for (int th = 2; th <= 4; th++) {
            fout << experiments_time_int[nd][4][th - 2] << endl;
        }

        fout << experiments_time_int[nd][5][0] << endl;

        for (int th = 2; th <= 4; th++) {
            fout << experiments_time_int[nd][6][th - 2] << endl;
        }

        fout << "double" << endl;
        fout << experiments_time_double[nd][0][0] << endl;
        fout << experiments_time_double[nd][1][0] << endl;

        for (int th = 2; th <= 4; th++) {
            fout << experiments_time_double[nd][2][th - 2] << endl;
        }

        fout << experiments_time_double[nd][3][0] << endl;

        for (int th = 2; th <= 4; th++) {
            fout << experiments_time_double[nd][4][th - 2] << endl;
        }

        fout << experiments_time_double[nd][5][0] << endl;

        for (int th = 2; th <= 4; th++) {
            fout << experiments_time_double[nd][6][th - 2] << endl;
        }
    }

    fout.close();

    for (int nd = 0; nd < nd_count; nd++) {
        for (int f = 0; f < funct_cnt; f++) {
            delete[] experiments_time_int[nd][f];
            delete[] experiments_time_double[nd][f];
        }
        delete[] experiments_time_int[nd];
        delete[] experiments_time_double[nd];
    }
    delete[] experiments_time_int;
    delete[] experiments_time_double;

    cout << "\nРезультаты сохранены в файл 'sorting_results.txt'" << endl;

    return 0;
}
