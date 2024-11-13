#pragma once

#include <string>

namespace Man {

	using std::size_t;

	class House
	{
	public:
		House(size_t address, size_t floor, size_t rooms, size_t space, std::string_view building_type);
		House() = default;
		~House() = default;

		inline bool operator<(const House& h) const { return m_space < h.m_space; }
		inline bool operator>(const House& h) const { return m_space > h.m_space; }
		inline bool operator<=(const House& h) const { return m_space <= h.m_space; }
		inline bool operator>=(const House& h) const { return m_space >= h.m_space; }
		
		inline size_t GetAddress() const { return m_address; }
		inline size_t GetFloor() const { return m_floor; }
		inline size_t GetRooms() const { return m_room_count; }
		inline size_t GetSpace() const { return m_space; }

		inline const std::string& GetBuildingType() const { return m_building_type; }

		inline void SetAddress(const size_t v) { m_address = v; }
		inline void SetFloor(const size_t v) { m_floor = v; }
		inline void SetRooms(const size_t v) { m_room_count = v; }
		inline void SetSpace(const size_t v) { m_space = v; }

		inline void SetBuildingType(std::string_view v) { m_building_type = v; }
		
	private:
		size_t m_address, m_floor, m_room_count, m_space;
		std::string m_building_type;
	};
	
}
