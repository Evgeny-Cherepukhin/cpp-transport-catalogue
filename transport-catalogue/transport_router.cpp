// Черепухин Евгений Сергеевич. Спринт 12 Версия 1. 
#include "transport_router.h"

#include <iostream>
namespace transport::router {

	using namespace transport::domains; 
	// Коэффициент для перевода скорости из км/ч в м/мин.
	static const double  SPEED_FACTOR_M_PER_MIN = 100.0/6.0;

	TransportRouter::TransportRouter(
		std::map<std::string_view, std::shared_ptr<Stop>>& stops,
		std::map<std::string_view, std::shared_ptr<Bus>>& buses,
		RoutingSettings settings
	)
		: stops_(stops)
		, buses_(buses)
		, settings_(settings) { }

	void TransportRouter::BuildGraph() {
		graph_ = graph::DirectedWeightedGraph<double>(stops_.size());

		FillVertexes();
		FillEdges();

		search_in_graph_ = std::make_unique<graph::Router<double>>(graph_);
	}

	void TransportRouter::FillVertexes() {
		size_t i = 0;
		for (auto& [_, stop] : stops_) {
			graph_vertexes_[stop] = i++;
		}
	}
	void TransportRouter::FillEdges() {
		for (auto& [_, route] : buses_) {

			std::vector<double> distance_forward;
			distance_forward.resize(route->stops.size());
			std::vector<double> distance_reverse;
			distance_reverse.resize(route->stops.size());

			double forward_sum = 0.0;
			double reverse_sum = 0.0;

			for (size_t index = 1; index < route->stops.size(); index++) {
				forward_sum += RealDistanceBeetwenStops(stops_.at(route->stops[index - 1]), stops_.at(route->stops[index]));
				distance_forward[index] = forward_sum;
				reverse_sum += RealDistanceBeetwenStops(stops_.at(route->stops[index]), stops_.at(route->stops[index - 1]));
				distance_reverse[index] = reverse_sum;
			}

			for (int s = 0; s + 1 < route->stops.size(); s++) {
				for (int s1 = s + 1; s1 < route->stops.size(); s1++) {
					RouteItem item;
					item.start_stop_idx = stops_.at(route->stops[s]);
					item.finish_stop_idx = stops_.at(route->stops[s1]);
					item.bus = route;
					item.stop_count = std::abs(s - s1);
					item.wait_time = settings_.bus_wait_time;
					item.trip_time = (s < s1 ? distance_forward[s1] - distance_forward[s] : distance_reverse[s] - distance_reverse[s1]) / (settings_.bus_velocity*SPEED_FACTOR_M_PER_MIN);

					graph_.AddEdge(
						graph::Edge<double>{
						graph_vertexes_[item.start_stop_idx],
							graph_vertexes_[item.finish_stop_idx],
							item.trip_time + item.wait_time
					}
					);
					graph_edges_.push_back(std::move(item));

					if (!route->is_roundtrip) {
						RouteItem item;
						item.start_stop_idx = stops_.at(route->stops[s1]);
						item.finish_stop_idx = stops_.at(route->stops[s]);
						item.bus = route;
						item.stop_count = std::abs(s - s1);
						item.wait_time = settings_.bus_wait_time;
						item.trip_time = (s1 < s ? distance_forward[s] - distance_forward[s1] : distance_reverse[s1] - distance_reverse[s]) / (settings_.bus_velocity * SPEED_FACTOR_M_PER_MIN);

						graph_.AddEdge(
							graph::Edge<double>{
							graph_vertexes_[item.start_stop_idx],
								graph_vertexes_[item.finish_stop_idx],
								item.trip_time + item.wait_time
						}
						);
						graph_edges_.push_back(std::move(item));
					}
				}
			}
		}
	}

	std::shared_ptr<std::vector <RouteItem>> TransportRouter::FindRoute(const std::string_view from, const std::string_view to) {
		std::shared_ptr<Stop> stop_from = stops_.at(from);
		std::shared_ptr<Stop> stop_to = stops_.at(to);
		if (stop_from == nullptr || stop_to == nullptr) return nullptr;

		std::shared_ptr<std::vector<RouteItem>> res = std::make_shared<std::vector<RouteItem>>();
		if (stop_from == stop_to)   return res;


		std::optional<graph::Router<double>::RouteInfo> res_tmp = search_in_graph_->BuildRoute(graph_vertexes_.at(stop_from), graph_vertexes_.at(stop_to));
		if (res_tmp == std::nullopt)  return nullptr;

		res->reserve(res_tmp->edges.size());
		for (auto e = res_tmp->edges.begin(); e != res_tmp->edges.end(); ++e) {
			res->push_back(graph_edges_.at(*e));
		}
		return res;
	}

	const RoutingSettings& TransportRouter::GetSettings() const {
		return settings_;
	}   
}