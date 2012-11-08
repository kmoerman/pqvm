#include "measure.h"
#include <ctime>

using namespace std;

//Measure the time [seconds] of n consecutive executions
//of of functional object F.
template <class Measurable>
double measure(int n, Measurable F) {
	clock_t zero = clock();
	for (; n > 0; --n) F();
	return ((double)(clock() - zero)) / CLOCKS_PER_SEC;
}