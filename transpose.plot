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

set key left top
plot 'data/transpose-prokopp-256.speedup.data' using 1:2 title 'prokopp 256' with linespoints lw 2 lt 0, 'data/transpose-prokopp-512.speedup.data' using 1:2 title 'prokopp 512' with linespoints lw 2 lt 1, 'data/transpose-prokopp-1024.speedup.data' using 1:2 title 'prokopp 1024' with linespoints lw 2 lt 2, 'data/transpose-prokopp-2048.speedup.data' using 1:2 title 'prokopp 2048' with linespoints lw 2 lt 3, 'data/transpose-prokopp-4096.speedup.data' using 1:2 title 'prokopp 4096' with linespoints lw 2 lt 4
set termoption dashed
set key right bottom
plot 'data/transpose-naive-256.speedup.data' using 1:2 title 'prokopp 256' with linespoints lw 2 lt 8, 'data/transpose-naive-512.speedup.data' using 1:2 title 'prokopp 512' with linespoints lw 2 lt 9, 'data/transpose-naive-1024.speedup.data' using 1:2 title 'prokopp 1024' with linespoints lw 2 lt 10, 'data/transpose-naive-2048.speedup.data' using 1:2 title 'prokopp 2048' with linespoints lw 2 lt 11, 'data/transpose-naive-4096.speedup.data' using 1:2 title 'prokopp 4096' with linespoints lw 2 lt 12