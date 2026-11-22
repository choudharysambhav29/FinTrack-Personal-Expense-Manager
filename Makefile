CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = fintrack

# Source and Object files
SRCS = main.c expense.c budget.c analysis.c chart.c export.c fileio.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	-del /Q /F *.o $(TARGET) $(TARGET).exe 2>nul
else
	rm -f *.o $(TARGET) $(TARGET).exe
endif

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
