OBJS = main.o avl.o brt.o bst.o test.o
FLAGS = -g

.PHONY: all clean

all: $(OBJS)
	gcc $(FLAGS) -o main $^

main.o: main.c
	gcc $(FLAGS) -c $^

bst.o: bst.c
	gcc $(FLAGS) -c $^

avl.o: avl.c
	gcc $(FLAGS) -c $^

brt.o: brt.c
	gcc $(FLAGS) -c $^

test.o: test.c
	gcc $(FLAGS) -c $^

clean:
	rm *.o main
