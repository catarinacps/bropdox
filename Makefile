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
#	Se make não recebe parâmetros, a ação default é all

#	Flags de compilaçao. Debug para uso no GDB
CC = g++
DFLAG = 
DEBUG = -g -fsanitize=address -O0
CFLAGS = -std=c++11 -Wall -Wextra -Wshadow -Wunreachable-code -lpthread -lstdc++fs $(if $(DFLAG), $(DEBUG), -O3)
TFLAGS = --error-printer
INC_FLAG := -I$(INC_DIR)

#	Diretorios do projeto
INC_DIR = include
OBJ_DIR = build
BIN_DIR = bin
SRC_DIR = src
TST_DIR = tests

#	Caminho do arquivo estático final
_TARGET = mserver mclient
TARGET = $(patsubst %, $(BIN_DIR)/%, $(_TARGET))

#	Dependencias, ou seja, arquivos de header
_DEPS = Server.hpp Client.hpp bropdoxUtil.hpp SocketHandler.hpp RequestHandler.hpp FileHandler.hpp
DEPS = $(patsubst %, $(INC_DIR)/%, $(_DEPS))

#	Objetos a serem criados
_OBJ = Server.o Client.o bropdoxUtil.o SocketHandler.o RequestHandler.o FileHandler.o
OBJ = $(patsubst %, $(OBJ_DIR)/%, $(_OBJ))

#	Target da regra test
T_TG = $(T_DIR)/AppTest

#	Headers/definições de suites de teste
_T_DEP = AppTest.hpp
T_DEP = $(patsubst %, $(T_DIR)/%, $(T_TG))

#	Codigo fonte a ser gerado para a suite de testes
T_SRC = $(T_TG).cpp

#	Exceções para a regra "clean"
# _EXP = placeholder
# EXP = $(patsubst %,$(OBJ_DIR)/%,$(_EXP)) $(patsubst %,$(BIN_DIR)/%,$(_EXP))

.DEFAULT_GOAL = all

$(TARGET): $(BIN_DIR)/%: $(OBJ_DIR)/%.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(INC_FLAG) $(CFLAGS)

$(T_TG): $(T_TG).o $(wildcard $(OBJ_DIR)/*.o)
	$(CC) -o $@ $^ $(CFLAGS)

$(T_TG).o: $(T_SRC)
	$(CC) -c -o $@ $< $(INC_FLAG) $(CFLAGS)

$(T_SRC): $(T_DEP)
	cxxtestgen $(TFLAGS) -o $@ $^

all: $(TARGET)

clean:
	rm -f $(OBJ_DIR)/*.o $(INC_DIR)/*~ $(TARGET) *~ *.o

redo: clean all

test: all 
	@echo "tests are in the making!"

.PHONY: all clean redo test