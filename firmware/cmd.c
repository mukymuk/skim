#include "global.h"
#include "protocol.h"
#include "cmd.h"
#include "uart.h"
#include "crc.h"

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
static uint8_t *            s_rx_data;
static uint8_t              s_rx_data_ndx;
static uint8_t              s_rx_data_count;
static uint16_t             s_cmd_good_count;
static uint16_t             s_cmd_bad_count;

void cmd_init( void )
{
}

static uint8_t nak( enum skim_client_nak_code_t code )
{
    struct skim_client_nak_t * p_nak = &s_tx_buffer.nak;
    p_nak->code = code;
    return sizeof(struct skim_client_nak_t); 
}


static void get_version( void )
{
    struct skim_client_version_t * p_version = &s_tx_buffer.version;
    p_version->major = 0;
    p_version->minor = 1;
}

struct dispatch_table_t
{
    void (*p_func)(void);
    uint8_t length;
};

void cmd_process( void )
{
    uint8_t c;
    static const struct dispatch_table_t s_dispatch_table[] =
    {
        { get_version, sizeof(struct skim_client_version_t) }
    };
    if( !uart_rx(&c) )
    {
        if( uart_get_timeout() >= 10 )
        {
            // restart the state machine if there's more than 10ms between
            // characters
            s_state = state_rx_id;
        }
        return;
    }
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
            uint8_t length = s_dispatch_table[s_rx_buffer.hdr.id].length;
            if( length && c == length )
            {
                // validate length
                s_rx_data = (uint8_t*)&s_rx_buffer.hdr.crc;
                s_rx_buffer.hdr.length = c;
                s_rx_data_count = c + sizeof(s_rx_buffer.hdr.crc);
                s_state = state_rx_data;
                s_rx_data_ndx = 0;
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
            if( s_rx_data_count )
            {
                s_rx_data[s_rx_data_ndx++] = c;
                if( !--s_rx_data_count )
                {
                    // validate crc
                    if( crc16(&s_rx_buffer,s_rx_buffer.hdr.length) == s_rx_buffer.hdr.crc )
                    {
                        s_dispatch_table[s_rx_buffer.hdr.id].p_func();
                        s_cmd_good_count++;
                    }
                    else
                        s_cmd_bad_count++;
                    s_state = state_rx_id;
                }
            }
            break;
        }
        default:
            break;
    }
}