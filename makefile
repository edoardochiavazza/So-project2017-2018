CC=gcc
DEPS = config.h sem_utils.h gestore_utils.h student_utils.h
OBJ = ./obj
BIN = ./bin
VPATH = src:src/gestore:src/student:src/lib
CFLAGS = -Wall -lm

all: gestore student

$(OBJ)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gestore: $(OBJ)/gestore.o $(OBJ)/config.o $(OBJ)/gestore_utils.o $(OBJ)/sem_utils.o
	$(CC) -o $(BIN)/gestore $(OBJ)/gestore.o $(OBJ)/config.o $(OBJ)/gestore_utils.o $(OBJ)/sem_utils.o $(CFLAGS)

student: $(OBJ)/student.o $(OBJ)/config.o $(OBJ)/gestore_utils.o $(OBJ)/sem_utils.o $(OBJ)/student_utils.o
	$(CC) -o $(BIN)/student $(OBJ)/student.o $(OBJ)/config.o $(OBJ)/gestore_utils.o $(OBJ)/sem_utils.o $(OBJ)/student_utils.o $(CFLAGS)

run:
	$(BIN)/gestore

.PHONY: clean
clean:
	rm $(OBJ)/*.o
