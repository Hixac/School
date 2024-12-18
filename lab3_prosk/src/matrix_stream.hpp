#pragma once

#include <iostream>
#include <fstream>

#include <t3dpoint.hpp>
#include <t2dpoint.hpp>
#include <matrix.hpp>

namespace Math {
	
	static std::istream& operator>>(std::istream& in, T3DPoint& t)
	{
	    in >> t.x >> t.y >> t.z;
		return in;
	}

	static std::istream& operator>>(std::istream& in, T2DPoint& t)
	{
	    in >> t.x >> t.y;
		return in;
	}

	static std::ostream& operator<<(std::ostream& out, const T3DPoint& t)
	{
	    out << t.x << ' ' << t.y << ' ' << t.z;
		return out;
	}

	static std::ostream& operator<<(std::ostream& out, const T2DPoint& t)
	{
	    out << t.x << ' ' << t.y;
		return out;
	}
	
	template<typename T>
	static std::istream& operator>>(std::istream& in, TPointMatrix<T>* m)
	{
		m->Input(in);
	}

	template<typename T>
	static std::ostream& operator<<(std::ostream& out, TPointMatrix<T>* m)
	{
	    m->Output(out);
	}

	template<typename T>
	static TPointMatrix<T> ReadFile(std::ifstream& in, TPointMatrix<T>* m)
	{
		in >> m;
	}

	template<typename T>
	static TPointMatrix<T> WriteFile(std::ofstream& out, TPointMatrix<T>* m)
	{
	    out << m;
	}
}
