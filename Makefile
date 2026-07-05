CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g
TARGET = gradebook
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) sample_students.csv

debug: $(TARGET)
	gdb ./$(TARGET)

clean:
	rm -f $(TARGET) grade_report.txt
