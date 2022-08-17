unset log 
unset label
unset key
unset style 
unset style line
set terminal postscript eps size 3.5,2.62 enhanced color font 'Helvetica,12' linewidth 2 
set output "spar_resultImg.eps"
#set style data lines	
#set title "Spar Scalability Comparison"
set key outside;
set key center top;
set xrange [0:17]
set yrange [0:12000]
set xlabel 'Number of Replicas'
set ylabel 'Execution Time (ms)'
plot "spar_result.txt" using 1:2 title 'Spar' lt 7 lc 22 w l, \
"" using 1:2:($2-$3):($2+$3) notitle w yerrorbar


 
