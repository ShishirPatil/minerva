#!/bin/bash

make clean
cp updated_predictor_code.h predict.h
make
python3 gen-byte-array.py _build/container_sdk15_blank.elf contents.h .container-code .container-data
make clean
cp predictor_code.h predict.h
make flash
rm predict.h

