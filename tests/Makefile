CXX     = clang

SOURCES = $(wildcard *.cpp)
DEPS    = ../performance.h ../options.h $(wildcard ../quantum/*.h) ../vector.h
TARGETS = $(basename $(SOURCES))
OBJECTS = $(addsuffix .o, $(TARGETS))

INCPATH = -I $$HOME/local/include
LIBPATH = -L $$HOME/local/lib
LIBS    = -lpfm -lpapi -ltbb -lstdc++ -lm
OFLAGS  = -Wall -O2
DFLAGS  = -g3
CFLAGS  = -march=native $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH) -fopenmp

UNAME   = $(shell uname)
ifeq ($(UNAME), Linux)
  LIBS += -lrt
endif

.PHONY: all clean

all: $(TARGETS)

$(TARGETS): % : %.o
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.cpp $(DEPS)
	$(CXX) -c $(CFLAGS) -o $@ $<

run-test: all
	./run-tests.sh

clean:
	rm -f $(TARGETS) $(OBJECTS)
