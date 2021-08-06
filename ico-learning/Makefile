LDLIBS= -lm /usr/local/lib/libcomedi.a -L./icolearning -licolearning
OPT= -g -march=native -Wall
INCLUDES=-I./icolearning

all: main

main.o: main.cpp
	g++ $(OPT) $(INCLUDES) -c main.cpp

main: main.o
	make -C icolearning
	g++ -lncurses main.o -licolearning $(OPT) $(LDLIBS) -o main
	chmod a+x main

clean:
	make -C icolearning clean
	rm -f main
	rm -f *.o
	rm -f *.dat
	rm -f video/*
	rm -f *~
