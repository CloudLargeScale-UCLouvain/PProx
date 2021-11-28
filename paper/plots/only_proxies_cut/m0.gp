set term postscript monochrome eps enhanced 22
set output "m0.eps"

load "common.gp"

set xrange [0.5:5.5]
set xtics ("50" 1, "250" 2, "500" 3, "750" 4, "1000" 5)
set yrange [0:3]
set ylabel offset 0.5,0

set boxwidth 0.15
set key left

plot \
	'm0/concat/50.dat'   using (1):1 ls 100 notitle, \
	'm0/concat/250.dat'  using (2):1 ls 100 notitle, \
	'm0/concat/500.dat'  using (3):1 ls 100 notitle, \
	'm0/concat/750.dat'  using (4):1 ls 100 notitle, \
	'm0/concat/1000.dat' using (5):1 ls 100 notitle

!epstopdf "m0.eps"
!rm "m0.eps"
quit

