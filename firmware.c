//
// firmware.c -- Firmware for PixelBox
//
//  Copyright (c) 2018 Ali Yousuf <aly.yousuf7@gmail.com>
//

#include <stdio.h>
#include "freedom.h"
#include "common.h"

extern char *_sbrk(int len);

// Main program
int main(void) {
    char i;
    char *heap_end;

    // Initialize all modules
    uart_init(115200);
    setvbuf(stdin, NULL, _IONBF, 0);        // No buffering

    // Blink LED pattern      rgbRGBrgbRGBrgb
    unsigned int pattern = 0b0010001100010001;
    unsigned int k = 5;
    while(k--) {
        RGB_LED(pattern & 4 ? 100 : 0, \
                pattern & 2 ? 100 : 0, \
                pattern & 1 ? 100 : 0);
        pattern >>= 3;
        delay(50);
    }
    RGB_LED(0, 0, 0); // OFF

    // Welcome banner
    iprintf("\r\n\r\n====== Freescale Freedom FRDM-KL25Z\r\n");
    iprintf("Built: %s %s\r\n\r\n", __DATE__, __TIME__);
    heap_end = _sbrk(0);
    iprintf("Reset code: 0x%02x 0x%02x\r\n", RCM_SRS0, RCM_SRS1);
    iprintf("Heap:  %p to %p (%d bytes used)\r\n", __heap_start, heap_end,
                heap_end - (char *)__heap_start);
    iprintf("Stack: %p to %p (%d bytes used)\r\n", &i, __StackTop,
                (char *)__StackTop - &i);
    iprintf("%d bytes free\r\n", &i - heap_end);

    for(;;) {
        iprintf("monitor> ");
        getchar();
        iprintf("\r\n");
    }
}
