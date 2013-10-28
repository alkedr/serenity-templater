RELEASE := y
BUILD_DIR := build

CXX := clang++ -std=c++11

CXXFLAGS_common  := -Iinclude -Ibuild -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-variable-declarations
CXXFLAGS_release := $(CXXFLAGS_common) -O3 -flto -s

ifeq ($(RELEASE), y)
	CXXFLAGS_debug := $(CXXFLAGS_common)
	CXXFLAGS := $(CXXFLAGS_release)
else
	CXXFLAGS_debug := $(CXXFLAGS_common) -ggdb3
	CXXFLAGS := $(CXXFLAGS_debug)
endif


.SUFFIXES:

HEADERS := $(wildcard include/*.hpp)

PRECOMPILED_HEADER := tests/catch.hpp.pch

HTMLTPP_SOURCE := src/main.cpp
HTMLTPP := $(BUILD_DIR)/serenity-htmltpp

TEST_SOURCE := tests/main.cpp
TEST := $(BUILD_DIR)/test

TEST_TEMPLATES_SOURCES := $(wildcard tests/templates/*.htmlt)
TEST_TEMPLATES := $(BUILD_DIR)/tests/templates.htmltc

BENCHMARK_SOURCE := benchmarks/main.cpp
BENCHMARK := $(BUILD_DIR)/benchmark

BENCHMARK_TEMPLATES_SOURCES := $(wildcard benchmarks/templates/*.htmlt)
BENCHMARK_TEMPLATES := $(BUILD_DIR)/benchmarks/templates.htmltc



.DEFAULT: $(HTMLTPP)

all: $(HTMLTPP) run-test run-benchmark

run-%: build/%
	@echo "RUN   $<"
	@$<

$(PRECOMPILED_HEADER): tests/catch.hpp Makefile
	@echo "PRECOMPILE $@"
	@mkdir -p $(dir $@)
	@$(CXX) -x c++-header -DCATCH_CONFIG_MAIN -Wno-unused-macros $(CXXFLAGS_debug) $< -o $@

$(HTMLTPP): $(HTMLTPP_SOURCE) include Makefile
	@echo "BUILD $@"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -lboost_program_options $< -o $@

$(TEST): $(TEST_SOURCE) $(TEST_TEMPLATES) include $(PRECOMPILED_HEADER) Makefile
	@echo "BUILD $@"
	@mkdir -p $(dir $@)
	@$(CXX) -DCATCH_CONFIG_MAIN -include "tests/catch.hpp" $(CXXFLAGS_debug) $< -o $@

$(BENCHMARK): $(BENCHMARK_SOURCE) $(BENCHMARK_TEMPLATES) include Makefile
	@echo "BUILD $@"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS_release) $< -o $@

$(BUILD_DIR)/%.htmltc: % $(HTMLTPP) include Makefile
	@echo "PREPROCESS $<"
	@mkdir -p $(dir $@)
	@$(HTMLTPP) $@ $(wildcard $</*.htmlt)


clean:
	@echo "CLEAN $(BUILD_DIR)"
	@rm -Rf $(BUILD_DIR)

full-clean: clean
	@echo "CLEAN tests/catch.hpp.pch"
	@rm -f tests/catch.hpp.pch

