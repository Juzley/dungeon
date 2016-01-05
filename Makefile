TARGET = dungeon
CC = gcc
CFLAGS = -std=c++11 -Werror -Wall -Wextra -Wno-unused-parameter
ifeq ($(OS),Windows_NT)
	LIBS = -mwindows -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -lvorbisfile -lvorbisenc -lstdc++ 
else
	LIBS = -lSDL2 -lSDL2_mixer -lm -lstdc++
endif

.PHONY: default all clean

default: $(TARGET)

all: default

debug: CFLAGS += -g -O0
debug: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -r $(TARGET)
