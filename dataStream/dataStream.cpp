#include <opencv2/opencv.hpp>

#include <zmq.hpp>
#include <zmq_addon.hpp>

#include "scheme/ndarray.capnp.h"
#include "capnp/serialize.h"

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>

using namespace std::chrono_literals;

int main() {

	// params

	const std::string fileName = "../../dataStream/video/video1080.webm";
	const int FREQUENCE = 20;	// fps

	const std::string ip = "127.0.0.1";
	const int port = 5558;

	zmq::context_t ctx{};
	zmq::socket_t sck{ ctx, zmq::socket_type::pub };
	sck.bind(std::string{ "tcp://" + ip + ":" + std::to_string(port) });

	cv::VideoCapture cap{ fileName };

	if (!cap.isOpened()) {
		std::cerr << "Error opening file" << std::endl;
		return -1;
	}

	std::cout << "Press for start" << std::endl;
	(void)std::getchar();

	std::atomic_uint16_t fps{};

	bool isEndWork{ false };
	// thread for fps count
	std::thread fpsThread{ [&fps, &isEndWork]() {

		auto&& timer = std::chrono::high_resolution_clock::now();

		while (true) {

			if (isEndWork)
				break;

			if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - timer).count() > 1000.) {

				std::cout << "FPS -> " << fps.load(std::memory_order_acquire) << std::endl;
				timer = std::chrono::high_resolution_clock::now();
				fps.store(0, std::memory_order_relaxed);
			}
			else {
				std::this_thread::sleep_for(10ms);
			}
		}
	} };

	std::thread streamThread{ [&cap, &fps, &sck, &isEndWork]() {

		cv::Mat frame{};
		uint32_t framesCounter{};

		try {

			while (true) {

				auto&& timerStart = std::chrono::high_resolution_clock::now();
				while (framesCounter < FREQUENCE) {

					if (isEndWork)
						throw std::exception{};

					cap >> frame;

					if (frame.empty())
						throw std::exception{};

					++framesCounter;
					++fps;

					// processing frame
					//cv::imshow("Frame", frame);
					//cv::waitKey(1);

					// serialization
					// 
					//create message

					capnp::MallocMessageBuilder message;
					NDArray::Builder ndarray = message.initRoot<NDArray>();

					// fill fields
					// shape
					auto&& shape = ndarray.initShape(2);
					shape.set(0, frame.size().height);
					shape.set(1, frame.size().width);
					// data
					kj::ArrayPtr<const kj::byte> data{ frame.data, frame.total() * frame.elemSize() };
					ndarray.setData(data);
					ndarray.setDtype(NDArray::DType::UINT8);

					kj::Array<capnp::word> serialized_message = capnp::messageToFlatArray(message);

					// send

					auto x = serialized_message.size() * sizeof(capnp::word);
					sck.send(zmq::message_t{ reinterpret_cast<void*>(serialized_message.begin()), x });
				}

				auto&& timerEnd = std::chrono::high_resolution_clock::now();
				auto&& diff = timerEnd - timerStart;
				auto&& elapsedTime = std::chrono::duration<double, std::milli>(diff).count();

				if (elapsedTime < 1000.) {
					std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000. - elapsedTime));
				}

				framesCounter = 0;

				if (isEndWork)
					throw std::exception{};
			}
		}
		catch (...) {

		}
	} };

	std::cout << "Press for exit" << std::endl;
	(void)std::getchar();

	isEndWork = true;

	fpsThread.join();
	streamThread.join();

	cap.release();
	cv::destroyAllWindows();

	return 0;
}