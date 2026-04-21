CC = gcc

CFLAGS = -Wall -Wextra -Werror -Iinclude

SRC = src/main.c src/database.c src/account.c
TARGET = bank

all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)

format:
	find . \( -name "*.c" -o -name "*.h" \) -print0 | xargs -0 clang-format -i
