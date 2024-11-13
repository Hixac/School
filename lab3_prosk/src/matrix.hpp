#pragma once

#include <base.hpp>

namespace Math {

    template<typename T, size_t SE, size_t SI>
	class TPointMatrix
	{
	public:
		struct Info {
		    double sum_of_neg;
		    double sum_of_pos;
		};
		
		TPointMatrix() = default;
		TPointMatrix(const TPointMatrix&) = default;
		TPointMatrix<T, SE, SI>& operator=(const TPointMatrix<T, SE, SI>& m);
		~TPointMatrix() = default;
		
		bool operator>(const TPointMatrix<T, SE, SI>& m);
		bool operator>=(const TPointMatrix<T, SE, SI>& m);
		
		bool operator<(const TPointMatrix<T, SE, SI>& m);
		bool operator<=(const TPointMatrix<T, SE, SI>& m);
		
		bool operator!=(const TPointMatrix<T, SE, SI>& m);
	    bool operator==(const TPointMatrix<T, SE, SI>& m);
		
		inline std::array<T, SI>& operator[](int x) { return m_arrt[x]; }
		
		inline ArrTwo<T, SE, SI> GetMatrix() { return m_arrt; }
		inline ArrTwo<T, SE, SI> SetMatrix(ArrTwo<T, SE, SI> arrtwo) { m_arrt = arrtwo; }

		void SetRandomValue();
		void GenerateRandomValues();
		T GetFarthest();
		Info CountSum();

    protected:
		ArrTwo<T, SE, SI> m_arrt;
	};
	
}
