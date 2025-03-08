LIB_DIRS := $(wildcard ./lib/*/include)

INCLUDE_DIRS := $(LIB_DIRS) ./include

SRC_DIR := ./src

BUILD_DIR := ./build

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

INCLUDE_FLAGS := $(INCLUDE_DIRS:%=-I%)

LIB_DIR_FLAGS := $(LIB_DIRS:%=-L%)

LIB_FLAGS := $(LIB_DIRS:./lib/%=-l%) -lreadline

CFLAGS := -Werror -Wall -Wextra

minishell: $(OBJ_FILES)
	$(CC) $(INCLUDE_FLAGS) $(CFLAGS) $(LIB_DIR_FLAGS) -g $^ -o $@ $(LIB_FLAGS)


$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(INCLUDE_FLAGS) $(CFLAGS) -c $? -o $@

.PHONY: clean
clean:
	rm build/*

