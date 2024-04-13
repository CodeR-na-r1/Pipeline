#pragma once

#include "IMonitoringMeasurements.hpp"

namespace Pipeline {

	namespace detail {

		struct MonitoringMeasurements : public IMonitoringMeasurements {

			std::unordered_map<std::size_t, std::size_t> queueLoad{};	// get load queues by stage id
			std::unordered_map<std::size_t, double> avgTimePerCallable{};	// get the average time spent executing 1 function in a stage thread by stage id

			MonitoringMeasurements(std::unordered_map<std::size_t, std::size_t> queueLoad, std::unordered_map<std::size_t, double> avgTimePerCallable) : IMonitoringMeasurements(), queueLoad(queueLoad), avgTimePerCallable(avgTimePerCallable) {}

			virtual const std::unordered_map<std::size_t, std::size_t>& getQueueLoad() override {

				return queueLoad;
			}

			virtual const std::unordered_map<std::size_t, double>& getAvgTimePerCallable() override {

				return avgTimePerCallable;
			}
		};
	}
}