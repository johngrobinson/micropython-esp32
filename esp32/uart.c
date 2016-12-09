/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>

#include "esp_intr.h"
#include "soc/uart_struct.h"

#include "py/mphal.h"

STATIC void uart_irq_handler(void *arg);

void uart_init(void) {
    xt_set_interrupt_handler(ETS_UART0_INUM, uart_irq_handler, NULL);
    ESP_INTR_ENABLE(ETS_UART0_INUM);
}

STATIC void IRAM_ATTR uart_irq_handler(void *arg) {
    volatile uart_dev_t *uart = &UART0;
    uart->int_clr.rxfifo_full = 1;
    uart->int_clr.frm_err = 1;
    uart->int_clr.rxfifo_tout = 1;
    while (uart->status.rxfifo_cnt) {
        uint8_t c = uart->fifo.rw_byte;
        if (c == mp_interrupt_char) {
            mp_keyboard_interrupt();
        } else {
            ringbuf_put(&stdin_ringbuf, c);
        }
    }
}
