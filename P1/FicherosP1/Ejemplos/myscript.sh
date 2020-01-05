cd #!/bin/bash

# Xukai Chen
gcc copia.c -o copia 2> error.txt
if test -s ./error.txt ; then
	cat error.txt
	exit 1
else 
	echo "successful"
fi	
echo "Estado de terminacion" $?

