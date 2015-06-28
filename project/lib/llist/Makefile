#The name of the library
NAME = llist
# The file that should be included to use the library
INCLUDE = isu_llist.h

OBJS = isu_llist.o
DEPS = isu_llist.h
CFLAGS = -I $(PROJ_ROOT)/include -g
LDFLAGS = -L $(PROJ_ROOT)/lib


LIB_NAME = lib$(NAME).so

all: check-env $(LIB_NAME)

$(LIB_NAME): $(OBJS)
	gcc -shared -Wl,-soname,$(LIB_NAME) $(LDFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	gcc $(CFLAGS) -o $@ -fPIC -c $<

install:
	cp $(LIB_NAME) $(PROJ_ROOT)/lib/$(LIB_NAME)
	mkdir -p $(PROJ_ROOT)/include/$(NAME)
	cp $(INCLUDE) $(PROJ_ROOT)/include/$(NAME)/

uninstall:
	rm -f $(PROJ_ROOT)/lib/$(LIB_NAME)
	rm -rf $(PROJ_ROOT)/include/$(NAME)/

clean:
	rm -rf *.o *.so

check-env:
ifndef PROJ_ROOT
	$(error PROJ_ROOT is undefined, source source.me in the root of the project)
endif
