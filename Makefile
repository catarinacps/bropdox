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
OBJ_DIR = bin
OUT_DIR = build
SRC_DIR = src
LIB_DIR = lib
TST_DIR = tests

#	Flags de compilaçao. Debug para uso no GDB
CC = g++ -std=c++14
DFLAG = 
DEBUG =\
	-g \
	-fsanitize=address
CFLAGS =\
	-Wall \
	-Wextra \
	-Wshadow \
	-Wunreachable-code
OPT = $(if $(DFLAG), -O0, -O3)
LIB = -L$(LIB_DIR)\
	-linotify-cpp \
	-lboost_system \
	-lboost_filesystem \
	-lpthread
INC = -I$(INC_DIR)

TFLAGS = --error-printer

####################################################################################################
#	Arquivos:

#	Fonte da main
MAIN = $(wildcard src/*.cpp)

#	Caminho do arquivo estático final
TARGET = $(patsubst %.cpp, $(OUT_DIR)/%, $(notdir $(MAIN)))

#	Outros arquivos fonte
SRC =\
	$(wildcard src/client/*.cpp)\
	$(wildcard src/server/*.cpp)\
	$(wildcard src/helpers/*.cpp)\
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

#	Binarios
$(TARGET): $(OUT_DIR)/%: $(SRC_DIR)/%.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(LIB) $(if $(DFLAG), $(DEBUG)) $(OPT)

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.cpp
	$(CC) -c -o $@ $< $(INC) $(CFLAGS) $(if $(DFLAG), $(DEBUG)) $(OPT)

#	Testes
$(T_TG): $(T_TG).o $(wildcard $(OBJ_DIR)/*.o)
	$(CC) -o $@ $^ $(CFLAGS)

$(T_TG).o: $(T_SRC)
	$(CC) -c -o $@ $< $(INC) $(CFLAGS)

$(T_SRC): $(T_DEP)
	cxxtestgen $(TFLAGS) -o $@ $^

####################################################################################################
#	Alvos:

.DEFAULT_GOAL = all

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
