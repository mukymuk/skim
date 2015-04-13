#include "global.h"
#include "uart.h"
#include "sx8724.h"
#include "tmr.h"

void interrupt isr(void)
{
    tmr_isr();
    sx8724_i2c_isr();
    uart_isr();
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}
