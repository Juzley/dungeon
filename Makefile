CC = gcc
CFLAGS = -std=c++11 -Werror -Wall -Wextra -Wno-unused-parameter
ifeq ($(OS),Windows_NT)
	TARGET = dungeon.exe
	LIBS = -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -lvorbisfile -lvorbisenc -lstdc++ 
else
	TARGET = dungeon
	LIBS = -lSDL2 -lm -lstdc++
endif

.PHONY: default all clean

default: $(TARGET)

all: default

debug: CFLAGS += -g -O0
debug: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h, *.hpp)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -r $(TARGET)
