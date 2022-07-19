// Черепухин Евгений Сергеевич. Сплит 11 Версия 1.
#include "json_reader.h"
#include "json_builder.h"
#include "request_handler.h"
#include "map_renderer.h"

using namespace std;
using namespace json::reader;
using namespace transport::catalogue;
using namespace transport::response;
using namespace transport::render;

int main() {

    const auto& input_doc = json::Load(cin);
    const auto& input_map = input_doc.GetRoot().AsDict();
    json::reader::JsonReader input_data;
    std::vector<std::shared_ptr<Stop>> stops = input_data.ParseStop(input_map.at("base_requests").AsArray());
    std::vector<std::shared_ptr<Bus>> buses = input_data.ParseBus(input_map.at("base_requests").AsArray());
    RenderSettings render_settings = input_data.ParseRenderSetting(input_map.at("render_settings").AsDict());

    TransportCatalogue tc(stops, buses, render_settings);
    RequestHelper requests(tc, input_map.at("stat_requests").AsArray());
    requests.GetResponses();
    requests.PrintResponse(std::cout);
    
}