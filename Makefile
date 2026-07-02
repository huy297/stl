# mystl — a from-scratch reimplementation of parts of the C++ STL
#
#   make          build every test into build/
#   make test     build, then run all tests
#   make clean    remove build/

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Iinclude -Wall -Wextra
BUILD    := build

SRCS    := $(shell find tests -name '*.cpp')
BINS    := $(addprefix $(BUILD)/,$(basename $(notdir $(SRCS))))
HEADERS := $(shell find include -name '*.hpp')

vpath %.cpp $(sort $(dir $(SRCS)))

.PHONY: all test clean

all: $(BINS)

$(BUILD)/%: %.cpp $(HEADERS) | $(BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

test: all
	@fail=0; \
	for t in $(BINS); do \
		printf '\033[1m==> %s\033[0m\n' "$$t"; \
		if ! "$$t"; then fail=1; echo "    FAILED"; fi; \
	done; \
	if [ $$fail -eq 0 ]; then echo "ALL TESTS PASSED"; else echo "SOME TESTS FAILED"; exit 1; fi

clean:
	rm -rf $(BUILD)
