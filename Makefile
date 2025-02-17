TARGET = bin/folio
SRC = $(wildcard src/*c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -h
	./$(TARGET) -n
	./$(TARGET) -f portfolio.db
	./$(TARGET) -f portfolio.db -a "BTC, 2025-01-31, 1, 100000, BUY"
	./$(TARGET) -f portfolio.db -a "BTC, 2025-01-31, 1, 100000, BUY"

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude

