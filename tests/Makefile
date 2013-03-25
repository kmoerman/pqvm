CC = g++

TARGETS = sigma-x sigma-z kronecker
DEPS    = ../performance.h ../quantum/*.h ../vector.h

SOURCES = $(addsuffix .cpp, $(TARGETS))
DEST    = $(addsuffix .o,   $(TARGETS))

INCPATH = #-I
LIBPATH = #-L
LIBS = -ltbb -lrt
OFLAGS = #-O2 -Wall
DFLAGS = #-g3
CFLAGS = $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH) -fopenmp

all: $(TARGETS)

$(TARGETS): $(DEST)
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGETS) $(DEST)
