CC=g++

SRC_DIR := src
OBJ_DIR := obj
LIB_OBJ_DIR := $(OBJ_DIR)/lib
BIN_DIR := bin
INS_DIR := ~/bin
AGIMUS_BIN_DIR := ../bin

SRC := $(wildcard $(SRC_DIR)/*.cpp)
LIB_SRC := $(wildcard $(SRC_DIR)/lib/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
LIB_OBJ := $(LIB_SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

EXECUTABLES := $(BIN_DIR)/automd $(BIN_DIR)/automd_production $(BIN_DIR)/automd_preproduction $(BIN_DIR)/automd_initialize #$(BIN_DIR)/automd_coldequilibrate $(BIN_DIR)/automd_heating $(BIN_DIR)/automd_minimize $(BIN_DIR)/automd_hotequilibrate 

CPPFLAGS := -Iinclude -MMD -MP
CFLAGS   := -Wall
LDFLAGS  := -Llib
LDLIBS   := -lm -lstdc++fs

.PHONY: all clean install

all: $(EXECUTABLES)

$(BIN_DIR)/automd: $(OBJ_DIR)/automd.o $(LIB_OBJ) | $(BIN_DIR) $(LIB_OBJ_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR)/automd_initialize: $(OBJ_DIR)/initialize.o $(LIB_OBJ) | $(BIN_DIR) $(LIB_OBJ_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR)/automd_production: $(OBJ_DIR)/production.o $(LIB_OBJ) | $(BIN_DIR) $(LIB_OBJ_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	
$(BIN_DIR)/automd_preproduction: $(OBJ_DIR)/preproduction.o $(LIB_OBJ) | $(BIN_DIR) $(LIB_OBJ_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(LIB_OBJ_DIR)/%.o: $(SRC_DIR)/lib/%.cpp | $(LIB_OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR) $(LIB_OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

install: 
	cp $(EXECUTABLES) $(INS_DIR)
	cp $(EXECUTABLES) $(AGIMUS_BIN_DIR)

-include $(OBJ:.o=.d)

