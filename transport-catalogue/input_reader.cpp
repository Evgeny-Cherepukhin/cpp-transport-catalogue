//��������� ������� ���������. ����� 9 ������ 2. ���������� ���������.
#include "input_reader.h"
#include "geo.h"

namespace transport_catalogue 
{
	namespace input_requests_processing
	{
		using namespace std::string_literals;

		namespace detail
		{
			// ��������������� ������� ��� �������� ���������� ����� ����� �����������. ���������� ComputeDistance �� �����: geo.h.
			double ComputeDistanceBetweenStopsInRoute(const TransportCatalogue& transport_catalogue, const std::string& previous_stop_name, const std::string& current_stop_name) {
				return geo::ComputeDistance(transport_catalogue.FindStop(previous_stop_name)->coordinates, transport_catalogue.FindStop(current_stop_name)->coordinates);
			}

			// ��������������� �������. ��������� ����������� ������ �� �������, ��������� ��������� � ����������, ����� ������� ��������� �� ������.
			// ���������� ��������� � ���������� ������.
			std::string GetToken(std::string& line, const std::string& delimiter) {
				size_t pos = line.find(delimiter);
				std::string token = std::move(line.substr(0, pos));
				line.erase(0, pos == std::string::npos ? pos : pos + delimiter.length());
				return token;
			}			
		}

		// ����� ���������� ������ �������� ���������.
		std::forward_list<std::string> InputRequest::GetBusRequests() {
			return bus_requests;
		}

		// ����� ���������� ������� �� ���������� ���������
		std::forward_list<std::string> InputRequest::GetStopRequests() {
			return stops_requests;
		}

		// ����� ��������� �� ���� ���������� �������� � ������� �����. ���������� ����� InputRequest.
		InputRequest InputRequest::MakeListRequest(TransportCatalogue& transport_catalogue, size_t number_of_requests, std::istream& in) {
			std::string line;
			for (size_t i = 0; i < number_of_requests; ++i) {
				std::getline(in, line);
				std::string request_type = detail::GetToken(line, " "s);
				// ���� ��� - ��������� ���������� ������ � ������� ���������.
				if (request_type == "Stop"s) {
					stops_requests.emplace_front(std::move(line));
				}
				// ���� ��� - �������, ���������� ������ � ������� ���������.
				if (request_type == "Bus"s) {
					bus_requests.emplace_front(std::move(line));
				}
			}
			return *this;
		}
		
		// ������� �������� ������ �� ������� �������� ���������. �� ������ ���� �����: 
		// �������� ��� � ���������� ���������, ��������� ���������� ������ � �������. 		 
		void AddStopInCatalogue(TransportCatalogue& transport_catalogue, InputRequest& requests) {
			for (std::string stop_request : requests.GetStopRequests()) {
				std::string stop_name = detail::GetToken(stop_request, ": "s);
				double latitude = std::stod(detail::GetToken(stop_request, ", "s));
				double longitude = std::stod(detail::GetToken(stop_request, ", "s));
				transport_catalogue.AddStop(stop_name, latitude, longitude);
			}
		}

		// ������� ������������ ���������� ��������� �� ��������.
		size_t CalculateStopsOnRoute(std::string& bus_request, RouteType& route_type) {
			std::string delimiter;
			size_t stops_on_route = 0;
			size_t delimiter_count = 0;
			route_type == RouteType::CircularRoute ? delimiter = " > " : delimiter = " - "s;
			size_t pos = 0;
			while ((pos = bus_request.find(delimiter, pos)) != std::string::npos)
			{
				++delimiter_count;
				++pos;
			}
			if (route_type == RouteType::CircularRoute) {
				stops_on_route = delimiter_count + 1;
			}
			if (route_type == RouteType::SimpleRoute) {
				stops_on_route = delimiter_count * 2 + 1;
			}
			return stops_on_route;
		}

		// �������  �������� �� ��������� ����� ����������� �� ������ �������������� ���������.
		double ComputeGeoRouteDistance(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request) {
			double route_length = 0.0;
			std::string previous_stop_name;
			std::string current_stop_name;
			std::string delimiter;
			route_type == RouteType::CircularRoute ? delimiter = " > "s : delimiter = " - ";
			size_t pos_0 = 0;
			for (size_t pos = 0; pos < bus_request.size();) {
				previous_stop_name = std::move(current_stop_name);
				pos = bus_request.find(delimiter, pos_0);
				current_stop_name = bus_request.substr(pos_0, pos - pos_0);
				if (!previous_stop_name.empty()) {
					route_length += detail::ComputeDistanceBetweenStopsInRoute(transport_catalogue, previous_stop_name, current_stop_name);
					pos_0 = pos + delimiter.length();
				}
				else {
					pos_0 = pos + delimiter.length();
				}
			}
			if (route_type == RouteType::SimpleRoute) {
				route_length *= 2;
			}
			return route_length;
		}

