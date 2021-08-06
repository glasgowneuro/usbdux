all: cmd dout outp din inp pwm info
	make -C outp_ncurses

cmd.o: cmd.c
	gcc -g -c cmd.c

dout.o: dout.c
	gcc -c dout.c

din.o: din.c
	gcc -c din.c

outp.o: outp.c
	gcc -c outp.c

cmd: cmd.o
	gcc -g -o cmd cmd.o -lcomedi -lm 

outp: outp.o
	gcc -o outp outp.o -lcomedi -lm 

dout: dout.o
	gcc -o dout dout.o -lcomedi -lm 

din: din.o
	gcc -o din din.o -lcomedi -lm 

inp: inp.c
	gcc -o inp inp.c -lcomedi -lm 

pwm: pwm.c
	gcc -o pwm pwm.c -lcomedi -lm 

info: info.c
	gcc -o info info.c -lcomedi -lm 

clean:
	rm -f *.o cmd dout outp *~ *.dat info pwm inp din
	make -C outp_ncurses clean

