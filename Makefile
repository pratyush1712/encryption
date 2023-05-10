# Makefile

CC = g++
CFLAGS = -std=c++2a -pg -Wall -Wpedantic -O3 -g -pthread
TARGET = bignum

all: $(TARGET)

$(TARGET): main.cpp bignum.cpp bignum.hpp
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp bignum.cpp

clean:
	rm -f $(TARGET)

zip:
	zip -r $(TARGET).zip main.cpp bignum.cpp bignum.hpp Makefile -x "*.git*" -x "*.vscode*"
