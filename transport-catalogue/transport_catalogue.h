//��������� ������� ���������. ����� 9 ������ 2. ���������� ���������.
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
	// ����� ������������ ��� ��������. 
	// CircularRoute - ���������, SimpleRoute - �������.
	enum class RouteType
	{
		CircularRoute,
		SimpleRoute
	};

	// ��������� ��������. ��������: ���, ��� ��������, ���-�� ���������� ���������,
	// ����� ���-�� ���������, ��������(�����������) ����� ��������, 
	// �������� ��������(��������� �������� ����� �������� � ��������� ����� �������� (���������� ����� ��������� �����������
	// ������, ������������ �� �������������� �����������)).
	struct Bus
	{
		std::string name;
		RouteType route_type;
		size_t unique_stops;
		size_t stops_on_route;
		unsigned int real_route_length;
		double route_curvature;
	};

	// ��������� - �������� �������������� ���������� (������ � �������).
	struct Stop
	{
		geo::Coordinates coordinates;
	};

	namespace detail 
	{
		// ��������� ���������� ���������� ��� ��������� ���������
		struct CmpBuses
		{
		public:
			bool operator() (const Bus* lhs, const Bus* rhs) const
			{
				return lhs->name < rhs->name;
			}
		};

		// ��������� ��� �������� ������ ���. ����� ������� ������ �� ���������
		// ���������, � ������� ��������� ������� "������", ��� �������� ���������� 
		// ��� �������� ���� ���.
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
		// �������� ���������� �������
		void AddBus(std::string bus_number, RouteType route_type, size_t unique_stops, size_t stops_on_route, unsigned int route_length, double route_curvature);

		// �������� ���������
		void AddStop(std::string name, double latitude, double longitude);

		// �������� ������� ���������� ����� ���������
		void AddBusRelatedToStop(const Stop* stop_ptr, const std::string& bus_number);

		// �������� ��������� ����� �����������.
		void AddDistanceBetweenStops(const std::string& from_stop, const std::string& to_stop, unsigned int distance);

		// ���� ��������� ����� �����������.
		unsigned int GetDistanceBetweenStops(const std::string& from_stop, const std::string& to_stop);

		// ����� ������� �� �����
		const Bus* FindBus(const std::string& name) const;

		// ����� ��������� �� �����
		const Stop* FindStop(const std::string& name) const;

		//  ���� ���������� � ��������
		std::tuple<size_t, size_t, unsigned int, double>GetRouteInfo(const Bus* ptr) const;

		// ���� ��������� ��������� ���������� ����� ���������
		const std::set < const Bus*, detail::CmpBuses>* GetBusesRelatedToStop(const Stop* ptr) const;

	private:
		// ������� ���������
		std::deque<Stop> stops_;

		// ������� ��� ��������� - ��������� �� ��������� � ����
		std::unordered_map<std::string, const Stop*> stopname_to_stop_;

		// ������� ���������
		std::deque<Bus> buses_;

		// ������� ������� - ��������� �� ������� � ����
		std::unordered_map<std::string, const Bus*> busname_to_bus_;

		// ������� ��������� - ��������� ���������, ������� ��������������� �� ���������
		std::unordered_map<const Stop*, std::set<const Bus*, detail::CmpBuses>> buses_to_stop;

		// ������� � ������� ����� - ��� ���� ���������, �������� - ���������� ����� ����.
		std::unordered_map<std::pair<const Stop*, const Stop*>, unsigned int, detail::PairPtrHash> stops_distances;
	};
}