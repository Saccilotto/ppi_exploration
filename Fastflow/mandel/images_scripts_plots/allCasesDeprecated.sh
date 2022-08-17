#!/usr/bin/expect
#set terminal postfile
#set output "allCase_plot.ps"

set timeout -1

while true
do
	echo "Press "0" to quit\r"
        read quit
	if [[quit = 0]] then
		break
	fi 
	set aux [lindex $argv $i] 
	spawn ./interaction	
	expect "Insert file name here: \r"
	send -- "$aux\r"
	add_file $aux
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
