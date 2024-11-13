#include <matrix.hpp>

#include <math.h>
#include <cfloat>
#include <random>

namespace Math {

	template<typename T, size_t SE, size_t SI>
	bool TPointMatrix<T, SE, SI>::operator<(const TPointMatrix<T, SE, SI>& m)
	{
		for (size_t i = 0; i < SE; ++i) {
			for (size_t j = 0; j < SI; ++j) {
				if (m_arrt[i][j] >= m.m_arrt[i][j]) {
					return false;
				}
			}
		}
	}
	
	template<typename T, size_t SE, size_t SI>
	bool TPointMatrix<T, SE, SI>::operator<=(const TPointMatrix<T, SE, SI>& m)
	{
		for (size_t i = 0; i < SE; ++i) {
			for (size_t j = 0; j < SI; ++j) {
				if (m_arrt[i][j] > m.m_arrt[i][j]) {
					return false;
				}
			}
		}
	}
	
	template<typename T, size_t SE, size_t SI>
	bool TPointMatrix<T, SE, SI>::operator!=(const TPointMatrix<T, SE, SI>& m)
	{
		return !(*this == m);
	}

	template<typename T, size_t SE, size_t SI>
	bool TPointMatrix<T, SE, SI>::operator>=(const TPointMatrix<T, SE, SI>& m)
	{
		for (size_t i = 0; i < SE; ++i) {
			for (size_t j = 0; j < SI; ++j) {
				if (m_arrt[i][j] <= m.m_arrt[i][j]) {
					return false;
				}
			}
		}
	}

	
	template<typename T, size_t SE, size_t SI>
	bool TPointMatrix<T, SE, SI>::operator>(const TPointMatrix<T, SE, SI>& m)
	{
		for (size_t i = 0; i < SE; ++i) {
			for (size_t j = 0; j < SI; ++j) {
				if (m_arrt[i][j] < m.m_arrt[i][j]) {
					return false;
				}
			}
		}
	}
	
	template<typename T, size_t SE, size_t SI>
	bool TPointMatrix<T, SE, SI>::operator==(const TPointMatrix<T, SE, SI>& m)
	{
		for (size_t i = 0; i < SE; ++i) {
			for (size_t j = 0; j < SI; ++j) {
				if (m_arrt[i][j] != m.m_arrt[i][j]) {
					return false;
				}
			}
		}
	}
	
	template<typename T, size_t SE, size_t SI>
	TPointMatrix<T, SE, SI>& TPointMatrix<T, SE, SI>::operator=(const TPointMatrix<T, SE, SI>& m)
	{
		m_arrt = m.m_arrt;

		return this;
	}

	template<typename T, size_t SE, size_t SI>
	void TPointMatrix<T, SE, SI>::SetRandomValue()
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(-1000, 1000);

	    m_arrt[dist(rng) % SE][dist(rng) % SI] = dist(rng);
	}
	
	template<typename T, size_t SE, size_t SI>
	void TPointMatrix<T, SE, SI>::GenerateRandomValues()
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(-1000, 1000);
		
		for (size_t i = 0; i < SE; ++i) {
			std::array<T, SI> arr = {};
			for (size_t j = 0; j < SI; ++j) {
				arr[j] = dist(rng);
			}
		}
	}

    template<typename T, size_t SE, size_t SI>
	T TPointMatrix<T, SE, SI>::GetFarthest()
	{
		double midarth = 0;
		for (auto arr : m_arrt) {
			double temp;
			for (auto el : arr) {
			    temp = el * el;
			}
			midarth += sqrt(temp);
		}
		midarth /= SE;

		double diff = 0; T val;
		for (auto arr : m_arrt) {
			for (auto el : arr) {
			    double new_diff = std::abs(midarth - el);
				if (new_diff > diff) {
				    diff = new_diff;
					val = el;
				}
			}
		}

		return val;
	}

    template<typename T, size_t SE, size_t SI>
    TPointMatrix<T, SE, SI>::Info TPointMatrix<T, SE, SI>::CountSum()
	{
		TPointMatrix::Info info {};
		for (size_t i = 0; i < SE; ++i) {
			if (i % 2 != 0) {
				bool is_neg = false;
				double temp = 0;
				for (auto el : m_arrt[i]) {
					if (el < 0) is_neg = true;
					temp += static_cast<double>(el*el);
				}
				if (is_neg) info.sum_of_neg += sqrt(temp);
				else info.sum_of_pos += sqrt(temp);
			}
		}
	}
	
}
