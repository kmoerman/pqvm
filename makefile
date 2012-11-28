CC = g++
CFLAGS = -ltbb

normalize: normalize/normalize.o measure/measure.o
	$(CC) $(CFLAGS) normalize/normalize.o measure/measure.o -o build/normalize