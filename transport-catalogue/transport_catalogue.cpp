// Черепухин Евгений Сергеевич. Спринт 12 Версия 1.
#include "transport_catalogue.h"

namespace transport::catalogue
{
	TransportCatalogue::TransportCatalogue(
		std::vector<std::shared_ptr<Stop>>& stops,
		std::vector<std::shared_ptr<Bus>>& buses,
		RenderSettings render_settings,
		RoutingSettings router_settings
	) {
		MakeStopsMap(stops);
		MakeBusesMap(buses);
		MakeRenderMap(stops_, buses_, render_settings);
		//MakeRouter(stops_, buses_, router_settings);
		router_ = std::make_unique<TransportRouter>(stops_, buses_, router_settings);
		router_->BuildGraph();
	}

	void TransportCatalogue::MakeStopsMap(std::vector<std::shared_ptr<Stop>>& stops) {
		for (auto& stop : stops) {
			stops_[stop->name] = stop;
		}
	}

	void TransportCatalogue::MakeBusesMap(std::vector<std::shared_ptr<Bus>>& buses) {
		for (auto& bus : buses) {
			buses_[bus->name] = bus;
			ComputeUniqueStops(bus);
			ComputeRealRouteDistance(bus);
			ComputeCurvature(bus);
			CalculateStopsOnRoute(bus);
		}
	}

	void TransportCatalogue::MakeRenderMap(std::map<std::string_view, std::shared_ptr<Stop>>& stops, std::map<std::string_view, std::shared_ptr<Bus>>& buses, RenderSettings& render_settings) {
		render_ = std::make_unique<MapRenderer>(stops, buses, render_settings);
		map_ = render_->RenderMap();
	}
	/*void TransportCatalogue::MakeRouter(std::map<std::string_view, std::shared_ptr<Stop>>& stops, std::map<std::string_view, std::shared_ptr<Bus>>& buses, RoutingSettings router_settings) {
		router_ = std::make_unique<TransportRouter>(stops, buses, router_settings);
		router_->BuildGraph();
	}*/
	

	const std::string& TransportCatalogue::GetMap() {
		return map_;
	}

	std::shared_ptr<std::vector<RouteItem>> TransportCatalogue::FindRouteInBase(std::string_view from, std::string_view to) {
		return router_->FindRoute(from.data(), to.data());
	}

	void TransportCatalogue::ComputeUniqueStops(std::shared_ptr<Bus>& bus) {
		std::set<std::string_view> stops_uniq;
		for (auto& stop : bus->stops) {
			stops_uniq.insert(stop);
			stops_[stop]->buses.insert(bus->name);
		}
		bus->unique_stop_count = stops_uniq.size();
	}

	void TransportCatalogue::ComputeRealRouteDistance(std::shared_ptr<Bus>& bus) {
		for (int i = 0; i + 1 < bus->stops.size(); ++i) {
			bus->route_length += RealDistanceBeetwenStops(stops_[bus->stops[i]], stops_[bus->stops[i + 1]]);
		}
		if (!bus->is_roundtrip) {
			for (int i = bus->stops.size() - 1; i - 1 > -1; --i) {
				bus->route_length += RealDistanceBeetwenStops(stops_[bus->stops[i]], stops_[bus->stops[i - 1]]);
			}
		}
	}

	void TransportCatalogue::CalculateStopsOnRoute(std::shared_ptr<Bus>& bus) {
		for (auto& stop : bus->stops) {
			stops_[stop]->buses.insert(bus->name);
		}
		if (bus->is_roundtrip) {
			bus->stop_count = bus->stops.size();
		}
		else {
			bus->stop_count = bus->stops.size() * 2 - 1;
		}
	}

	void TransportCatalogue::ComputeCurvature(std::shared_ptr<Bus>& bus) {
		double len_by_coordinates = 0;
		for (size_t i = 0; i + 1 < bus->stops.size(); i++) {
			len_by_coordinates += geo::ComputeDistance(
				stops_[bus->stops[i]]->coordinates,
				stops_[bus->stops[i + 1]]->coordinates
			);
		}
		if (!bus->is_roundtrip) {
			len_by_coordinates *= 2;
		}
		bus->curvature = (bus->route_length / len_by_coordinates) * 1.0;
	}



	std::optional<Stop> TransportCatalogue::GetStopInfo(std::string_view stop_name) {
		auto result = StopByName(stop_name);
		if (result == nullptr) return {};
		return *result;
	}

	std::optional<Bus> TransportCatalogue::GetBusInfo(std::string_view bus_name) {
		auto result = BusByName(bus_name);
		if (result == nullptr) return {};
		return *result;
	}

	std::shared_ptr<Stop> TransportCatalogue::StopByName(std::string_view name) {
		auto it = stops_.find(name);
		if (it == stops_.end()) return nullptr;
		return it->second;
	}
	std::shared_ptr<Bus> TransportCatalogue::BusByName(std::string_view name) {
		auto it = buses_.find(name);
		if (it == buses_.end()) return nullptr;
		return it->second;
	}
}