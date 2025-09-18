
#include <iostream>
#include <vector>
#include <string.h>
#include <functional>
#include <math.h>
//  Модуль OpenMP
#include <omp.h>

////////////////////////////////////////////////////////////////
// Определение функций работы с данными (обработка массивов)

////////////////////////////////////////////////////////////////
// Функция рассчета среднеарифметического значения в доверительном интервале
double AvgTrustedIntervalMED(double*& times, int cnt);
// Функция для замера времени Функции работы с данными 
double MeasureFTime(std::function<double()> func, int expCNT);

////////////////////////////////////////////////////////////////
// Функции заполнения массивов последовательный и параллельный варианты
double Fillarr_posl(double* arr1, double* arr2, int size);
double Fillarr_par_for(double* arr1, double* arr2, int size); 
// Функции Сложения массивов последовательный и два параллельных варианта
double SumArrays_posl(double* arr_in1, double* arr_in2, double* arr_res, int size);
double SumArrays_par_for(double* arr_in1, double* arr_in2, double* arr_res, int size);
double SumArrays_par_section(double* arr_in1, double* arr_in2, double* arr_res, int size);
// Функции СлПолучение суммы элементов массива последовательный и два параллельных варианта
double ElArrSum_posl(double* arr, int size, double& sum);
double ElArrSum_par_for_reduction(double* arr, int size, double& sum);
double ElArrSum_par_for_crit_sec(double* arr, int size, double& sum);


////////////////////////////////////////////////////////////////
// Механизмы для рассчета времени в доверительном интервале на основе медианного значения
#pragma region TimeMeasure

////////////////////////////////////////////////////////////////
// Функция сортировки массива(сортировка пузырьком)
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

////////////////////////////////////////////////////////////////
// Функция расчёта среднеарифметического значения в доверительном интервале
// на основе медианного значения
// times - динамический одномерный массив с временем выполнения алгоритма
// cnt - количество элементов в массиве (выполненных экспериментов)
double AvgTrustedIntervalMED(double*& times, int cnt)
{
    // вычисление среднеарифметического значения
    double avg = 0;
    for (int i = 0; i < cnt; i++)
    {
        // подсчет в переменную суммы
        avg += times[i];
    }
    // деление на количество
    avg /= cnt;
    // сортировка массива для получение медианы
    bubbleSort(times, cnt);
    // получение медианы
    double med = times[cnt / 2];
    // подсчет стандартного отклонения
    double sd = 0, newAVg = 0;
    int newCnt = 0;
    for (int i = 0; i < cnt; i++)
    {
        sd += (times[i] - avg) * (times[i] - avg);
    }
    sd /= (cnt - 1.0);
    sd = std::sqrt(sd);
    // вычисление нового среднего значения в доверительном интервале
    // с использованием медианного значения
    for (int i = 0; i < cnt; i++)
    {
        if (med - sd <= times[i] && times[i] <= med + sd)
        {
            newAVg += times[i];
            newCnt++;
        }
    }
    if (newCnt == 0) newCnt = 1;
    return newAVg / newCnt;
}

////////////////////////////////////////////////////////////////
// Функция выполнения рассчета времени (проведения замера в доверительном интервале)
// func - конкретная функция для исследования
// expCNT - количество итераций для выполнения иследования
double MeasureFTime(std::function<double()> func, int expCNT)
{
    double* times = new double[expCNT];
    for (int i = 0; i < expCNT; i++)
    {
        // Замер времени выполнения функции
        times[i] = func();
        // Отображение на экране прогресса
        std::cout << "+";
    }
    std::cout << std::endl;
    // Определение значение ссреднего времени в доверительном интервале
    double avg_time = AvgTrustedIntervalMED(times, expCNT);
    delete[] times;
    return avg_time;
}

#pragma endregion


// Реализация функций по работе с массивами
#pragma region ArrayWorkFunction
double Fillarr_posl(double* arr1, double* arr2, int size)
{
    double t_start = omp_get_wtime();
    for (int i = 0; i < size; i++)
    {
        arr1[i] = (std::cos((i - 0.5) / 2.0));
        arr2[i] = (std::sin(i * 2 / 3.0));
    }
    double t_end = omp_get_wtime();
    return (t_end - t_start) * 1000;
}

