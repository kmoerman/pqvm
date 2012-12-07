#! /bin/bash

gnuplot <<- EOF
	#set xlabel "Threads"
	#set ylabel "Execution time [s]"
	set term png
	set output "plot/$1.png"
	set style data boxplot
	plot "build/$1.dat" using (1.0):2:(0):1 title "$1"
EOF