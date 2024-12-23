#pragma once

#include <string>
#include <iostream>

namespace Tran {

	class CargoVehicle
	{
	public:
		CargoVehicle(std::string_view name, std::string_view dep, std::string_view dest, size_t capacity)
			: m_name(name), m_dep(dep), m_dest(dest), m_capacity(capacity) {}
		CargoVehicle() = default;
		inline CargoVehicle(const CargoVehicle& c) {
			m_name = c.m_name;
			m_dep = c.m_dep;
			m_dest = c.m_dest;
			m_capacity = c.m_capacity;
		}
		virtual ~CargoVehicle() = default;

		inline std::string GetName() const { return m_name; }
		inline std::string GetDeparture() const { return m_dep; }
		inline std::string GetDestination() const { return m_dest; }
		inline size_t GetCapacity() const { return m_capacity; }

		inline void SetName(std::string_view buf) { m_name = buf; }
		inline void SetDeparture(std::string_view buf) { m_dep = buf; }
		inline void SetDestination(std::string_view buf) { m_dest = buf; }
		inline void SetCapacity(size_t buf) { m_capacity = buf; } 

		virtual std::istream& in(std::istream& in) {
			in >> m_name >> m_dep >> m_dest >> m_capacity;
			return in;
		};
		
		virtual std::ostream& out(std::ostream& out) const {
			out << m_name << ' ' << m_dep << ' ' << m_dest << ' ' << m_capacity;
			return out;
		};
		
		virtual std::string GetType() = 0;
		
	protected:
		std::string m_name;
		std::string m_dep;
		std::string m_dest;

		size_t m_capacity;
	};
	
}
