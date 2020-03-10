#!/bin/bash

if [ "$#" -ne 3 ]; then
	echo "Usage: $0 <dir to search for firmware files> <output dir> <file to search>"
	exit 1
fi

FILES_DIR=$1
OUTPUT_DIR=$2
FILE_NAME=$3

FIRMWARE_PATHS=$(find $FILES_DIR -type f -name "$FILE_NAME")
mkdir -p $OUTPUT_DIR

for path in $FIRMWARE_PATHS
do
BOARD_NAME=$(echo $path | grep -Eo "BOARD_[^/]+")
	cp $path ${OUTPUT_DIR}/${BOARD_NAME}_$(basename $path)
done
