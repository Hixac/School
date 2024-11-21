#pragma once

namespace Math {

	class T2DPoint
	{
	public:
		T2DPoint() = default;
		T2DPoint(int t) : x(t), y(t) {}
		T2DPoint(int x, int y) : x(x), y(y) {}
		T2DPoint(const T2DPoint&) = default;
		~T2DPoint() = default;

		T2DPoint operator+(const T2DPoint& p);
		T2DPoint operator-(const T2DPoint& p);
		T2DPoint operator*(const T2DPoint& p);
		
	    double GetLength() const;
		
		bool operator>(const T2DPoint& p);
		bool operator>=(const T2DPoint& p);
		
		bool operator<(const T2DPoint& p);
		bool operator<=(const T2DPoint& p);
		
		bool operator!=(const T2DPoint& p);
	    bool operator==(const T2DPoint& p);
		
		int x, y;
	};
	
}
