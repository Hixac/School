#pragma once

namespace Math {

	class T2DPoint
	{
	public:
		T2DPoint() = default;
		inline T2DPoint(int x, int y) : x(x), y(y) {}
		T2DPoint(const T2DPoint&) = default;
		~T2DPoint() = default;

		bool operator>(const T2DPoint& p);
		bool operator>=(const T2DPoint& p);
		
		bool operator<(const T2DPoint& p);
		bool operator<=(const T2DPoint& p);
		
		bool operator!=(const T2DPoint& p);
	    bool operator==(const T2DPoint& p);
		
		int x, y;
	};
	
}
