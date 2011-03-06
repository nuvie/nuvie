#!/bin/sh
#
# A quick and dirty Nuvie diskimage builder.
#
#

if [ $# -lt 2 ]; then
	echo "Not enough args.";
	exit;
fi

DIR=$1
SDL_DIR=$2

DATE=`date "+%Y%m%d"`

mkdir -p $DIR/Nuvie.app/Contents/Frameworks

#cp -r $DIR/../../dmg_template/* $DIR/nuvie-$DATE
cp -r $SDL_DIR/SDL.Framework $DIR/Nuvie.app/Contents/Frameworks

## strip out the CVS directories.
#rm -rf `find $DIR/nuvie-$DATE | grep "CVS$"`

cp  $DIR/../../../ChangeLog $DIR/ChangeLog.txt

$DIR/../../misc/buildDMG.pl -debug -dmgName Nuvie -buildDir $DIR -volName Nuvie-$DATE  -compressionLevel 9 $DIR/Nuvie.app -deleteHeaders $DIR/../../misc/README_MacOSX.txt $DIR/ChangeLog.txt

#rm -rf $DIR/nuvie-$DATE
rm $DIR/ChangeLog.txt
