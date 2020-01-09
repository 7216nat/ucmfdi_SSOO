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
	../../mytar -x -f tarball.mtar
	
	if diff fichero1.txt ../fichero1.txt; then
		echo "fichero1 copiado."
		if diff fichero2.txt ../fichero2.txt; then
			echo "fichero2 copiado."
			if diff fichero3.dat ../fichero3.dat; then
				echo "fichero3 copiado."
			fi
		fi
	fi
fi




