#pragma once

#include <iostream>
#include <fstream>

#include <tdpoint.hpp>
#include <matrix.hpp>

namespace Math {

	template<size_t SE>
	static std::istream& operator>>(std::istream& in, T2DPoint<SE> m)
	{
		for (int i = 0; i < SE; ++i) {
			std::array<float, 2> arr = {};
			for (int j = 0; j < 2; ++j) {
				in >> arr[j];
			}
			m[i] = arr;
		}
	}

	template<size_t SE>
	static std::ostream& operator<<(std::ostream& out, T2DPoint<SE> m)
	{
		for (int i = 0; i < SE; ++i) {
			for (int j = 0; j < 2; ++j) {
				out << m[i][j] << ' ';
			}
			out << '\n';
		}
	}

	
	template<size_t SE>
	static std::istream& operator>>(std::istream& in, T3DPoint<SE> m)
	{
		for (int i = 0; i < SE; ++i) {
			std::array<float, 3> arr = {};
			for (int j = 0; j < 3; ++j) {
				in >> arr[j];
			}
			m[i] = arr;
		}
	}

	template<size_t SE>
	static std::ostream& operator<<(std::ostream& out, T3DPoint<SE> m)
	{
		for (int i = 0; i < SE; ++i) {
			for (int j = 0; j < 3; ++j) {
				out << m[i][j] << ' ';
			}
			out << '\n';
		}
	}
	
	template<typename T, size_t SE, size_t SI>
	static std::istream& operator>>(std::istream& in, TPointMatrix<T, SE, SI>* m)
	{
		for (int i = 0; i < SE; ++i) {
			std::array<T, SI> arr = {};
			for (int j = 0; j < SI; ++j) {
				in >> arr[j];
			}
			m[i] = arr;
		}
	}

	template<typename T, size_t SE, size_t SI>
	static std::ostream& operator<<(std::ostream& out, TPointMatrix<T, SE, SI>* m)
	{
		for (int i = 0; i < SE; ++i) {
			for (int j = 0; j < SI; ++j) {
				out << m[i][j] << ' ';
			}
			out << '\n';
		}
	}

	template<typename T, size_t SE, size_t SI>
	static TPointMatrix<T, SE, SI> ReadFile(std::ifstream& in, TPointMatrix<T, SE, SI>* m)
	{
		in >> m;
	}

	template<typename T, size_t SE, size_t SI>
	static TPointMatrix<T, SE, SI> WriteFile(std::ofstream& out, TPointMatrix<T, SE, SI>* m)
	{
	    out << m;
	}
}
