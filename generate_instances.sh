# !/bin/bash

GENERATOR='generate_instance.py'
TSP=$1
STP=$2

mkdir $STP
for tsp_file in $TSP/*.tsp
do
  tsp_file=$(basename -- "$tsp_file")
  tsp_filename="${tsp_file%.*}"
  for s in {5,10,20}
  do
    for f in {0.3,0.5,1.0}
    do
      printf "Start $tsp_file $s $f\n"
      suffix=${s}_${f:0:1}${f:2:1}
      python3 $GENERATOR $TSP/$tsp_file $STP/${tsp_filename}_${suffix}.stp $s $f || {
        printf "\nFAILURE: $tsp_file\n\n"
      }
      printf "\n"
    done
  done
done
