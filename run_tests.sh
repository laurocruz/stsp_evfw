# !/bin/bash

FW_FOLDER="Framework/Release"
STP=$1
OUT_FOLDER=$2
MAX_SIZE=$3

if [ $4 -eq 1 ]
then
    cd $FW_FOLDER
    make clean;make
    cd -
fi

mkdir $OUT_FOLDER

for stp_file in $STP/*.stp
do
    # File size in KB
    file_size=$(expr $(wc -c <"$stp_file") / 1024)
    stp_basename=$(basename -- "$stp_file")
    stp_filename="${stp_basename%.*}"
    printf "Start $stp_filename\n"
    if [ -f "${OUT_FOLDER}/${stp_filename}.out"  ]
    then
        printf "DONE \n\n"
        continue
    fi
    if [ $file_size -gt $MAX_SIZE ]
    then
        printf "FAILURE: $stp_basename's size is too big.\n\n"
        continue
    fi
    ./$FW_FOLDER/Framework $stp_file > $OUT_FOLDER/$stp_filename.out || {
        printf "FAILURE: EXECUTION for $stp_basename\n\n"
    }
    printf "DONE \n\n"
done
