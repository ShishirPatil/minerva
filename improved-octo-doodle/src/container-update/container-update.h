#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "contents.h"
#include "uart.h"
#include "sha256.h"
#include "checksum.h"

// The linker sets the following variables,
// which point to the start of the ML code an
// ML data "containers" within flash memory. 
// The linker also tells us the corresponding lengths
// of these sections.
extern unsigned int __ContainerCodeStart;
extern unsigned int __ContainerCodeLength;
extern unsigned int __ContainerDataStart;
extern unsigned int __ContainerDataLength;

// Utility function to print the bytes from a memory
// region out over serial. We use this to examine the
// contents of memory at certain points during program
// execution;
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

//
// Swaps the data from src into dst and from dst into src by
// utilizing a temperorary buffer. This allows us to "atomically"
// swap the old container memory out for the new update. We can then
// have the option of swapping back to the old container if the new update
// proves to be suboptimal.
// Note: currently src and dst must be the same size buffers
//
void perform_container_swap(void *dst, void *src, size_t num_bytes, size_t tmp_size) {

    uint8_t tmp_buffer[tmp_size];

    while (num_bytes > 0) {
        //
        // Transfer the min of num_bytes and tmp_size
        //
        size_t bytes_to_transfer = num_bytes > tmp_size ? tmp_size : num_bytes;
        //
        // Copy: dst -> tmp, src -> dst, tmp -> src
        // make sure that src is at least as big as dst!
        //
        memcpy(&tmp_buffer[0], dst, bytes_to_transfer); 
        memcpy(dst, src, bytes_to_transfer);
        memcpy(src, &tmp_buffer[0], bytes_to_transfer);

        //
        // Update pointers
        //
        dst += bytes_to_transfer;
        src += bytes_to_transfer;
        num_bytes -= bytes_to_transfer;
    }
}

// Benchmark function that performs "atomic" container swaps with
// a varying temporary buffer size. Toggles a pin on the board such that
// the execution time can be measured with an oscilloscope. This allows us
// to determine the proper temporary buffer size for a given platform.
void benchmark_container_swap(void *dst, void *src, size_t num_bytes) {

    //
    // For different values of tmp_size, we will toggle PIN before and after the container swap,
    // such that the time can be measured with an oscilloscope. 
    //
    for (size_t tmp_size = 0; tmp_size < 256; tmp_size++) {
        //
        // TODO: do we need to do something to restore processor state
        // to cold? For instance, if there were a cache, then this benchmark
        // would need to flush it.
        //
        nrf_delay_ms(2000); // Give us time to setup trigger on the oscilloscope
        nrf_gpio_pin_toggle(PIN); // Trigger on rising edge
        perform_container_swap(dst, src, num_bytes, tmp_size);
        nrf_gpio_pin_toggle(PIN);
    }
}

// Utility function. We hash the update memory contents and compare with the
// checksum sent to us. If the two results differ, the memory must be corrupted
// in some way and we must reject the update.
bool check_hash() {

    printf("Beginning to check hash\r\n");

    // Hash the update memory contents
    unsigned char code_hash[32];
    unsigned char data_hash[32];
    sha256_hash((unsigned char *)__ContainerCodeContents, __ContainerCodeContentsLength, code_hash);
    sha256_hash((unsigned char *)__ContainerDataContents, __ContainerDataContentsLength, data_hash);

    printf("Hashed\r\n");

    // Toggle pin for benchmarking
    nrf_gpio_pin_toggle(PIN);

    // Compare the hashes of the code update memory
    for (int i = 0; i < 4; i++) {
        long ours = ((long *)code_hash)[i];
        long theirs = ((long *)__ContainerCodeContentsHash)[i];
        if (ours != theirs) {
            printf("Code hashes do not match!\r\n");
            return false;
        }
    }

    // Compare the contents of the data update memory
    for (int i = 0; i < 4; i++) {
        long ours = ((long *)data_hash)[i];
        long theirs = ((long *)__ContainerDataContentsHash)[i];
        if (ours != theirs) {
            printf("Data hashes do not match!\r\n");
            return false;
        }
    }

    nrf_gpio_pin_toggle(PIN);
    return true;
}

// This function walks two sets of pointers through the code memory
// update. The first two pointers point to the beginning of the current
// container code memory contents and the beginning of the update memory
// contents. The other two pointers point to the end of these memory regions.
// We then walk the first two pointers forward and the last two pointers
// backwards through the memory regions and stop when the data they point at is
// not equal. This gives us a range of addresses that are different between
// the current memory contents and the update. We can then opt to just
// reconcile this small region rather than performing the entire update.
bool mini_diff() {

    // Set up pointers
    long current_start = 0, current_end = __ContainerCodeLength - 1;
    long update_start = 0, update_end = __ContainerCodeContentsLength - 1;

    unsigned char *current_data = (unsigned char *)&__ContainerCodeStart;
    unsigned char *update_data = (unsigned char *)__ContainerCodeContents;

    // Walk the first two pointers forward through memory
    while (current_data[current_start] == update_data[update_start]) {
        if (current_start == current_end - 1 || update_start == update_end - 1) {
            break;
        }
        current_start++;
        update_start++;
    }

    // If we got to the end of the update, then the two regions must be the same
    if (current_start == update_end && update_start == current_end) {
        printf("Container contents are the same (1)!\n");
        return true;
    }

    // Now walk the other two pointers back
    while (current_data[current_end - 1] == update_data[update_end - 1]) {
        if (current_end == 0 || update_end == 0) {
            break;
        }
        current_end--;
        update_end--;
    }

    // If we got to the beginning, then the two regions are the same
    if (current_end == 0 && update_end == 0) {
        printf("Container contents are the same (2)!\n");
        return true;
    }

    // We can then determine the length of the memory contents that
    // differ between the two memory regions
    long current_divergent_length = current_end + 1 - current_start;
    long update_divergent_length = update_end + 1 - update_start;

    // If we cant replace the current container divergent area with
    // the update divergent area, then we exit
    if (current_divergent_length < update_divergent_length) {
        printf("Cannot mini-diff: overflow\n");
        return false;
    }

    //
    // We now know that the divergent area for the current container
    // is greater than or equal to the desired update. We can
    // simply memcpy then!
    //
    memcpy(current_data + current_start, update_data + update_start, update_divergent_length);

    //
    // Now we have a memory layout as follows
    // 0x0:           -------------------------------
    //                | Original Container Data     |
    // current_start: -------------------------------
    //                | Updated Container Data      |
    // update_end+1:  -------------------------------
    //                | Garbage Data                |
    // current_end+1: -------------------------------
    //                | Original Container Data     |
    //                 ------------------------------
    //                 

    printf("Completed mini_diff\n");
    return true;
}


// Performs the container swap depending on the size of the temporary buffer
// the user specifies 
void switch_container(int temp_size) {

    if (temp_size > 0) {
        perform_container_swap((void *)&__ContainerCodeStart, (void *)__ContainerCodeContents, __ContainerCodeContentsLength, temp_size);
        perform_container_swap((void *)&__ContainerDataStart, (void *)__ContainerDataContents, __ContainerDataContentsLength, temp_size);
    } else {
        memcpy((void *)&__ContainerCodeStart, 
               (void *)__ContainerCodeContents, 
               __ContainerCodeContentsLength); 
        memcpy((void *)&__ContainerDataStart, 
               (void *)__ContainerDataContents, 
               __ContainerDataContentsLength); 
    }
}


