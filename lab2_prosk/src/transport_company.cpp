#include <iostream>

#include <transport_company.hpp>
#include <transport_stream.hpp>

namespace Tran {

	TransportCompany::TransportCompany()
		: m_vehicles() {}

	TransportCompany::TransportCompany(const TransportCompany& t)
	{
		for (auto v : m_vehicles) {
			delete v;
		}
		
		for (auto v : t.m_vehicles) {
			if (v->GetType() == "Lorry") {
				auto temp = dynamic_cast<Lorry*>(v);
				CreateInstanceOfLorry(*temp);
			} else {
				auto temp = dynamic_cast<Ship*>(v);
				CreateInstanceOfShip(*temp);
			}
		}
	}
	
	TransportCompany::~TransportCompany()
	{
		for (auto v : m_vehicles) {
			delete v;
		}
	}
	
	Lorry& TransportCompany::CreateInstanceOfLorry(std::string_view name, std::string_view dep, std::string_view dest, size_t capacity, size_t wheels)
	{
		Lorry* lorry = new Lorry(name, dep, dest, capacity, wheels);
		m_vehicles.push_back(lorry);

		return *lorry;
	}

	Ship& TransportCompany::CreateInstanceOfShip(std::string_view name, std::string_view dep, std::string_view dest, size_t capacity, size_t containers)
	{
		Ship* ship = new Ship(name, dep, dest, capacity, containers);
		m_vehicles.push_back(ship);

		return *ship;
	}

	Lorry& TransportCompany::CreateInstanceOfLorry(Lorry& l)
    {
		Lorry* lorry = new Lorry(l);
		m_vehicles.push_back(lorry);

		return *lorry;
	}
	
	Ship& TransportCompany::CreateInstanceOfShip(Ship& s)
	{
		Ship* ship = new Ship(s);
		m_vehicles.push_back(ship);

		return *ship;
	}

	void TransportCompany::ReadFromFile(std::ifstream& in)
	{
		// type name dep dest cap (wheels | containers)
		CargoVehicle* veh;
		std::string type;
		while (in >> type) {
			if (type == "Lorry") {
			    Lorry* l = new Lorry();
				in >> *l;
				veh = l;
			} else if (type == "Ship") {
			    Ship* s = new Ship();
				in >> *s;
				veh = s;
			} else {
				continue;
			}
			m_vehicles.push_back(veh);
		}
	}

	void TransportCompany::WriteToFile(std::ofstream& out) const
	{
		for (auto v : m_vehicles) {
			if (v->GetType() == "Lorry") {
				Lorry* l = dynamic_cast<Lorry*>(v);
				out << *l;
			} else {
				Ship* s = dynamic_cast<Ship*>(v);
				out << *s;
			}
			out << '\n';
		}
	}
	
	void TransportCompany::PrintInfo() const
	{
		for (auto v : m_vehicles) {
			std::cout << GetBaseInfo(v) << std::endl;
		}
	}

	std::string TransportCompany::GetBaseInfo(CargoVehicle* v) const
	{
	    return v->GetType() + " " + v->GetName() + " " + v->GetDeparture() + " " + v->GetDestination() + " " + std::to_string(v->GetCapacity());
	}

	void TransportCompany::PrintCountDepDest() const
	{
		size_t dep = 0, dest = 0;
		std::vector<std::string_view> deps, dests;
		for (auto v : m_vehicles) {
			dep++; dest++;
			for (auto s : deps) {
				if (v->GetDeparture() == s) {
					dep--;
				    break;
				}
			}
			for (auto s : dests) {
				if (v->GetDestination() == s) {
					dest--;
				    break;
				}
			}
			deps.push_back(v->GetDeparture());
			dests.push_back(v->GetDestination());
		}

		std::cout << "Кол-во отправлений: " << dep << "\nКол-во назначений: " << dest << std::endl;
	}

	std::string TransportCompany::GetDepDest(CargoVehicle* v) const
	{
		 return v->GetName() + " " + " travels from " + " " + v->GetDeparture() + " " + " to " + " " + v->GetDestination();
	}

	void TransportCompany::PrintSortByCap(size_t min_cap, size_t max_cap) const
	{
		for (auto v : m_vehicles) {
			if (v->GetCapacity() <= max_cap && v->GetCapacity() >= min_cap) {
				std::cout << GetBaseInfo(v) << std::endl;
			}
		}
	}
}
