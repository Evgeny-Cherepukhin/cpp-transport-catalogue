// Черепухин Евгений Сергеевич. Спринт 14 Версия 1. 
#include "domain.h"

#include <iostream>
#include <iomanip>
#include <ostream>

namespace transport::domains
{
	using namespace std::literals;

	void Stop::Parse(const json::Dict& request) {
		name = request.at("name"s).AsString();
		coordinates = {
			request.at("latitude"s).AsDouble(),
			request.at("longitude"s).AsDouble()
		};

		for (const auto& [stop_name, distance] : request.at("road_distances"s).AsDict()) {
			stops_distances[stop_name] = distance.AsInt();
		}
	}

	void Bus::Parse(const json::Dict& request) {
		name = request.at("name"s).AsString();
		is_roundtrip = request.at("is_roundtrip"s).AsBool();

		for (const auto& stop_name : request.at("stops"s).AsArray()) {
			stops.push_back(stop_name.AsString());
		}
	}

	unsigned int RealDistanceBeetwenStops(std::shared_ptr<Stop> from, std::shared_ptr<Stop> to) {
		if (from == nullptr || to == nullptr) throw std::invalid_argument("stops no find");

		auto it = from->stops_distances.find(to->name);
		if (it != from->stops_distances.end()) return it->second;

		it = to->stops_distances.find(from->name);
		if (it != to->stops_distances.end()) return it->second;

		return 0;
	}
}