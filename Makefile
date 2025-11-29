CC      = gcc
CFLAGS  = -Wall -Wextra -g -fprofile-arcs -ftest-coverage -Iinclude
LDFLAGS = -fprofile-arcs -ftest-coverage

NAME    = bin/my_sh

SRC     = src/main.c src/shell.c src/error.c
OBJ     = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	mkdir -p bin
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) src/*.gcda src/*.gcno src/*.gcov *.gcda *.gcno *.gcov

fclean: clean
	rm -f $(NAME)

re: fclean all

coverage:	all
	./bin/my_sh <<EOF
	ls
	date
	EOF
	gcov src/*.c
