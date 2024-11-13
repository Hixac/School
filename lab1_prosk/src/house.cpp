#include <house.hpp>

namespace Man {

	House::House(size_t address, size_t floor, size_t rooms, size_t space, std::string_view building_type)
		: m_address(address),
		  m_floor(floor),
		  m_room_count(rooms),
		  m_space(space),
		  m_building_type(building_type)
	{ }
	
}
