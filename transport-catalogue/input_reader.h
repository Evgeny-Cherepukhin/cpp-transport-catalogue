//Черепухин Евгений Сергеевич. Сплит 9 Версия 3. Исправлены замечания.
#pragma once
#include "transport_catalogue.h"

#include <algorithm>
#include <forward_list>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>

namespace transport_catalogue 
{
	namespace input_requests_processing 
	{
		namespace detail 
		{
			// Вспомогательная функция для рачёта расстояния между остановками на маршруте.
			double ComputeDistanceBetweenStopsInRoute(const TransportCatalogue& transport_catalogue, const std::string& previous_stop_name, const std::string& corrent_stop_name);
			
			// Вспомогательная функция для парсинга запросов.
			std::string GetToken(std::string& line, const std::string& delimiter);			
		}

		// Класс предназначен для обработки входных запросов
		class  InputRequest
		{
		public:			

			// Метод возвращает список запросов на добавление автобусов 
			std::forward_list<std::string> GetBusRequests();
			
			// Метод возвращает список запросов на добавление остановок
			std::forward_list<std::string>GetStopRequests();

			// Метод создает список входящих запросов на добавление остановок и маршрутов. Метод также добавляет остаовки 
			// в транспортный справочник			
			InputRequest MakeListRequest(TransportCatalogue& transport_catalogue, size_t number_of_requests, std::istream& in);

		private:
			// Однонаправленные списки для хранения списков запросов на обновление справочника
			std::forward_list<std::string> stops_requests;
			std::forward_list<std::string> bus_requests;
		};

		// Добавление данных остановки в каталог.
		void AddStopInCatalogue(TransportCatalogue& transport_catalogue, InputRequest& requests);

		// Рассчёт количества остановок на маршруте.
		size_t CalculateStopsOnRoute(std::string& bus_request, RouteType& route_type);

		// Рассчёт  маршрута по дистанции между остановками на основе географических координат.
		double ComputeGeoRouteDistance(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request);

		// Функция рассчитывает реальное(фактическое) расстояние между остановками.		
		unsigned int ComputeRealRouteDisnance(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request);

		// Определить номер маршрута (выделяет номер маршрута из запроса).
		std::string IdentifyBusNumber(std::string& bus_request);

		// Оределить тип маршрута.
		RouteType IdentyfyRouteType(std::string& bus_request);

		// Вставляет маршрута в каталог.
		void InsertBusInCatalogue(TransportCatalogue& transport_catalogue, InputRequest& requests);

		// Создаёт множества расстояний между остановками.
		void MakeSetDistanceBetweenStops(TransportCatalogue& transport_catalogue, InputRequest& requests);
		
		// Создаёт неупорядоченное множество уникальных остановок.
		std::unordered_set<const Stop*> MakeUniqueStopsInRoute(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request);

		// Функция обновления транспортного справочника - обрабатывает запросы на доавление остановок и маршрутов.
		void UpdateTransportCatalogue(TransportCatalogue& transport_catalogue, std::istream& in = std::cin);	
	}	
}