double Fillarr_par_for(double* arr1, double* arr2, int size)
{
    double t_start = omp_get_wtime();
#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        arr1[i] = (std::cos((i - 0.5) / 2.0));
        arr2[i] = (std::sin(i * 2 / 3.0));
    }
    double t_end = omp_get_wtime();
    return (t_end - t_start) * 1000;
}

double SumArrays_posl(double* arr_in1, double* arr_in2, double* arr_res, int size)
{
    double t_start = omp_get_wtime();
    for (int i = 0; i < size; i++)
        arr_res[i] = arr_in1[i] + arr_in2[i];
    double t_end = omp_get_wtime();
    return (t_end - t_start) * 1000;
}

double SumArrays_par_for(double* arr_in1, double* arr_in2, double* arr_res, int size)
{
    double t_start = omp_get_wtime();
#pragma omp parallel for
    for (int i = 0; i < size; i++)
        arr_res[i] = arr_in1[i] + arr_in2[i];
    double t_end = omp_get_wtime();
    return (t_end - t_start) * 1000;
}

double SumArrays_par_section(double* arr_in1, double* arr_in2, double* arr_res, int size)
{
    double t_start = omp_get_wtime();
#pragma omp parallel sections
    {
#pragma omp section
        {
            for (int i = 0; i < size; i++)
                arr_res[i] = arr_in1[i] + arr_in2[i];
        }
    }
    double t_end = omp_get_wtime();
    return (t_end - t_start) * 1000;
}

double ElArrSum_posl(double* arr, int size, double& sum)
{
    double temp_summ = 0;
    double t_start = omp_get_wtime();
    for (int i = 0; i < size; i++)
        temp_summ += arr[i];
    sum = temp_summ;
    double t_end = omp_get_wtime();
    return (t_end - t_start) * 1000;
}

double ElArrSum_par_for_reduction(double* arr, int size, double& sum)
{
    double temp_summ = 0;
    double t_start = omp_get_wtime();
#pragma omp parallel for reduction(+:temp_summ)
    for (int i = 0; i < size; i++)
        temp_summ += arr[i];
    sum = temp_summ;
    double t_end = omp_get_wtime();
    return (t_end - t_start) * 1000;
}

double ElArrSum_par_for_crit_sec(double* arr, int size, double& sum)
{
    double t_start = omp_get_wtime();
    double temp_summ = 0;
#pragma omp parallel for
        for (int i = 0; i < size; i++) {
#pragma omp critical 
            {
                temp_summ += arr[i];
            }
        }
    sum = temp_summ;
    double t_end = omp_get_wtime();
    return (t_end - t_start) * 1000;
}

#pragma endregion

////////////////////////////////////////////////////////////////
// Функция для проведения экспериментального исследования по набору данных
// experiments_time - двумерный массив experiments_time[функции][замер_времени]  
// function_names - вектор текстовых обозначений исследуемых функций
// size - размер набора данных
// iterations - количество итераций для выполнения иследования
void test_functions(double** experiments_time, std::vector<std::string> function_names, int size, int iterations)
{
    // Выделение памяти под массивы с которыме будет выполняться работа
    double* mas1, * mas2, * mas3;
    mas1 = new double[size];
    mas2 = new double[size];
    mas3 = new double[size];

    // Временная переменная для хранения суммы элементов массива
    double tsum = 0;
    // вектор содржащий функции для исследования
    std::vector<std::function<double()>> Functions = {
        [&]() { return Fillarr_posl(mas1, mas2, size); },
        [&]() { return Fillarr_par_for(mas1, mas2, size); },
        [&]() { return SumArrays_posl(mas1, mas2, mas3, size); },
        [&]() { return SumArrays_par_for(mas1, mas2, mas3, size); },
        [&]() { return SumArrays_par_section(mas1, mas2, mas3, size); },
        [&]() { return ElArrSum_posl(mas3, size, tsum); },
        [&]() { return ElArrSum_par_for_reduction(mas3, size, tsum); },
        [&]() { return ElArrSum_par_for_crit_sec(mas3, size, tsum); }
    };

    // количество исследуемых функций
    size_t funct_cnt = std::min(Functions.size(), function_names.size());

    // цикл перебора потоков
    for (int threads = 1; threads <= 4; threads++)
    {
        // установка количества потоков
        omp_set_num_threads(threads);
        // Вывод на экран сообщения о том сколько потоков создано
        // используется функци omp_get_max_threads() поскольку запрос из 
        // последовательного участка кода
        std::cout << "\nКоличество потоков:" << omp_get_max_threads() << std::endl;

        // цикл перебора исследуемых функций
        for (int f = 0; f < funct_cnt; f++)
        {
            // Проверка количества потоков
            if (threads == 1)
            {
                // В случае только одного потока будут запускаться функции с индексами 0 2 5
                if (f == 0 || f == 2 || f == 5)
                {
                    std::cout << function_names[f] << "\t";
                    experiments_time[f][0] = MeasureFTime(Functions[f], iterations);
                    experiments_time[f][1] = experiments_time[f][0];
                    experiments_time[f][2] = experiments_time[f][0];
                }
            }
            else
            {
                // Если количество потоков отличается от одного то запускать 
                // на исследования функции у которых индекс не равен 0 2 5
                if (f != 0 && f != 2 && f != 5)
                {
                    std::cout << function_names[f] << "\t";
                    experiments_time[f][threads - 2] = MeasureFTime(Functions[f], iterations);
                }
            }
        }
    }

    delete[] mas1;
    delete[] mas2;
    delete[] mas3;
}


