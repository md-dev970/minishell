INCLUDE_DIRS := ./libft/include ./include

SRC_DIR := ./src

BUILD_DIR := ./build

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

INCLUDE_FLAGS := $(INCLUDE_DIRS:%=-I%)


minishell: $(OBJ_FILES)
	$(CC) $(INCLUDE_FLAGS) $(CFLAGS) -L./libft -g $^ -o $@ -lft -lreadline


$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(INCLUDE_FLAGS) $(CFLAGS) -c $? -o $@
