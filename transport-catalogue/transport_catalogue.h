// ��������� ������� ���������. ����� 11 ������ 1.
#pragma once

#include "domain.h"
#include "geo.h"
#include "map_renderer.h"

#include <string>

namespace transport::catalogue
{
	using namespace transport::domains;
	using namespace transport::render;

	class TransportCatalogue
	{
	public:
		TransportCatalogue() = default;

		// ����������� ������������� ��������. ��������� ������� ��������� � ��������� �� ��������������� ��������.
		TransportCatalogue(
			std::vector<std::shared_ptr<Stop>>& stops,
			std::vector<std::shared_ptr<Bus>>& buses,
			RenderSettings render_settings
		);
		// ����������.
		~TransportCatalogue() {}

		// ���������� ���������� �� ���������.
		std::optional<Stop> GetStopInfo(std::string_view stop_name);

		// ���������� ��������� � ���������.
		std::optional<Bus> GetBusInfo(std::string_view bus_name);

		// ���������� ��������� �� ��������� �� �����.
		std::shared_ptr<Stop> StopByName(std::string_view name);

		// ���������� ��������� �� ������� �� �����.
		std::shared_ptr<Bus> BusByName(std::string_view name);

		// ���������� ������ �� ���������� �� ���������� �����.
		const std::string& GetMap();


	private:
		// ��������� ������� ���������. 
		void MakeStopsMap(std::vector<std::shared_ptr<Stop>>& stops);

		// ��������� ������� ���������.
		void MakeBusesMap(std::vector<std::shared_ptr<Bus>>& buses);

		// ������ ����� ���������.
		void MakeRenderMap(std::map<std::string_view, std::shared_ptr<Stop>>& stops, std::map<std::string_view, std::shared_ptr<Bus>>& buses, RenderSettings& render_settings);

		// ������� ���������� ���������� ��������� ��������.
		void ComputeUniqueStops(std::shared_ptr<Bus>& bus);

		// ������� �������� ��������� ��������.
		void ComputeRealRouteDistance(std::shared_ptr<Bus>& bus);

		// ������������ �������� ��������(��������� �������� ����� �������� � ��������������� ����������)
		void ComputeCurvature(std::shared_ptr<Bus>& bus);

		// ������� ������ ���������� ��������� �� ��������
		void CalculateStopsOnRoute(std::shared_ptr<Bus>& bus);
		// ������ �� ���������� ������ � �������� ��������� � ���������. (��� - ��������� �� ������).
		std::map<std::string_view, std::shared_ptr<Stop>> stops_;
		std::map<std::string_view, std::shared_ptr<Bus>> buses_;
		// ������ ����������� ��� ��������� �����.
		std::unique_ptr<MapRenderer> render_;
		std::string map_;
	};
} // namespace transport::catalogue