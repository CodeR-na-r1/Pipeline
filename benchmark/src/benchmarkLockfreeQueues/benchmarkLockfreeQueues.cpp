#include <iostream>

#include <memory>

#include <thread>

#include "boost/lockfree/spsc_queue.hpp"
#include "cds/container/vyukov_mpmc_cycle_queue.h"

#define QUANTITY 67'108'864
using queueType = uint64_t;

using spscQueueT = boost::lockfree::spsc_queue<queueType, boost::lockfree::capacity<QUANTITY>>;

// replace 'VyukovMPMCCycleQueue' with 'VyukovMPSCCycleQueue' to test in benchmark mpsc queue instead of mpmc
using mpmcQueueT = cds::container::VyukovMPMCCycleQueue<
	queueType, typename cds::container::vyukov_queue::make_traits<
	cds::opt::buffer<
	cds::opt::v::initialized_static_buffer<void*, QUANTITY>>,
	cds::opt::item_counter< cds::atomicity::item_counter>
	>::type>;

int main() {

	std::cout << "Parameters for benchmark: " << std::endl;
	std::cout << "Amount of elements: " << QUANTITY << std::endl;
	std::cout << "Size of queue (megabytes): " << (QUANTITY * sizeof(queueType)) / 1024 / 1024 << std::endl;

	// test spsc queue
	{
		// create queue
		auto spscQueue = std::shared_ptr<spscQueueT>{ new spscQueueT{} };
		
		// create timer points
		std::chrono::steady_clock::time_point startWorkTime{}, finishWorkTime{};

		std::cout << "Benchmark for test spsc queue started!" << std::endl;

		// consumer thread
		std::thread consumerSPSCQ{ [spscQueue, &finishWorkTime]() {

			uint64_t counter{}, accumulator{};

			while (true) {

				if (spscQueue->read_available()) {

					accumulator += spscQueue->front();
					spscQueue->pop();
					++counter;
				}
				if (counter == QUANTITY) {
					break;
				}
			}
			finishWorkTime = std::chrono::high_resolution_clock::now();
			std::cout << "Consumer thread with spsc queue finished!\tAccumulator: " << accumulator << ";\tCounter: " << counter << std::endl;
			} };

		// producer thread
		std::thread producerSPSCQ{ [spscQueue, &startWorkTime]() {

			startWorkTime = std::chrono::high_resolution_clock::now();

			for (size_t i{ 0 }; i < QUANTITY; ++i)
				while (!spscQueue->push(i)) {}
			} };

		// join threads
		producerSPSCQ.join();
		consumerSPSCQ.join();
		
		auto timer = std::chrono::duration<double, std::milli>(finishWorkTime - startWorkTime).count();

		// print result
		std::cout << "Time for spscQueue -> " << timer << "ms" << std::endl;
	}

	// test mpmc queue
	{
		// create queue
		auto mpmcQueue = std::shared_ptr<mpmcQueueT>{ new mpmcQueueT{} };

		// create timer points
		std::chrono::steady_clock::time_point startWorkTime{}, finishWorkTime{};

		std::cout << "Benchmark for test mpmc queue started!" << std::endl;

		// consumer thread
		std::thread consumerMPMCQ{ [mpmcQueue, &finishWorkTime]() {

			uint64_t counter{}, accumulator{};

			while (true) {

				if (mpmcQueue->size()) {

					uint64_t data;
					mpmcQueue->pop(data);

					accumulator += data;
					++counter;
				}
				if (counter == QUANTITY) {
					break;
				}
			}
			finishWorkTime = std::chrono::high_resolution_clock::now();
			std::cout << "Consumer thread with mpmc queue finished!\tAccumulator: " << accumulator << ";\tCounter: " << counter << std::endl;
			} };

		// producer thread
		std::thread producerMPMCQ{ [mpmcQueue, &startWorkTime]() {

			startWorkTime = std::chrono::high_resolution_clock::now();

			for (size_t i{ 0 }; i < QUANTITY; ++i)
				while (!mpmcQueue->push(i)) {}
			} };

		// join threads
		producerMPMCQ.join();
		consumerMPMCQ.join();

		auto timer = std::chrono::duration<double, std::milli>(finishWorkTime - startWorkTime).count();

		// print result
		std::cout << "Time for mpmcQueue -> " << timer << "ms" << std::endl;
	}

	return 0;
}