set size 1,0.85

unset key

set xlabel 'Requests per second' # offset graph -1,0
set ylabel 'Round-trip time (ms)'
set grid y
set ytics auto nomirror

set lmargin 7
set rmargin 1
set bmargin 4
set tmargin 1

set style line 100 lt 1 lw 6

set style data boxplot
set style boxplot nooutliers 
set boxwidth 25

