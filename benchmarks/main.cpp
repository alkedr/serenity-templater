#include "hayai.hpp"
#include <serenity/templater.hpp>
#include <assert.h>


int dataInt[1000];


__attribute__((constructor)) static void init() {
	for (auto & x : dataInt) {
		x = rand();
	}
}


BENCHMARK(templater, int, 10, 100) {
	const auto & data = dataInt;
	std::string res =
#	include <benchmarks/templates/array1000.htmltc>
	;
	assert(res.back() == '\n');
}


int main() {
	Hayai::Benchmarker::RunAllTests();
	return 0;
}
