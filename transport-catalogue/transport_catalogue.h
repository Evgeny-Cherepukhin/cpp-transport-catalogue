// Черепухин Евгений Сергеевич. Спринт 12 Версия 1.
#pragma once

#include "domain.h"
#include "geo.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <string>

namespace transport::catalogue
{
	using namespace transport::domains;
	using namespace transport::render;
	using namespace transport::router;

	class TransportCatalogue
	{
	public:
		TransportCatalogue() = default;

		// Конструктор транспортного каталога. Заполняет словари остановок и маршрутов по соответствующим векторам.
		TransportCatalogue(
			std::vector<std::shared_ptr<Stop>>& stops,
			std::vector<std::shared_ptr<Bus>>& buses,
			RenderSettings render_settings,
			RoutingSettings router_settings
		);
		// Деструктор.
		~TransportCatalogue() {}

		// Возвращает информацию об остановке.
		std::optional<Stop> GetStopInfo(std::string_view stop_name);

		// Возвращает нформацию о маршрутах.
		std::optional<Bus> GetBusInfo(std::string_view bus_name);

		// Возвращает указатель на остановку по имени.
		std::shared_ptr<Stop> StopByName(std::string_view name);

		// Возвращает указатель на маршрут по имени.
		std::shared_ptr<Bus> BusByName(std::string_view name);

		// Возвращает ссылку на информацию на построение карты.
		const std::string& GetMap();

		std::shared_ptr<std::vector<RouteItem>> FindRouteInBase(std::string_view from, std::string_view to);


	private:
		// Заполняет словарь остановок. 
		void MakeStopsMap(std::vector<std::shared_ptr<Stop>>& stops);

		// Заполняет словарь маршрутов.
		void MakeBusesMap(std::vector<std::shared_ptr<Bus>>& buses);

		// Рисует карту маршрутов.
		void MakeRenderMap(std::map<std::string_view, std::shared_ptr<Stop>>& stops, std::map<std::string_view, std::shared_ptr<Bus>>& buses, RenderSettings& render_settings);

		// Маршрутизатор составляет маршрут между остановками.
		void MakeRouter(std::map<std::string_view, std::shared_ptr<Stop>>& stops, std::map<std::string_view, std::shared_ptr<Bus>>& buses, RoutingSettings router_settings);

		// Считает количество уникальных остановок маршрута.
		void ComputeUniqueStops(std::shared_ptr<Bus>& bus);

		// Считает реальную дистанцию маршрута.
		void ComputeRealRouteDistance(std::shared_ptr<Bus>& bus);

		// Рассчитывает кривизну маршрута(отношение реальной длины маршрута к географическому расстоянию)
		void ComputeCurvature(std::shared_ptr<Bus>& bus);

		// Подсчёт общего количества остановок на маршруте
		void CalculateStopsOnRoute(std::shared_ptr<Bus>& bus);
		// Данные об остановках храним в словарях остановок и маршрутов. (Имя - указатель на данные).
		std::map<std::string_view, std::shared_ptr<Stop>> stops_;
		std::map<std::string_view, std::shared_ptr<Bus>> buses_;
		// Данные необходимые для отрисовки карты.
		std::unique_ptr<MapRenderer> render_;
		std::string map_;
		// Данные для построения маршрутов.
		std::unique_ptr<TransportRouter>router_;
	};
} // namespace transport::catalogue