#include <iostream>

#include <t3dpoint.hpp>
#include <t2dpoint.hpp>
#include <matrix_stream.hpp>

int main()
{
	std::string buf;
	std::cout << "Заполнить элемент массива 1) ручно или 2) случайным значением? (1/2) ";
	std::cin >> buf;
	
	Math::TPointMatrix<Math::T3DPoint> pts;
	pts.GenerateRandomValues();
	if (buf == "1") {
	    size_t exindex;
		size_t inindex;
		
		std::cout << "Введите индекс (от 0 до 8) ";
		std::cin >> exindex;
		
		std::cout << "Введите индекс (от 0 до 2) ";
		std::cin >> inindex;
		
		std::cout << "Введите желаемое значение: ";
		std::cin >> pts[exindex][inindex];
	} else if (buf == "2") {
		size_t exindex;
		size_t inindex;
		
		std::cout << "Введите индекс (от 0 до 8) ";
		std::cin >> exindex;
		
		std::cout << "Введите индекс (от 0 до 2) ";
		std::cin >> inindex;

	    pts.SetRandomValue(exindex, inindex);
	} else {
		return 0;
	}

	std::cout << "Наиболее удалённые от среднеарифмитического значение " << pts.GetFarthest() << '\n';
	auto sums = pts.CountSum();
	std::cout << "Сумма положительных и отрицательных элементов в нечётных столбцах " << sums.sum_of_pos << ' ' << sums.sum_of_neg;
	
	return 0;
}
