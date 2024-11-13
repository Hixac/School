#pragma once

#include <vector>
#include <fstream>

#include <vehicle.hpp>
#include <lorry.hpp>
#include <ship.hpp>

namespace Tran {

	class TransportCompany
	{
	public:
		TransportCompany();
		TransportCompany(const TransportCompany& t);
		~TransportCompany();
		
	    Lorry& CreateInstanceOfLorry(std::string_view name, std::string_view dep, std::string_view dest, size_t capacity, size_t wheels);
	    Ship& CreateInstanceOfShip(std::string_view name, std::string_view dep, std::string_view dest, size_t capacity, size_t containers);

		Lorry& CreateInstanceOfLorry(Lorry& l);
	    Ship& CreateInstanceOfShip(Ship& l);
		
		void ReadFromFile(std::ifstream& in);
		void WriteToFile(std::ofstream& out) const;
		
		void PrintInfo() const;
		void PrintCountDepDest() const;

		void PrintSortByCap(size_t min_cap, size_t max_cap) const;
		
	private:
		std::string GetDepDest(CargoVehicle* v) const;
		std::string GetBaseInfo(CargoVehicle* v) const;
		
		std::vector<CargoVehicle*> m_vehicles;
	};
	
}
