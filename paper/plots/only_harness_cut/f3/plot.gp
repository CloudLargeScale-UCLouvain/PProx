set term postscript monochrome eps enhanced 22
set output "f3.eps"

load "../common.gp"

plot \
	'concat/50.dat'  using (50):1  ls 100, \
	'concat/250.dat' using (250):1 ls 100, \
	'concat/500.dat' using (500):1 ls 100, \
	'concat/750.dat' using (750):1 ls 100

!epstopdf f3.eps
!rm f3.eps
quit
