CC = clang
SOURCES = qvm.cpp kronecker_test.cpp
TARGETS = qvm kronecker_test

INCPATH = #-I
LIBPATH = #-L
LIBS = -lsexp -lquantum -ltbb
OFLAGS = #-O2 -Wall
DFLAGS = #-g3
CFLAGS = $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH)

DEST_OBJS=$(SOURCES:.cpp=.o)

all: qvm

test: measure.cpp measure.h
	$(CC) $(CFLAGS) -c kronecker_test.cpp
	$(CC) $(CFLAGS) -o kronecker_test  kronecker_test.o $(LIBS)

qvm: qvm.cpp qvm.h
	$(CC) $(CFLAGS) -c qvm.cpp
	$(CC) $(CFLAGS) -o qvm qvm.o $(LIBS)

clean:
	rm -f $(TARGETS) $(DEST_OBJS)
