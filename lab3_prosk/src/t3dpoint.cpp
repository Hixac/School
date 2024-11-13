#include <t3dpoint.hpp>

#include <math.h>

namespace Math {

	double T3DPoint::operator+(const T3DPoint& p)
	{
		return GetLength() + p.GetLength();
	}
	
	double T3DPoint::operator-(const T3DPoint& p)
	{
		return GetLength() - p.GetLength();
	}
	
	double T3DPoint::operator*(const T3DPoint& p)
	{
		return x*p.x + y*p.y + z*p.z;
	}
	
    double T3DPoint::GetLength() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	
	bool T3DPoint::operator>(const T3DPoint& p)
	{
		return x > p.x && y > p.y && z > p.z;
	}
	
	bool T3DPoint::operator>=(const T3DPoint& p)
	{
		return x >= p.x && y >= p.y && z >= p.z;
	}
	
	bool T3DPoint::operator<(const T3DPoint& p)
	{
		return x < p.x && y < p.y && z < p.z;
	}
	
	bool T3DPoint::operator<=(const T3DPoint& p)
	{
		return x <= p.x && y <= p.y && z <= p.z;
	}
	
	bool T3DPoint::operator!=(const T3DPoint& p)
	{
		return !(*this == p);
	}
	
	bool T3DPoint::operator==(const T3DPoint& p)
	{
		return x == p.x && y == p.y && z == p.z;
	}

	double operator+(double d, const T3DPoint& p)
	{
		return d + p.GetLength();
	}
	
	double operator-(double d, const T3DPoint& p)
	{
		return d - p.GetLength();
	}
}
