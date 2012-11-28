#! /bin/bash

gnuplot <<- EOF
	set xlabel "Threads"
	set ylabel "Execution time [s]"
	set term png
	set output "plot/$1.png"
	plot "build/$1.dat" using 1:2 title "$1"
EOF