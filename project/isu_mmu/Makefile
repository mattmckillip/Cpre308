OBJDIR = $(PROJ_ROOT)/isu_mmu
OBJS = $(OBJDIR)/isu_mmu.o
DEPS = isu_mmu.h ../page_req/isu_mem_req.h
CFLAGS = -I $(PROJ_ROOT)/include -I $(PROJ_ROOT) -g
LDFLAGS = -L $(PROJ_ROOT)/lib
LIBRARIES = -lllist

all: $(OBJS)
	# gcc $(LDFLAGS) $(LIBRARIES) -o $@ $^

$(OBJDIR)/%.o: %.c $(DEPS)
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -rf *.o $(OBJS)
