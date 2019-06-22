# !/bin/bash

TSP='tsp_instances'
DESTINATION=$1
MAX_SIZE=$2

mkdir $DESTINATION

for tsp_file in $TSP/*.tsp
do
    tsp_basename=$(basename -- "$tsp_file")
    tsp_filename="${tsp_file%.*}"
    size=${tsp_filename//[!0-9]/}
    printf "Copying $tsp_basename\n"
    if [ $size -gt $MAX_SIZE ]
    then
      printf "    FAIL\n"
      continue
    fi
    cp $tsp_file $DESTINATION/$tsp_basename
    printf "    SUCCESS\n"
done
