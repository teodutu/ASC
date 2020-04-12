#!/bin/bash

if [ $# -ne 1 ]
then
	printf "Usage ./run.sh tema2_<mode>\n"
	exit
fi

make clean
make

printf "\nJob done\nChecking the refs\n"

EPS=0.001

./$1 input
./compare out/out1 refs/out1 $EPS
./compare out/out2 refs/out2 $EPS
./compare out/out3 refs/out3 $EPS
