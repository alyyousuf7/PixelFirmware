//
// spi.c -- Serial I/O
//
//  Copyright (c) 2018 Ali Yousuf <aly.yousuf7@gmail.com>
//

#include "freedom.h"
#include "common.h"

#define BUFFER_SIZE (16*32*3) + 2
static uint32_t slaveIndex = BUFFER_SIZE;
static volatile unsigned int slaveFinished = 0;

void SPI0_IRQHandler(void) {
    if (slaveFinished) {
        slaveFinished = 0;
        slaveIndex = BUFFER_SIZE;
    }

    if (SPI0_S & SPI_S_SPRF_MASK) {
        // destBuff[BUFFER_SIZE - slaveIndex] = SPI0_D;
        uint8_t val = SPI0_D;
        iprintf(val);

        slaveIndex--;
    }

    if (!slaveIndex) {
        slaveFinished = 1;
    }
}

void spi_init(void) {
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

	// Enable clock network to SPI0
	SIM_SCGC4 |= SIM_SCGC4_SPI0_MASK;
	
	// configure output crossbar
	PORTD_PCR0 = PORT_PCR_MUX(2);  // PCS
	PORTD_PCR1 = PORT_PCR_MUX(2);  // SCK
	PORTD_PCR2 = PORT_PCR_MUX(2);  // MOSI
	PORTD_PCR3 = PORT_PCR_MUX(2);  // MISO
	
	// configure gpio address select
	/* here */
	
	// Config registers, turn on SPI0 as slave
	SPI0_C2 = 0;
    SPI0_C1 = SPI_C1_SPE_MASK;

    // Enable interrupt
    SPI0_C1 |= SPI_C1_SPIE_MASK;
    enable_irq(INT_SPI0);
}

// Write out all characters in supplied buffer to register at address
void spi_write(char *p, int len) {
	int i;
	// set SPI line to output (BIDROE = 1)
	//SPI0_C2 |= 0x04;
	for (i = 0; i < len; ++i) {
		// poll until empty
		while ((SPI0_S & SPI_S_SPTEF_MASK) != SPI_S_SPTEF_MASK);
		SPI0_D = p[i];
	}
}

// Read size number of characters into buffer p from register at address
void spi_read(char *p, int len) {
	int i;
	// set SPI line to input (BIDROE = 0)
	//SPI0_C2 &= 0xF7;
	for (i = 0; i < len; ++i) {
		// poll until full
		SPI0_D = 0x00;
		while ((SPI0_S & SPI_S_SPRF_MASK) != SPI_S_SPRF_MASK);
		p[i] = SPI0_D;
	}
}