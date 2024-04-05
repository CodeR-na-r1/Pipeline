#pragma once

namespace Pipeline {

	namespace detail {

		struct IMeasurements {

			virtual void push(double) = 0;

			virtual double pull() = 0;
		};
	}
}