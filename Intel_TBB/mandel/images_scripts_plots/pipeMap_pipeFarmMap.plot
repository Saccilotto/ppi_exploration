unset log
unset label
unset key
unset style
unset style line
set terminal postscript eps size 3.5,2.62 enhanced color font 'Helvetica,12' linewidth 2
set output "TBB_pipeMap_farmMap.eps"
#set style data lines   
#set title "Spar Scalability Comparison"
set key outside;
set key center top;
set xrange [0:17]
set yrange [0:12500]
set xlabel 'Number of Replicas'
set ylabel 'Execution Time (ms)'
plot "pipeline_with_farm_map_result.txt" w lines lw 1.5 linecolor rgb "#FF8C00" title "PipelineFarmMap", "pipeline_with_farm_map_result.txt" using 1:2:($2-$3):($2+$3) w yerr linecolor rgb "#FF0000" not, "pipeline_with_map_result.txt" w lines lw 1.5 linecolor rgb "#000080" title "PipelineMap", "pipeline_with_map_result.txt" using 1:2:($2-$3):($2+$3) w yerr linecolor rgb "#FF0000" not,    
