#include "global.h"

#if( BUILD_TARGET == skim )
#include "uart.h"
static bit s_escaping;

#define SERDES_TX(a)  uart_tx(a)
#else

#endif

#include "serdes.h"

#define SERDES_SOF          0x55
#define SERDES_EOF          0xAA

static const uint8_t s_esc[] = {  0x87, 0xdd, 0xdc, 0x10, 0x35, 0xbc, 0x5c, 0xb6, 0xca, 0x0a };
static uint8_t s_tx_esc_ndx;

void serdes_init(void)
{
}

void serdes_sof( void )
{
    SERDES_TX( SERDES_SOF );
    s_tx_esc_ndx = 0;
    SERDES_TX( s_esc[s_tx_esc_ndx] );
    s_escaping = false;
}

void serdes_eof( void )
{
    SERDES_TX( SERDES_EOF );
}

void serdes_serialize( const void *pv_data, uint8_t length )
{
    const uint8_t *p_data = (const uint8_t*)pv_data;
    uint8_t i;
    uint8_t c;
    for(i=0;i<length;i++)
    {
        c = p_data[i];
        if( c == s_esc[s_tx_esc_ndx] || c == SERDES_SOF || c == SERDES_EOF )
        {
            SERDES_TX( s_esc[s_tx_esc_ndx++] );
            if(s_tx_esc_ndx >= ARRAY_SIZE(s_esc))
                s_tx_esc_ndx = 0;
        }
        SERDES_TX( c );
    }
}

void serdes_deserialize( void *pv_data, uint8_t length )
{
    
}

