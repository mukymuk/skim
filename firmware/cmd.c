#include "global.h"
#include "protocol.h"
#include "cmd.h"
#include "uart.h"


enum state_t
{
    state_idle,
    state_rx_id,
    state_rx_length,
    state_rx_data,
    state_rx_crc
};

static enum state_t         s_state;
static union skim_host_t    s_rx_buffer;
static union skim_client_t  s_tx_buffer;

void cmd_init( void )
{
}

static uint8_t nak( enum skim_client_nak_code_t code )
{
    struct skim_client_nak_t * p_nak = &s_tx_buffer.nak;
    p_nak->code = code;
    return sizeof(struct skim_client_nak_t); 
}


static uint8_t get_version( void )
{
    struct skim_client_version_t * p_version = &s_tx_buffer.version;
    p_version->major = 0;
    p_version->minor = 1;
    return sizeof(struct skim_client_version_t);
}

struct dispatch_table_t
{
    uint8_t (*p_func)(void);
    uint8_t length;
};

void cmd_process( void )
{
    static const struct dispatch_table_t s_dispatch_table[] =
    {
        { get_version, sizeof(struct skim_client_version_t) }
    };
    uint8_t ret, c;
    if( !uart_rx(&c) )
        return;
 redo:
    switch( s_state )
    {
        case state_rx_id:
        {
            if( c < ARRAY_SIZE(s_dispatch_table))
            {
                // valid dispatch id
                s_state = state_rx_length;
                s_rx_buffer.hdr.id = (enum skim_host_id_t)c;
            }
            break;
        }
        case state_rx_length:
        {
            if( c == s_dispatch_table[s_rx_buffer.hdr.id].length )
            {
                // validate length
                s_state = state_rx_data;
            }
            else
            {
                s_state = state_rx_id;
                goto redo;
            }
            break;
        }
        case state_rx_data:
        {
            break;
        }
        case state_rx_crc:
        {
            break;
        }
        default:
            break;
    }

}