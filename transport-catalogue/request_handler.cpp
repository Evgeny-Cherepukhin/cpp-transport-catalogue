// Черепухин Евгений Сергеевич. Сплит 10 Версия 2. 
#include "request_handler.h"

namespace transport::response {

	using namespace transport::catalogue;	
	using namespace std::literals;

	RequestHelper::RequestHelper(TransportCatalogue& tc, const json::Array& stat_requests)
		: catalogue_(tc)
	{
		requests_.reserve(stat_requests.size());
		for (const json::Node& n : stat_requests) {
			Request request;
			const auto& node_map = n.AsMap();
			request.id = node_map.at("id"s).AsInt();

			std::string type = node_map.at("type"s).AsString();
			if (type == "Stop"s) {
				request.name = node_map.at("name"s).AsString();
				request.type = RequestType::STOP;
			}
			else if (type == "Bus"s) {
				request.name = node_map.at("name"s).AsString();
				request.type = RequestType::BUS;
			}
			else if (type == "Map"s) {
				
				request.type = RequestType::MAP;
			}
			else {
				throw json::ParsingError("Request invalid"s);
			}
			requests_.push_back(request);
		}
	}

	void RequestHelper::GetResponses() {		
		for (const Request& request : requests_) {
			switch (request.type) {
			case RequestType::STOP: {
				if (auto stop_info = catalogue_.GetStopInfo(request.name); stop_info) {
					responses_.push_back(CreateJsonResponseStop(request.id, *stop_info));
				}
				else {
					responses_.push_back(CreateJsonResponseError(request.id));
				}
			} break;
			case RequestType::BUS: {
				if (auto route_info = catalogue_.GetBusInfo(request.name); route_info) {
					responses_.push_back(CreateJsonResponseBus(request.id, *route_info));
				}
				else {
					responses_.push_back(CreateJsonResponseError(request.id));
				}
			} break;
			case RequestType::MAP: {
				auto map_render_data = catalogue_.GetMap();
				responses_.push_back(CreateJsonResponseMap(request.id, map_render_data));
			}break;
			default:
				throw std::logic_error("unknown type");
			}
		}
	}

	void RequestHelper::PrintResponse(std::ostream& out) {		
		json::Print(json::Document{ responses_ }, out);
	}

	json::Node RequestHelper::CreateJsonResponseError(const int request_id) {
		json::Dict tmp;
		tmp["request_id"s] = request_id;
		tmp["error_message"s] = "not found"s;
		return tmp;
	}

	json::Node RequestHelper::CreateJsonResponseStop(const int request_id, const domains::Stop& data) {
		json::Dict tmp;	
		json::Array tmp_1;
		for (auto& bus : data.buses) {
			tmp_1.push_back(bus);
		}		
		tmp["buses"s]= tmp_1;
		tmp["request_id"s]= request_id;
		return tmp;
	}

	json::Node RequestHelper::CreateJsonResponseBus(const int request_id, const domains::Bus data) {
		json::Dict tmp;
		tmp["curvature"s] = data.curvature;
		tmp["request_id"s] = request_id;
		tmp["route_length"s] = data.route_length;
		tmp["stop_count"s] = data.stop_count;
		tmp["unique_stop_count"] = data.unique_stop_count;
		return tmp;
	}

	json::Node RequestHelper::CreateJsonResponseMap(const int request_id, const std::string map_render_data) {
		json::Dict tmp;
		tmp["map"s] = map_render_data;
		tmp["request_id"s]= request_id;
		return tmp;
	}
}