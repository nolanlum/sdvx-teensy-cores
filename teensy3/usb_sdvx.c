/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "usb_dev.h"
#include "usb_sdvx.h"
#include "core_pins.h" // for yield()
#include "HardwareSerial.h"
#include <string.h> // for memcpy()

#ifdef SDVX_INTERFACE // defined by usb_dev.h -> usb_desc.h

uint8_t usb_sdvx_data[4];

// Maximum number of transmit packets to queue so we don't starve other endpoints for memory
#define TX_PACKET_LIMIT 3

static uint8_t transmit_previous_timeout=0;

// When the PC isn't listening, how long do we wait before discarding data?
#define TX_TIMEOUT_MSEC 30

#if F_CPU == 96000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 596)
#elif F_CPU == 48000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 428)
#elif F_CPU == 24000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 262)
#endif

int usb_sdvx_send(void) {
    uint32_t wait_count = 0;
    usb_packet_t *tx_packet;

    while (1) {
        if (!usb_configuration) {
            return -1;
        }
        if (usb_tx_packet_count(SDVX_TX_ENDPOINT) < TX_PACKET_LIMIT) {
            tx_packet = usb_malloc();
            if (tx_packet) break;
        }
        if (++wait_count > TX_TIMEOUT || transmit_previous_timeout) {
            transmit_previous_timeout = 1;
            return -1;
        }
        yield();
    }
    transmit_previous_timeout = 0;
    memcpy(tx_packet->buf, usb_sdvx_data, 4);
    tx_packet->len = 4;
    usb_tx(SDVX_TX_ENDPOINT, tx_packet);
    return 0;
}

uint8_t usb_sdvx_receive(void) {
    usb_packet_t *rx_packet;
    uint8_t leds_state = 0;

    while (1) {
        if (!usb_configuration) break;
        rx_packet = usb_rx(SDVX_RX_ENDPOINT);
        if (!rx_packet) break;
        leds_state = rx_packet->buf[0];
        usb_free(rx_packet);

        yield();
    }

    return leds_state;
}


#endif // SDVX_INTERFACE
