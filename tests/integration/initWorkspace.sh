#!/bin/bash

source configure.sh

# Setup workspace
mkdir -p $WORKSPACE

for block in $1
do
	cp $BLOCKPATH/$block/$block.conf $WORKSPACE
done
