// Черепухин Евгений Сергеевич. Сплит 10 Версия 2.
#pragma once

#include "geo.h"
#include "json.h"

#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <ostream>

namespace transport::domains 
{
	// Структура остановка. Хранит: Имя, Координаты, Расстояния до других остановок,
	// Автобусы проходящие через эту остановку.
	struct Stop {
		Stop() = default;

		std::string name;
		geo::Coordinates coordinates;
		std::map<std::string, int> stops_distances;
		std::set<std::string> buses;

	// Проводит парсинг запроса на заполнение остановки.
		void Parse(const json::Dict& request);
	};

	// Структура автобусы(маршруты). Хранит: Имя, Флаг(bool)колцевого маршрута, Количество остановок,
	// Количество уникальных остановок, Длину маршрута, Кривизну маршрута, Вектор остановок.
	//
	struct Bus {
		Bus() = default;

		std::string name;
		bool is_roundtrip;
		int stop_count = 0;
		int unique_stop_count = 0;
		double route_length = 0.0;
		double curvature = 0.0;
		std::vector<std::string> stops;

	// Производит парсинг маршрута, заполняет вектор остановок маршрута.
		void Parse(const json::Dict& request);
	};
	// Возвращает дистанцию между остановками.
	unsigned int RealDistanceBeetwenStops(std::shared_ptr<Stop> from, std::shared_ptr<Stop> to);
}
