#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <cfloat>
#include <random>
#include <cassert>

namespace Math {

    template<typename T>
	class TPointMatrix
	{
	public:
		struct Info {
		    double sum_of_neg;
		    double sum_of_pos;
		};
		
		TPointMatrix() = default;
		TPointMatrix(const TPointMatrix&) = default;
		TPointMatrix<T>& operator=(const TPointMatrix<T>& m);
		~TPointMatrix() = default;
		
		bool operator>(const TPointMatrix<T>& m);
		bool operator>=(const TPointMatrix<T>& m);
		
		bool operator<(const TPointMatrix<T>& m);
		bool operator<=(const TPointMatrix<T>& m);
		
		bool operator!=(const TPointMatrix<T>& m);
	    bool operator==(const TPointMatrix<T>& m);
		
		inline std::vector<T>& operator[](int x) { return m_arrt[x]; }
		
		inline std::vector<T> GetMatrix() { return m_arrt; }
		inline std::vector<T> SetMatrix(std::vector<T> arrtwo) { m_arrt = arrtwo; }

		std::istream& Input(std::istream& in);
		std::ostream& Output(std::ostream& out);
		
		void SetRandomValue(int extern_id, int inner_id);
		void GenerateRandomValues();
		T GetFarthest();
		Info CountSum();

    private:
		std::vector<std::vector<T>> m_arrt;
	};

	
	template<typename T>
	bool TPointMatrix<T>::operator<(const TPointMatrix<T>& m)
	{
	    return m_arrt.size() < m.m_arrt.size();
	}
	
	template<typename T>
	bool TPointMatrix<T>::operator<=(const TPointMatrix<T>& m)
	{
	    return m_arrt.size() <= m.m_arrt.size();
	}
	
	template<typename T>
	bool TPointMatrix<T>::operator!=(const TPointMatrix<T>& m)
	{
		return !(*this == m);
	}

	template<typename T>
	bool TPointMatrix<T>::operator>=(const TPointMatrix<T>& m)
	{
	    return m_arrt.size() >= m.m_arrt.size();
	}
	
	template<typename T>
	bool TPointMatrix<T>::operator>(const TPointMatrix<T>& m)
	{
	    return m_arrt.size() > m.m_arrt.size();
	}
	
	template<typename T>
	bool TPointMatrix<T>::operator==(const TPointMatrix<T>& m)
	{
	    return m_arrt.size() == m.m_arrt.size();
	}
	
	template<typename T>
	TPointMatrix<T>& TPointMatrix<T>::operator=(const TPointMatrix<T>& m)
	{
		m_arrt = m.m_arrt;

		return this;
	}

	template<typename T>
	void TPointMatrix<T>::SetRandomValue(int extern_id, int inner_id)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<int> dist(-1000, 1000);

	    m_arrt[extern_id][inner_id] = dist(rng);
	}
	
	template<typename T>
	void TPointMatrix<T>::GenerateRandomValues()
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<int> dist(-1000, 1000);
		
		for (size_t i = 0; i < 9; ++i) {
			std::vector<T> v = {dist(rng), dist(rng), dist(rng)};
		    m_arrt.push_back(v);
		}
	}

    template<typename T>
	T TPointMatrix<T>::GetFarthest()
	{
		double midarth = 0;
		for (auto arr : m_arrt) {
			for (auto el : arr) {
			    midarth += el;
			}
		}
		midarth /= m_arrt.size();

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

	template<typename T>
	TPointMatrix<T>::Info TPointMatrix<T>::CountSum()
	{
		TPointMatrix::Info info {0, 0};
		for (size_t i = 0; i < m_arrt.size(); ++i) {
			if (i % 2 != 0) {
				for (auto el : m_arrt[i]) {
					if (el < 0) {
						info.sum_of_neg += el;
					} else {
						info.sum_of_pos += el;
					}
				}
			}
		}
		
		return info;
	}
	
	template<typename T>                   
	std::istream& TPointMatrix<T>::Input(std::istream& in)
	{
		m_arrt.clear();

		// 3 3
		// 1 2 3
		// 4 5 6
		// 7 8 9

		int extern_size; 
		in >> extern_size;
		
		int inner_size; 
		in >> inner_size;

		std::vector<int> vec;
		
	    int buf, i = 0, j = 0;
		while (in >> buf && i < extern_size) {
			i++; j++;
		    vec.push_back(buf);
			if (j == inner_size) {
				m_arrt.push_back(vec);
				j = 0;
			}
		}

		return in;
	}

		template<typename T>
		std::ostream& TPointMatrix<T>::Output(std::ostream& out)
		{
			out << m_arrt.size();
			for (auto& v : m_arrt) {
				for (auto& el : v) {
					out << el;
				}
				out << '\n';
			}

			return out;
		}

}
