//��������� ������� ���������. ����� 9 ������ 3. ���������� ���������.
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
			// ��������������� ������� ��� ������ ���������� ����� ����������� �� ��������.
			double ComputeDistanceBetweenStopsInRoute(const TransportCatalogue& transport_catalogue, const std::string& previous_stop_name, const std::string& corrent_stop_name);
			
			// ��������������� ������� ��� �������� ��������.
			std::string GetToken(std::string& line, const std::string& delimiter);			
		}

		// ����� ������������ ��� ��������� ������� ��������
		class  InputRequest
		{
		public:			

			// ����� ���������� ������ �������� �� ���������� ��������� 
			std::forward_list<std::string> GetBusRequests();
			
			// ����� ���������� ������ �������� �� ���������� ���������
			std::forward_list<std::string>GetStopRequests();

			// ����� ������� ������ �������� �������� �� ���������� ��������� � ���������. ����� ����� ��������� �������� 
			// � ������������ ����������			
			InputRequest MakeListRequest(TransportCatalogue& transport_catalogue, size_t number_of_requests, std::istream& in);

		private:
			// ���������������� ������ ��� �������� ������� �������� �� ���������� �����������
			std::forward_list<std::string> stops_requests;
			std::forward_list<std::string> bus_requests;
		};

		// ���������� ������ ��������� � �������.
		void AddStopInCatalogue(TransportCatalogue& transport_catalogue, InputRequest& requests);

		// ������� ���������� ��������� �� ��������.
		size_t CalculateStopsOnRoute(std::string& bus_request, RouteType& route_type);

		// �������  �������� �� ��������� ����� ����������� �� ������ �������������� ���������.
		double ComputeGeoRouteDistance(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request);

		// ������� ������������ ��������(�����������) ���������� ����� �����������.		
		unsigned int ComputeRealRouteDisnance(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request);

		// ���������� ����� �������� (�������� ����� �������� �� �������).
		std::string IdentifyBusNumber(std::string& bus_request);

		// ��������� ��� ��������.
		RouteType IdentyfyRouteType(std::string& bus_request);

		// ��������� �������� � �������.
		void InsertBusInCatalogue(TransportCatalogue& transport_catalogue, InputRequest& requests);

		// ������ ��������� ���������� ����� �����������.
		void MakeSetDistanceBetweenStops(TransportCatalogue& transport_catalogue, InputRequest& requests);
		
		// ������ ��������������� ��������� ���������� ���������.
		std::unordered_set<const Stop*> MakeUniqueStopsInRoute(TransportCatalogue& transport_catalogue, RouteType& route_type, std::string& bus_request);

		// ������� ���������� ������������� ����������� - ������������ ������� �� ��������� ��������� � ���������.
		void UpdateTransportCatalogue(TransportCatalogue& transport_catalogue, std::istream& in = std::cin);	
	}	
}