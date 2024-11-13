#pragma once

#include <iostream>

#include <vehicle.hpp>

namespace Tran {

	static std::istream& operator>>(std::istream& in, Tran::CargoVehicle& c)
	{
		c.in(in);
		return in;
	}

	static std::ostream& operator<<(std::ostream& out, Tran::CargoVehicle& c)
	{
		c.out(out);
		return out;
	}
	
}
