RELEASE := y
BUILD_DIR := build

CXX := clang++ -std=c++11

CXXFLAGS_common  := -Iinclude -Ibuild -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-c99-extensions -Wno-exit-time-destructors -Wno-missing-variable-declarations
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
TEST_TEMPLATES := $(addprefix $(BUILD_DIR)/,$(TEST_TEMPLATES_SOURCES:.htmlt=.htmltc))


.DEFAULT: $(HTMLTPP)

all: test

test: $(TEST)
	@echo "RUN   $<"
	@$<

$(PRECOMPILED_HEADER): tests/catch.hpp Makefile
	@echo "PRECOMPILE $@"
	@mkdir -p $(dir $@)
	@$(CXX) -x c++-header -DCATCH_CONFIG_MAIN -Wno-unused-macros $(CXXFLAGS_debug) $< -o $@

$(HTMLTPP): $(HTMLTPP_SOURCE) $(HEADERS) Makefile
	@echo "BUILD $@"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $< -o $@

$(TEST): $(TEST_SOURCE) $(TEST_TEMPLATES) $(HEADERS) $(PRECOMPILED_HEADER) Makefile
	@echo "BUILD $@"
	@mkdir -p $(dir $@)
	@$(CXX) -DCATCH_CONFIG_MAIN -include "tests/catch.hpp" $(CXXFLAGS_debug) $< -o $@

$(BUILD_DIR)/%.htmltc: %.htmlt $(HTMLTPP) $(HEADERS) Makefile
	@echo "PREPROCESS $<"
	@mkdir -p $(dir $@)
	@$(HTMLTPP) $< $@


clean:
	@echo "CLEAN $(BUILD_DIR)"
	@rm -Rf $(BUILD_DIR)
	@rm tests/catch.hpp.pch

