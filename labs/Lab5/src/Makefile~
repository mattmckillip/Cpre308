NETID := $(shell whoami)

EXES=main
OBJS=main.o queue.o pdf_printer.o print_job.o
CFLAGS=-D NETID=\"$(NETID)\"  -D_GNU_SOURCE
DEBUG=-g -Wall

all: $(EXES)

main: $(OBJS)
	gcc -o main $^ -pthread $(DEBUG)

%.o: %.c *.h
	gcc -c $< $(CFLAGS) $(DEBUG)

doc: 
	doxygen

clean:
	rm -rf *.o
	rm -rf $(EXES)
	
.PHONY: doc