		// ������� ������������ ��������(�����������) ���������� ����� ����������� �� ������ ������ ��
		// ������������� �����������. ��� �������� ����������: ��� �������� � ������ �� ���������� ��������.
		unsigned int ComputeRealRouteDisnance(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request) {
			unsigned int real_route_length = 0;
			std::string current_stop_name;
			std::string delimiter;
			std::string first_stop_name;
			std::string previous_stop_name;
			route_type == RouteType::CircularRoute ? delimiter = " > "s : delimiter = " - "s;
			for (; bus_request.find(delimiter) != std::string::npos;) {
				previous_stop_name = std::move(current_stop_name);
				current_stop_name = detail::GetToken(bus_request, delimiter);
				if (!previous_stop_name.empty()) {
					real_route_length += transport_catalogue.GetDistanceBetweenStops(previous_stop_name, current_stop_name);
					if (route_type == RouteType::SimpleRoute) {
						real_route_length += transport_catalogue.GetDistanceBetweenStops(current_stop_name, previous_stop_name);
					}
				}
				else {
					first_stop_name = current_stop_name;
				}
			}
			if (route_type == RouteType::CircularRoute) {
				real_route_length += transport_catalogue.GetDistanceBetweenStops(current_stop_name, first_stop_name);
			}
			else if (route_type == RouteType::SimpleRoute) {
				previous_stop_name = std::move(current_stop_name);
				current_stop_name = detail::GetToken(bus_request, delimiter);
				real_route_length += transport_catalogue.GetDistanceBetweenStops(previous_stop_name, current_stop_name) + transport_catalogue.GetDistanceBetweenStops(current_stop_name, previous_stop_name);
			}
			return real_route_length;
		}

		// ������� ���������� ����� ��������.
		std::string IdentifyBusNumber(std::string& bus_request) {
			return detail::GetToken(bus_request, ": ");
		}

		// ������� ���������� ��� ��������: ��������� ��� �������.
		RouteType IdentyfyRouteType(std::string& bus_request) {
			if (bus_request.find(" - "s) == std::string::npos) {
				return RouteType::CircularRoute;
			}
			else {
				return RouteType::SimpleRoute;
			}
		}

		// ������� ������������ ������� ������� �� ���������� � ������������ ������� ���������
		void InsertBusInCatalogue(TransportCatalogue& transport_catalogue, InputRequest& requests) {
			for (std::string bus_request : requests.GetBusRequests()) {
				// � ���������� bus_number ��������� �������� ��������
				std::string bus_number = IdentifyBusNumber(bus_request);
				// ������� ��������������� �������� - ��� �������� (������������ ��� �������� ���������� � ������ ���������� ��������� �� ��������).
				RouteType route_type = IdentyfyRouteType(bus_request);
				// ��������� ����� ���������� ��������� �� ��������
				size_t stops_on_route = CalculateStopsOnRoute(bus_request, route_type);
				// ������ ��������������� ��������� ���������� ��������� �� ��������.
				std::unordered_set<const Stop*> route = MakeUniqueStopsInRoute(transport_catalogue, route_type, bus_request);
				// ������������ ������ ��������, �� ������ �������������� ���������, ��� ����� ���������� ���������� ����� �����������.				
				double route_length = ComputeGeoRouteDistance(transport_catalogue, route_type, bus_request);
				// ������������ ����������� ����� ��������.
				unsigned int real_route_length = ComputeRealRouteDisnance(transport_catalogue, route_type, bus_request);
				// ��������� ������� � ������������ ����������.
				transport_catalogue.AddBus(bus_number, route_type, route.size(), stops_on_route, real_route_length, static_cast<double>(real_route_length) / route_length);

				// ��������� � ������� ������ ���������, ���������� ����� ���������.
				for (const auto& stop : route) {
					transport_catalogue.AddBusRelatedToStop(stop, bus_number);
				}
			}
		}

		// ������� ��������� � ����� transport_catalogue ���������� ����� �����������
		void MakeSetDistanceBetweenStops(TransportCatalogue& transport_catalogue, InputRequest& requests) {
			for (auto& stop_request : requests.GetStopRequests()) {
				std::string from_stop = detail::GetToken(stop_request, ": "s);
				// ��������� ��� ������ ����� ��� �������� �������������� ��������� �� �������.				
				detail::GetToken(stop_request, ", "s);
				detail::GetToken(stop_request, ", "s);
				while (stop_request.find("m "s) != std::string::npos) {
					unsigned int distance = std::stoul(detail::GetToken(stop_request, "m to "s));
					std::string to_stop = detail::GetToken(stop_request, ", "s);
					transport_catalogue.AddDistanceBetweenStops(from_stop, to_stop, distance);
				}
			}
		}

		// ������ ��������������� ��������� ���������� ���������.
		std::unordered_set<const Stop*> MakeUniqueStopsInRoute(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request) {
			std::unordered_set<const Stop*> route;
			std::string delimiter;
			size_t pos_0 = 0;
			route_type == RouteType::CircularRoute ? delimiter = " > "s : delimiter = " - "s;
			for (size_t pos = 0; pos < bus_request.size();) {
				pos = bus_request.find(delimiter, pos_0);
				std::string stop_name = bus_request.substr(pos_0, pos - pos_0);
				route.emplace(transport_catalogue.FindStop(stop_name));
				pos_0 = pos + delimiter.length();
			}
			return route;
		}

		void UpdateTransportCatalogue(TransportCatalogue& transport_catalogue, std::istream& in) {
			// ��������� �������� �� ���������� ���� ������. ������ ������ ������� � ������������� �������� � ��������� � ���� ���������,
			// ��������� ��������� ���������� ����� �����������, ��������� �������� � �� ��������� � ���� ������.						
			std::string line;
			// ��������� ���������� �������� � ����������� � �����, ��������� stoul
			std::getline(in, line);
			size_t number_of_requests = std::stoul(line);
			InputRequest requests;
			requests.MakeListRequest(transport_catalogue, number_of_requests, in);
			AddStopInCatalogue(transport_catalogue, requests);
			MakeSetDistanceBetweenStops(transport_catalogue, requests);
			InsertBusInCatalogue(transport_catalogue, requests);
		}		
	}		
}