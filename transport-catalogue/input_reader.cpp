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
			// Вспомогательная функция. Считывает посимвольно строку до границы, сохраняет считанное в переменную, затем стирает считанное из строки.
		// Возвращает считанную в переменную строку.
			std::string GetToken(std::string& line, const std::string& delimiter) {
				size_t pos = line.find(delimiter);
				std::string token = std::move(line.substr(0, pos));
				line.erase(0, pos == std::string::npos ? pos : pos + delimiter.length());
				return token;
			}

			// Вспомогательная функция для рассчёта расстояния между двумя остановками. Используем ComputeDistance из файла: geo.h.
			double ComputeDistanceBetweenStopsInRoute(const TransportCatalogue& transport_catalogue, const std::string& previous_stop_name, const std::string& current_stop_name) {
				return geo::ComputeDistance(transport_catalogue.FindStop(previous_stop_name)->coordinates, transport_catalogue.FindStop(current_stop_name)->coordinates);
			}
		}

		

		void UpdateTransportCatalogue(TransportCatalogue& transport_catalogue) {
			// Обработка запросов на обновление базы данных
			// 1. Обработка остановок
			//std::forward_list<std::pair<std::string, std::string>> stops_requests;
			// Очередь остановок, маршрутов - однонапрвленный список.
			std::forward_list<std::pair<std::string, std::string>> stops_requests;
			std::forward_list<std::string> bus_requests;
			std::string line;
			// Считываем количество запросов и преобразуем в число, используя stoul
			std::getline(std::cin, line);
			size_t number_of_requests = std::stoul(line);
			for (size_t i = 0; i < number_of_requests; ++i) {
				// Считываем тип запроса		
				std::getline(std::cin, line);
				std::string request_type = detail::GetToken(line, " "s);
				// Если тип - остановка, обрабатываем остановку
				if (request_type == "Stop"s) {
					// Формируем имя остановки
					std::string stop_name = detail::GetToken(line, ": "s);
					// Формируем координаты остановки 
					double latitude = std::stod(detail::GetToken(line, ", "s));
					double longitude = std::stod(detail::GetToken(line, ", "s));
					transport_catalogue.AddStop(stop_name, latitude, longitude);
					stops_requests.emplace_front(std::move(stop_name), std::move(line));
				}

				// Если тип - автобус, запмсываем запрос в очередь маршрутов
				if (request_type == "Bus"s) {
					bus_requests.emplace_front(std::move(line));
				}
			}

			// Обрабатываем расстояния между остановками.
			for (auto& stop_request : stops_requests) {
				while (stop_request.second.find("m "s) != std::string::npos) {
					unsigned int distance = std::stoul(detail::GetToken(stop_request.second, "m to "s));
					std::string to_stop = detail::GetToken(stop_request.second, ", "s);
					transport_catalogue.AddDistanceBetweenStops(stop_request.first, to_stop, distance);
				}
			}

			// 2. Обработка очереди запросов маршрутов. Проходим циклом по списку запросов о маршрутах.
			for (std::string bus_request : bus_requests) {
				// В переменную bus_number записывам название маршрута
				std::string bus_number = detail::GetToken(bus_request, ": "s);
				// Объявляем переменные тип маршрута, разграничитель маршрута и название первой остановки.
				RouteType route_type;
				std::string delimiter;
				std::string first_stop_name;
				// Если в запросе  находится знак ">", то тип маршрута - Циклический(Кольцевой), а если "-" то тип - простой.
				if (bus_request.find(" - "s) == std::string::npos) {
					route_type = RouteType::CircularRoute;
					delimiter = " > "s;
				}
				else {
					route_type = RouteType::SimpleRoute;
					delimiter = " - "s;
				}
				// Создаём unordered_set маршрута, в которую заносим указатели на остановки. 
				// Переменная route_length используется для расчёта длины маршрута (с использованием кратчайших расстояний между остановками).
				// Переменная real_route_length используется для рассчёта реальной (фактической) длины маршрута. 
				// Переменные previous_stop_name и current_stop_name используем для хранения имён предыдущей 
				// и текущей остановок. В переменной stops_on_route храним количество остановок на маршруте	
				std::unordered_set<const Stop*> route;
				double route_length = 0.0;
				unsigned int real_route_length = 0;
				std::string previous_stop_name;
				std::string current_stop_name;
				size_t stops_on_route = 0;
				// В цикле проходим по остановкам маршрута, добавляем уникальные остановки в множество route. 
				// Рассчитываем длину маршрута, используя функцию ComputeDistanceBetweenStopsInRoute.	
				for (; bus_request.find(delimiter) != std::string::npos; ++stops_on_route) {
					previous_stop_name = std::move(current_stop_name);
					current_stop_name = detail::GetToken(bus_request, delimiter);
					route.emplace(transport_catalogue.FindStop(current_stop_name));
					if (stops_on_route) {
						// Рассчитываем промежуточную длину маршрута и промежуточную фактическую длину маршрута.
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
				// Рассчитываем длину маршрута в зависимости от типа.
				// В круговом маршруте доавляем к длине маршрута расстояние между первой и последней остановками.
				if (route_type == RouteType::CircularRoute) {
					// На каждом шаге цикла увеличиваем счётчик остановок.
					++stops_on_route;
					route_length += detail::ComputeDistanceBetweenStopsInRoute(transport_catalogue, current_stop_name, first_stop_name);
					real_route_length += transport_catalogue.GetDistanceBetweenStops(current_stop_name, first_stop_name);
				}
				// На простом маршруте добавляем к длине маршрута расстоние между текущей и предыдущей остановками, 
				// после чего уваиваем полученный результат, так как автобус возвращается назад по тому же маршруту.	
				else if (route_type == RouteType::SimpleRoute) {
					stops_on_route = stops_on_route * 2 + 1;
					previous_stop_name = std::move(current_stop_name);
					current_stop_name = detail::GetToken(bus_request, delimiter);
					route.emplace(transport_catalogue.FindStop(current_stop_name));
					real_route_length += transport_catalogue.GetDistanceBetweenStops(previous_stop_name, current_stop_name) + transport_catalogue.GetDistanceBetweenStops(current_stop_name, previous_stop_name);
					route_length += detail::ComputeDistanceBetweenStopsInRoute(transport_catalogue, previous_stop_name, current_stop_name);
					route_length *= 2;
				}
				// Добавляем маршрут в транспортный каталог методом AddBus.
				transport_catalogue.AddBus(bus_number, route_type, route.size(), stops_on_route, real_route_length, static_cast<double>(real_route_length) / route_length);

				// Добавляем в словарь номера маршрутов, проходящих через остановки.
				for (const auto& stop : route) {
					transport_catalogue.AddBusRelatedToStop(stop, bus_number);
				}
			}
		}
	}	
}