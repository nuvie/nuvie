#!/bin/sh

FILES=`find $1`

for FILE in $FILES; do
    NEWNAME=`echo $FILE | tr A-Z a-z`
    mv $FILE $NEWNAME~
    mv $NEWNAME~ $NEWNAME
done;

