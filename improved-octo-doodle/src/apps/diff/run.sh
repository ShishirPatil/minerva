#!/bin/bash

exit 1;
make clean || { exit 1; }
cp updated_predictor_code.h predict.h || { exit 1; }
make || { exit 1; }
python3 gen-byte-array.py _build/diff_sdk15_blank.elf contents.h .container-code .container-data || { exit 1; }
make clean || { exit 1; }
cp predictor_code.h predict.h || { exit 1; }
make flash || { exit 1; }
rm predict.h || { exit 1; }

