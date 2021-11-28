#!/bin/bash

xps="m0 m1234 m356 m6789"

for xp in $(ls -d */)
do
	cd $xp
	cd concat
	for i in `ls *.bz2`
	do
		uncompressed_filename=`echo $i|sed 's/\.dat.bz2$/.dat/g'`
		if [[ ! -e $uncompressed_filename || $i -nt $uncompressed_filename ]]
		then
			echo "uncompressing ${i} for ${xp}"
			bunzip2 -fk $i
		fi
	done
	cd ../..
done

for xp in ${xps}
do
	echo "plotting ${xp}"
	gnuplot ${xp}.gp
done

#for xp in $(ls -d */)
#do
#	cd $xp
#	echo "plotting single ${xp}"
#	gnuplot plot.gp
#	cd ..
#done

