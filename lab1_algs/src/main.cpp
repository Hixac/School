#include <iostream>
#include <random>
#include <chrono>
#include <cassert>
#include <fstream>

void shell_sort(int* arr, int n, int& coms, int& chns)
{
	int com = 0, chn = 0;
    for (int gap = n/2; gap > 0; gap /= 2)
    {
        for (int i = gap; i < n; i += 1)
        {
            int temp = arr[i];

            int j;            
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
				com++; chn++;
			}
            
            arr[j] = temp;
			chn++;
        }
    }
	coms = com;
	chns = chn;
}

void bubble_sort(int* arr, int size, int& coms, int& chns)
{
	int com = 0, chn = 0;
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = (size - 1); j > i; j--)
        {
            if (arr[j - 1] > arr[j])
            {
                int temp = arr[j - 1];
                arr[j - 1] = arr[j];
                arr[j] = temp;

				com++; chn += 2;
            }
        }
    }
	coms = com;
	chns = chn;
}

void print_arr(int arr[], int n)
{
    for (int i=0; i<n; i++)
		std::cout << arr[i] << " ";
	std::cout << std::endl;
}

void input_in_file(int* arr, int size, int dtime)
{
	std::ofstream f("arr.txt", std::ios::app);

	//for (int i = 0; i < size; ++i) {
	//	f << arr[i] << ' ';
	//}
	//f << '\n';
	f << dtime << " миллисекунд\n";
	f.close();
}

int* init_arr(size_t size)
{
    size_t st;
	size_t en;
	std::cout << "Диапазон значений массива: ";
	std::cin >> st;
	std::cin >> en;

	if (st > en) {
		assert(false && "Неверный диапазон значений массива (ОТ > ДО)!");
	}

	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(st, en);

	int* arr = new int[size];
	for (int i = 0; i < size; ++i) {
		arr[i] = dist6(rng);
	}

	return arr;
}

int* init_arr_by_hand(int size)
{
	std::cout << "Инициализация массива ручно: ";
	
	int* arr = new int[size];
	for (int i = 0; i < size; ++i) {
		std::cin >> arr[i];
	}

	return arr;
}

void regime_two()
{
	size_t size;
	std::cout << "Задайте конечный размер массива: ";
	std::cin >> size;
	if (size <= 0) {
		assert(false && "Неверный конечный размер массива (< 0)!");
	}

	size_t offset;
	std::cout << "Укажите шаг измерения размера массива (> 0) ";
	std::cin >> offset;
	if (offset < 0) {
		assert(false && "Неверно выбран шаг!");
	}

	size_t st;
	size_t en;
	std::cout << "Диапазон значений массива: ";
	std::cin >> st >> en;

	if (st > en) {
		assert(false && "Неверный диапазон значений массива (ОТ > ДО)!");
	}
	
	int type;
	std::cout << "Способ инициализации массива (1. ручно, 2. случайным образом) ";
	std::cin >> type;
	if (type != 1 && type != 2) {
		assert(false && "Неверный выбор инициализации массива!");
	}

	int chs;
	std::cout << "Метод сортировки (1. пузырьковый, 2. шеллом) ";
	std::cin >> chs;

	if (chs != 1 && chs != 2) {
		assert(false && "Неверный выбор метода сортировки (Выбран не 1 или 2)!");
	}

	int kind;
	std::cout << "Тип массива: массив со случайными значениями,\nотсортированный упорядоченный массив,\nотсортированный обратный массив\n(1/2/3) ";
	std::cin >> kind;
	
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(st, en);
	
	size_t real_size = offset;
	for (int i = 0; i < size/offset; ++i) {
		int* arr;
		if (type == 1) {
			arr = init_arr_by_hand(real_size);
		} else {
		    arr = new int[real_size];

			if (kind == 2) {
				for (int j = 0; j < real_size; ++j) {
					arr[j] = j;
				}
			} else if (kind == 3) {
				for (int j = real_size - 1; j > -1; --j) {
					arr[j] = j;
				}
			} else {
				for (int j = 0; j < real_size; ++j) {
					arr[j] = dist6(rng);
				}
			}
		}

		std::chrono::steady_clock::time_point begin;
		std::chrono::steady_clock::time_point end;
	
		int com, chn;
		if (chs == 1) {
			begin = std::chrono::steady_clock::now();
			bubble_sort(arr, real_size, com, chn);
			end = std::chrono::steady_clock::now();
		} else {
			begin = std::chrono::steady_clock::now();
			shell_sort(arr, real_size, com, chn);
			end = std::chrono::steady_clock::now();
		}
	
		input_in_file(arr, real_size, std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1000);
		real_size += offset;
		
		delete[] arr;
    }
}

void regime_one()
{
	size_t size;
	std::cout << "Задайте размер массива: ";
	std::cin >> size;
	if (size <= 0) {
		assert(false && "Неверный размер массива (< 0)!");
	}
	
	int* arr = init_arr(size);

	int chs;
	std::cout << "Метод сортировки (1. пузырьковый, 2. шеллом) ";
	std::cin >> chs;

	if (chs != 1 && chs != 2) {
		assert(false && "Неверный выбор метода сортировки (Выбран не 1 или 2)!");
	}

	std::cout << "Массив до сортировки: \n";
	print_arr(arr, size);
	
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	
	int com, chn;
	if (chs == 1) {
		begin = std::chrono::steady_clock::now();
		bubble_sort(arr, size, com, chn);
		end = std::chrono::steady_clock::now();
	} else {
		begin = std::chrono::steady_clock::now();
		shell_sort(arr, size, com, chn);
		end = std::chrono::steady_clock::now();
	}

	std::cout << "После: \n";
	print_arr(arr, size);
	
	input_in_file(arr, size, std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count());
	std::cout << "Было сделано " << com << " сравнений и " << chn << " перестановок" << std::endl;
}

int main()
{
	int reg;
	std::cout << "Выберите режим программы (1/2/вых) ";
	std::cin >> reg;

	if (reg == 1) {
		regime_one();
	} else if (reg == 2) {
		regime_two();
	} else {
		std::cout << "Выходим..." << std::endl;
	}
	
    return 0;
}

