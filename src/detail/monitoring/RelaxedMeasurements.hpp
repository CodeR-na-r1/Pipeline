#pragma once

#include "IMeasurements.hpp"

#include <mutex>

namespace Pipeline {

    namespace detail {

        struct RelaxedMeasurements : public IMeasurements {

            std::uint64_t counter{};
            double accumulator{};
            std::mutex guard{};

            RelaxedMeasurements() : IMeasurements() {}

            /* blocking method
            * push time and increment counter
            */
            virtual void push(double value) override {

                if (guard.try_lock()) {

                    accumulator += value;
                    ++counter;

                    guard.unlock();
                }                
            }

            /* blocking method
            * reset accumulator and counter
            * return average time
            */
            virtual double pull() override {

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