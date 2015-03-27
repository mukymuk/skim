#include "global.h"

#define TX_BUFFER_SIZE	128

static uint8_t s_tx_buffer[TX_BUFFER_SIZE];
static volatile uint8_t s_tx_write_ndx = 0;
static volatile uint8_t s_tx_read_ndx = 0;
static volatile uint8_t s_tx_count = 0;

void uart_init( void )
{
    PORTGbits.RG1 = 1;
    TRISGbits.TRISG1 = 0;
    TRISGbits.TRISG2 = 1;
    RCSTA2 = 0b10001000;	// enable serial port
    BAUDCON2 = 0b00001000;
    SPBRGH2 = 0;
    SPBRG2 = 68;                // BAUDRATE = 2E6/(SPBRG2 + 1) = ~57600
    TXSTA2 = 0b00000100;	// asynchronous mode
    //PIE3bits.TX2IE = 1;
}

void uart_putchar( const char c )
{
    while(  s_tx_count >= TX_BUFFER_SIZE );
    PIE3bits.TX2IE = 0;
    s_tx_buffer[s_tx_write_ndx++] = c;
    if( s_tx_write_ndx >= TX_BUFFER_SIZE )
    {
        s_tx_write_ndx = 0;
    }
    s_tx_count++;
    TXSTA2bits.TXEN = 1;
    PIE3bits.TX2IE = 1;
}

void uart_tx( const void * pv_data, uint8_t length )
{
    uint8_t len = length;
    const uint8_t *p_data = (const uint8_t *)pv_data;
    uint8_t * p;
    while( length > TX_BUFFER_SIZE-s_tx_count );
    PIE3bits.TX2IE = 0;
    p = &s_tx_buffer[ s_tx_write_ndx ];
    while( len )
    {
        *p = *p_data;
        len--;
        if( ++s_tx_write_ndx <  TX_BUFFER_SIZE )
        {
            p++;
        }
        else
        {
            s_tx_write_ndx = 0;
            p = &s_tx_buffer[0];
        }
        p_data++;
    }
    if( !s_tx_count )
    {
        TXSTA2bits.TXEN = 1;
    }
    s_tx_count += length;
    PIE3bits.TX2IE = 1;
}

void uart_isr( void )
{
    if( PIR3bits.TX2IF )
    {
        if( s_tx_count )
        {
            TXREG2 = s_tx_buffer[ s_tx_read_ndx++ ];
            if( s_tx_read_ndx >= TX_BUFFER_SIZE )
            {
                s_tx_read_ndx = 0;
            }
            s_tx_count--;
        }
        else
        {
            TXSTA2bits.TXEN = 0;
       }
    }
}
