#include "global.h"
#include "uart.h"
#include "sx8724.h"

void interrupt isr(void)
{
    sx8724_i2c_isr();
    uart_isr();
}
