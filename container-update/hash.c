#include <stdlib.h>
#include <memory.h>
#include "sha256.h"
#include "contents.h"
#include <stdio.h>

int main(int argc, char** argv) {

    unsigned char code_hash[32];
    unsigned char data_hash[32];
    sha256_hash((unsigned char *)&__ContainerCodeContents, __ContainerCodeContentsLength, code_hash);
    sha256_hash((unsigned char *)&__ContainerDataContents, __ContainerDataContentsLength, data_hash);

    for (int i = 0; i < 32; i++) {
        printf("0x%02x,\n", code_hash[i]);
    }

    printf("=\n");

    for (int i = 0; i < 32; i++) {
        printf("0x%02x,\n", data_hash[i]);
    }

}
