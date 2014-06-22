CC=clang++
CFLAGS=-Wall -std=c++11 -g -I/usr/local/include
LDFLAGS=-lgdal -L/usr/local/lib
SOURCE_DIR=src/
SOURCES=$(SOURCE_DIR)main.cpp $(SOURCE_DIR)visvalingam_algorithm.cpp $(SOURCE_DIR)geo_types.cpp
HEADERS=$(SOURCE_DIR)visvalingam_algorithm.h $(SOURCE_DIR)geo_types.h $(SOURCE_DIR)heap.hpp
OBJECTS=$(SOURCES:.cpp=.o)
BIN_DIR=bin/
BINARY=$(BIN_DIR)simplify

all: $(BINARY) check

$(BINARY): $(SOURCES) $(HEADERS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $@

.PHONY: check clean

check: $(BINARY)
	$(BINARY) --check

clean:
	rm -rf $(BIN_DIR)*


