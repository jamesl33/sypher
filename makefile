TARGET = steganography

CC=g++
CFLAGS = -std=c++11 $(shell pkg-config --cflags opencv)

LINKER = g++
LFLAGS = $(shell pkg-config --libs opencv)

SRC_EXT = cpp
INC_EXT = hpp

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SRC_LST = $(wildcard $(SRC_DIR)/*.$(SRC_EXT))
INC_LST = $(wildcard $(SRC_DIR)/*.$(INC_EXT))
OBJ_LST = $(SRC_LST:$(SRC_DIR)/%.$(SRC_EXT)=$(BUILD_DIR)/%.o)

MKDIR = mkdir --parent
RM = rm -f

$(BIN_DIR)/$(TARGET) : $(OBJ_LST)
	@$(MKDIR) $(BIN_DIR)
	@$(LINKER) $(OBJ_LST) $(LFLAGS) -o $@
	@echo "Successfully linked '"$(TARGET)"'!"

$(OBJ_LST) : $(BUILD_DIR)/%.o : $(SRC_DIR)/%.$(SRC_EXT)
	@$(MKDIR) $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled '"$<"' to '"$@"' successfully!"

.PHONY: configure
configure:
	@curl https://raw.githubusercontent.com/myint/optparse/master/optparse.h --silent --create-dirs --output src/optparse.hpp

.PHONY: clean
clean:
	@$(RM) $(OBJ_LST)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(RM) "src/argparse.hpp"
	@$(RM) $(BIN_DIR)/$(TARGET)
	@echo "Executable removed!"
