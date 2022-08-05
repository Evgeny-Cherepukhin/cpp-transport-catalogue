// Черепухин Евгений Сергеевич. Спринт 12 Версия 1.
#pragma once

#include "json.h"
#include "geo.h"
#include "domain.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <string>
#include <vector>

namespace json::reader {
	using namespace transport::domains;
	using namespace transport::render;
	using namespace transport::router;

	//Класс для чтения входных запросов на создание транспортного справочника
	class JsonReader
	{
	public:
		// Выполняет парсинг запросов и создает указатели на остановки.
		std::vector<std::shared_ptr<Stop>> ParseStop(const json::Array& base_requests);

		// Выполняет парсинг запросов на создание стравочника и создает указатели на маршруты.
		std::vector<std::shared_ptr<Bus>> ParseBus(const json::Array& base_requests);

		// Вспомогательная функция для парсинга цвета.
		svg::Color ParseColor(const json::Node& node);

		// Выполняет парсинг входных запросов и возвращает установки отрисовки карты.
		transport::render::RenderSettings ParseRenderSetting(const json::Dict& render_settings);

		// Выполняет парсинг входных запросов и возвращает установки маршрутизации 
		// (времяожидания автобуса на остановке и скорость автобуса в км/ч)
		RoutingSettings ParseRouterSetting(const json::Dict& router_settings);
	
	};
}