//Черепухин Евгений Сергеевич. Сплит 9 Версия 2. Исправлены замечания.
#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transport_catalogue 
{
	namespace output_requests_processing 
	{
		// Класс предназначен для создания очередей запросов к базе данных.
		class OutputRequests 
		{			
		public:
			OutputRequests MakeOutputRequests(size_t number_of_requests, std::istream& in);

			std::list<std::string> GetBusRequests();

			std::list<std::string> GetStopRequests();

		private:
			std::list<std::string> list_of_buses_requests;
			std::list<std::string> list_of_stops_requests;
		};
		
		// Ответ на запрсы о маршрутах.
		void ResponseBusRequests(const TransportCatalogue& transport_catalogue, OutputRequests& requests);
		
		// Ответ на запрсы об остановках.
		void ResponseStopRequests(const TransportCatalogue& transport_catalogue, OutputRequests& requests);
		
		// Функция обрабатывает запросы к транспортному справочнику и формирует вывод (ответ на запросы к справочнику).
		void TransportCatalogueRequests(const TransportCatalogue& transportCatalogue, std::istream& in = std::cin);



	}
}