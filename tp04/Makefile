CC = gcc
CFLAGS = -Wall -pthread -Wno-pointer-sign
DEPS = writenoncanonical.h noncanonical.h data_link.h utils.h files.h
OBJ = writenoncanonical.o noncanonical.o data_link.o utils.o files.c
TARGETS = app_writer app_reader

all: app_writer app_reader

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $@

app_writer: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo $@

app_reader: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo $@

clean:
	@rm *.o $(TARGETS)
