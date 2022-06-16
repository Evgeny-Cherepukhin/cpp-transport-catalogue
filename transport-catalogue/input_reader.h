#pragma once
#include "transport_catalogue.h"

#include <string>

namespace transport_catalogue 
{
	namespace input_requests_processing 
	{
		namespace detail 
		{
			// Вспомогательная функция для парсинга запросов.
			std::string GetToken(std::string& line, const std::string& delimiter);

			// Вспомогательная функция для рачёта расстояния между остановками на маршруте.
			double ComputeDistanceBetweenStopsInRoute(const TransportCatalogue& transport_catalogue, const std::string& previous_stop_name, const std::string& corrent_stop_name);
		}
		// Функция обновления транспортного справочника - обрабатывает запросы на доавление остановок и маршрутов.
		void UpdateTransportCatalogue(TransportCatalogue& transport_catalogue);
	}	
}