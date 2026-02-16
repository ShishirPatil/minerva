#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "contents.h"
#include "uart.h"
#include "sha256.h"
#include "checksum.h"

extern unsigned int __ContainerCodeStart;
extern unsigned int __ContainerCodeLength;

extern unsigned int __ContainerDataStart;
extern unsigned int __ContainerDataLength;

void hex_dump(void *addr, int len) 
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0) {
                printf("  %s\r\n", buff);
                nrf_delay_ms(10);
            }

            // Output the offset.
            printf("  %04x ", i);
            nrf_delay_ms(10);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        } else {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf("  %s\r\n", buff);
    nrf_delay_ms(10);
}

void switch_container() {

    unsigned char code_hash[32];
    unsigned char data_hash[32];
    sha256_hash((unsigned char *)__ContainerCodeContents, __ContainerCodeContentsLength, code_hash);
    sha256_hash((unsigned char *)__ContainerDataContents, __ContainerDataContentsLength, data_hash);

    nrf_gpio_pin_toggle(PIN);

    for (int i = 0; i < 4; i++) {
        long ours = ((long *)code_hash)[i];
        long theirs = ((long *)__ContainerCodeContentsHash)[i];
        if (ours != theirs) {
            printf("Code hashes do not match!\r\n");
        }
    }
    for (int i = 0; i < 4; i++) {
        long ours = ((long *)data_hash)[i];
        long theirs = ((long *)__ContainerDataContentsHash)[i];
        if (ours != theirs) {
            printf("Data hashes do not match!\r\n");
        }
    }
    nrf_gpio_pin_toggle(PIN);

    memcpy((void *)&__ContainerCodeStart, 
           (void *)__ContainerCodeContents, 
           __ContainerCodeContentsLength); 
    memcpy((void *)&__ContainerDataStart, 
           (void *)__ContainerDataContents, 
           __ContainerDataContentsLength); 
}
