#	-- bropdox --
#
#	Makefile do projeto bropdox
#
#	@param target
#		"all" - compila
#		"clean" - limpa os binários gerados na compilação
#		"redo" - limpa binários e então compila
#		"test" - compila e realiza testes
#
#	Se make não recebe parâmetros, a ação default é redo

#	Flags de compilaçao. Debug para uso no GDB
CC = gcc
DEBUG = -g
CFLAGS = -Wall -std=c99 $(DEBUG)
INC_FLAG = -I$(INC_DIR)

#	Diretorios do projeto
INC_DIR = include
OBJ_DIR = build
BIN_DIR = bin
SRC_DIR = src
TST_DIR = tests

#	Caminho do arquivo estático final
_TARGET = dropboxServer dropboxClient
TARGET = $(patsubst %,$(BIN_DIR)/%,$(_TARGET))

#	Dependencias, ou seja, arquivos de header
_DEPS = dropboxServer.h dropboxClient.h dropboxUtil.h
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

#	Objetos a serem criados
_OBJ = dropboxServer.o dropboxClient.o dropboxUtil.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

#	Exceções para a regra "clean"
_EXP = placeholder
EXP = $(patsubst %,$(OBJ_DIR)/%,$(_EXP)) $(patsubst %,$(BIN_DIR)/%,$(_EXP))

.DEFAULT_GOAL = redo

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(INC_FLAG) $(CFLAGS)

clean:
	rm -f $(OBJ) $(INC_DIR)/*~ $(TARGET) *~ *.o

redo: clean all

test: all
	@echo "tests are in the making!"

.PHONY: all clean redo test