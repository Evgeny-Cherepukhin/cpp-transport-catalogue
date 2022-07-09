// Черепухин Евгений Сергеевич. Сплит 10 Версия 1.
#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"

//#include <sstream>
//#include <iostream>


using namespace std;
using namespace json::reader;
using namespace transport::catalogue;
using namespace transport::response;
using namespace transport::render;

int main() {
    
    const auto& input_doc = json::Load(cin);
    const auto& input_map = input_doc.GetRoot().AsMap();
    std::vector<std::shared_ptr<Stop>> stops = json::reader::ParseStop(input_map.at("base_requests").AsArray());
    std::vector<std::shared_ptr<Bus>> buses = json::reader::ParseBus(input_map.at("base_requests").AsArray());
    RenderSettings render_settings = json::reader::ParseRenderSetting(input_map.at("render_settings").AsMap());
    
    TransportCatalogue tc(stops, buses, render_settings);
    RequestHelper requests(tc, input_map.at("stat_requests").AsArray());
    requests.GetResponses();   
    requests.PrintResponse(std::cout);
}