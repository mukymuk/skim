#include "global.h"
#include "uart.h"

void interrupt isr(void)
{
    uart_isr();
}
