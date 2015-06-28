# The name of the library
NAME = module
# The file that should be included to use the library
INCLUDE = isu_module.h

OBJS = isu_module.o
DEPS = isu_module.h
CFLAGS = -I $(PROJ_ROOT)/include
LDFLAGS = -L $(PROJ_ROOT)/lib
LIBRARIES = -ldl

LIB_NAME = lib$(NAME).so

$(LIB_NAME): $(OBJS)
	gcc -shared -Wl,-soname,$(LIB_NAME) $(LDFLAGS) $(LIBRARIES) -o $@ $^

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
