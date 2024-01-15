INCLUDE_DIR = include
SRC_DIR = src
BIN_DIR = bin
CXXFLAGS = -I$(INCLUDE_DIR) -std=c++11 -pedantic \
	-Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
	-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op \
	-Wmissing-declarations -Wmissing-include-dirs -Wnoexcept \
	-Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
	-Wshadow -Wsign-conversion -Wsign-promo \
	-Wstrict-null-sentinel -Wstrict-overflow=5 \
	-Wswitch-default -Wundef -Werror -Wno-unused \
	-Wswitch-enum -Weffc++
TARGET = test_main

all: directories $(SRC_DIR)/main.cpp
	g++ $(CXXFLAGS) $(SRC_DIR)/main.cpp -o $(BIN_DIR)/$(TARGET)

directories:
	@mkdir -p $(BIN_DIR)
