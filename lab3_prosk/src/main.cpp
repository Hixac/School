#include <iostream>

#include <tdpoint.hpp>
#include <matrix_stream.hpp>

int main(void)
{
	std::string buf;
	std::cout << "Заполнить массив 1) ручно или 2) случайными значениями? (1/2) ";
	std::cin >> buf;
	
	Math::TPointMatrix<Math::T3DPoint, 10, 3> pts;
	pts.GenerateRandomValues();
	if (buf == "1") {
	    size_t exindex;
		size_t inindex;
		
		std::cout << "Введите индекс (от 0 до 9) ";
		std::cin >> exindex;
		
		std::cout << "Введите индекс (от 0 до 2) ";
		std::cin >> inindex;
		
		std::cout << "Введите желаемое значение: ";
		std::cin >> pts[exindex][inindex];
	} else if (buf == "2") {
	    pts.SetRandomValue();
	} else {
		return 0;
	}
	
	return 0;
}
