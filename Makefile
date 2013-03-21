CC = c++
SOURCES = qvm.cpp test-*.cpp
TARGETS = qvm test-*

INCPATH = #-I
LIBPATH = #-L
LIBS = -lsexp -lquantum -ltbb -lc++
OFLAGS = #-O2 -Wall
DFLAGS = #-g3
CFLAGS = $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH)

DEST_OBJS=$(SOURCES:.cpp=.o)

all: test

test: test-quantum.cpp
	$(CC) $(CFLAGS) -c test-quantum.cpp
	$(CC) $(CFLAGS) -o test-quantum  test-quantum.o $(LIBS)

qvm: qvm.cpp qvm.h
	$(CC) $(CFLAGS) -c qvm.cpp
	$(CC) $(CFLAGS) -o qvm qvm.o $(LIBS)

clean:
	rm -f $(TARGETS) $(DEST_OBJS)
