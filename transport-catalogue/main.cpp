#include "domain.h"
#include "json_reader.h"
#include "transport_catalogue.h"
#include "request_handler.h"

#include <fstream>
#include <iostream>
#include <string_view>

using namespace std::literals;
using namespace std;
using namespace transport::catalogue;
using namespace transport::response;
using namespace json::reader;

string ReadFile(const string& file_name) {
    ifstream file(file_name, ios::binary);    
    stringstream ss;
    ss << file.rdbuf();
    string str = ss.str();
    return str;
}

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RUS");

    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    const auto& input_doc = json::Load(cin);
    const auto& input_map = input_doc.GetRoot().AsDict();

    if (mode == "make_base"sv) {
        std::vector<std::shared_ptr<Stop>> stops = json::reader::ParseStop(input_map.at("base_requests").AsArray());
        std::vector<std::shared_ptr<Bus>> buses = json::reader::ParseBus(input_map.at("base_requests").AsArray());
        

        TransportCatalogue mainBD(           
            stops,
            buses,
            json::reader::ParseRenderSetting(input_map.at("render_settings").AsDict()),
            json::reader::ParseRouterSetting(input_map.at("routing_settings").AsDict())
        );

        const string& file_name = input_map.at("serialization_settings").AsDict().at("file").AsString();
        ofstream file(file_name, ios::binary);
        file << mainBD.Serialize();
        return 0;

    } else if (mode == "process_requests"sv) {

        const string& file_name = input_map.at("serialization_settings").AsDict().at("file").AsString();

        TransportCatalogue mainBD;
        mainBD.Deserialize(ReadFile(file_name));

        RequestHelper requests(mainBD, input_map.at("stat_requests").AsArray());
        requests.GetResponses();
        requests.PrintResponse(std::cout);
        return 0;


    } else {
        PrintUsage();
        return 1;
    }
}