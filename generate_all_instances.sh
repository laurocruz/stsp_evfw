# !/bin/bash

set +e

GENERATOR='generate_instance.py'
TSP='tsp_instances'
STP='stp_instances'

mkdir $STP
for tsp_file in $TSP/*.tsp
do
    tsp_file=$(basename -- "$tsp_file")
    tsp_filename="${tsp_file%.*}"
    printf "Start $tsp_file\n"
    python3 $GENERATOR $TSP/$tsp_file $STP/$tsp_filename.stp $1 $2 || {
        printf "\nFAILURE: $tsp_file\n\n"
    }
    printf "\n"
done
