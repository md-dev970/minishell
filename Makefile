BUILD_TYPE ?= debug

LIB_DIRS := $(wildcard ./lib/*)

INCLUDE_DIRS := $(LIB_DIRS:./lib/%=./lib/%/include) ./include

SRC_DIR := ./src

BUILD_DIR := ./build

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

INCLUDE_FLAGS := $(INCLUDE_DIRS:%=-I%)

LIB_DIR_FLAGS := 

LDFLAGS := $(LIB_DIRS:./lib/%=-L./lib/%) $(LIB_DIRS:./lib/%=-l%) -lreadline

CFLAGS := -Werror -Wall -Wextra

ifeq ($(BUILD_TYPE), debug)
	LDFLAGS += -g -DDEBUG
	CFLAGS += -g -DDEBUG
endif

ifeq ($(BUILD_TYPE), release)
	LDFLAGS += -O3
	CFLAGS += -O3
endif

minishell: $(OBJ_FILES)
	$(CC) $(INCLUDE_FLAGS) $^ -o $@ $(LDFLAGS)


$(OBJ_FILES): $(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(INCLUDE_FLAGS) $(CFLAGS) -c $? -o $@

.PHONY: clean
clean:
	rm build/*

