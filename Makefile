CXX = g++
FLAGS = -Wall -Werror -Wextra
SRCS = main.cpp buffer.cpp program.cpp
HEADERS = buffer.h logger.h program.h
EXEC = main.out

all: main

main:
	$(CXX) $(SRCS) -o $(EXEC)

clean:
	rm -rf $(EXEC)

rebuild: clean all