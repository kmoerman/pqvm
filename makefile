CC = g++
CFLAGS = -ltbb

normalize: normalize/normalize.o measure/measure.o

tbb: tbb/tbb_test.o