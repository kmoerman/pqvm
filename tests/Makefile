CC = g++

SOURCES = $(wildcard *.cpp)
DEPS    = ../performance.h $(wildcard ../quantum/*.h) ../vector.h
TARGETS = $(basename $(SOURCES))
DEST    = $(addsuffix .o,   $(TARGETS))

INCPATH = #-I
LIBPATH = #-L
LIBS = -ltbb
OFLAGS = -Wall #-O2
DFLAGS = -g3
CFLAGS = $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH) -fopenmp

UNAME = $(shell uname)
ifeq ($(UNAME), Linux)
LIBS += -lrt
endif

all: $(TARGETS)

$(TARGETS): $(SOURCES) $(DEST)
	$(CC) $(CFLAGS) $@.o $(LIBS) -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(DEST)

virgin:
	rm -f $(TARGETS) $(DEST) *.data *~