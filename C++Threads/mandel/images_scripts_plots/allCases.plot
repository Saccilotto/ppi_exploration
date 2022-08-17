unset log
unset label
unset key
unset style
unset style line
set terminal postscript eps size 3.5,2.62 enhanced color font 'Helvetica,12' linewidth 2
set output "C++Threads_allplots.eps"
set key outside;
set key center top;
set autoscale
set grid
set xrange [0:16]
set xtics 1
set yrange [0:12000]
set ytics 1000
set xlabel 'Number of Replicas'
set ylabel 'Execution Time (ms)'
plot "farm_result.txt"  w lines lw 1.5 linecolor rgb "#00FF00" title "Farm", "" using 1:2:($2-$3):($2+$3) w yerr linecolor rgb "#00FF00" not, "pipeline_result.txt" w lines lw 1.5 linecolor rgb "#3333FF" title "Pipeline", "" using 1:2:($2-$3):($2+$3) w yerr linecolor rgb "#3333FF" not, "seq_result.txt" w lines lw 1.5 linecolor rgb "#FF0000" title "Seq", "" using  1:2:($2-$3):($2+$3) w yerr linecolor rgb "#FF0000" not, 
