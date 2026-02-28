# ==========================================
# CONFIGURAZIONE DEL COMPILATORE
# ==========================================
CC = gcc
# -Wall -Wextra: Abilita tutti i warning per scrivere codice sicuro
# -g: Aggiunge i simboli di debug per poter usare GDB o Valgrind
# -I./include: Spiega al compilatore dove trovare i file .h
CFLAGS = -Wall -Wextra -g -I./include

# ==========================================
# DIRETTORIO DEL PROGETTO
# ==========================================
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Trova tutti i file .c nella cartella src
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Sostituisce l'estensione .c con .o e cambia la cartella in obj/
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Nome dell'eseguibile finale
TARGET = $(BIN_DIR)/app

# ==========================================
# REGOLE DI COMPILAZIONE
# ==========================================

# Regola di default invocata digitando solo 'make'
all: $(TARGET)

# Fase di LINKING: Unisce tutti i file .o per creare l'eseguibile
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Fase di COMPILAZIONE: Trasforma ogni singolo .c in un .o
# $< indica la dipendenza (il .c), $@ indica il target (il .o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regola per fare pulizia profonda digitando 'make clean'
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Evita conflitti se per caso crei un file chiamato "clean" o "all"
.PHONY: all clean
