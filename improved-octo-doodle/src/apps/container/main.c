#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "uart.h"
#include "contents.h"
#include "predict.h"
#include "hexdump.h"

extern unsigned int __ContainerCodeStart;
extern unsigned int __ContainerCodeLength;

extern unsigned int __ContainerDataStart;
extern unsigned int __ContainerDataLength;

void switch_container() {

    printf("Container-Code contents were:\r\n ");
    hex_dump((void *)&__ContainerCodeStart, __ContainerCodeContentsLength);
    printf("Container-Code contents are being written with: \r\n");
    hex_dump(&__ContainerCodeContents, __ContainerCodeContentsLength);
    memcpy((void *)&__ContainerCodeStart, 
           (void *)__ContainerCodeContents, 
           __ContainerCodeContentsLength); 
    printf("Container-Code contents are now: \r\n");
    hex_dump((void *)&__ContainerCodeStart, __ContainerCodeContentsLength);

    printf("Container-Data contents were:\r\n ");
    hex_dump((void *)&__ContainerDataStart, __ContainerDataContentsLength);
    printf("Container-Data contents are being written with: \r\n");
    hex_dump(&__ContainerDataContents, __ContainerDataContentsLength);
    memcpy((void *)&__ContainerDataStart, 
           (void *)__ContainerDataContents, 
           __ContainerDataContentsLength); 
    printf("Container-Data contents are now: \r\n");
    hex_dump((void *)&__ContainerDataStart, __ContainerDataContentsLength);
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

  switch_container();
  printf("\r\n\r\n");  

  while (1) {
    predict();
    nrf_gpio_pin_toggle(LED);
    nrf_delay_ms(2000);
    printf("\r\n\r\n");  
  }
}

