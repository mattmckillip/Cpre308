OBJDIR = $(PROJ_ROOT)/obj
OBJS = $(OBJDIR)/isu_task.o
DEPS = isu_task.h 
CFLAGS = -I $(PROJ_ROOT)/include -I $(PROJ_ROOT) -g
LDFLAGS = -L $(PROJ_ROOT)/lib
LIBRARIES = -lllist

all: $(OBJS)
#	gcc $(LDFLAGS) $(LIBRARIES) -o $@ $^

$(OBJDIR)/%.o: %.c $(DEPS)
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -rf *.o $(OBJS)
