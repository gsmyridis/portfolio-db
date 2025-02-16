TARGET = bin/folio
SRC = $(wildcard src/*c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -h
	./$(TARGET) -n
	./$(TARGET) -f portfolio.db

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude

