#! /bin/bash

NAME = $1;
SIZE = $2
LIMIT = $3

PLOTFILE = $NAME;

cat <<- HEAD
\# Plot file for $NAME.
set multiplot
set xlabel "Threads \$p\$"
set ylabel "\frac{T_1}{T_p}"
set term latex
set output "$NAME.tex"
HEAD
> $PLOTFILE;

until [$SIZE -lt $LIMIT]; do
	../ 

gnuplot <<- EOF
	set xlabel "Threads p"
	set ylabel "Speedup S [\frac{T_1}{T_p}]"
	set term latex
	set output "plot/$1.tex"
	set style data boxplot
	plot "build/$NAME.dat" using (1.0):2:(0):1 title "$1"
EOF