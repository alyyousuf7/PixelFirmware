#include "freedom.h"
#include "common.h"
#include "display.h"
#include "images.h"

unsigned int* screenBuf;
unsigned int* bufPtr;

unsigned int firstDelay = 10;
int row = 0;
int plane = 0;

void PIT_IRQHandler() {
    // 2. Disable the screen
    GPIOB_PSOR = 1 << DISPLAY_OE_SHIFT;

    // 3. Set row address
    GPIOB_PCOR = DISPLAY_ROW_MASK;
    if (row & 4) GPIOB_PSOR = 1 << DISPLAY_A_SHIFT;
    if (row & 2) GPIOB_PSOR = 1 << DISPLAY_B_SHIFT;
    if (row & 1) GPIOB_PSOR = 1 << DISPLAY_C_SHIFT;

    // 4. Latch the data
    GPIOB_PSOR = 1 << DISPLAY_LAT_SHIFT;
    GPIOB_PCOR = 1 << DISPLAY_LAT_SHIFT;

    // 5. Enable the screen
    GPIOB_PCOR = 1 << DISPLAY_OE_SHIFT;

    // 6. Set time to wait and reload timer
    PIT_LDVAL0 = firstDelay << plane;
    PIT_TCTRL0 = 0;     // Disable timer
    PIT_TFLG0 |= PIT_TFLG_TIF_MASK;     // Clear the timer interrupt flag 
    PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK;     // Enable time

    // Reset row
    if (++plane >= PLANES) {
        plane = 0;

        if (++row >= ROWS) {
            row = 0;
            bufPtr = screenBuf;
        }
    }

    // 1. Shift Data
    for (int i = 0; i < COLS; i++) {
        GPIOC_PCOR = DISPLAY_DATA_MASK;
        GPIOC_PSOR = *bufPtr++; // & DISPLAY_DATA_MASK;

        GPIOB_PSOR = 1 << DISPLAY_CLK_SHIFT;
        GPIOB_PCOR = 1 << DISPLAY_CLK_SHIFT;
    }
}

void InitDisplay() {
    // Setup up Control Pins
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK;
    PORTB_PCR(DISPLAY_CLK_SHIFT) = PORT_PCR_MUX(1);
    PORTB_PCR(DISPLAY_LAT_SHIFT) = PORT_PCR_MUX(1);
    PORTB_PCR(DISPLAY_OE_SHIFT)  = PORT_PCR_MUX(1);
    PORTB_PCR(DISPLAY_A_SHIFT)   = PORT_PCR_MUX(1);
    PORTB_PCR(DISPLAY_B_SHIFT)   = PORT_PCR_MUX(1);
    PORTB_PCR(DISPLAY_C_SHIFT)   = PORT_PCR_MUX(1);
    GPIOB_PDDR |= (1 << DISPLAY_CLK_SHIFT) | (1 << DISPLAY_LAT_SHIFT) | (1 << DISPLAY_OE_SHIFT) |
                  (1 << DISPLAY_A_SHIFT)   | (1 << DISPLAY_B_SHIFT)   | (1 << DISPLAY_C_SHIFT);

    // Setup Data Pins
    PORTC_PCR(0) = PORT_PCR_MUX(1); // B1
    PORTC_PCR(1) = PORT_PCR_MUX(1); // G1
    PORTC_PCR(2) = PORT_PCR_MUX(1); // R1
    PORTC_PCR(4) = PORT_PCR_MUX(1); // B0
    PORTC_PCR(5) = PORT_PCR_MUX(1); // G0
    PORTC_PCR(6) = PORT_PCR_MUX(1); // R0
    GPIOC_PDDR |= 0b0000000001110111;

    // Load image
    screenBuf = loadingImg;

    // Display loop
    bufPtr = screenBuf;

    // Enable PIT Timer
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT_MCR = PIT_MCR_FRZ_MASK;
    PIT_LDVAL0 = firstDelay; // Default counter value
    PIT_TCTRL0 = PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK;
    enable_irq(INT_PIT);
}
