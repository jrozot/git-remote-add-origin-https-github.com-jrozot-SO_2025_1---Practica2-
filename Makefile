CC = cc
CFLAGS = -Wall -O2

# Targets
all: search_module user_interface

search_module: search_module.c hash_module.c hash_module.h
	$(CC) $(CFLAGS) search_module.c hash_module.c -o search_module

user_interface: p1-dataProgram.c hash_module.c hash_module.h
	$(CC) $(CFLAGS) p1-dataProgram.c hash_module.c -o user_interface

clean:
	rm -f search_module user_interface
