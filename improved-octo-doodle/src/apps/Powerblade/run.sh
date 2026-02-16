#!/bin/bash

make clean || { exit 1; }
cp new_utils.h utils.h || { exit 1; }
cp new_fastGRNNModel.h fastGRNNModel.h || { exit 1; }
make || { exit 1; }
python3 ../../container-update/gen-byte-array.py _build/*.elf container-update/contents.h .container-code .container-data || { exit 1; }
make clean || { exit 1; }
cp old_fastGRNNModel.h fastGRNNModel.h || { exit 1; }
cp old_utils.h utils.h || { exit 1; }
make flash || { exit 1; }

