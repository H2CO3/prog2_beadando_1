CXX = clang++
LD = $(CXX)

CXXFLAGS = -c -std=c++11 -Wall -O0 -g -I. $(shell wx-config --cflags)
LDFLAGS = $(shell wx-config --libs) -lsqlite3

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: beadando_1

beadando_1: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf *.o beadando_1

.PHONY: all clean

