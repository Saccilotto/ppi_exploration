#!/bin/bash
#script impletation based on SPAR_NUM_WORKERS which refactors compilation with all possible number of threads
DIM=1000
NITER=30000
RETRIES=5

APP="spar"
for i in {1..16}
do
	unset SPAR_NUM_WORKERS
	make cleanall
	export SPAR_NUM_WORKERS=$i
	make
        echo "running: $APP with $i threads."
        ./$APP $DIM $NITER $RETRIES > temp.txt
        avg=`grep "Average" temp.txt | cut -d" " -f6` 
        std=`grep "Average" temp.txt | cut -d" " -f10` 
        echo "$i $avg $std" >> $APP"_result.txt" 
done

exit
