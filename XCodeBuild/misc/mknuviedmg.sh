#!/bin/sh
#
# A quick and dirty Nuvie diskimage builder.
#
#

if [ $# -lt 1 ]; then
	echo "Not enough args.";
	exit;
fi

DIR=$1

DATE=`date "+%Y%m%d"`

mkdir -p $DIR/nuvie-$DATE
cp -r $DIR/../../../data $DIR/nuvie-$DATE

cp -r $DIR/../../dmg_template/* $DIR/nuvie-$DATE
cp -r $DIR/Nuvie.app $DIR/nuvie-$DATE

## strip out the CVS directories.
rm -rf `find $DIR/nuvie-$DATE | grep "CVS$"`

$DIR/../../misc/buildDMG.pl -dmgName Nuvie -buildDir $DIR -volName Nuvie-$DATE  -compressionLevel 9 $DIR/nuvie-$DATE -deleteHeaders $DIR/../../misc/README_MacOSX.txt

rm -rf $DIR/nuvie-$DATE
