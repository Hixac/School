#pragma once

#include <vehicle.hpp>

namespace Tran {

	class Lorry : public CargoVehicle
	{
	public:
		Lorry(std::string_view name, std::string_view dep, std::string_view dest, size_t capacity, size_t wheels)
			: CargoVehicle(name, dep, dest, capacity), m_wheels(wheels) {}
		Lorry() = default;
		inline Lorry(const Lorry& l) : CargoVehicle(l.m_name, l.m_dep, l.m_dest, l.m_capacity) {
			m_wheels = l.m_wheels;
		}
		~Lorry() = default;

		inline size_t GetWheels() { return m_wheels; }
		inline void SetWheels(size_t buf) { m_wheels = buf; }

		inline std::istream& in(std::istream& in) override {
			CargoVehicle::in(in);
			in >> m_wheels;
			return in;
		}
			
		inline std::ostream& out(std::ostream& out) const override {
			CargoVehicle::out(out);
			out << m_wheels;
			return out;
		}

		inline std::string GetType() override { return "Lorry"; };
		
	private:
		size_t m_wheels;
	};
	
}
