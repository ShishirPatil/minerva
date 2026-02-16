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
  ret_code_t error_code = NRF_SUCCESS;

  // initialize GPIO driver
  if (!nrfx_gpiote_is_init()) {
    error_code = nrfx_gpiote_init();
  }
  APP_ERROR_CHECK(error_code);

  // configure leds
  // manually-controlled (simple) output, initially set
  //nrfx_gpiote_out_config_t out_config = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
  //for (int i=0; i<4; i++) {
  //  error_code = nrfx_gpiote_out_init(LEDS[i], &out_config);
  //  APP_ERROR_CHECK(error_code);
  //}

  // configure buttons
  // input pin, trigger on either edge, low accuracy (allows low-power operation)
  //nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
  //in_config.pull = NRF_GPIO_PIN_PULLUP;
  //for (int i=0; i<4; i++) {
  //  error_code = nrfx_gpiote_in_init(BTNS[i], &in_config, pin_change_handler);
  //  APP_ERROR_CHECK(error_code);

  //  // enable events from the input pin and enable interrupts
  //  nrfx_gpiote_in_event_enable(BTNS[i], true);
  //}

  // initialize serial library
  error_code = nrf_serial_init(&serial_uart_instance, &serial_uart_config, &serial_config);
  APP_ERROR_CHECK(error_code);

  // write init message
  static char tx_message[] = "UART Initialized!\n";
  error_code = nrf_serial_write(&serial_uart_instance, tx_message, strlen(tx_message), NULL, 0);
  APP_ERROR_CHECK(error_code);

  // initialize RTT library
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  // two examples of writing messages to RTT
  NRF_LOG_INFO("Log messages initialized");
  printf("Printing with printf works too!\n");

  // loop forever
  while (1) {
    // enter sleep mode
    nrf_pwr_mgmt_run();
  }
}








//int main(void) {
//
//  nrf_gpio_cfg_output(LED);
//  nrf_gpio_pin_set(LED);
//  uart_init();
//
//  clear_screen();
//  printf("Starting ================================================= \r\n\r\n");  
//
//
//  while (1) {
//    nrf_delay_ms(2000);
//
//    printf("loop \r\n");
//
//    nrf_gpio_pin_toggle(LED);
//    printf("\r\n\r\n");  
//  }
//}

