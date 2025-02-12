INCLUDE_DIRS := ./libft/include ./include

SRC_DIR := ./src

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')

OBJ_FILES := $(SRC_FILES:%.c=%.o)

INCLUDE_FLAGS := $(INCLUDE_DIRS:%=-I%)


minishell: $(SRC_FILES)
	$(CC) $(INCLUDE_FLAGS) $(CFLAGS) -g $^ -o $@ ./libft/libft.a -lreadline

