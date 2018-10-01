//
// main.c -- Firmware for PixelDisplay
//
//  Copyright (c) 2018 Ali Yousuf <aly.yousuf7@gmail.com>
//

#include <stdio.h>
#include "freedom.h"
#include "common.h"
#include "display.h"

extern char *_sbrk(int len);

// Main program
int main(void) {
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

    iprintf("\r\n\r\n====== Freescale Freedom FRDM-KL25Z\r\n");
    iprintf("PixelDisplay\r\n");

    // Initialize LED Pins
    InitDisplay();

    while (1) {
        RGB_LED(0, 0, 100);
        delay(500);
        RGB_LED(0, 0, 0);
        delay(500);
    }
}
