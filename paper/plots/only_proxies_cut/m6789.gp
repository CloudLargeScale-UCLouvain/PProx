set term postscript color eps enhanced 22
set output "m6789.eps"

load "common.gp"

set xrange [0.5:5.5]
set xtics ("50" 1, "250" 2, "500" 3, "750" 4, "1000" 5) out nomirror
set logscale y
set yrange [10:3000]
#set yrange [0:1450] # disable log
set ylabel offset 1.5,0

set style fill solid 1 border -1

set boxwidth 0.15
unset key # right

set label "m6" at 0.7,1600 rotate by 90 font "Courier bold, 20"
set label "m6" at 1.7,400 rotate by 90 font "Courier bold, 20"

set label "m7" at 0.9,1600 rotate by 90 font "Courier bold, 20"
set label "m7" at 1.9,400 rotate by 90 font "Courier bold, 20"
set label "m7" at 2.8,210 rotate by 90 font "Courier bold, 20"

set label "m8" at 1.1,1600 rotate by 90 font "Courier bold, 20"
set label "m8" at 2.1,400 rotate by 90 font "Courier bold, 20"
set label "m8" at 3.0,210 rotate by 90 font "Courier bold, 20"
set label "m8" at 3.9,210 rotate by 90 font "Courier bold, 20"

set label "m9" at 1.3,1600 rotate by 90 font "Courier bold, 20"
set label "m9" at 2.3,400 rotate by 90 font "Courier bold, 20"
set label "m9" at 3.2,210 rotate by 90 font "Courier bold, 20"
set label "m9" at 4.1,210 rotate by 90 font "Courier bold, 20"
set label "m9" at 5.0,210 rotate by 90 font "Courier bold, 20"

plot \
	'm6/concat/50.dat'   using (0.7):1 ls 100 lt rgb "#4292c6" title 'm6', \
	'm6/concat/250.dat'  using (1.7):1 ls 100 lt rgb "#4292c6" notitle, \
\
	'm7/concat/50.dat'   using (0.9):1 ls 100 lt rgb "#2171b5" title 'm7', \
	'm7/concat/250.dat'  using (1.9):1 ls 100 lt rgb "#2171b5" notitle, \
	'm7/concat/500.dat'  using (2.8):1 ls 100 lt rgb "#2171b5" notitle, \
\
	'm8/concat/50.dat'   using (1.1):1 ls 100 lt rgb "#08519c" title 'm8', \
	'm8/concat/250.dat'  using (2.1):1 ls 100 lt rgb "#08519c" notitle, \
	'm8/concat/500.dat'  using (3):1   ls 100 lt rgb "#08519c" notitle, \
	'm8/concat/750.dat'  using (3.9):1 ls 100 lt rgb "#08519c" notitle, \
\
	'm9/concat/50.dat'   using (1.3):1 ls 100 lt rgb "#08306b" title 'm9', \
	'm9/concat/250.dat'  using (2.3):1 ls 100 lt rgb "#08306b" notitle, \
	'm9/concat/500.dat'  using (3.2):1 ls 100 lt rgb "#08306b" notitle, \
	'm9/concat/750.dat'  using (4.1):1 ls 100 lt rgb "#08306b" notitle, \
	'm9/concat/1000.dat' using (5):1   ls 100 lt rgb "#08306b" notitle

!epstopdf "m6789.eps"
!rm "m6789.eps"
quit

