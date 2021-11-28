set size 1,0.85

set ylabel 'Round-trip time (ms)'
set grid y
set ytics auto nomirror
set ylabel offset 1,0
set xlabel 'Requests per second'

unset key

set lmargin 7
set rmargin 1
set bmargin 4
set tmargin 1

set style line 100 lt 1 lw 6

set style data boxplot
set style boxplot nooutliers 

set xrange [0.5:5.5]
set xtics ("50" 1, "250" 2, "500" 3, "750" 4, "1000" 5) out nomirror
set logscale y
set yrange [1:3000]
set ylabel offset 1.5,0

set boxwidth 0.15

