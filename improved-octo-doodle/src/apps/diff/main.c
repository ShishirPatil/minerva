#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "uart.h"
#include "diffcontents.h"
#include "predict.h"
#include "hexdump.h"

extern unsigned int __ContainerCodeStart;
extern unsigned int __ContainerCodeLength;

extern unsigned int __ContainerDataStart;
extern unsigned int __ContainerDataLength;

void diff_function() {

    printf("Original Function Was: \r\n");
    hex_dump((void *)&__ContainerCodeStart, __OriginalFunctionLength);
    printf("Original function being replaced by shorter version: \r\n");
    hex_dump((void *)&__NewFunctionContents, __UpdatedFunctionLength);
    memcpy((void *)&__ContainerCodeStart, 
           (void *)__NewFunctionContents, 
           __UpdatedFunctionLength); 
    memset((void *)((char *)&__ContainerCodeStart + __UpdatedFunctionLength), 0, __OriginalFunctionLength - __UpdatedFunctionLength);
    printf("Container-Code contents are now: \r\n");
    hex_dump((void *)&__ContainerCodeStart, __OriginalFunctionLength);
    printf("Finished\r\n");
}

void clear_screen() { for (int _ = 0; _< 20; _++) printf("\r\n"); }

int main(void) {

  nrf_gpio_cfg_output(LED);
  nrf_gpio_pin_set(LED);
  uart_init();

  clear_screen();
  printf("Starting ================================================= \r\n\r\n");  

  for (int i = 0; i < 2; i++) {
    predict();
    nrf_gpio_pin_toggle(LED);
    nrf_delay_ms(2000);
    printf("\r\n\r\n");  
  }

  diff_function();
  printf("\r\n\r\n");  

  while (1) {
    nrf_delay_ms(2000);
    predict();
    nrf_gpio_pin_toggle(LED);
    printf("\r\n\r\n");  
  }
}

