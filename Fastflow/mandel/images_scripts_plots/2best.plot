unset log
unset label
unset key
unset style
unset style line
set terminal postscript eps size 3.5,2.62 enhanced color font 'Helvetica,12' linewidth 2
set output "FF_2_BestPlots.eps"
set key outside;
set key center top;
set xrange [0:17]
set yrange [0:17500]
set xlabel 'Number of Replicas'
set ylabel 'Execution Time (ms)'
plot "map_result.txt" w lines lw 1.5 linecolor rgb "#00FF00" title "Map", "map_result.txt" using 1:2:($2-$3):($2+$3) w yerr linecolor rgb "#FF0000" not, "farm_result.txt" w lines lw 1.5 linecolor rgb "#00FFFF" title "Farm", "farm_result.txt" using 1:2:($2-$3):($2+$3) w yerr linecolor rgb "#FF0000" not,   
