set term postscript color eps enhanced 22
set output "b1234.eps"

load "common.gp"

unset key # top left
set style fill solid 1 border -1

set label "b1" at 0.7,45 rotate by 90 font "Courier bold, 20"
set label "b1" at 1.7,50 rotate by 90 font "Courier bold, 20"

set label "b2" at 0.9,45 rotate by 90 font "Courier bold, 20"
set label "b2" at 1.9,50 rotate by 90 font "Courier bold, 20"
set label "b2" at 2.8,130 rotate by 90 font "Courier bold, 20"

set label "b3" at 1.1,45 rotate by 90 font "Courier bold, 20"
set label "b3" at 2.1,50 rotate by 90 font "Courier bold, 20"
set label "b3" at 3.0,130 rotate by 90 font "Courier bold, 20"
set label "b3" at 3.9,400 rotate by 90 font "Courier bold, 20"

set label "b4" at 1.3,45 rotate by 90 font "Courier bold, 20"
set label "b4" at 2.3,50 rotate by 90 font "Courier bold, 20"
set label "b4" at 3.2,130 rotate by 90 font "Courier bold, 20"
set label "b4" at 4.1,400 rotate by 90 font "Courier bold, 20"
set label "b4" at 5.0,450 rotate by 90 font "Courier bold, 20"

plot \
	'b1/concat/50.dat'  using (0.7):1 ls 100 lt rgb "#fef0d9" title 'b1', \
	'b1/concat/250.dat' using (1.7):1 ls 100 lt rgb "#fef0d9" notitle, \
\
	'b2/concat/50.dat'  using (0.9):1 ls 100 lt rgb "#fdcc8a" title 'b2', \
	'b2/concat/250.dat' using (1.9):1 ls 100 lt rgb "#fdcc8a" notitle, \
	'b2/concat/500.dat' using (2.8):1 ls 100 lt rgb "#fdcc8a" notitle, \
\
	'b3/concat/50.dat'  using (1.1):1 ls 100 lt rgb "#fc8d59" title 'b3', \
	'b3/concat/250.dat' using (2.1):1 ls 100 lt rgb "#fc8d59" notitle, \
	'b3/concat/500.dat' using (3):1   ls 100 lt rgb "#fc8d59" notitle, \
	'b3/concat/750.dat' using (3.9):1 ls 100 lt rgb "#fc8d59" notitle, \
\
	'b4/concat/50.dat'  using (1.3):1 ls 100 lt rgb "#d7301f" title 'b4', \
	'b4/concat/250.dat' using (2.3):1 ls 100 lt rgb "#d7301f" notitle, \
	'b4/concat/500.dat' using (3.2):1 ls 100 lt rgb "#d7301f" notitle, \
	'b4/concat/750.dat' using (4.1):1 ls 100 lt rgb "#d7301f" notitle, \
	'b4/concat/1000.dat' using (5):1  ls 100 lt rgb "#d7301f" notitle

!epstopdf "b1234.eps"
!rm "b1234.eps"
quit

