OPT= -O3 -g -Wall
#OPT=-g -march=pentium -Wall

all: demo

demo.o: demo.cpp
	g++ $(OPT) -c demo.cpp

icolearning.o: icolearning.cpp icolearning.h
	g++ $(OPT) -c icolearning.cpp

trace.o: trace.cpp trace.h
	g++ $(OPT) -c trace.cpp

demo: demo.o icolearning.o trace.o
	g++ demo.o trace.o icolearning.o -o demo
	chmod a+x demo

clean:
	rm -f demo
	rm -f *.o
	rm -f *.dat
	rm -f *.ps *.eps
	rm -f *~
