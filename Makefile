CC      = gcc
CFLAGS  = -Wall -Wextra -g -fprofile-arcs -ftest-coverage -Iinclude
LDFLAGS = -fprofile-arcs -ftest-coverage

NAME    = bin/my_shell

SRC     = src/main.c src/shell.c src/error.c src/parser.c src/executer.c src/built_in.c src/history.c src/alias.c
OBJ     = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	mkdir -p bin
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) src/*.gcda src/*.gcno src/*.gcov *.gcda *.gcno *.gcov bin/my_shell 

fclean: clean
	rm -f $(NAME)

re: fclean all

coverage:	all
	./bin/my_shell <<EOF
	ls
	date
	EOF
	gcov src/*.c
