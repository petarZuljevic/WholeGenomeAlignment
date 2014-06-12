#!/bin/bash

echo "set terminal png
set output 'mummer.png'
set key right bottom
plot 'Mumms-plot.txt' using 1:2 w l lt rgb 'red' title 'forward' " | gnuplot
