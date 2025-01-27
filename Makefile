INCLUDE_FLAGS = -I./libft/include

main: ./src/main.c ./libft/libft.a
	@$(CC) $(INCLUDE_FLAGS) $(CFLAGS) -g $^ -o $@ -lreadline
