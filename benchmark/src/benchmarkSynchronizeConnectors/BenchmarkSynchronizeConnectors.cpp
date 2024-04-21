#pragma once

#include "connector/SyncMPMCConnector.hpp"

#include <iostream>
#include <thread>

#define QUANTITY 10'000'000

using Pipeline::Connector::SyncMapConnector;
using Pipeline::Frame::IDFrame;

using idT = std::uint64_t;
using DataT = std::uint64_t;
using FrameT = std::shared_ptr<IDFrame<idT, DataT>>;
using SyncMapConnectorT = std::shared_ptr<SyncMapConnector<FrameT, 1024>>;

using namespace std;

int main() {

	std::cout << "Parameters for benchmark: " << std::endl;
	std::cout << "Amount of elements: " << QUANTITY << std::endl;
	std::cout << "Size of connector (megabytes): " << (QUANTITY * sizeof(FrameT)) / 1024 / 1024 << std::endl;

	// test syncMapConnector
	{
		// create connector
		auto syncConnector = SyncMapConnectorT{ new Pipeline::Connector::SyncMapConnector<FrameT, 1024>{} };

		// create timer points
		decltype(std::chrono::high_resolution_clock::now()) startWorkTime{}, finishWorkTime{};

		std::cout << "Benchmark for test map sync connector started!" << std::endl;

		// consumer thread
		std::thread consumerSyncMapConnector{ [syncConnector, &finishWorkTime]() {

			uint64_t counter{}, accumulator{};
			FrameT value;

			while (true) {

				if (syncConnector->hasValue()) {

					syncConnector->extract(value);
					accumulator += value->getData();
					++counter;
				}
				if (counter == QUANTITY) {
					break;
				}
			}
			finishWorkTime = std::chrono::high_resolution_clock::now();
			std::cout << "Consumer thread with map sync connector finished!\tAccumulator: " << accumulator << ";\tCounter: " << counter << std::endl;
			} };

		// producer thread
		std::thread producerSyncMapConnector{ [syncConnector, &startWorkTime]() {

			startWorkTime = std::chrono::high_resolution_clock::now();

			for (size_t i{ 0 }; i < QUANTITY; ++i)
				while (!syncConnector->push(FrameT{ new IDFrame<idT, DataT>{i, i} })) {}
			} };

		// join threads
		producerSyncMapConnector.join();
		consumerSyncMapConnector.join();

		auto timer = std::chrono::duration<double, std::milli>(finishWorkTime - startWorkTime).count();

		// print result
		std::cout << "Time for syncMapConnector -> " << timer << "ms" << std::endl;
	}

	return 0;
}
