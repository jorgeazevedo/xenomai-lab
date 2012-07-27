#!/bin/bash


toUpper() {
echo $1 | tr  "[:lower:]" "[:upper:]"
}

allcaps=`toUpper $1`

regex="s/template/$1/g"
regex2="s/TEMPLATE/$allcaps/g"
sed -i $regex Makefile
sed -i $regex *.c
sed -i $regex *.h
sed -i $regex2 *.h

mv template.c $1.c
mv template.conf $1.conf
mv template_settings.c $1_settings.c
mv template_settings.h $1_settings.h

cd settings_gui
sed -i $regex *.h
sed -i $regex *.cpp
sed -i $regex *.pro
