#include "global.h"
#include <stdlib.h>

#define TX_BUFFER_SIZE	128
#define RX_BUFFER_SIZE	128

static uint8_t s_tx_buffer[TX_BUFFER_SIZE];

static volatile uint8_t s_tx_write_ndx = 0;
static volatile uint8_t s_tx_read_ndx = 0;
static volatile uint8_t s_tx_count = 0;

static uint8_t s_rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t s_rx_write_ndx = 0;
static volatile uint8_t s_rx_read_ndx = 0;
static volatile uint8_t s_rx_count = 0;

static uint8_t  s_rx_overflow;

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

void uart_tx_buf( const void * pv_data, uint8_t length )
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
        if( ++s_tx_write_ndx < TX_BUFFER_SIZE )
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

void uart_tx( uint8_t data )
{
    if( s_tx_count >= TX_BUFFER_SIZE )
        while(1);   // tx overflow is a fatal condition
    PIE3bits.TX2IE = 0;
    s_tx_buffer[ s_tx_write_ndx++ ] = data;
    if( s_tx_write_ndx >= TX_BUFFER_SIZE )
        s_tx_write_ndx = 0;
    if( !s_tx_count )
    {
        TXSTA2bits.TXEN = 1;
    }
    s_tx_count++;
    PIE3bits.TX2IE = 1;
}

bool uart_rx( uint8_t *p_data )
{
    bool ret;
    PIE3bits.RC2IE = 0;
    if( s_rx_count )
    {
        *p_data = s_rx_buffer[ s_rx_read_ndx++ ];
        if( s_rx_read_ndx >= RX_BUFFER_SIZE )
        {
            s_rx_read_ndx = 0;
        }
        s_rx_count--;
        ret = true;
    }
    else
    {
        ret = false;
    }
    PIE3bits.RC2IE = 1;
    return ret;
}

uint8_t uart_rx_buf( void * pv_msg, uint8_t max_length )
{
    uint8_t length;
    uint8_t * p_msg = (uint8_t*)pv_msg;
    if( s_rx_count )
    {
        uint8_t i;
        PIE3bits.RC2IE = 0;
        length = min(max_length,s_rx_count);
        for(i=0;i<length;i++)
        {
            p_msg[i] = s_rx_buffer[ s_rx_read_ndx++ ];
            if( s_rx_read_ndx >= RX_BUFFER_SIZE )
            {
                s_rx_read_ndx = 0;
            }
        }
        s_rx_count -= length;
        PIE3bits.RC2IE = 1;
    }
    else
    {
        length = 0;
    }
    return length;
}


void uart_isr( void )
{
    if( PIR3bits.RC2IF )
    {
        uint8_t c = RCREG2;
        if( s_rx_count < RX_BUFFER_SIZE )
        {
            s_rx_buffer[ s_rx_write_ndx++ ] = c;
            s_rx_count++;
            if( s_rx_write_ndx >= RX_BUFFER_SIZE )
                s_rx_write_ndx = 0;
        }
        else
        {
            s_rx_overflow++;
        }
    }
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
