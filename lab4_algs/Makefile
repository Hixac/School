OBJS   = main.o
DIR    = src
TARGET = main
INC    = -I$(DIR)/

.PHONY: all main-goal clean

all: main-goal

main-goal: $(OBJS)
	g++ -o $(TARGET) $(OBJS)

main.o: $(DIR)/main.cpp
	g++ -c $^ $(INC)

clean:
	rm *.o $(TARGET)
