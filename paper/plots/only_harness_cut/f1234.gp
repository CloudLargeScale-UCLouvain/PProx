set term postscript color eps enhanced 22
set output "f1234.eps"

load "common.gp"

unset key # bottom left
set style fill solid 1 border -1

set yrange [10:3000]

set label "f1" at 0.7,1600 rotate by 90 font "Courier bold, 20"
set label "f1" at 1.7,400 rotate by 90 font "Courier bold, 20"

set label "f2" at 0.9,1600 rotate by 90 font "Courier bold, 20"
set label "f2" at 1.9,400 rotate by 90 font "Courier bold, 20"
set label "f2" at 2.8,400 rotate by 90 font "Courier bold, 20"

set label "f3" at 1.1,1600 rotate by 90 font "Courier bold, 20"
set label "f3" at 2.1,400 rotate by 90 font "Courier bold, 20"
set label "f3" at 3.0,400 rotate by 90 font "Courier bold, 20"
set label "f3" at 3.9,450 rotate by 90 font "Courier bold, 20"

set label "f4" at 1.3,1600 rotate by 90 font "Courier bold, 20"
set label "f4" at 2.3,400 rotate by 90 font "Courier bold, 20"
set label "f4" at 3.2,400 rotate by 90 font "Courier bold, 20"
set label "f4" at 4.1,450 rotate by 90 font "Courier bold, 20"
set label "f4" at 5.0,800 rotate by 90 font "Courier bold, 20"

plot \
	'f1/concat/50.dat'  using (0.7):1 ls 100 lt rgb "#f2f0f7" title 'f1', \
	'f1/concat/250.dat' using (1.7):1 ls 100 lt rgb "#f2f0f7" notitle, \
\
	'f2/concat/50.dat'  using (0.9):1 ls 100 lt rgb "#cbc9e2" title 'f2', \
	'f2/concat/250.dat' using (1.9):1 ls 100 lt rgb "#cbc9e2" notitle, \
	'f2/concat/500.dat' using (2.8):1 ls 100 lt rgb "#cbc9e2" notitle, \
\
	'f3/concat/50.dat'  using (1.1):1 ls 100 lt rgb "#9e9ac8" title 'f3', \
	'f3/concat/250.dat' using (2.1):1 ls 100 lt rgb "#9e9ac8" notitle, \
	'f3/concat/500.dat' using (3):1   ls 100 lt rgb "#9e9ac8" notitle, \
	'f3/concat/750.dat' using (3.9):1 ls 100 lt rgb "#9e9ac8" notitle, \
\
	'f4/concat/50.dat'  using (1.3):1 ls 100 lt rgb "#6a51a3" title 'f4', \
	'f4/concat/250.dat' using (2.3):1 ls 100 lt rgb "#6a51a3" notitle, \
	'f4/concat/500.dat' using (3.2):1 ls 100 lt rgb "#6a51a3" notitle, \
	'f4/concat/750.dat' using (4.1):1 ls 100 lt rgb "#6a51a3" notitle, \
	'f4/concat/1000.dat' using (5):1  ls 100 lt rgb "#6a51a3" notitle

!epstopdf "f1234.eps"
!rm "f1234.eps"
quit

