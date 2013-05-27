CXX = g++

TARGETS = pqvm
DEPS    = $(wildcard ../quantum/*.h) vector.h
SOURCES = $(addsuffix .cpp, $(TARGETS))
OBJECTS = $(addsuffix .o,   $(TARGETS))

INCPATH = -I $$HOME/local/include
LIBPATH = -L $$HOME/local/lib
LIBS = -lsexp -ltbb
OFLAGS = -Wall -O2
DFLAGS = -g3
CFLAGS = -march=native $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH) -fopenmp

UNAME = $(shell uname)
ifeq ($(UNAME), Linux)
LIBS += -lrt
endif

.PHONY: all clean

all: $(TARGETS)

pqvm: $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LIBS) -o $@ 

%.o: %.cpp $(DEPS)
	$(CXX) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) $(OBJECTS)