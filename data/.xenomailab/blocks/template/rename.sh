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

cd template_settings_proj/template_settings_proj/
sed -i $regex *.h
sed -i $regex *.cpp
sed -i $regex *.pro
mv template_settings.pro $1_settings.pro
cd ..
mv template_settings_proj $1_settings_proj
cd ..
mv template_settings_proj $1_settings_proj
