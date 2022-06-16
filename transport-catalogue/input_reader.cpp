#include "input_reader.h"
#include "geo.h"

#include <iostream>
#include <unordered_set>
#include <forward_list>

namespace transport_catalogue 
{
	namespace input_requests_processing 
	{
		using namespace std::string_literals;

		namespace detail 
		{
			// ��������������� �������. ��������� ����������� ������ �� �������, ��������� ��������� � ����������, ����� ������� ��������� �� ������.
		// ���������� ��������� � ���������� ������.
			std::string GetToken(std::string& line, const std::string& delimiter) {
				size_t pos = line.find(delimiter);
				std::string token = std::move(line.substr(0, pos));
				line.erase(0, pos == std::string::npos ? pos : pos + delimiter.length());
				return token;
			}

			// ��������������� ������� ��� �������� ���������� ����� ����� �����������. ���������� ComputeDistance �� �����: geo.h.
			double ComputeDistanceBetweenStopsInRoute(const TransportCatalogue& transport_catalogue, const std::string& previous_stop_name, const std::string& current_stop_name) {
				return geo::ComputeDistance(transport_catalogue.FindStop(previous_stop_name)->coordinates, transport_catalogue.FindStop(current_stop_name)->coordinates);
			}
		}

		

		void UpdateTransportCatalogue(TransportCatalogue& transport_catalogue) {
			// ��������� �������� �� ���������� ���� ������
			// 1. ��������� ���������
			//std::forward_list<std::pair<std::string, std::string>> stops_requests;
			// ������� ���������, ��������� - ��������������� ������.
			std::forward_list<std::pair<std::string, std::string>> stops_requests;
			std::forward_list<std::string> bus_requests;
			std::string line;
			// ��������� ���������� �������� � ����������� � �����, ��������� stoul
			std::getline(std::cin, line);
			size_t number_of_requests = std::stoul(line);
			for (size_t i = 0; i < number_of_requests; ++i) {
				// ��������� ��� �������		
				std::getline(std::cin, line);
				std::string request_type = detail::GetToken(line, " "s);
				// ���� ��� - ���������, ������������ ���������
				if (request_type == "Stop"s) {
					// ��������� ��� ���������
					std::string stop_name = detail::GetToken(line, ": "s);
					// ��������� ���������� ��������� 
					double latitude = std::stod(detail::GetToken(line, ", "s));
					double longitude = std::stod(detail::GetToken(line, ", "s));
					transport_catalogue.AddStop(stop_name, latitude, longitude);
					stops_requests.emplace_front(std::move(stop_name), std::move(line));
				}

				// ���� ��� - �������, ���������� ������ � ������� ���������
				if (request_type == "Bus"s) {
					bus_requests.emplace_front(std::move(line));
				}
			}

			// ������������ ���������� ����� �����������.
			for (auto& stop_request : stops_requests) {
				while (stop_request.second.find("m "s) != std::string::npos) {
					unsigned int distance = std::stoul(detail::GetToken(stop_request.second, "m to "s));
					std::string to_stop = detail::GetToken(stop_request.second, ", "s);
					transport_catalogue.AddDistanceBetweenStops(stop_request.first, to_stop, distance);
				}
			}

			// 2. ��������� ������� �������� ���������. �������� ������ �� ������ �������� � ���������.
			for (std::string bus_request : bus_requests) {
				// � ���������� bus_number ��������� �������� ��������
				std::string bus_number = detail::GetToken(bus_request, ": "s);
				// ��������� ���������� ��� ��������, �������������� �������� � �������� ������ ���������.
				RouteType route_type;
				std::string delimiter;
				std::string first_stop_name;
				// ���� � �������  ��������� ���� ">", �� ��� �������� - �����������(���������), � ���� "-" �� ��� - �������.
				if (bus_request.find(" - "s) == std::string::npos) {
					route_type = RouteType::CircularRoute;
					delimiter = " > "s;
				}
				else {
					route_type = RouteType::SimpleRoute;
					delimiter = " - "s;
				}
				// ������ unordered_set ��������, � ������� ������� ��������� �� ���������. 
				// ���������� route_length ������������ ��� ������� ����� �������� (� �������������� ���������� ���������� ����� �����������).
				// ���������� real_route_length ������������ ��� �������� �������� (�����������) ����� ��������. 
				// ���������� previous_stop_name � current_stop_name ���������� ��� �������� ��� ���������� 
				// � ������� ���������. � ���������� stops_on_route ������ ���������� ��������� �� ��������	
				std::unordered_set<const Stop*> route;
				double route_length = 0.0;
				unsigned int real_route_length = 0;
				std::string previous_stop_name;
				std::string current_stop_name;
				size_t stops_on_route = 0;
				// � ����� �������� �� ���������� ��������, ��������� ���������� ��������� � ��������� route. 
				// ������������ ����� ��������, ��������� ������� ComputeDistanceBetweenStopsInRoute.	
				for (; bus_request.find(delimiter) != std::string::npos; ++stops_on_route) {
					previous_stop_name = std::move(current_stop_name);
					current_stop_name = detail::GetToken(bus_request, delimiter);
					route.emplace(transport_catalogue.FindStop(current_stop_name));
					if (stops_on_route) {
						// ������������ ������������� ����� �������� � ������������� ����������� ����� ��������.
						route_length += detail::ComputeDistanceBetweenStopsInRoute(transport_catalogue, previous_stop_name, current_stop_name);
						real_route_length += transport_catalogue.GetDistanceBetweenStops(previous_stop_name, current_stop_name);
						if (route_type == RouteType::SimpleRoute) {
							real_route_length += transport_catalogue.GetDistanceBetweenStops(current_stop_name, previous_stop_name);
						}
					}
					else {
						first_stop_name = current_stop_name;
					}
				}
				// ������������ ����� �������� � ����������� �� ����.
				// � �������� �������� �������� � ����� �������� ���������� ����� ������ � ��������� �����������.
				if (route_type == RouteType::CircularRoute) {
					// �� ������ ���� ����� ����������� ������� ���������.
					++stops_on_route;
					route_length += detail::ComputeDistanceBetweenStopsInRoute(transport_catalogue, current_stop_name, first_stop_name);
					real_route_length += transport_catalogue.GetDistanceBetweenStops(current_stop_name, first_stop_name);
				}
				// �� ������� �������� ��������� � ����� �������� ��������� ����� ������� � ���������� �����������, 
				// ����� ���� �������� ���������� ���������, ��� ��� ������� ������������ ����� �� ���� �� ��������.	
				else if (route_type == RouteType::SimpleRoute) {
					stops_on_route = stops_on_route * 2 + 1;
					previous_stop_name = std::move(current_stop_name);
					current_stop_name = detail::GetToken(bus_request, delimiter);
					route.emplace(transport_catalogue.FindStop(current_stop_name));
					real_route_length += transport_catalogue.GetDistanceBetweenStops(previous_stop_name, current_stop_name) + transport_catalogue.GetDistanceBetweenStops(current_stop_name, previous_stop_name);
					route_length += detail::ComputeDistanceBetweenStopsInRoute(transport_catalogue, previous_stop_name, current_stop_name);
					route_length *= 2;
				}
				// ��������� ������� � ������������ ������� ������� AddBus.
				transport_catalogue.AddBus(bus_number, route_type, route.size(), stops_on_route, real_route_length, static_cast<double>(real_route_length) / route_length);

				// ��������� � ������� ������ ���������, ���������� ����� ���������.
				for (const auto& stop : route) {
					transport_catalogue.AddBusRelatedToStop(stop, bus_number);
				}
			}
		}
	}	
}