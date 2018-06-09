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

####################################################################################################
#	Definições:

#	Diretorios do projeto
INC_DIR = include
OBJ_DIR = build
BIN_DIR = bin
SRC_DIR = src
TST_DIR = tests

#	Flags de compilaçao. Debug para uso no GDB
CC = g++
DFLAG = 
DEBUG =\
	-g \
	-fsanitize=address \
	-O0
CFLAGS =\
	-std=c++14 \
	-Wall \
	-Wextra \
	-Wshadow \
	-Wunreachable-code \
	-lboost_system \
	-lboost_filesystem \
	$(if $(DFLAG), $(DEBUG), -O3)
TFLAGS = --error-printer
INC = -I$(INC_DIR)

####################################################################################################
#	Arquivos:

#	Caminho do arquivo estático final
TARGET = $(patsubst src/%.cpp, $(BIN_DIR)/%, $(wildcard src/*.cpp))

#	Arquivos fonte
SRC =\
	$(wildcard src/client/*.cpp)\
	$(wildcard src/server/*.cpp)\
	$(wildcard src/util/*.cpp)

#	Objetos a serem criados
OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC)))

#	Target da regra test
T_TG = $(T_DIR)/AppTest

#	Headers/definições de suites de teste
T_DEP = $(wildcard tests/*.hpp)

#	Codigo fonte a ser gerado para a suite de testes
T_SRC = $(T_TG).cpp

####################################################################################################
#	Regras:

.DEFAULT_GOAL = all

#	Binarios

$(TARGET): $(BIN_DIR)/%: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.cpp
	$(CC) -c -o $@ $< $(INC) $(CFLAGS)

#	Testes

$(T_TG): $(T_TG).o $(wildcard $(OBJ_DIR)/*.o)
	$(CC) -o $@ $^ $(CFLAGS)

$(T_TG).o: $(T_SRC)
	$(CC) -c -o $@ $< $(INC) $(CFLAGS)

$(T_SRC): $(T_DEP)
	cxxtestgen $(TFLAGS) -o $@ $^

####################################################################################################
#	Alvos:

all: $(TARGET)

clean:
	rm -f $(OBJ_DIR)/*.o $(INC_DIR)/*~ $(TARGET) *~ *.o

redo: clean all

test: all 
	@echo "tests are in the making!"

#	Debug de variaveis da make
print-%:
	@echo $* = $($*)

.PHONY: all clean redo test
