#include "global.h"
#include "ui.h"

void ui_init(void)
{
    TRISDbits.TRISD1 = 0;   // top white
    
}


void ui_led_top_white(bool on)
{
    PORTDbits.RD1 = on;
}