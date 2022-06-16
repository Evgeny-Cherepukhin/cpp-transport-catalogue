#pragma once

#include "transport_catalogue.h"

namespace transport_catalogue 
{
	namespace output_requests_processing 
	{
		// Функция обрабатывает запросы к транспортному справочнику и формирует вывод (ответ на запросы к справочнику).
		void TransportCatalogueRequests(const TransportCatalogue& transportCatalogue);
	}
}