RELEASE := y
BUILD_DIR := build

CXX := clang++ -std=c++11

CXXFLAGS_common  := -Iinclude -Ibuild
CXXFLAGS_release := $(CXXFLAGS_common) -O3 -flto -s
CXXFLAGS_debug   := $(CXXFLAGS_common) -ggdb3

CXXFLAGS_warnings := -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-global-constructors -Wno-exit-time-destructors -Wno-deprecated
CXXFLAGS_no_warnings := -w -Wno-deprecated

ifeq ($(RELEASE), y)
	CXXFLAGS := $(CXXFLAGS_release)
else
	CXXFLAGS := $(CXXFLAGS_debug)
endif


.SUFFIXES:

HEADERS := $(wildcard include/*.hpp)

PRECOMPILED_CATCH := tests/catch.hpp.pch

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

$(PRECOMPILED_CATCH): tests/catch.hpp Makefile
	@echo "PRECOMPILE $@"
	@mkdir -p $(dir $@)
	@$(CXX) -x c++-header -DCATCH_CONFIG_MAIN -Wno-unused-macros $(CXXFLAGS_debug) $(CXXFLAGS_no_warnings) $< -o $@

$(HTMLTPP): $(HTMLTPP_SOURCE) include Makefile
	@echo "BUILD $@"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(CXXFLAGS_warnings) $< -o $@

$(TEST): $(TEST_SOURCE) $(TEST_TEMPLATES) include $(PRECOMPILED_CATCH) Makefile
	@echo "BUILD $@"
	@mkdir -p $(dir $@)
	@$(CXX) -DCATCH_CONFIG_MAIN -include "tests/catch.hpp" $(CXXFLAGS_debug) $(CXXFLAGS_warnings) $< -o $@

$(BENCHMARK): $(BENCHMARK_SOURCE) $(BENCHMARK_TEMPLATES) include Makefile
	@echo "BUILD $@"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS_release) $(CXXFLAGS_warnings) $< -o $@

$(BUILD_DIR)/%.htmltc: % $(HTMLTPP) include Makefile
	@echo "PREPROCESS $<"
	@mkdir -p $(dir $@)
	@$(HTMLTPP) $@ $(wildcard $</*.htmlt)


clean:
	@echo "CLEAN $(BUILD_DIR)"
	@rm -Rf $(BUILD_DIR)

full-clean: clean
	@echo "CLEAN $(PRECOMPILED_CATCH)"
	@rm -f $(PRECOMPILED_CATCH)
