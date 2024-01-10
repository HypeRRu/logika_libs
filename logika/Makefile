INCLUDE_DIR = include
SRC_DIR = src
BIN_DIR = bin
CXXFLAGS = -I$(INCLUDE_DIR) -std=c++11
TARGET = test_main

all: directories $(SRC_DIR)/main.cpp
	g++ $(CXXFLAGS) $(SRC_DIR)/main.cpp -o $(BIN_DIR)/$(TARGET)

directories:
	@mkdir -p $(BIN_DIR)
