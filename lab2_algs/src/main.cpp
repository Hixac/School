#include <iostream>
#include <chrono>

#include <fgen.hpp>
#include <merge_sort.hpp>

enum class Error {
	Clean = 0, WrongAnswer, WrongRange, WrongSize, Error
};

Error PlayOne() {
	size_t size;
	std::cout << "Размер числовой последовательности (число > 0) ";
	std::cin >> size;

	if (size <= 0) {
		return Error::WrongSize;
	}
	
	int from, to;
	std::cout << "Диапазон числовой последовательности (от, до) ";
	std::cin >> from >> to;
	
	if (from >= to) {
		return Error::WrongRange;
	}

	int res;
	std::cout << "Выберите функцию внешней сортировки [1. прямое слияние, 2. многофазная сорт] (1/2) ";
	std::cin >> res;

	if (res != 1 && res != 2) {
		return Error::WrongAnswer;
	}

	const std::filesystem::path file_name = "output.txt";
	My::generate_file(file_name, from, to, size);

	My::Counter::Get().SetBlockSize(100); // default val
	
	if (res == 1) {
		My::external_merge_sort(file_name, file_name);
	} else {
		My::external_multiway_sort(file_name, file_name);
	}

	std::cout << "Сравнений " << My::Counter::Get().GetComp() << "; Перестановок " << My::Counter::Get().GetMov() << std::endl;

	return Error::Clean;
}

Error PlayTwo() {
	int res;
	std::cout << "Выберите способ формирования элементов [1. случайные значения, 2. упорядоченная последовательность, 3. сортированные значения в обратном порядке] (1/2/3) ";
	std::cin >> res;

	if (res != 1 && res != 2 && res != 3) {
		return Error::WrongAnswer;
	}

	size_t size;
	std::cout << "Размер числовой последовательности (число > 0) ";
	std::cin >> size;

	if (size <= 0) {
		return Error::WrongSize;
	}

	size_t offset;
	std::cout << "Шаг изменения размера числовой последовательности (число > 0) ";
	std::cin >> offset;

	if (offset <= 0) {
		return Error::WrongSize;
	}

    int from, to;
	std::cout << "Диапазон числовой последовательности (от, до) ";
	std::cin >> from >> to;
	
	if (from >= to) {
		return Error::WrongRange;
	}

	int sort;
	std::cout << "Выберите функцию внешней сортировки [1. прямое слияние, 2. многофазная сорт] (1/2) ";
	std::cin >> sort;

	if (sort != 1 && sort != 2) {
		return Error::WrongAnswer;
	}
	
	const std::filesystem::path file_name = "output.txt";

	std::ofstream statistic_file("stats.txt", std::ios::trunc);
	for (size_t i = 0; i < size; i += offset) {
		std::chrono::steady_clock::time_point begin;
		std::chrono::steady_clock::time_point end;
		
		if (res == 1) {
			My::generate_file(file_name, from, to, i);
		} else if (res == 2) {
			My::generate_sort_file(file_name, i);
		} else {
			My::generate_versa_sort_file(file_name, i);
		}

		if (res == 1) {
			begin = std::chrono::steady_clock::now();
			My::external_merge_sort(file_name, file_name);
			end = std::chrono::steady_clock::now();
		} else {
			begin = std::chrono::steady_clock::now();
			My::external_multiway_sort(file_name, file_name);
			end = std::chrono::steady_clock::now();
		}

		statistic_file << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1000 << '\n';
	}

	return Error::Clean;
}

Error interface() {

	int res;
	std::cout << "Введите режим программы (1/2) ";
	std::cin >> res;

	if (res != 1 && res != 2) {
		return Error::WrongAnswer;
	}

	if (res == 1) {
		return PlayOne();
	} else {
		return PlayTwo();
	}
}

int main() {
	while (true) {
		switch (interface()) {
		case Error::Clean: return 0;
		case Error::Error: return 1;
	    default: break;
		}
	}
	
	return 2;
}
