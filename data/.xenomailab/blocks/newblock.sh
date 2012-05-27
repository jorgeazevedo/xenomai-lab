#!/bin/bash
cp -r template $1
cd $1
./rename.sh $1
rm rename.sh
