MOD_NAME = sched-fcfs.mod
OBJS = isu_sched_fcfs.o
DEPS = $(PROJ_ROOT)/scheduler/isu_sched_algorithm.h
CFLAGS= -I $(PROJ_ROOT)/ -I $(PROJ_ROOT)/include

all: $(MOD_NAME)

$(MOD_NAME): $(OBJS)
	gcc -shared -Wl,-soname,$(MOD_NAME) -g -o $@ $^

%.o: %.c $(DEPS)
	gcc $(CFLAGS) -o $@ -g -fPIC -c $<

clean:
	rm -rf *.o *.mod
