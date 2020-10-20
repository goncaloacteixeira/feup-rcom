CC = gcc
CFLAGS = -Wall -pthread
DEPS = writenoncanonical.h noncanonical.h data_link.h application.h utils.h
OBJ = writenoncanonical.o noncanonical.o data_link.o application.o utils.o
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
