

.PHONY: all main-goal clean

all: main-goal clean

main-goal: main.o
	g++ -g -Wall -std=c++20 -o main $^

main.o: main.cpp
	g++ -c $^

clean:
	rm *.o
