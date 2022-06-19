//��������� ������� ���������. ����� 9 ������ 2. ���������� ���������.
#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transport_catalogue 
{
	namespace output_requests_processing 
	{
		// ����� ������������ ��� �������� �������� �������� � ���� ������.
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
		
		// ����� �� ������ � ���������.
		void ResponseBusRequests(const TransportCatalogue& transport_catalogue, OutputRequests& requests);
		
		// ����� �� ������ �� ����������.
		void ResponseStopRequests(const TransportCatalogue& transport_catalogue, OutputRequests& requests);
		
		// ������� ������������ ������� � ������������� ����������� � ��������� ����� (����� �� ������� � �����������).
		void TransportCatalogueRequests(const TransportCatalogue& transportCatalogue, std::istream& in = std::cin);



	}
}