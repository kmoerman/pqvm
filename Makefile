CC = g++


TARGETS = pqvm
DEPS    = $(wildcard ../quantum/*.h) vector.h
SOURCES = $(addsuffix .cpp, $(TARGETS))
DEST    = $(addsuffix .o,   $(TARGETS))


INCPATH = -I ../local/include
LIBPATH = -L ../local/lib
LIBS = -lsexp -ltbb
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