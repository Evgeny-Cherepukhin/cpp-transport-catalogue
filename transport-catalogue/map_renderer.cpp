// Черепухин Евгений Сергеевич. Сплит 10 Версия 2.
#include "map_renderer.h"

namespace transport::render
{
	using namespace transport::domains;
	using namespace std::literals;

	bool IsZero(double value) {
		return std::abs(value) < EPSILON;
	}

	MapRenderer::MapRenderer(
		std::map<std::string_view, std::shared_ptr<Stop>>& stops_unique,
		std::map<std::string_view, std::shared_ptr<Bus>>& buses_unique,
		RenderSettings settings
	)
		: stops_unique_(stops_unique)
		, buses_unique_(buses_unique)
		, settings_(settings) 
	{
		std::vector<geo::Coordinates> geo_coor;
		for (auto& stop : stops_unique) {
			if (!stop.second.get()->buses.empty()) {
				geo_coor.push_back(stop.second.get()->coordinates);
			}			
		}
		map_points_ = std::make_unique<SphereProjector>(
														  geo_coor.begin()
														, geo_coor.end()
														, settings.width_
														, settings.height_
														, settings.padding_);
	}		 

	std::string MapRenderer::RenderMap() const {
		svg::Document result;
		CreateRoutes(result);
		CreateRouteNumber(result);
		CreateStopPoint(result);
		CreateStopName(result);
		std::stringstream render;
		result.Render(render);
		return render.str();
	}

	void MapRenderer::CreateRoutes(svg::Document& result) const {
		size_t color = 0;		
		for (const auto& [_, bus] : buses_unique_) {
			if (bus->unique_stop_count < 2) {
				continue;
			}
			svg::Polyline route_line = svg::Polyline()
				.SetStrokeColor(settings_.color_palette_[color])
				.SetStrokeWidth(settings_.line_width_)
				.SetFillColor({})
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

			for (const std::string& stop : bus->stops) {
				route_line.AddPoint(map_points_->operator()(stops_unique_.at(stop)->coordinates));
			}
			if (!bus->is_roundtrip) {
				for (int stop = bus->stops.size() - 2; stop > -1; --stop) {
					route_line.AddPoint(map_points_->operator()(stops_unique_.at(bus->stops[stop])->coordinates));
				}
			}
			result.Add(route_line);
			++color;
			if (color == settings_.color_palette_.size()) {
				color = 0;
			}
		}
	}

	void MapRenderer::CreateRouteNumber(svg::Document& result) const {
		int color = 0;
		for (const auto& [_, bus] : buses_unique_) {

			std::vector<svg::Text> bus_numbers;

			svg::Text first_background = svg::Text()
				.SetFillColor(settings_.underlayer_color_)
				.SetStrokeColor(settings_.underlayer_color_)
				.SetStrokeWidth(settings_.underlayer_width_)
				.SetFontWeight("bold")
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
				.SetPosition(map_points_->operator()(stops_unique_.at(bus->stops[0])->coordinates))
				.SetOffset(settings_.bus_label_offset_)
				.SetFontSize(settings_.bus_label_font_size_)
				.SetFontFamily("Verdana")
				.SetData(bus->name);
			bus_numbers.push_back(first_background);

			svg::Text first_number = svg::Text()
				.SetFillColor(settings_.color_palette_[color])
				.SetFontWeight("bold")
				.SetPosition(map_points_->operator()(stops_unique_.at(bus->stops[0])->coordinates))
				.SetOffset(settings_.bus_label_offset_)
				.SetFontSize(settings_.bus_label_font_size_)
				.SetFontFamily("Verdana")
				.SetData(bus->name);

			bus_numbers.push_back(first_number);

			if (!bus->is_roundtrip) {
				if (bus->stops.size() > 1 && stops_unique_.at(bus->stops[0])->name != stops_unique_.at(bus->stops[bus->stops.size() - 1])->name) {
					svg::Text second_background = svg::Text()
						.SetFillColor(settings_.underlayer_color_)
						.SetStrokeColor(settings_.underlayer_color_)
						.SetStrokeWidth(settings_.underlayer_width_)
						.SetFontWeight("bold")
						.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
						.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
						.SetPosition(map_points_->operator()(stops_unique_.at(bus->stops[bus->stops.size() - 1])->coordinates))
						.SetOffset(settings_.bus_label_offset_)
						.SetFontSize(settings_.bus_label_font_size_)
						.SetFontFamily("Verdana")
						.SetData(bus->name);
					bus_numbers.push_back(second_background);

					svg::Text second_number = svg::Text()
						.SetFillColor(settings_.color_palette_[color])
						.SetFontWeight("bold")
						.SetPosition(map_points_->operator()(stops_unique_.at(bus->stops[bus->stops.size() - 1])->coordinates))
						.SetOffset(settings_.bus_label_offset_)
						.SetFontSize(settings_.bus_label_font_size_)
						.SetFontFamily("Verdana")
						.SetData(bus->name);

					bus_numbers.push_back(second_number);
				}
			}


			for (auto& number : bus_numbers) {
				result.Add(number);
			}

			color++;
			if (color == settings_.color_palette_.size())  color = 0;
		}
	}

	void MapRenderer::CreateStopPoint(svg::Document& result) const {
		for (const auto& [_, stop] : stops_unique_) {
			if (!stop.get()->buses.empty()) {
				svg::Circle point = svg::Circle()
					.SetCenter(map_points_->operator()(stop->coordinates))
					.SetRadius(settings_.stop_radius_)
					.SetFillColor("white"s);
				result.Add(point);
			}			
		}
	}

	void MapRenderer::CreateStopName(svg::Document& result) const {
		for (const auto& [_, stop] : stops_unique_) {
			if (!stop.get()->buses.empty()) {
				svg::Text background = svg::Text()
					.SetData(stop->name)
					.SetPosition(map_points_->operator()(stop->coordinates))
					.SetOffset(settings_.stop_label_offset_)
					.SetFontSize(settings_.stop_label_font_size_)
					.SetFontFamily("Verdana")
					.SetFontWeight({})
					.SetFillColor(settings_.underlayer_color_)
					.SetStrokeColor(settings_.underlayer_color_)
					.SetStrokeWidth(settings_.underlayer_width_)
					.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

				result.Add(background);

				svg::Text number = svg::Text()
					.SetData(stop->name)
					.SetPosition(map_points_->operator()(stop->coordinates))
					.SetOffset(settings_.stop_label_offset_)
					.SetFontSize(settings_.stop_label_font_size_)
					.SetFontFamily("Verdana")
					.SetFontWeight({})
					.SetFillColor("black");

				result.Add(number);
			}			
		}
	}
}