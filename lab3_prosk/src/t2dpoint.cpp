
#include <t2dpoint.hpp>

namespace Math {

	bool T2DPoint::operator>(const T2DPoint& p)
	{
		return x > p.x && y > p.y;
	}
	
	bool T2DPoint::operator>=(const T2DPoint& p)
	{
		return x >= p.x && y >= p.y;
	}
	
	bool T2DPoint::operator<(const T2DPoint& p)
	{
		return x < p.x && y < p.y;
	}
	
	bool T2DPoint::operator<=(const T2DPoint& p)
	{
		return x <= p.x && y <= p.y;
	}
	
	bool T2DPoint::operator!=(const T2DPoint& p)
	{
		return !(*this == p);
	}
	
	bool T2DPoint::operator==(const T2DPoint& p)
	{
		return x == p.x && y == p.y;
	}
	
}
