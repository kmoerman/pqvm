CC = g++
CFLAGS = -ltbb -O3

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CFLAGS += -lrt
endif

#ifeq ($(UNAME), Darwin)
##mac os - darwin kernel
#endif

transpose: transpose/transpose.o measure/measure.o
	$(CC) $(CFLAGS) transpose/transpose.o measure/measure.o -o build/transpose.out

normalize: normalize/normalize.o measure/measure.o
	$(CC) $(CFLAGS) normalize/normalize.o measure/measure.o -o build/normalize.out