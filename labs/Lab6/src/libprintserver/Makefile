# compile the lib_printsever library and the lib_printserver_test.c for now

all:
	gcc -Wall -fPIC -c lib_printserver.c
	gcc -shared -Wl,-soname,libprintserver.so -o libprintserver.so lib_printserver.o
	gcc -L./ lib_printserver_test.c -lprintserver -lrt -o test
	gcc -L./ print_file.c -lprintserver -lrt -o print_file
	
