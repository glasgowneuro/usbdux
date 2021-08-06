all: cmd

cmd.o: cmd.c
	gcc -c cmd.c

cmd: cmd.o
	gcc -o cmd cmd.o -lcomedi -lm 

clean:
	rm -f *.o cmd *~ *.dat

