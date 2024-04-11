#pragma once

#include "IMonitoringManager.hpp"

namespace Pipeline {

	namespace Monitoring {

		template <typename DaoT>
		class MonitoringManager : public IMonitoringManager<DaoT> {

			std::unordered_map<std::size_t, Connector::IConnector<DaoT>> queuesMap;
			std::unordered_map<std::size_t, detail::IMeasurements> measurementsMap;

			// cond variable
			// собирает данные и ддает доступ, оповещая
			// для того чтобы все сращзу не долбилдись до стадий, тем самым только мешая
			// пользователь может сам написать функцию любую, в том числе и для отправки показателей куда угодно, просто добавив в билдер пайплайна функцию, которая принимает поинтер на конст объект (интерфейс) мониторинга

		public:

			MonitoringManager() : IMonitoringManager<DaoT>() {}


		};
	}
}