set term postscript monochrome eps enhanced 22
set output "b0.eps"

load "common.gp"

unset logscale y

set xtics ("1" 1, "25" 2, "50" 3, "75" 4, "100" 5) out nomirror
set yrange [0:50]
set ylabel offset -0.5,0

set key right

plot \
	'b0/concat/1.dat'   using (1):1 ls 100 notitle, \
	'b0/concat/25.dat'  using (2):1 ls 100 notitle, \
	'b0/concat/50.dat'  using (3):1 ls 100 notitle, \
	'b0/concat/75.dat'  using (4):1 ls 100 notitle, \
	'b0/concat/100.dat' using (5):1 ls 100 notitle

!epstopdf "b0.eps"
!rm "b0.eps"
quit

