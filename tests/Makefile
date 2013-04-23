CXX = g++
SOURCES = $(wildcard *.cpp)
DEPS    = ../performance.h $(wildcard ../quantum/*.h) ../vector.h
TARGETS = $(basename $(SOURCES))
DEST    = $(addsuffix .o,   $(TARGETS))

INCPATH = -I../../local/include
LIBPATH = -L../../local/lib
LIBS = -lpfm -lpapi -ltbb
OFLAGS = -Wall -O2
DFLAGS = -g3
CFLAGS = -march=native $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH) -fopenmp

UNAME = $(shell uname)
ifeq ($(UNAME), Linux)
LIBS += -lrt
endif

all: $(TARGETS)

$(TARGETS): $(SOURCES) $(DEPS) $(DEST)
	$(CXX) $(CFLAGS) $@.o $(LIBS) -o $@

%.o: %.cpp $(DEPS)
	$(CXX) $(CFLAGS) -c $< -o $@

run: all
	./run-tests.sh

clean:
	rm -f $(TARGETS) $(DEST)