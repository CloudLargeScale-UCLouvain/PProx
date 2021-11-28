set term postscript color eps enhanced 22
set output "m1234.eps"

load "common.gp"

set xrange [0.5:5.5]
set xtics ("50" 1, "100" 2, "150" 3, "200" 4, "250" 5) out nomirror
set yrange [0:50]
set ylabel offset -0.5,0

set style fill solid 1 border -1

set boxwidth 0.15
unset key # left

set label "m1" at 0.7,25   rotate by 90 font "Courier bold, 20"
set label "m1" at 1.7,31   rotate by 90 font "Courier bold, 20"
set label "m1" at 2.7,35   rotate by 90 font "Courier bold, 20"
set label "m1" at 3.7,41   rotate by 90 font "Courier bold, 20"
set label "m1" at 4.7,45.5 rotate by 90 font "Courier bold, 20"

set label "m2" at 0.9,25   rotate by 90 font "Courier bold, 20"
set label "m2" at 1.9,31   rotate by 90 font "Courier bold, 20"
set label "m2" at 2.9,35   rotate by 90 font "Courier bold, 20"
set label "m2" at 3.9,41   rotate by 90 font "Courier bold, 20"
set label "m2" at 4.9,45.5 rotate by 90 font "Courier bold, 20"

set label "m3" at 1.1,25   rotate by 90 font "Courier bold, 20"
set label "m3" at 2.1,31   rotate by 90 font "Courier bold, 20"
set label "m3" at 3.1,35   rotate by 90 font "Courier bold, 20"
set label "m3" at 4.1,41   rotate by 90 font "Courier bold, 20"
set label "m3" at 5.1,45.5 rotate by 90 font "Courier bold, 20"

set label "m4" at 1.3,25   rotate by 90 font "Courier bold, 20"
set label "m4" at 2.3,31   rotate by 90 font "Courier bold, 20"
set label "m4" at 3.3,35   rotate by 90 font "Courier bold, 20"
set label "m4" at 4.3,41   rotate by 90 font "Courier bold, 20"
set label "m4" at 5.3,45.5 rotate by 90 font "Courier bold, 20"

plot \
	'm1/concat/50.dat'  using (0.7):1 ls 100 lt rgb "#f7fbff" title 'm1', \
	'm1/concat/100.dat' using (1.7):1 ls 100 lt rgb "#f7fbff" notitle, \
	'm1/concat/150.dat' using (2.7):1 ls 100 lt rgb "#f7fbff" notitle, \
	'm1/concat/200.dat' using (3.7):1 ls 100 lt rgb "#f7fbff" notitle, \
	'm1/concat/250.dat' using (4.7):1 ls 100 lt rgb "#f7fbff" notitle, \
\
	'm2/concat/50.dat'  using (0.9):1 ls 100 lt rgb "#deebf7" title 'm2', \
	'm2/concat/100.dat' using (1.9):1 ls 100 lt rgb "#deebf7" notitle, \
	'm2/concat/150.dat' using (2.9):1 ls 100 lt rgb "#deebf7" notitle, \
	'm2/concat/200.dat' using (3.9):1 ls 100 lt rgb "#deebf7" notitle, \
	'm2/concat/250.dat' using (4.9):1 ls 100 lt rgb "#deebf7" notitle, \
\
	'm3/concat/50.dat'  using (1.1):1 ls 100 lt rgb "#c6dbef" title 'm3', \
	'm3/concat/100.dat' using (2.1):1 ls 100 lt rgb "#c6dbef" notitle, \
	'm3/concat/150.dat' using (3.1):1 ls 100 lt rgb "#c6dbef" notitle, \
	'm3/concat/200.dat' using (4.1):1 ls 100 lt rgb "#c6dbef" notitle, \
	'm3/concat/250.dat' using (5.1):1 ls 100 lt rgb "#c6dbef" notitle, \
\
	'm4/concat/50.dat'  using (1.3):1 ls 100 lt rgb "#9ecae1" title 'm4', \
	'm4/concat/100.dat' using (2.3):1 ls 100 lt rgb "#9ecae1" notitle, \
	'm4/concat/150.dat' using (3.3):1 ls 100 lt rgb "#9ecae1" notitle, \
	'm4/concat/200.dat' using (4.3):1 ls 100 lt rgb "#9ecae1" notitle, \
	'm4/concat/250.dat' using (5.3):1 ls 100 lt rgb "#9ecae1" notitle

!epstopdf "m1234.eps"
!rm "m1234.eps"
quit

