#pragma once

#include <mutex>

namespace Pipeline {

	namespace detail {

        struct Measurements {

            std::uint64_t counter;
            double accumulator;
            std::mutex guard;

            /* blocking method
            * push time and increment counter
            */
            void push(double value) {

                guard.lock();

                accumulator += value;
                ++counter;

                guard.unlock();
            }

            /* blocking method
            * reset accumulator and counter
            * return average time
            */
            double pull() {

                if (counter == 0)
                    return 0.0;

                guard.lock();

                auto acc = accumulator;
                accumulator = 0.0;

                auto count = counter;
                counter = 0;

                guard.unlock();

                if (count != 0)
                    return acc / count;
                else
                    return 0.0;
            }
        };
	}

}