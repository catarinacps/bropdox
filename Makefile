#	-- bropdox --
#
#	Makefile do arquivo
#
#	@param target
#		"all" - compila
#		"clean" - limpa os binários gerados na compilação
#		"redo" - realiza a regra clean e então compila

#	Flags de compilaçao. Debug para uso no GDB
CC = gcc
DEBUG = -g
CFLAGS = -I$(INC_DIR) -Wall -m32 -std=c99 $(DEBUG)

#	Diretorios do projeto
INC_DIR = include
OBJ_DIR = bin
LIB_DIR = lib
SRC_DIR = src

#	Caminho do arquivo estático final
TARGET = 

#	Dependencias, ou seja, arquivos de header
_DEPS =
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

#	Objetos a serem criados
_OBJ =
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

#	Exceções para a regra "clean"
_EXP =
EXP = $(patsubst %,$(LIB_DIR)/%,$(_EXP))


all: $(TARGET)

$(TARGET): $(OBJ) $(EXP)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJ) $(INC_DIR)/*~ $(LIB_DIR)/*.a *~

redo: clean
	make all

.PHONY: clean all

.DEFAULT_GOAL: redo