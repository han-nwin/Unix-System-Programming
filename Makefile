# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable
TARGET = my_program

# Automatically find all .c files in the current directory
SRCS = $(wildcard *.c)

# Default rule: Build the executable from .c files
all: $(TARGET)

# Rule to compile all .c files and create the final executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Clean up the executable
clean:
	rm -f $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

