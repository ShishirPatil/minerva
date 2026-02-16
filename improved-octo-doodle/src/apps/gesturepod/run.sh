#!/bin/bash

make clean || { exit 1; }
cp utils/new_utils.h utils.h || { exit 1; }
cp models/new_model.h model.h || { exit 1; }
make || { exit 1; }
python3 ../../container-update/gen-byte-array.py _build/gesturepod_sdk15_blank.elf container-update/contents.h .container-code .container-data || { exit 1; }
make clean || { exit 1; }
cp models/old_model.h model.h || { exit 1; }
cp utils/old_utils.h utils.h || { exit 1; }
make flash || { exit 1; }

