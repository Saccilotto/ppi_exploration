#!/bin/bash

DIM=1000
NITER=30000
RETRIES=5

j = 0
APP="farm"

echo "running: $APP with $j threads."
./$APP $DIM $NITER $RETRIES > temp.txt
avg=`grep "Average" temp.txt | cut -d" " -f6`
std=`grep "Average" temp.txt | cut -d" " -f10`
echo "$j $avg $std" >> $APP"_resultFinal.txt"

for i in {1..16}
do
        echo "running: $APP with $i threads."
        ./$APP $DIM $NITER $i $RETRIES > temp.txt
        avg=`grep "Average" temp.txt | cut -d" " -f6`
        std=`grep "Average" temp.txt | cut -d" " -f10`
        echo "$i $avg $std" >> $APP"_resultFinal.txt"
done

exit
