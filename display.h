#define DISPLAY_DATA_MASK   0x0077 // 0000 0000 0RGB 0RGB
#define DISPLAY_CLK_SHIFT   0
#define DISPLAY_LAT_SHIFT   1
#define DISPLAY_OE_SHIFT    2
#define DISPLAY_A_SHIFT     8
#define DISPLAY_B_SHIFT     9
#define DISPLAY_C_SHIFT     10
#define DISPLAY_ROW_MASK    (1 << DISPLAY_A_SHIFT) | (1 << DISPLAY_B_SHIFT) | (1 << DISPLAY_C_SHIFT)

#define ROWS    8   // 2x
#define COLS    32
#define PLANES  8   // number of bit in each colour

void InitDisplay();
void PIT_IRQHandler() __attribute__((interrupt("IRQ")));