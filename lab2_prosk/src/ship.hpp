#pragma once

#include <vehicle.hpp>

namespace Tran {

	class Ship : public CargoVehicle
	{
	public:
		Ship(std::string_view name, std::string_view dep, std::string_view dest, size_t capacity, size_t containers)
			: CargoVehicle(name, dep, dest, capacity), m_containers(containers) {}
		Ship() = default;
		inline Ship(const Ship& l) : CargoVehicle(l.m_name, l.m_dep, l.m_dest, l.m_capacity) {
		    m_containers = l.m_containers;
		}
		~Ship() = default;

		inline std::istream& in(std::istream& in) override {
			CargoVehicle::in(in);
			in >> m_containers;
			return in;
		}
			
		inline std::ostream& out(std::ostream& out) const override {
			CargoVehicle::out(out);
			out << m_containers;
			return out;
		}
		
		inline size_t GetContainers() { return m_containers; }
		inline void SetContainers(size_t buf) { m_containers = buf; }

		inline std::string GetType() override { return "Ship"; };
		
	private:
		size_t m_containers;
	};
	
}
