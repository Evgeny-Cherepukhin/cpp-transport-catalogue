//��������� ������� ���������. ����� 9 ������ 2. ���������� ���������.
#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

int main() {
	using namespace transport_catalogue;

	TransportCatalogue transport_catalogue;
	input_requests_processing::UpdateTransportCatalogue(transport_catalogue);
	output_requests_processing::TransportCatalogueRequests(transport_catalogue);

	return 0;
}