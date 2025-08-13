CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -Wextra -pthread
SRC_DIR = src
SRCS = src/main.cpp src/buffer.cpp src/program.cpp
HEADERS = src/buffer.h src/logger.h src/program.h
EXEC = main.out

all: main

main:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(EXEC)

clean:
	rm -rf $(EXEC)

rebuild: clean all

valgrind: main
	valgrind --leak-check=full ./$(EXEC)