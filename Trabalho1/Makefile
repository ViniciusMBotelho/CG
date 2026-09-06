CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -I/usr/include
LDFLAGS = -lGL -lGLU -lglut

SRC_DIR = src
OBJ_DIR = build
TARGET = tp1

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Ligando $(TARGET)..."
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Compilação concluída com sucesso! Execute com 'make run' ou './$(TARGET)'."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@echo "Executando $(TARGET)..."
	./$(TARGET)

clean:
	@echo "Limpando arquivos compilados..."
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Limpeza concluída."
