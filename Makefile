CC = g++
SOURCES = qvm.cpp measure.cpp
TARGETS = qvm measure

INCPATH = #-I
LIBPATH = #-L
LIBS = -lsexp -lquantum -ltbb
OFLAGS = #-O2 -Wall
DFLAGS = #-g3
CFLAGS = $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH)

DEST_OBJS=$(SOURCES:.cpp=.o)

all: qvm

measure: measure.cpp measure.h
	$(CC) $(CFLAGS) -c measure.cpp
	$(CC) $(CFLAGS) -o measure  measure.o $(LIBS)

qvm: qvm.cpp qvm.h
	$(CC) $(CFLAGS) -c qvm.cpp
	$(CC) $(CFLAGS) -o qvm qvm.o $(LIBS)

clean:
	rm -f $(TARGETS) $(DEST_OBJS)
