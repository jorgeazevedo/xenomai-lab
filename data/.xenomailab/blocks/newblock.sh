#!/bin/bash
cp -r template $1
cd $1
./rename.sh $1
rm rename.sh
cd ..
regex=":a;N;\$!ba;s/\n\n/\n$1=square\n\n/g"
sed -i $regex blocks.conf
