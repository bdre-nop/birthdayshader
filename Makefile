HOMEBREW_PREFIX = $(shell brew --prefix)
CXX = clang++
CC = clang
CXXFLAGS = -std=c++11 -I$(HOMEBREW_PREFIX)/include
CFLAGS = -I$(HOMEBREW_PREFIX)/include
LDFLAGS = -L$(HOMEBREW_PREFIX)/lib -lglfw -lGLEW -framework OpenGL

# Default target C++
all: birthdayshader

# Build the C++ version
birthdayshader: birthdayshader.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Build the C version
c: birthdayshader_c
birthdayshader_c: birthdayshader.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f birthdayshader birthdayshader_c

.PHONY: all clean c
