#!/bin/bash

# A run script which tests our application update
# system by compiling two versions of the code, one with
# and one without the update. We then use the python script
# `gen-byte-array.py` to extract the update from the first binary
# and send it to the second binary so that it can update its
# data and code sections while running

make clean || { exit 1; }
cp utils/new_utils.h utils.h || { exit 1; }
cp models/new_model.h model.h || { exit 1; }
make || { exit 1; }
python3 container-update/gen-byte-array.py _build/*.elf container-update/contents.h .container-code .container-data || { exit 1; }
make clean || { exit 1; }
cp models/old_model.h model.h || { exit 1; }
cp utils/old_utils.h utils.h || { exit 1; }
make flash || { exit 1; }

