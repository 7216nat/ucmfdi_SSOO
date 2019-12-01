#!/bin/bash

SRC="./src"
TEMP="./temp"
MOUNT="./mount-point"
AUDITE="./my-fsck virtual-disk"

FILE1="fuseLib.c"
FILE2="myFS.h"
FILE3="MyFileSystem.c"

rm -R -f $TEMP
mkdir $TEMP

cp $SRC/$FILE1 $TEMP/$FILE1 
cp $SRC/$FILE1 $MOUNT/$FILE1

cp $SRC/$FILE2 $TEMP/$FILE2
cp $SRC/$FILE2 $MOUNT/$FILE2

$AUDITE

if [ "$(diff $MOUNT/$FILE1 $TEMP/$FILE1)" == "" ];
then echo "$FILE1 es igual en ambas rutas"
else echo "$FILE1 es distinta en ambas rutas"
fi

if [ "$(diff $MOUNT/$FILE2 $TEMP/$FILE2)" == "" ];
then echo "$FILE2 es igual en ambas rutas"
else echo "$FILE2 es distinta en ambas rutas"
fi

truncate --size=-4086 $TEMP/$FILE1
truncate --size=-4086 $MOUNT/$FILE1

$AUDITE

if [ "$(diff $SRC/$FILE1 $MOUNT/$FILE1)" == "" ]; 
then echo "$FILE1 truncado es igual en ambas rutas"
else echo "$FILE1 truncado es distinta en ambas rutas"
fi

cp $SRC/$FILE3 $MOUNT/$FILE3 

$AUDITE

if [ "$(diff $SRC/$FILE3 $MOUNT/$FILE3)" == "" ]; 
then echo "$FILE3 es igual en ambas rutas"
else echo "$FILE3 es diferente en ambas rutas"
fi

truncate --size=+4096 $TEMP/$FILE2
truncate --size=+4096 $MOUNT/$FILE2

$AUDITE

if [ "$(diff $SRC/$FILE2 $MOUNT/$FILE2)" == "" ]; 
then echo "$FILE2 truncado en mount es igual"
else echo "$FILE2 truncado en mount es diferente"
fi

if [ "$(diff $SRC/$FILE2 $TEMP/$FILE2)" == "" ]; 
then echo "$FILE2 truncado en tmp es igual"
else echo "$FILE2 truncado en tmp es diferente"
fi
