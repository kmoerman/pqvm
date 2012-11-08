#compiler
CC = gcc
CFLAGS = -I.
LIB = 


#objects
OBJECTS = normalize.o measure.o

#build
main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o normalize

normalize.o: normalize.cpp measure.h
	$(CC) $(CFLAGS) -c normalize.cpp

measure.o: measure.cpp measure.h
	$(CC) $(CFLAGS) -c measure.cpp

clean:
	rm -f *.o