#include "stat_reader.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <list>

namespace transport_catalogue 
{
	namespace output_requests_processing 
	{
		using namespace std::string_literals;

		void TransportCatalogueRequests(const TransportCatalogue& transport_catalogue) {
			// ��������� ������, � ������� ����� ��������� ������� � ���������� 
			std::list<std::string> requests;
			// ��������� ����� �������� � ������������� ����������� 
			std::string line;
			std::getline(std::cin, line);
			size_t number_of_requests = std::stoul(line);
			// ��������� ������ ���������
			for (size_t i = 0; i < number_of_requests; ++i) {
				std::getline(std::cin, line);
				requests.emplace_back(std::move(line));
			}
			// ������������ ������� � ������� ������ � �������
			for (std::string& request : requests) {
				// ������������ ������� � ���������.
				// ������� � ������� ����� "Bus" � ����� �������� (���������� ������� �������)
				std::string delimiter = " "s;
				size_t pos = request.find(delimiter);
				std::string request_name = request.substr(0, pos);
				std::string request_param = request.substr(pos + delimiter.length());
				// ���� ������ "Bus" ������� ������� �� ������ ������� FindBus, ����� ��������� ������ � ������ ������� GetRouteInfo.
				// ���� ������� ����������, �� ������� � ����� ��������������� ������ �� �������.
				if (request_name == "Bus"s) {
					const Bus* bus = transport_catalogue.FindBus(request_param);
					if (bus != nullptr) {
						auto [unique_stops, stops_on_route, route_length, curvature] = transport_catalogue.GetRouteInfo(bus);
						std::cout << std::setprecision(6) << "Bus " << request_param << ": " << stops_on_route << " stops on route, " << unique_stops << " unique stops, " << route_length << " route length, " << curvature << " curvature" << std::endl;
					}
					// ���� ������ �������� ���, ������� � ����� "Bus X: not found" 
					else {
						std::cout << "Bus " << request_param << ": not found" << std::endl;
					}
				}
				// ������������ ������� �� ����������.
				else if (request_name == "Stop"s) {
					const Stop* stop = transport_catalogue.FindStop(request_param);
					if (stop != nullptr) {
						auto buses_by_stop = transport_catalogue.GetBusesRelatedToStop(stop);
						if (buses_by_stop != nullptr) {
							std::cout << "Stop " << request_param << ": buses";
							for (const auto& bus : *buses_by_stop) {
								std::cout << " " << bus->name;
							}
							std::cout << std::endl;
						}
						else {
							std::cout << "Stop " << request_param << ": no buses" << std::endl;
						}
					}
					else {
						std::cout << "Stop " << request_param << ": not found" << std::endl;
					}
				}
			}
		}
	}
}