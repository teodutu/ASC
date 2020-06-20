#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Usage ./run_on_q.sh tema2_<mode>"
	exit
fi

make && 
qsub -cwd -q ibm-nehalem.q -b y ./$1 input_extended

sleep 1

while true
do
	if [ $(qstat | wc -l) -eq 0 ]
	then
		break
	fi
	sleep 1
done

cat $1.o*

cat $1.o* >> ibm-runs.log

printf "\n\n ------------------------------------- \n\n" >> ibm-runs.log

#./compare out1 /export/asc/tema2/out1 0.001
#./compare out2 /export/asc/tema2/out2 0.001
#./compare out3 /export/asc/tema2/out3 0.001

rm $1.e* $1.o*
make clean

echo "Job done"

