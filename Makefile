#	-- bropdox --
#
#	Makefile do projeto bropdox.
#
#	Exemplo de utilização:
#		make <target> [DFLAG=true]
#
#	@param target
#		Pode ser as seguintes opções:
#		all - compila.
#		clean - limpa os binários gerados na compilação.
#		redo - limpa binários e então compila.
#		test - compila e realiza testes.
#		print-<var> - imprime o conteúdo da variável do makefile (exemplo de uso: make print-OBJ).
#
#	@param "DFLAG=true"
#		Quando presente, o programa será compilado em modo debug.
#
#	Se make não recebe parâmetros, a ação default é all


####################################################################################################
#	Definições:

#	-- Diretorios do projeto
INC_DIR = include
OBJ_DIR = bin
OUT_DIR = build
SRC_DIR = src
LIB_DIR = lib

DFLAG = 

#	-- Flags de compilação
#	Compilador e versão da linguagem
CC = g++ -std=c++17
#	Caso DFLAG esteja definida, ativa compilação debug e coloca o address sanitizer junto ao executável
DEBUG = $(if $(DFLAG),-g -fsanitize=address)
CFLAGS =\
	-Wall \
	-Wextra \
	-Wpedantic\
	-Wshadow \
	-Wunreachable-code
OPT = $(if $(DFLAG),-O0,-O3)
LIB = -L$(LIB_DIR)\
	-linotify-cpp \
	-lboost_system \
	-lboost_filesystem \
	-lpthread
INC = -I$(INC_DIR)

####################################################################################################
#	Arquivos:

#	-- Fonte da main
#	Presume que todos os fontes "main" estão na raiz do diretório SRC_DIR
MAIN := $(wildcard $(SRC_DIR)/*.cpp)

#	-- Caminho(s) do(s) binário(s) final/finais
TARGET := $(patsubst %.cpp, $(OUT_DIR)/%, $(notdir $(MAIN)))

#	-- Outros arquivos fonte
SRC := $(filter-out $(MAIN), $(shell find $(SRC_DIR) -name '*.cpp'))

#	-- Objetos a serem criados
OBJ := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC)))

####################################################################################################
#	Regras:

#	-- Executáveis
$(TARGET): $(OUT_DIR)/%: $(SRC_DIR)/%.cpp $(OBJ)
	$(CC) -o $@ $^ $(INC) $(LIB) $(DEBUG) $(OPT)

#	-- Objetos
$(OBJ_DIR)/%.o:
	$(CC) -c -o $@ $(filter %/$*.cpp, $(SRC)) $(INC) $(CFLAGS) $(DEBUG) $(OPT)

####################################################################################################
#	Alvos:

.DEFAULT_GOAL = all

all: $(TARGET)

clean:
	rm -f $(OBJ_DIR)/*.o $(INC_DIR)/*~ $(TARGET) *~ *.o

redo: clean all

#	Debug de variaveis da make
print-%:
	@echo $* = $($*)

.PHONY: all clean redo print-%
