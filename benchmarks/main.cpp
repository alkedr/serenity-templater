#include "benchmarker.hpp"
#include <benchmarks/templates.htmltc>
#include <assert.h>

namespace {

int dataInt[1000];

__attribute__((constructor)) static void init() {
	for (auto & x : dataInt) {
		x = rand();
	}
}

}


int main() {
	BENCHMARK("1000 variables") {
		const auto & data = dataInt;
		std::string res = TEMPLATE(array1000);
		assert(res.back() == '\n');
	};

	serenity::benchmarker::run();
}

