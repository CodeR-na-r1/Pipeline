#pragma once

#include "IMonitoringManager.hpp"

namespace Pipeline {

	namespace Monitoring {

		template <typename DaoT>
		class MonitoringManager : public IMonitoringManager<DaoT> {

			std::unordered_map<std::size_t, Connector::IConnector<DaoT>> queuesMap;
			std::unordered_map<std::size_t, detail::IMeasurements> measurementsMap;

			// cond variable
			// �������� ������ � ����� ������, ��������
			// ��� ���� ����� ��� ������ �� ���������� �� ������, ��� ����� ������ �����
			// ������������ ����� ��� �������� ������� �����, � ��� ����� � ��� �������� ����������� ���� ������, ������ ������� � ������ ��������� �������, ������� ��������� ������� �� ����� ������ (���������) �����������

		public:

			MonitoringManager() : IMonitoringManager<DaoT>() {}


		};
	}
}