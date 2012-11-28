CC = g++
CFLAGS = -ltbb

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CFLAGS += -lrt
endif

normalize: normalize/normalize.o measure/measure.o
	$(CC) $(CFLAGS) normalize/normalize.o measure/measure.o -o build/normalize