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

#ifndef USBsdvx_h_
#define USBsdvx_h_

#if defined(USB_SDVX)

#include <inttypes.h>

// C language implementation
#ifdef __cplusplus
extern "C" {
#endif
int usb_sdvx_send(void);
uint8_t usb_sdvx_receive(void);
extern uint8_t usb_sdvx_data[4];
#ifdef __cplusplus
}
#endif

// C++ interface
#ifdef __cplusplus
class usb_sdvx_class {
public:
    void begin(void) { }
    void end(void) { }
    void button(uint8_t button, bool val) {
        if (--button >= 7) return;
        if (val) usb_sdvx_data[0] |= (1 << button);
        else usb_sdvx_data[0] &= ~(1 << button);
        if (!manual_mode) usb_sdvx_send();
    }
    void encLeft(unsigned int val) {
        if (val > 1023) val = 1023;
        usb_sdvx_data[1] = val & 0xFF;
        usb_sdvx_data[2] = (usb_sdvx_data[2] & 0xFC) | (val >> 8);
        if (!manual_mode) usb_sdvx_send();
    }
    void encRight(unsigned int val) {
        if (val > 1023) val = 1023;
        //*usb_sdvx_data_int = (*usb_sdvx_data_int & 0xF003FFFF) | (val << 18);
        usb_sdvx_data[2] = (usb_sdvx_data[2] & 0x03) | ((val & 0x3F) << 2);
        usb_sdvx_data[3] = (val & 0x3E0) >> 6;
        if (!manual_mode) usb_sdvx_send();
    }

    void useManualSend(bool mode) {
        manual_mode = mode;
    }
    void send_now(void) {
        usb_sdvx_send();
    }

    void update() { usb_sdvx_class::sdvx_leds = usb_sdvx_receive(); }
    uint8_t ledState() { return usb_sdvx_class::sdvx_leds; }
private:
    static uint8_t manual_mode;
    static uint8_t sdvx_leds;
};

extern usb_sdvx_class SDVX;

#endif // __cplusplus

#endif // USB_SDVX
#endif // USBsdvx_h_
