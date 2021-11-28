set term postscript monochrome eps enhanced 22
set output "m0.eps"

load "../common.gp"

# specific for xp1
set yrange [0:5]
set ylabel offset -0.5,0

plot \
	'concat/50.dat'  using (50):1  ls 100, \
	'concat/100.dat' using (100):1 ls 100, \
	'concat/150.dat' using (150):1 ls 100, \
	'concat/200.dat' using (200):1 ls 100, \
	'concat/250.dat' using (250):1 ls 100, \
	'concat/500.dat' using (500):1  ls 100, \
	'concat/750.dat' using (750):1 ls 100, \
	'concat/1000.dat' using (1000):1 ls 100

!epstopdf m0.eps
!rm m0.eps
quit
