#include <t3dpoint.hpp>

#include <math.h>

namespace Math {

    T3DPoint T3DPoint::operator+(const T3DPoint& p)
	{
	    x += p.x;
		y += p.y;
		z += p.z;

		return *this;
	}
	
    T3DPoint T3DPoint::operator-(const T3DPoint& p)
    {
	    x -= p.x;
		y -= p.y;
		z -= p.z;

		return *this;
	}
	
    T3DPoint T3DPoint::operator*(const T3DPoint& p)
	{
	    x *= p.x;
		y *= p.y;
		z *= p.z;

		return *this;
	}
	
    double T3DPoint::GetLength() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	
	bool T3DPoint::operator>(const T3DPoint& p)
	{
		return GetLength() > p.GetLength();
	}
	
	bool T3DPoint::operator>=(const T3DPoint& p)
	{
	    return GetLength() >= p.GetLength();
	}
	
	bool T3DPoint::operator<(const T3DPoint& p)
	{
	    return GetLength() < p.GetLength();
	}
	
	bool T3DPoint::operator<=(const T3DPoint& p)
	{
		return GetLength() <= p.GetLength();
	}
	
	bool T3DPoint::operator!=(const T3DPoint& p)
	{
		return !(*this == p);
	}
	
	bool T3DPoint::operator==(const T3DPoint& p)
	{
	    return GetLength() == p.GetLength();
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
