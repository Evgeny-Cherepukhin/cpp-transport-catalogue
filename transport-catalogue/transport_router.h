// ��������� ������� ���������. ������ 12 ������ 1. 
#pragma once
#include "domain.h"
#include "router.h"

#include <optional>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <ostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <memory>



namespace transport::router {

    using namespace transport::domains;

    using minutes = double;    

    using km_ch = double; 

    // ����������� ��� ��������� ��������������.
    struct RoutingSettings {
        RoutingSettings() = default;
        RoutingSettings(minutes wait_time, km_ch velocity)
            : bus_wait_time(wait_time)
            , bus_velocity(velocity)
        {}

        minutes bus_wait_time;
        km_ch bus_velocity;
    };

    // ��������� ��� �������� ������������ ���������� ������������� (��������� � �������� ���������, �������, ���������� ���������, 
    // ����� � ���� � ����� ��������).
    struct RouteItem {
        RouteItem() = default;
        RouteItem(std::shared_ptr<Stop> start_stop,
            std::shared_ptr<Stop> finish_stop,
            std::shared_ptr<Bus> new_bus,
            int new_trip_stop_count,
            minutes new_trip_time,
            minutes new_wait_time
        ) :
            start_stop_idx(start_stop),
            finish_stop_idx(finish_stop),
            bus(new_bus),
            stop_count(new_trip_stop_count),
            trip_time(new_trip_time),
            wait_time(new_wait_time) {
        }

        std::shared_ptr<Stop> start_stop_idx = nullptr;
        std::shared_ptr<Stop> finish_stop_idx = nullptr;
        std::shared_ptr<Bus> bus = nullptr;

        int stop_count = 0;
        minutes trip_time = 0.0;
        minutes wait_time = 0.0;
    };

    // ����� ������������� ���������� ����������� �������� � �������������� �����.
    class TransportRouter {
    public:
        // ���������� ������.
        TransportRouter(
            std::map<std::string_view, std::shared_ptr<Stop>>& stops,
            std::map<std::string_view, std::shared_ptr<Bus>>& buses,
            RoutingSettings settings);
        
        // ����� ������ ����.
        void BuildGraph();

        // ���������� ��������� �������������.
        const RoutingSettings& GetSettings() const;

        // ���� ����������� ������� �� from ��������� from �� ��������� to.
        std::shared_ptr<std::vector<RouteItem>> FindRoute(const std::string_view from, const std::string_view to);               

    private:
        const std::map<std::string_view, std::shared_ptr<Stop>>& stops_;
        const std::map<std::string_view, std::shared_ptr<Bus>>& buses_;
        const RoutingSettings settings_;

        graph::DirectedWeightedGraph<double> graph_;
        std::unique_ptr<graph::Router<double>> search_in_graph_ = nullptr;

        std::vector<RouteItem> graph_edges_;
        std::unordered_map<std::shared_ptr<Stop>, size_t> graph_vertexes_;

        // ������ ��������� ������� � ����� �����.
        void FillVertexes();
        void FillEdges();
    };
}