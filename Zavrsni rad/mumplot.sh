#!/bin/bash

echo "set terminal png
set output '$2'
set key right bottom
plot '$1' using 1:2 w l lt rgb 'green' title 'Forward', \
'$1' using 3:4 w l lt rgb 'red' title 'Reverse'" | gnuplot
