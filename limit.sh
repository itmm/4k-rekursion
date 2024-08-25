#!/bin/bash

siz=`wc -c < "$1"`
max=4096

if [ $siz -gt $max ]; then
	echo $1 is $(($siz - $max)) bytes too big
	exit 10
else
	echo $1 can contain $(($max - $siz)) more bytes
fi
