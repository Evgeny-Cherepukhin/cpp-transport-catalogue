//Черепухин Евгений Сергеевич. Сплит 9 Версия 3. Исправлены замечания.
#include "transport_catalogue.h"

namespace transport_catalogue 
{
	// Добавить маршрут. Добавляем маршрут в деку маршрутов, а также добавляем 
	// анордередмапу ссылку на маршрут в деке по номеру маршрута.
	void TransportCatalogue::AddBus(std::string bus_number, RouteType route_type, size_t unique_stops, size_t stops_on_route, unsigned int route_length, double route_curvature) {
		buses_.emplace_back(Bus{ bus_number, route_type, unique_stops, stops_on_route, route_length, route_curvature });
		busname_to_bus_.emplace(std::move(bus_number), &buses_.back());
	}

	// Добавить остановку. Добавляем остановку в деку остановок, а также добавляем 
	// анордередмапу ссылку на остановку в деке по имени остановки.
	void TransportCatalogue::AddStop(std::string name, double latitude, double longitude) {
		stops_.emplace_back(Stop{ latitude, longitude });
		stopname_to_stop_.emplace(std::move(name), &stops_.back());
	}

	// Добавить автобус, проходящий через остановку. Добавляем в unordered_map
	// автобус, проходящий через остановку.
	void TransportCatalogue::AddBusRelatedToStop(const Stop* stop_ptr, const std::string& bus_number) {
		buses_to_stop[stop_ptr].emplace(FindBus(bus_number));
	}

	// Найти маршрут по номеру. Находим в анордередмап маршрут по номеру,
	// если такой маршрут есть то возвращаем указатель на него в деке маршрутов,
	// если такого маршрута нет, то возвращаем nullptr.
	const Bus* TransportCatalogue::FindBus(const std::string& name)const {
		auto it = busname_to_bus_.find(name);
		return it == busname_to_bus_.end() ? nullptr : it->second;
	}

	// Метод находит остановку по имени. Возвращает указатель на остановку, если остановки нет в справочнике возвращает пустой указатель.
	const Stop* TransportCatalogue::FindStop(const std::string& name) const {
		auto it = stopname_to_stop_.find(name);
		return it == stopname_to_stop_.end() ? nullptr : it->second;
	}

	// Метод возвращает информацию о маршруте в формате: кол-во уникальных остановок, всего остановок на маршруте, фактическая длина маршрута,
	// кривизна маршрута.
	std::tuple<size_t, size_t, unsigned int, double>TransportCatalogue::GetRouteInfo(const Bus* ptr) const {
		return std::make_tuple(ptr->unique_stops, ptr->stops_on_route, ptr->real_route_length, ptr->route_curvature);
	}

	// Метод возвращает множество автобусов, которые останавливаются на данной остановке.
	const std::set < const Bus*, detail::CmpBuses>* TransportCatalogue::GetBusesRelatedToStop(const Stop* ptr) const {
		auto it = buses_to_stop.find(ptr);
		return it == buses_to_stop.end() ? nullptr : &it->second;
	}

	// Метод добавляет в словарь дистанцию между остановками. Ключём в словаре служит пара остановок.
	void TransportCatalogue::AddDistanceBetweenStops(const std::string& from_stop, const std::string& to_stop, unsigned int distance) {
		stops_distances.emplace(std::make_pair(FindStop(from_stop), FindStop(to_stop)), distance);
	}

	// Метод возвращает расстояние между двумя остановками. 
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