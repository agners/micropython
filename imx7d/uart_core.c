#include <unistd.h>
#include "py/mpconfig.h"
#include <debug_console_imx.h>

/*
 * Core UART functions to implement for a port
 */

#if MICROPY_MIN_USE_STM32_MCU
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
} periph_uart_t;
#define USART1 ((periph_uart_t*)0x40011000)
#endif

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    return debug_getchar();
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
	debug_putchar(*str++);
    }
}
