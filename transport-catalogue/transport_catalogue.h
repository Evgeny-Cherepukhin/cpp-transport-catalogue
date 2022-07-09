// Черепухин Евгений Сергеевич. Сплит 10 Версия 2.
#pragma once

#include "domain.h"
#include "geo.h"
#include "map_renderer.h"

#include <optional>
#include <string>
#include <set>
#include <utility>

namespace transport::catalogue
{
	using namespace transport::domains;
	using namespace transport::render;	

	class TransportCatalogue
	{
	public:
		TransportCatalogue() = default;

	// Конструктор транспортного каталога. Заполняет словари остановок и маршрутов по соответствующим векторам.
		TransportCatalogue(
			std::vector<std::shared_ptr<Stop>>&stops,
			std::vector<std::shared_ptr<Bus>>&buses,
			RenderSettings render_settings
		);

		~TransportCatalogue() {}

		std::optional<Stop> GetStopInfo(std::string_view stop_name);

		std::optional<Bus> GetBusInfo(std::string_view bus_name);

		std::shared_ptr<Stop> StopByName(std::string_view name);

		std::shared_ptr<Bus> BusByName(std::string_view name);

		const std::string& GetMap();


	private:
		// Заполняет словарь остановок. 
		void MakeStopsMap(std::vector<std::shared_ptr<Stop>>& stops);

		// Заполняет словарь маршрутов.
		void MakeBusesMap(std::vector<std::shared_ptr<Bus>>& buses);

		// Рисует карту маршрутов.
		void MakeRenderMap(std::map<std::string_view, std::shared_ptr<Stop>>& stops, std::map<std::string_view, std::shared_ptr<Bus>>& buses, RenderSettings& render_settings);

		// Считает количество уникальных остановок маршрута.
		void ComputeUniqueStops(std::shared_ptr<Bus>& bus);

		void ComputeRealRouteDistance(std::shared_ptr<Bus>& bus);

		void ComputeCurvature(std::shared_ptr<Bus>& bus);

		void CalculateStopsOnRoute(std::shared_ptr<Bus>& bus);

		std::map<std::string_view, std::shared_ptr<Stop>> stops_;
		std::map<std::string_view, std::shared_ptr<Bus>> buses_;
		std::unique_ptr<MapRenderer> render_;
		std::string map_;
	};
} // namespace transport::catalogue