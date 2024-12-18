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

		T3DPoint& operator=(double d);

		double operator+(double d);
		double operator-(double d);
		double operator+=(const double& d);
		double operator-=(const double& d);
		
	    T3DPoint operator+(const T3DPoint& p);
	    T3DPoint operator-(const T3DPoint& p);
	    T3DPoint operator*(const T3DPoint& p);
		
	    double GetLength() const;

		bool operator<(double d);
		bool operator<(int d);
		bool operator>(const T3DPoint& p);
		bool operator>=(const T3DPoint& p);
		
		bool operator<(const T3DPoint& p);
		bool operator<=(const T3DPoint& p);
		
		bool operator!=(const T3DPoint& p);
	    bool operator==(const T3DPoint& p);
				
		int x, y, z;
	};

	double operator+(const double& d, const T3DPoint& p);
	double operator-(const double& d, const T3DPoint& p);
	double operator+=(double& d, const T3DPoint& p);
	double operator-=(double& d, const T3DPoint& p);

	bool operator<(double d, const T3DPoint& p);
	bool operator<(int d, const T3DPoint& p);
}