int main()
{
    // задание количества итераций для исследования
    int iteration_cnt = 300;


    std::vector<std::string> function_names = { "Заполнение массивов (Посл.)", "Заполнение массивов (Пар. 2)",
        "Сумма массивов A и B (Посл)", "Сумма массивов A и B (Пар. For)","Сумма массивов A и B (Пар. Section)"
            ,"Сумма элементов массива С (Посл.)","Сумма элементов массива С (Пар. Редуктор)","Сумма элементов массива С (Пар. Crit Sect)"
            , "Предполагаемая иная функция для которой не указана реализация в тесте (Functions)" };

    // определение параметров экспериментального исследования
    int nd_count = 4; // количество наборов данных
    int funct_cnt = 8; // количество исследуемых функций
    double*** experiments_time = new double** [nd_count];
    for (int nd = 0; nd < nd_count; nd++)
    {
        experiments_time[nd] = new double* [funct_cnt];
        for (int f = 0; f < funct_cnt; f++)
        {
            experiments_time[nd][f] = new double[3];
        }
    }

    // Определение параметров для наборов данных
    int nd_base_size = 100000;	// Начальный размер набора данных
    int nd_step = 70000; 		// Шаг изменения набора данных

    // Организация цикла перебора по набору данных
    for (int nd = 0; nd < 4; nd++)
    {
        // Определения размера набора данных
        int nd_size = nd_base_size + nd_step * nd;
        std::cout << std::endl << "======================================================================" << std::endl;
        std::cout << "Обработка набора данных (НД): " << nd_size << std::endl;
        // Запуск функции сбора данных о времени работы функций с учетом доверительного интервала
        test_functions(&experiments_time[nd][0], function_names, nd_size, iteration_cnt);
    }

    // Вывод на экран результатов экспериментального исследование
    std::cout << "Результаты экспериментов:";
    // Перебор по набору данных
    for (int nd = 0; nd < 4; nd++)
    {
        int nd_size = nd_base_size + nd_step * nd;
        // Вывод сведений о размере набора данных
        std::cout << std::endl << "Размер НД:" << nd_size << std::endl;
        // Перебор функций для которых был произведен замер времени
        for (int f = 0; f < funct_cnt; f++)
        {
            // Если это последовательная функция (индек 0 2 или 5)
            if (f == 0 || f == 2 || f == 5)
            {
                // просто выводится имя функции и време
                std::cout << function_names[f] << "\t" << experiments_time[nd][f][0] << std::endl;
            }
            else
            {
                // в противном случае организуется цикл с выводом времени для каждого из исследуемых потоков (2 3 4)
                for (int th = 0; th < 3; th++)
                    std::cout << function_names[f] << " [потоков:" << (th + 2) << "]\t" << experiments_time[nd][f][th] << std::endl;
            }
        }
    }

    return 0;
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
