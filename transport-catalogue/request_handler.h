// ��������� ������� ���������. ����� 11 ������ 1.
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

	// ������������ ����� ��� �������.(���������, �������, �����)
	enum class RequestType
	{
		STOP, BUS, MAP
	};

	// ��������� - ������ ���, ����� � ��� �������.
	struct Request {
		Request() = default;

		int id;
		std::string name = ""s;
		RequestType type;
	};

	// ��������������� �����. �������� ������� ����� �� ������.
	class RequestHelper {
	public:
		//����������� ������.
		RequestHelper(TransportCatalogue& tc, const json::Array& stat_requests);

		// ����� ������� ������ ������� �� �������
		void GetResponses();

		// ������� ������ �� ������� � �����.
		void PrintResponse(std::ostream& out);

	private:
		// ������ ����� �� ������.
		json::Node CreateJsonResponseError(const int request_id);

		// ������ ����� �� ������ �� ���������.
		json::Node CreateJsonResponseStop(const int request_id, const domains::Stop& data);

		// ������ ����� �� ������ � ��������.
		json::Node CreateJsonResponseBus(const int request_id, const domains::Bus data);

		// ������ ����� �� ������ � ���������� ����� ���������.
		json::Node CreateJsonResponseMap(const int request_id, const std::string map_render_data);

		// ������������ �������, ������ �������� � ���� � ������ ������� json::Array.
		TransportCatalogue& catalogue_;
		std::vector<Request> requests_;
		json::Array responses_;

	};
}
