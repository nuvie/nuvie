#!/bin/sh

FILES=`find $1`

for FILE in $FILES; do
	mv $1 `echo $FILE | tr A-Z a-z`;
done;

