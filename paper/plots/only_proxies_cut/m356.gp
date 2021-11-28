set term postscript color eps enhanced 22
set output "m356.eps"

load "common.gp"

set xrange [0.5:5.5]
set xtics ("50" 1, "100" 2, "150" 3, "200" 4, "250" 5) out nomirror
set yrange [0:500]
set ylabel offset 0.5,0

set style fill solid 1 border -1

unset mytics
unset mxtics

set boxwidth 0.15
unset key # right

set label "m3" at 0.8,460 rotate by 90 font "Courier bold, 20"
set label "m3" at 1.8,260 rotate by 90 font "Courier bold, 20"
set label "m3" at 2.8,210 rotate by 90 font "Courier bold, 20"
set label "m3" at 3.8,210 rotate by 90 font "Courier bold, 20"
set label "m3" at 4.8,160 rotate by 90 font "Courier bold, 20"

set label "m5" at 1.0,460 rotate by 90 font "Courier bold, 20"
set label "m5" at 2.0,260 rotate by 90 font "Courier bold, 20"
set label "m5" at 3.0,210 rotate by 90 font "Courier bold, 20"
set label "m5" at 4.0,210 rotate by 90 font "Courier bold, 20"
set label "m5" at 5.0,160 rotate by 90 font "Courier bold, 20"

set label "m6" at 1.2,460 rotate by 90 font "Courier bold, 20"
set label "m6" at 2.2,260 rotate by 90 font "Courier bold, 20"
set label "m6" at 3.2,210 rotate by 90 font "Courier bold, 20"
set label "m6" at 4.2,210 rotate by 90 font "Courier bold, 20"
set label "m6" at 5.2,160 rotate by 90 font "Courier bold, 20"

plot \
	'm3/concat/50.dat'  using (0.8):1 ls 100 lt rgb "#c6dbef" title 'm3', \
	'm3/concat/100.dat' using (1.8):1 ls 100 lt rgb "#c6dbef" notitle, \
	'm3/concat/150.dat' using (2.8):1 ls 100 lt rgb "#c6dbef" notitle, \
	'm3/concat/200.dat' using (3.8):1 ls 100 lt rgb "#c6dbef" notitle, \
	'm3/concat/250.dat' using (4.8):1 ls 100 lt rgb "#c6dbef" notitle, \
\
	'm5/concat/50.dat'  using (1):1 ls 100 lt rgb "#6baed6" title 'm5', \
	'm5/concat/100.dat' using (2):1 ls 100 lt rgb "#6baed6" notitle, \
	'm5/concat/150.dat' using (3):1 ls 100 lt rgb "#6baed6" notitle, \
	'm5/concat/200.dat' using (4):1 ls 100 lt rgb "#6baed6" notitle, \
	'm5/concat/250.dat' using (5):1 ls 100 lt rgb "#6baed6" notitle, \
\
	'm6/concat/50.dat'  using (1.2):1 ls 100 lt rgb "#4292c6" title 'm6', \
	'm6/concat/100.dat' using (2.2):1 ls 100 lt rgb "#4292c6" notitle, \
	'm6/concat/150.dat' using (3.2):1 ls 100 lt rgb "#4292c6" notitle, \
	'm6/concat/200.dat' using (4.2):1 ls 100 lt rgb "#4292c6" notitle, \
	'm6/concat/250.dat' using (5.2):1 ls 100 lt rgb "#4292c6" notitle, \

!epstopdf "m356.eps"
!rm "m356.eps"
quit

