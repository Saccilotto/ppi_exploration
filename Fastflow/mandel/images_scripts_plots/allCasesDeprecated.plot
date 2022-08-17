#!/usr/bin/expect
#set terminal postfile
#set output "$fileName_plot.ps"

set timeout -1

control = true
i = 0

while [$control = true]
	echo "Press "0" to quit"
        read quit
	if [quit = "0"]
		break
	fi 
	spawn ./interaction	
	expect "Insert file name here: \r"
	set aux [lindex $argv $i] 
	add_file $aux 
	i = i+1
done


add_file()
{
FILE=$1
if      [-a "$FILE.txt"]
then
	gnuplot
	set title "FastFlow Implementations' Scalability"
	set xlabel "Number of Replicas"
	set ylabel "Execution Time (ms)"
	set xrange [0:17]
	set yrange [0:40000]
	plot "$FILE.txt" using 1:2 title "$FILE" w l, "$FILE.txt" using 2:3 title "Errorbars" w yerrorbars
else
fi
}
