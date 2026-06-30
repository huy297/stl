# mystl — a from-scratch reimplementation of parts of the C++ STL
#
#   make          build every test in tests/ into build/
#   make test     build, then run all tests
#   make clean    remove build/

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Iinclude -Wall -Wextra
BUILD    := build

SRCS    := $(wildcard tests/*.cpp)
BINS    := $(patsubst tests/%.cpp,$(BUILD)/%,$(SRCS))
HEADERS := $(wildcard include/mystl/*.hpp)

.PHONY: all test clean

all: $(BINS)

$(BUILD)/%: tests/%.cpp $(HEADERS) | $(BUILD)
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
