set term postscript monochrome eps enhanced 22
set output "b2.eps"

load "../common.gp"

plot \
	'concat/50.dat'  using (50):1  ls 100, \
	'concat/250.dat' using (250):1 ls 100, \
	'concat/500.dat' using (500):1 ls 100

!epstopdf b2.eps
!rm b2.eps
quit
