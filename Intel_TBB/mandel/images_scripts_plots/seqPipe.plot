unset log
unset label
unset key
unset style
unset style line
set terminal postscript eps size 3.5,2.62 enhanced color font 'Helvetica,12' linewidth 2
set output "TBB_seqPipe.eps"
#set style data lines   
#set title "Spar Scalability Comparison"
set key outside;
set key center top;
set xrange [0:17]
set yrange [0:12500]
set xlabel 'Number of Replicas'
set ylabel 'Execution Time (ms)'
plot "seq_result.txt" w lines lw 1.5 linecolor rgb "#00FF00" title "Sequential", "seq_result.txt" using 1:2:($2-$3):($2+$3) w yerr linecolor rgb "#FF0000" not, "pipeline_result.txt" w lines lw 1.5 linecolor rgb "#0000FF" title "Pipeline", "pipeline_result.txt" using 1:2:($2-$3):($2+$3) w yerr linecolor rgb "#FF0000" not, 
