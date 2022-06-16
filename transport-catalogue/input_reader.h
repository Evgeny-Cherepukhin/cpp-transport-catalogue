#pragma once
#include "transport_catalogue.h"

#include <string>

namespace transport_catalogue 
{
	namespace input_requests_processing 
	{
		namespace detail 
		{
			// ��������������� ������� ��� �������� ��������.
			std::string GetToken(std::string& line, const std::string& delimiter);

			// ��������������� ������� ��� ������ ���������� ����� ����������� �� ��������.
			double ComputeDistanceBetweenStopsInRoute(const TransportCatalogue& transport_catalogue, const std::string& previous_stop_name, const std::string& corrent_stop_name);
		}
		// ������� ���������� ������������� ����������� - ������������ ������� �� ��������� ��������� � ���������.
		void UpdateTransportCatalogue(TransportCatalogue& transport_catalogue);
	}	
}