//Черепухин Евгений Сергеевич. Сплит 9 Версия 2. Исправлены замечания.
#pragma once

#include "geo.h"

#include <string>
#include <deque>
#include <unordered_map>
#include <tuple>
#include <set>
#include <utility>


namespace transport_catalogue 
{
	// Задаём перечислимый тип маршрута. 
	// CircularRoute - кольцевой, SimpleRoute - обычный.
	enum class RouteType
	{
		CircularRoute,
		SimpleRoute
	};

	// Структура маршрута. Содержит: Имя, Тип маршрута, Кол-во уникальных остановок,
	// Общее кол-во остановок, Реальную(фактическую) длину маршрута, 
	// Кривизну маршрута(отношение Реальной длины маршрута к Идеальной длине маршрута (расстояние между соседними остановками
	// прямая, рассчитанная по географическим координатам)).
	struct Bus
	{
		std::string name;
		RouteType route_type;
		size_t unique_stops;
		size_t stops_on_route;
		unsigned int real_route_length;
		double route_curvature;
	};

	// Остановка - содержит географические координаты (широту и долготу).
	struct Stop
	{
		geo::Coordinates coordinates;
	};

	namespace detail 
	{
		// Структура компоратор необходима для сравнения маршрутов
		struct CmpBuses
		{
		public:
			bool operator() (const Bus* lhs, const Bus* rhs) const
			{
				return lhs->name < rhs->name;
			}
		};

		// Структура для рассчёта хешера пар. Такой рассчет найден на просторах
		// интернета, с помощью поисковой системы "Яндекс", как наиболее подходящий 
		// для рассчёта хеша пар.
		struct PairPtrHash
		{
			template<class T1, class T2>
			std::size_t operator()(const std::pair<T1, T2>& p) const {
				auto h1 = std::hash<const void*>{}(p.first);
				auto h2 = std::hash<const void*>{}(p.second);
				std::size_t seed = 0;
				seed ^= h1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				return seed;
			}
		};
	}

	

	class TransportCatalogue
	{
	public:
		// Добавить автобусный маршрут
		void AddBus(std::string bus_number, RouteType route_type, size_t unique_stops, size_t stops_on_route, unsigned int route_length, double route_curvature);

		// Добавить остановку
		void AddStop(std::string name, double latitude, double longitude);

		// Добавить автобус проходящий через остановку
		void AddBusRelatedToStop(const Stop* stop_ptr, const std::string& bus_number);

		// Добавить дистанцию между остановками.
		void AddDistanceBetweenStops(const std::string& from_stop, const std::string& to_stop, unsigned int distance);

		// Дать дистанцию между остановками.
		unsigned int GetDistanceBetweenStops(const std::string& from_stop, const std::string& to_stop);

		// Найти маршрут по имени
		const Bus* FindBus(const std::string& name) const;

		// Найти остановку по имени
		const Stop* FindStop(const std::string& name) const;

		//  Дать информацию о маршруте
		std::tuple<size_t, size_t, unsigned int, double>GetRouteInfo(const Bus* ptr) const;

		// Дать множество автобусов проходящих через остановку
		const std::set < const Bus*, detail::CmpBuses>* GetBusesRelatedToStop(const Stop* ptr) const;

	private:
		// Очередь остановок
		std::deque<Stop> stops_;

		// Словарь имя остановки - указатель на остановку в дэке
		std::unordered_map<std::string, const Stop*> stopname_to_stop_;

		// Очередь маршрутов
		std::deque<Bus> buses_;

		// Словарь маршрут - указатель на маршрут в дэке
		std::unordered_map<std::string, const Bus*> busname_to_bus_;

		// Словарь остановка - множество автобусов, которые останавливаются на остановке
		std::unordered_map<const Stop*, std::set<const Bus*, detail::CmpBuses>> buses_to_stop;

		// Словарь в котором ключи - это пары остановок, значение - расстояние между ними.
		std::unordered_map<std::pair<const Stop*, const Stop*>, unsigned int, detail::PairPtrHash> stops_distances;
	};
}