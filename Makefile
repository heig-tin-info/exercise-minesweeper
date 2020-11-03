CSRCS=$(wildcard *.cpp)
COBJS=$(patsubst %.cpp,%.o,$(CSRCS))
EXEC?=minesweeper

CC=g++
CFLAGS=-std=c++11 -g -Wall -pedantic
LDFLAGS=-lncurses

all: build

build: $(EXEC)

-include $(COBJS:.o=.d)

$(EXEC): $(COBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $< -MMD -MF $(@:.o=.d)

lib: lib$(EXEC).so

lib$(EXEC).so: $(CSRCS)
	$(CC) -shared -o $@ $^ -fPIC

clean:
	$(RM) $(EXEC) *.o a.out lib$(EXEC).so $(COBJS:.o=.d)

.PHONY: all format build lib test
