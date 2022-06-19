//��������� ������� ���������. ����� 9 ������ 3. ���������� ���������.
#include "stat_reader.h"

namespace transport_catalogue 
{
	namespace output_requests_processing 
	{
		using namespace std::string_literals;		
		
		// ����� ������ OutputRequests ���������� ������ �������� � ���������.
		std::list<std::string> OutputRequests::GetBusRequests() {
			return list_of_buses_requests;
		}

		// ����� ������ OutputRequests ���������� ������ �������� �� ����������.
		std::list<std::string> OutputRequests::GetStopRequests() {
			return list_of_stops_requests;
		}

		// ����� ������ OutputRequests ������ ������ �������� ��������� � ��������� � �����������.
		OutputRequests OutputRequests::MakeOutputRequests(size_t number_of_requests, std::istream& in) {
			std::string line;
			for (size_t i = 0; i < number_of_requests; ++i) {
				std::getline(in, line);
				std::string delimiter = " "s;
				size_t pos = line.find(delimiter);
				std::string request_name = line.substr(0, pos);
				std::string request_param = line.substr(pos + delimiter.length());
				if (request_name == "Bus"s) {
					list_of_buses_requests.emplace_back(request_param);
				}
				if (request_name == "Stop"s) {
					list_of_stops_requests.emplace_back(request_param);
				}
			}
			return *this;
		}
		
		// ������� ������� ����� �� ������� � ����������� � ���������. 
		void ResponseBusRequests(const TransportCatalogue& transport_catalogue, OutputRequests& requests) {
			for (std::string request : requests.GetBusRequests()) {
				const Bus* bus = transport_catalogue.FindBus(request);
				if (bus != nullptr) {
					auto [unique_stops, stops_on_route, route_length, curvature] = transport_catalogue.GetRouteInfo(bus);
					std::cout << std::setprecision(6) << "Bus " << request << ": " << stops_on_route << " stops on route, " << unique_stops << " unique stops, " << route_length << " route length, " << curvature << " curvature" << std::endl;
				}
				// ���� ������ �������� ���, ������� � ����� "Bus X: not found" 
				else {
					std::cout << "Bus " << request << ": not found" << std::endl;
				}
			}
		}

		//������� ������� ����� �� ������� � ����������� �� ����������.
		void ResponseStopRequests(const TransportCatalogue& transport_catalogue, OutputRequests& requests) {
			for (std::string request : requests.GetStopRequests()) {
				const Stop* stop = transport_catalogue.FindStop(request);
				if (stop != nullptr) {
					auto buses_by_stop = transport_catalogue.GetBusesRelatedToStop(stop);
					if (buses_by_stop != nullptr) {
						std::cout << "Stop " << request << ": buses";
						for (const auto& bus : *buses_by_stop) {
							std::cout << " " << bus->name;
						}
						std::cout << std::endl;
					}
					else {
						std::cout << "Stop " << request << ": no buses" << std::endl;
					}
				}
				else {
					std::cout << "Stop " << request << ": not found" << std::endl;
				}
			}
		}

		void TransportCatalogueRequests(const TransportCatalogue& transport_catalogue, std::istream& in) {
			// ��������� ����� �������� � ������������� ����������� 
			std::string line;
			std::getline(in, line);
			size_t number_of_requests = std::stoul(line);
			// ������ ������ �������� � �����������.
			OutputRequests requests;
			requests.MakeOutputRequests(number_of_requests, in);
			// ��������� ������ �� �������.
			ResponseBusRequests(transport_catalogue, requests);
			ResponseStopRequests(transport_catalogue, requests);
		}
	}
}