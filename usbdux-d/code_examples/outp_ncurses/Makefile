PROGNAME := outp_ncurses
LIBRARIES := -lcurses -lcomedi -lm

CFLAGS := -ggdb -march=native

SOURCES := $(wildcard *.c)
OBJS    := $(patsubst %.c, %.o, $(SOURCES))

.PHONY: all clean

all: $(PROGNAME)

%.o: %.c
	gcc $(CFLAGS) -c $<

$(PROGNAME): $(OBJS)
	gcc $(CFLAGS) $(OBJS) $(LIBRARIES) -o $(PROGNAME)

clean:
	rm -f $(PROGNAME) *.o *~

