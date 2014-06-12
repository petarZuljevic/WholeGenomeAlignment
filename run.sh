#!/bin/bash

printf "\n"
printf "%.0s*" {1..40};
printf "\n";

#2 ulazna filea spajamo u jednog
python konkateniraj.py $1 $2 $3

g++ *.cpp -o build -lpthread

#Ako je uspjesno kompajlano pokreni
if [[ $? -eq 0 ]]; then
        ./build INPUT.txt > $3
								./mumplot.sh $3 "$3.png"
fi

printf "%.0s*" {1..40};
printf "\n";
