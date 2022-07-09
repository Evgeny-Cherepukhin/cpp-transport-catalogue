// Черепухин Евгений Сергеевич. Сплит 10 Версия 2.
#pragma once

#include "json.h"
#include "geo.h"
#include "domain.h"
#include "map_renderer.h"

#include <exception>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace json::reader {
	using namespace transport::domains;
	using namespace transport::render;

	class JsonReader
	{
	public:
		std::vector<std::shared_ptr<Stop>> ParseStop(const json::Array& base_requests);

		std::vector<std::shared_ptr<Bus>> ParseBus(const json::Array& base_requests);

		svg::Color ParseColor(const json::Node& node);

		transport::render::RenderSettings ParseRenderSetting(const json::Dict& render_settings);
	};	
}