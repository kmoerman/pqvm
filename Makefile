CC = g++
TEST    = quantum
SOURCES = qvm.cpp tests/*.cpp
TARGETS = qvm tests/$(TEST)

INCPATH = #-I
LIBPATH = #-L
LIBS = -lsexp -lquantum -ltbb -lc++
OFLAGS = #-O2 -Wall
DFLAGS = #-g3
CFLAGS = $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH)

DEST_OBJS=$(SOURCES:.cpp=.o)

all: test

test: tests/$(TEST).cpp
	$(CC) $(CFLAGS) -c tests/$(TEST).cpp -o tests/$(TEST).o
	$(CC) $(CFLAGS) -o tests/$(TEST)  tests/$(TEST).o $(LIBS)

qvm: qvm.cpp qvm.h
	$(CC) $(CFLAGS) -c qvm.cpp
	$(CC) $(CFLAGS) -o qvm qvm.o $(LIBS)

clean:
	rm -f $(TARGETS) $(DEST_OBJS)