#pragma once

#include <algorithm>
#include <house.hpp>
#include <vector>

namespace Man {

	static std::vector<House> get_houses_by_rooms(const std::vector<House>& houses, size_t rooms)
	{
		auto buf = std::vector<House>();
		for (auto& house : houses) {
			if (house.GetRooms() == rooms) buf.push_back(house);
		}

		return buf;
	}

	static std::vector<House> get_houses_by_rooms_floors(const std::vector<House>& houses, size_t rooms, size_t start_floor, size_t end_floor)
	{
		auto buf = std::vector<House>();
		for (auto& house : houses) {
			if (house.GetRooms() == rooms && start_floor > house.GetFloor() && house.GetFloor() < end_floor) buf.push_back(house);
		}

		return buf;
	}

	static std::vector<House> get_houses_by_space(const std::vector<House>& houses, size_t space)
	{
		auto buf = std::vector<House>();
		for (auto& house : houses) {
			if (house.GetSpace() > space) buf.push_back(house);
		}

		return buf;
	}

	static std::vector<House> sort_houses_by_space(const std::vector<House>& houses)
	{
		auto buf = std::vector<House>(houses);
		std::sort(buf.begin(),buf.end(), [](const House &x, const House &y){ return (x < y); });

		return buf;
	}
	
}
