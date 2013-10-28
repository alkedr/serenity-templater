#pragma once

#include <functional>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>


namespace serenity {
namespace benchmarker {

static std::map<std::string, std::function<void()>> map;

static void run() {
	double minDuration = 100000000;
	double maxDuration = -1;
	double avgDuration = 0;
	for (const auto & pair : map) {
		for (int i=0; i<1000; i++) {
			struct timespec begin, end;

			clock_gettime(CLOCK_MONOTONIC, &begin);
			pair.second();
			clock_gettime(CLOCK_MONOTONIC, &end);

			double duration = (end.tv_sec - begin.tv_sec) + ((double)(end.tv_nsec - begin.tv_nsec)) / 1000000000;
			minDuration = std::min(minDuration, duration);
			maxDuration = std::max(maxDuration, duration);
			avgDuration += duration;
		}
		avgDuration /= 1000;
		std::cout.precision(6);
		std::cout << std::fixed << pair.first << " " << minDuration * 1000 << "ms " << avgDuration * 1000 << "ms " << maxDuration * 1000 << "ms" << std::endl;
	}
}

}
}

#define BENCHMARK(DESCRIPTION)  serenity::benchmarker::map[DESCRIPTION] = []()

