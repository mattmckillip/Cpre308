OBJDIR = $(PROJ_ROOT)/obj
OBJS = sched_test.o $(OBJDIR)/*.o
MEMS = mem_test.o $(OBJDIR)/isu_mmu.o $(OBJDIR)/isu_mem_req.o
CFLAGS = -I $(PROJ_ROOT)/include -I $(PROJ_ROOT) -g
LDFLAGS = -L $(PROJ_ROOT)/lib
LIBRARIES = -lllist -lmodule -ldl

all: sub_dirs sched_test mem_test

mem_test: $(MEMS)
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

sched_test: $(OBJS)
	gcc $(LDFLAGS) -o $@ $^ $(LIBRARIES)

%.o: %.c $(DEPS)
	gcc $(CFLAGS) -o $@ -c $<

$(OBJDIR)/*.o: sub_dirs

sub_dirs: force_look
	cd scheduler; $(MAKE) $(MFLAGS)
	cd task; $(MAKE) $(MFLAGS)
	cd page_req; $(MAKE) $(MFLAGS)
	cd isu_mmu; $(MAKE) $(MFLAGS)

clean:
	rm -rf *.o $(OBJS) sched_test mem_test

force_look:
	true

