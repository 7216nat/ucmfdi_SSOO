#! /bin/bash

if ! test -e mytar; then
	echo "Mytar not found"
	exit 1
elif ! test -x mytar; then
	echo "Mytar not executeble"
	exit 1
else
	if test -d tmp; then 
		rm -r tmp
	fi

	mkdir tmp
	cd tmp
	echo "Hello World!"> fichero1.txt
	head -10 /etc/passwd > fichero2.txt
	head -c 1024 /dev/urandom > fichero3.dat
	
	../mytar -c -f tarball.mtar fichero1.txt fichero2.txt fichero3.dat
	mkdir out
 	mv tarball.mtar out

	cd out
	../../mytar -f tarball.mtar -d fichero2.txt
	
	../../mytar -x -f tarball.mtar
fi




