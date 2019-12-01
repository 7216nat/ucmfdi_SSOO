#! /bin/bash

if [! -e ./mytar]; then
	echo "Mytar not found"
	exit 1
elif [! -x ./mytar]; then
	echo "Mytar not executeble"
	exit 1
fi

if [-d tmp]; then 
	rm -r tmp
fi

mkdir tmp
cd tmp



