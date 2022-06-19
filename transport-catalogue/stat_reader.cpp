//Черепухин Евгений Сергеевич. Сплит 9 Версия 3. Исправлены замечания.
#include "stat_reader.h"

namespace transport_catalogue 
{
	namespace output_requests_processing 
	{
		using namespace std::string_literals;		
		
		// Метод класса OutputRequests возвращает список запросов о маршрутах.
		std::list<std::string> OutputRequests::GetBusRequests() {
			return list_of_buses_requests;
		}

		// Метод класса OutputRequests возвращает список запросов об остановках.
		std::list<std::string> OutputRequests::GetStopRequests() {
			return list_of_stops_requests;
		}

		// Метод класса OutputRequests создаёт списки запросов остановок и маршрутов к справочнику.
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
		
		// Функция выводит ответ на запросы к справочнику о маршрутах. 
		void ResponseBusRequests(const TransportCatalogue& transport_catalogue, OutputRequests& requests) {
			for (std::string request : requests.GetBusRequests()) {
				const Bus* bus = transport_catalogue.FindBus(request);
				if (bus != nullptr) {
					auto [unique_stops, stops_on_route, route_length, curvature] = transport_catalogue.GetRouteInfo(bus);
					std::cout << std::setprecision(6) << "Bus " << request << ": " << stops_on_route << " stops on route, " << unique_stops << " unique stops, " << route_length << " route length, " << curvature << " curvature" << std::endl;
				}
				// Если такого маршрута нет, выводим в поток "Bus X: not found" 
				else {
					std::cout << "Bus " << request << ": not found" << std::endl;
				}
			}
		}

		//Функция выводит ответ на запросы к справочнику об остановках.
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
			// Считываем число запросов к транспортному справочнику 
			std::string line;
			std::getline(in, line);
			size_t number_of_requests = std::stoul(line);
			// Создаём список запросов к справочнику.
			OutputRequests requests;
			requests.MakeOutputRequests(number_of_requests, in);
			// Формируем ответы на запросы.
			ResponseBusRequests(transport_catalogue, requests);
			ResponseStopRequests(transport_catalogue, requests);
		}
	}
}