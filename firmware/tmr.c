#include "global.h"
#include "tmr.h"

uint16_t s_ms;

static void inline set_tmr0( void )
{
    // 65535-(32MHz/4/1000)
    TMR0H=0x0E;
    TMR0L=0xBF; 
}
void tmr_init(void)
{
    set_tmr0();
    T0CONbits.TMR0ON = 1;
}

uint16_t tmr_getms( void )
{
    uint16_t ms;
    INTCONbits.TMR0IE = 0;
    ms = s_ms;
    INTCONbits.TMR0IE = 1; 
    return ms;
}

void tmr_isr( void )
{
    if(INTCONbits.TMR0IF == 1)
    {
        set_tmr0();
        s_ms++;
        INTCONbits.TMR0IF  = 0;
    }
}