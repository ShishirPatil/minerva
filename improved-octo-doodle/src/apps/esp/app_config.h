// application-specific configuration
// augments the base configuration in sdk/<SDK>/config/<IC>/config/sdk_config.h

#pragma once

#define NRFX_UARTE_ENABLED 1
#define NRFX_UART_ENABLED 1
#define UART_ENABLED 1
#define UART0_ENABLED 1
#define APP_FIFO_ENABLED 1
#define APP_UART_ENABLED 1
#define RETARGET_ENABLED 1
#define HCI_UART_TX_PIN 6
#define HCI_UART_RX_PIN 8

#define NRFX_GPIOTE_ENABLED 1
#define NRF_SERIAL_ENABLED 1
#define PWR_MGMT_ENABLED 1
