CC = g++
CFLAGS = -Wall -std=c++11
TARGET = prime-calculator

all: $(TARGET)

$(TARGET): prime-calculator.cpp
	$(CC) $(CFLAGS) -o $(TARGET) prime-calculator.cpp

clean:
	rm -f $(TARGET)

