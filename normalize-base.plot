set title "Vector Normalize Speedup"

set xlabel "threads"
set ylabel "speedup"
set xrange [1:8]
set yrange [1:8]

set terminal pdf
set output "normalize.pdf"

set style data linespoints

f(x) = x

set multiplot

plot f(x) with lines lt 0 lw 0.5 title ''

set key left top
plot 