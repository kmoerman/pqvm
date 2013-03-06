CC = gcc
SOURCES = pqvm.cpp
TARGETS = pqvm

INCPATH = #-I
LIBPATH = #-L
LIBS = -lsexp -lquantum -ltbb
OFLAGS = #-O2 -Wall
DFLAGS = #-g3
CFLAGS = $(OFLAGS) $(DFLAGS) $(INCPATH) $(LIBPATH)

DEST_OBJS=$(SOURCES:.cpp=.o)

all: pqvm

pqvm: $(DEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $(DEST_OBJS) $(LIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGETS) $(DEST_OBJS)
