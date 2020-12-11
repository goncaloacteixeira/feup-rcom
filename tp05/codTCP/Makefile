CC = gcc
CFLAGS = -Wall -pthread -Wno-pointer-sign -g
DEPS = connection.h getip.h 
OBJ = connection.o getip.o
TARGETS = download

all: download

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $@

download: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo $@

clean:
	@rm *.o $(TARGETS)
