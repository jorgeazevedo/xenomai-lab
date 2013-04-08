#!/bin/bash

source configure.sh

# Note that by cding into the blocks, vim can open the file/line that caused
# the error
INITIALPWD=$(pwd)

for block in $1
do
	cd $BLOCKPATH/$block/
	colormake cleanall 1> /dev/null #2>/dev/null
done

for block in $1
do
	echo "Compiling $block"
	cd $BLOCKPATH/$block/
	colormake -j2 1> /dev/null #2>/dev/null
done

cd $INITIALPWD
