#include "global.h"
#include "uart.h"
#include "ui.h"
#include "cmd.h"
#include "sx8724.h"
#include "tmr.h"

#pragma config 	XINST = OFF 		// no extended instructions
#pragma config 	STVREN = ON 		// stack overflow causes reset
#pragma config 	WDTEN = OFF			// watchdog disabled
#pragma config 	IESO = OFF			// two-speed startup is disabled (no external osc)
#pragma config 	FCMEN = OFF			// fail-safe clock disabled (no external osc)
#pragma config 	FOSC = INTOSCPLL	// internal osc with RA6 and RA7 as port bits
#pragma config  WDTPS = 512			
#pragma config  MSSPMSK = MSK7		// 7-Bit Address Masking mode enable  
#pragma config  CCP2MX = DEFAULT 	// ECCP2/P2A is multiplexed with RC1  
#pragma config 	CP0 = OFF 			// code memory is not protected


void sleep( void )
{
    // non-isr only
    // called when program flow must stop with interrupts enabled
    while( sx8724_isr() );   // simulate isr mechanism for adc
    SLEEP();
}

int main(int argc, char** argv) 
{
    bool on = 0;
    
    TRISAbits.TRISA4 = 1;
    OSCCON = 0b01110000;
    OSCTUNEbits.PLLEN = 1;
    __delay_ms(2);

    tmr_init();
    uart_init();
    ui_init();
    sx8724_init();
    cmd_init();
    
            
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;
    while(1)
    {
        //uart_putchar('7');
        ui_led_top_white(on);
        cmd_process();
        on = !on;
        //sleep();
    }
}

