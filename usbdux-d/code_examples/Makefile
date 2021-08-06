all: pwm cmd dout outp inp info din
	make -C outp_ncurses

cmd.o: cmd.c
	gcc -c cmd.c

dout.o: dout.c
	gcc -c dout.c

outp.o: outp.c
	gcc -c outp.c

info.o: info.c
	gcc -c info.c

pwm.o: pwm.c
	gcc -c pwm.c

cmd: cmd.o
	gcc -o cmd cmd.o -lcomedi -lm 

outp: outp.o
	gcc -o outp outp.o -lcomedi -lm 

dout: dout.o
	gcc -o dout dout.o -lcomedi -lm 

info: info.o
	gcc -o info info.o -lcomedi -lm 

pwm: pwm.o
	gcc -o pwm pwm.o -lcomedi -lm 

din: din.c
	gcc -o din din.c -lcomedi -lm 

inp: inp.c
	gcc -o inp inp.c -lcomedi -lm 

clean:
	rm -f *.o info pwm cmd dout outp inp *~ *.dat din
	make -C outp_ncurses clean
