set term postscript monochrome eps enhanced 22
set output "m7.eps"

load "../common.gp"

set yrange [0:998]
set ylabel offset 1,0

plot \
	'concat/50.dat'  using (50):1  ls 100, \
	'concat/100.dat' using (100):1 ls 100, \
	'concat/150.dat' using (150):1 ls 100, \
	'concat/200.dat' using (200):1 ls 100, \
	'concat/250.dat' using (250):1 ls 100, \
	'concat/500.dat' using (500):1 ls 100

!epstopdf m7.eps
!rm m7.eps
quit
