#!/bin/bash

DIM=1000
NITER=30000
RETRIES=5

APP="map"
for i in {1..16}
do
        echo "running: $APP with $i threads."
        ./$APP $i $DIM $NITER $RETRIES > temp.txt
        avg=`grep "Average" temp.txt | cut -d" " -f6` 
        std=`grep "Average" temp.txt | cut -d" " -f10` 
        echo "$i $avg $std" >> $APP"_result.txt" 
done

APP="farm"
for i in {1..16}
do
	echo "running: $APP with $i threads."
	./$APP $i $DIM $NITER $RETRIES > temp.txt
	avg=`grep "Average" temp.txt | cut -d" " -f6` 
	std=`grep "Average" temp.txt | cut -d" " -f10` 
	echo "$i $avg $std" >> $APP"_result.txt" 
done

APP="pipeline_with_map"
for i in {1..16}
do
	echo "running: $APP with $i threads."
	./$APP $i $DIM $NITER $RETRIES > temp.txt
	avg=`grep "Average" temp.txt | cut -d" " -f6` 
	std=`grep "Average" temp.txt | cut -d" " -f10` 
	echo "$i $avg $std" >> $APP"_result.txt" 
done

exit
