#pragma once

namespace Math {

	class T3DPoint
	{
	public:
		T3DPoint() = default;
		T3DPoint(int t) : x(t), y(t), z(t) {}
		T3DPoint(int x, int y, int z) : x(x), y(y), z(z) {}
		T3DPoint(const T3DPoint&) = default;
		~T3DPoint() = default;

	    T3DPoint operator+(const T3DPoint& p);
	    T3DPoint operator-(const T3DPoint& p);
	    T3DPoint operator*(const T3DPoint& p);
		
	    double GetLength() const;
		
		bool operator>(const T3DPoint& p);
		bool operator>=(const T3DPoint& p);
		
		bool operator<(const T3DPoint& p);
		bool operator<=(const T3DPoint& p);
		
		bool operator!=(const T3DPoint& p);
	    bool operator==(const T3DPoint& p);
				
		int x, y, z;
	};

	double operator+(double d, const T3DPoint& p);
	double operator-(double d, const T3DPoint& p);
}
