// Черепухин Евгений Сергеевич. Сплит 11 Версия 1.
#pragma once

#include "json.h"
#include "json_builder.h"
#include "transport_catalogue.h"
//#include "json_reader.h"

#include <map>
#include <string>
#include <vector>

namespace transport::response
{

	using namespace transport::catalogue;
	using namespace std::literals;

	// Перечислимый класс тип запроса.(Остановка, Маршрут, Карта)
	enum class RequestType
	{
		STOP, BUS, MAP
	};

	// Структура - хранит имя, номер и тип запроса.
	struct Request {
		Request() = default;

		int id;
		std::string name = ""s;
		RequestType type;
	};

	// Вспомогательный класс. Помогает создать ответ на запрос.
	class RequestHelper {
	public:
		//Конструктор класса.
		RequestHelper(TransportCatalogue& tc, const json::Array& stat_requests);

		// Метод создает вектор ответов на запросы
		void GetResponses();

		// Выводит ответы на запросы в поток.
		void PrintResponse(std::ostream& out);

	private:
		// Создаёт ответ об ошибке.
		json::Node CreateJsonResponseError(const int request_id);

		// Создаёт ответ на запрос об остановке.
		json::Node CreateJsonResponseStop(const int request_id, const domains::Stop& data);

		// Создаёт ответ на запрос о маршруте.
		json::Node CreateJsonResponseBus(const int request_id, const domains::Bus data);

		// Создаёт ответ на запрос о построении карты маршрутов.
		json::Node CreateJsonResponseMap(const int request_id, const std::string map_render_data);

		// Транспортный каталог, вектор запросов к нему и вектор ответов json::Array.
		TransportCatalogue& catalogue_;
		std::vector<Request> requests_;
		json::Array responses_;

	};
}
