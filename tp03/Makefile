CC = gcc
CFLAGS = -Wall -pthread
DEPS = writenoncanonical.h # adicionar noncanonical.h
OBJ = writenoncanonical.o  # adicionar noncanonical.o
TARGETS = app_writer       # adicionar app_reader

all: app_writer						 # adicionar app_reader

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
