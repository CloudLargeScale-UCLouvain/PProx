set term postscript monochrome eps enhanced 22
set output "b0.eps"

load "../common.gp"

plot \
	'concat/1.dat'  using (1):1  ls 100, \
	'concat/25.dat' using (25):1 ls 100, \
	'concat/50.dat' using (50):1 ls 100, \
	'concat/75.dat' using (75):1 ls 100, \
	'concat/100.dat' using (100):1 ls 100

!epstopdf b0.eps
!rm b0.eps
quit
