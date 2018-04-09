#	-- bropdox --
#
#	Makefile do arquivo
#
#	@param target
#		"all" - compila
#		"clean" - limpa os binários gerados na compilação
#		"redo" - realiza a regra clean e então compila
#
#	Se make não recebe parâmetros, a ação default é clean e make all 

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


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c $@ $< $(INC_FLAG) $(CFLAGS)

clean:
	rm -f $(OBJ) $(INC_DIR)/*~ $(TARGET) *~

redo: clean
	make all

test: all
	@echo "tests are in the making!"

.PHONY: clean all

.DEFAULT_GOAL: redo