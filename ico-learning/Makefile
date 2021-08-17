LDLIBS= -lm -lcomedi icolearning/icolearning.o icolearning/trace.o -lncurses
OPT= -g -march=native -Wall
INCLUDES=-I./icolearning

all: main

main.o: main.cpp
	g++ $(OPT) $(INCLUDES) -c main.cpp

main: main.o
	make -C icolearning
	g++ main.o $(OPT) $(LDLIBS) -o main
	chmod a+x main

clean:
	make -C icolearning clean
	rm -f main
	rm -f *.o
	rm -f *.dat
	rm -f video/*
	rm -f *~
