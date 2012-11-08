#compiler
CC = gcc
CFLAGS = -I ../tbb/include -L ../tbb/lib/libtbb.dylib


#objects
OBJECTS = normalize.o measure.o

#build

dummy:

test: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o normalize

normalize.o: normalize.cpp measure.h
	$(CC) $(CFLAGS) -c normalize.cpp

measure.o: measure.cpp measure.h
	$(CC) $(CFLAGS) -c measure.cpp

tbb: tbb_test.cpp
	$(CC) $(CFLAGS) tbb_test.cpp

clean:
	rm -f *.o
