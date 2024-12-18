#include <t3dpoint.hpp>

#include <math.h>

namespace Math {

	T3DPoint& T3DPoint::operator=(double d)
	{
		x = d; y = d; z = d;
		return *this;
	}

	double T3DPoint::operator+(double d)
	{
		return GetLength() + d;
	}
		
	double T3DPoint::operator-(double d)
	{
		return GetLength() - d;
	}

	double T3DPoint::operator+=(const double& d)
	{
		return GetLength() + d;
	}
		
	double T3DPoint::operator-=(const double& d)
	{
		return GetLength() - d;
	}
	
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

	bool T3DPoint::operator<(double d)
	{
		return d > x || d > y || d > z;
	}
	
	bool T3DPoint::operator<(int d)
	{
		return d > x || d > y || d > z;
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

	double operator+(const double& d, const T3DPoint& p)
	{
		return d + p.GetLength();
	}
	
	double operator-(const double& d, const T3DPoint& p)
	{
		return d - p.GetLength();
	}

	double operator+=(double& d, const T3DPoint& p)
	{
	    d += p.GetLength();
		return d;
	}
	
	double operator-=(double& d, const T3DPoint& p)
	{
	    d -= p.GetLength();
		return d;
	}

	bool operator<(double d, const T3DPoint& p)
	{
		return d < p.GetLength();
	}

	bool operator<(int d, const T3DPoint& p)
	{
		return d < p.GetLength();
	}
}
