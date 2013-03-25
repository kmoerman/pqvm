CC = g++

TARGETS = quantum performance
DEPS    = ../performance.h ../quantum.h ../vector.h

SOURCES = $(addsuffix .cpp, $(TARGETS))
DEST    = $(addsuffix .o,   $(TARGETS))

INCPATH = #-I
LIBPATH = #-L
LIBS = -lsexp -ltbb -lc++
OFLAGS = #-O2 -Wall
DFLAGS = #-g3
CFLAGS = $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH)

all: $(TARGETS)

$(TARGETS): $(DEST)
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGETS) $(DEST)