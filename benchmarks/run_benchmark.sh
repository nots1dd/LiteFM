

N=100
total_time=0

#Compile the project with optimisations
find ./src -name "*.c" | xargs gcc -O2 -o litefm




for i in $(seq 1 $N); do
    echo "Running iteration $i..."

     run_time=$( { time ./LiteFM; } 2>&1 | grep real | awk '{print $2}')
     min=$(echo $run_time | cut -d'm' -f 1)
     sec=$(echo $run_time | cut -d'm' -f 2 | sed 's/s//')

     #calculate total time
     total=$(echo "$min*60 + $sec"|bc)
     total_time=$(echo "$total_time+ $total"|bc)

done



avg_time=$(echo "scale=3; $total_time/$N"|bc)
echo "Average execution time: $avg_time seconds"