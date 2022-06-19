//��������� ������� ���������. ����� 9 ������ 3. ���������� ���������.
#include "transport_catalogue.h"

namespace transport_catalogue 
{
	// �������� �������. ��������� ������� � ���� ���������, � ����� ��������� 
	// ������������� ������ �� ������� � ���� �� ������ ��������.
	void TransportCatalogue::AddBus(std::string bus_number, RouteType route_type, size_t unique_stops, size_t stops_on_route, unsigned int route_length, double route_curvature) {
		buses_.emplace_back(Bus{ bus_number, route_type, unique_stops, stops_on_route, route_length, route_curvature });
		busname_to_bus_.emplace(std::move(bus_number), &buses_.back());
	}

	// �������� ���������. ��������� ��������� � ���� ���������, � ����� ��������� 
	// ������������� ������ �� ��������� � ���� �� ����� ���������.
	void TransportCatalogue::AddStop(std::string name, double latitude, double longitude) {
		stops_.emplace_back(Stop{ latitude, longitude });
		stopname_to_stop_.emplace(std::move(name), &stops_.back());
	}

	// �������� �������, ���������� ����� ���������. ��������� � unordered_map
	// �������, ���������� ����� ���������.
	void TransportCatalogue::AddBusRelatedToStop(const Stop* stop_ptr, const std::string& bus_number) {
		buses_to_stop[stop_ptr].emplace(FindBus(bus_number));
	}

	// ����� ������� �� ������. ������� � ������������ ������� �� ������,
	// ���� ����� ������� ���� �� ���������� ��������� �� ���� � ���� ���������,
	// ���� ������ �������� ���, �� ���������� nullptr.
	const Bus* TransportCatalogue::FindBus(const std::string& name)const {
		auto it = busname_to_bus_.find(name);
		return it == busname_to_bus_.end() ? nullptr : it->second;
	}

	// ����� ������� ��������� �� �����. ���������� ��������� �� ���������, ���� ��������� ��� � ����������� ���������� ������ ���������.
	const Stop* TransportCatalogue::FindStop(const std::string& name) const {
		auto it = stopname_to_stop_.find(name);
		return it == stopname_to_stop_.end() ? nullptr : it->second;
	}

	// ����� ���������� ���������� � �������� � �������: ���-�� ���������� ���������, ����� ��������� �� ��������, ����������� ����� ��������,
	// �������� ��������.
	std::tuple<size_t, size_t, unsigned int, double>TransportCatalogue::GetRouteInfo(const Bus* ptr) const {
		return std::make_tuple(ptr->unique_stops, ptr->stops_on_route, ptr->real_route_length, ptr->route_curvature);
	}

	// ����� ���������� ��������� ���������, ������� ��������������� �� ������ ���������.
	const std::set < const Bus*, detail::CmpBuses>* TransportCatalogue::GetBusesRelatedToStop(const Stop* ptr) const {
		auto it = buses_to_stop.find(ptr);
		return it == buses_to_stop.end() ? nullptr : &it->second;
	}

	// ����� ��������� � ������� ��������� ����� �����������. ������ � ������� ������ ���� ���������.
	void TransportCatalogue::AddDistanceBetweenStops(const std::string& from_stop, const std::string& to_stop, unsigned int distance) {
		stops_distances.emplace(std::make_pair(FindStop(from_stop), FindStop(to_stop)), distance);
	}

	// ����� ���������� ���������� ����� ����� �����������. 
	unsigned int TransportCatalogue::GetDistanceBetweenStops(const std::string& from_stop, const std::string& to_stop) {
		auto it_forward = stops_distances.find(std::make_pair(FindStop(from_stop), FindStop(to_stop)));
		if (it_forward == stops_distances.end()) {
			auto it_back = stops_distances.find(std::make_pair(FindStop(to_stop), FindStop(from_stop)));
			return it_back->second;
		}
		else {
			return it_forward->second;
		}
	}
}