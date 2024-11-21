#include <t2dpoint.hpp>

#include <math.h>

namespace Math {

	T2DPoint T2DPoint::operator+(const T2DPoint& p)
	{
	    x += p.x;
		y += p.y;
		
		return *this;
	}
	
    T2DPoint T2DPoint::operator-(const T2DPoint& p)
    {
	    x -= p.x;
		y -= p.y;
		
		return *this;
	}
	
    T2DPoint T2DPoint::operator*(const T2DPoint& p)
	{
	    x *= p.x;
		y *= p.y;
		
		return *this;
	}
	
    double T2DPoint::GetLength() const
	{
		return sqrt(x * x + y * y);
	}
	
	bool T2DPoint::operator>(const T2DPoint& p)
	{
		return GetLength() > p.GetLength();
	}
	
	bool T2DPoint::operator>=(const T2DPoint& p)
	{
	    return GetLength() >= p.GetLength();
	}
	
	bool T2DPoint::operator<(const T2DPoint& p)
	{
	    return GetLength() < p.GetLength();
	}
	
	bool T2DPoint::operator<=(const T2DPoint& p)
	{
		return GetLength() <= p.GetLength();
	}
	
	bool T2DPoint::operator!=(const T2DPoint& p)
	{
		return !(*this == p);
	}
	
	bool T2DPoint::operator==(const T2DPoint& p)
	{
	    return GetLength() == p.GetLength();
	}
	
}
