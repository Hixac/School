#pragma once

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

#include <house.hpp>

namespace Man {

	static std::ostream& operator<<(std::ostream& os, const House& h)
	{
		os << "Аддр: " << h.GetAddress() << "; Комнаты: " << h.GetRooms() << "; Этаж: " << h.GetFloor() << "; Площадь: " << h.GetSpace() << "; Тип здания: " << h.GetBuildingType();
		return os;
	}

	static std::istream& operator>>(std::istream& is, House& h)
	{
		size_t addr, rooms, floor, space;
		is >> addr >> rooms >> floor >> space;
		std::string build;
		is >> build;

		h.SetAddress(addr);
		h.SetRooms(rooms);
		h.SetFloor(floor);
		h.SetSpace(space);
		h.SetBuildingType(build);
		
		return is;
	}

	static void save_houses_in_file(std::ofstream& out, const std::vector<House>& houses)
	{
		if (!out.is_open()) assert(false && "FATAL: file doesn't opened!");

		for (auto& h : houses) {
			out << h.GetAddress() << " " << h.GetRooms() << " " << h.GetFloor() << " " << h.GetSpace() << " " << h.GetBuildingType() << '\n';
		}

		out.close();
	}
	
	static std::vector<House> get_houses_from_file(std::ifstream& in)
	{
		if (!in.is_open()) assert(false && "FATAL: file doesn't opened!");

		std::vector<House> houses;
		House h;
		std::string buf;
		while (in >> buf) {
			h.SetAddress(std::stoi(buf.c_str()));
			in >> buf;
			h.SetRooms(std::stoi(buf.c_str()));
			in >> buf;
			h.SetFloor(std::stoi(buf.c_str()));
			in >> buf;
			h.SetSpace(std::stoi(buf.c_str()));
			in >> buf;
			h.SetBuildingType(buf);
			
			houses.push_back(h);
		}

		in.close();
		
		return houses;
	}
	
}
