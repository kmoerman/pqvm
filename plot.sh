#! /bin/bash

# generate data
cd data
for i in {$1..$2}
do
	s = 2^$i
	../build/transpose.out $s 50
done

# generate plotfile
cd ..

PLOT = "transpose.plot";

cat <<- HEAD
set title "Matrix Transpose Speedup"
set size square
set xlabel "threads"
set ylabel "speedup"
set xrange [1:8]
set yrange [1:8]

set terminal pdf
set output "prokopp.pdf"

set style data linespoints

f(x) = x

set multiplot

plot f(x) with lines lt 0 lw 0.5 title ''

set ket left top
plot 
HEAD
> $PLOT

for i in {$1..$2}
do
	s = 2^$i
	echo "'data/transpose-prokopp-$s.speedup.data' using 1:2 title 'prokopp $s' with linespoints lw 2 lt $i" >> $PLOT
	if [$i != $2]; then
		echo ", " >> $PLOT
	fi
done

echo <<- STYLE

set termoption dashed
set key right bottom

plot 
STYLE >> $PLOT
for i in {$1..$2}
do
	s = 2^$i
	echo "'data/transpose-naive-$s.speedup.data' using 1:2 title 'prokopp $s' with linespoints lw 2 lt $i" >> $PLOT
	if [$i != $2]; then
		echo ", " >> $PLOT
	fi
done

#generate graph