#include "global.h"
#include "protocol.h"
#include "cmd.h"
#include "uart.h"
#include "crc.h"
#include "sx8724.h"

union rx_packet_buffer_t
{
    protocol_header_t               hdr;
    protocol_version_request_t      version_request;
    protocol_set_gain_offset_t      set_gain_offset;
};
union rx_packet_buffer_t s_rx_packet_buffer;

union tx_packet_buffer_t
{
    protocol_header_t               hdr;
    protocol_version_response_t     version_response;
    protocol_ack_t                  ack;
};
union tx_packet_buffer_t s_tx_packet_buffer;

static protocol_length_t    s_rx_ndx;
static protocol_length_t    s_tx_ndx;

typedef void(*dispatch_func_t)(void);

struct dispatch_table_t
{
    dispatch_func_t p_func;
    
};
static dispatch_func_t s_pending_dispatch;


void cmd_init( void )
{
}


static void set_gain_offset( void )
{
    sx8724_gain( s_rx_packet_buffer.set_gain_offset.channel, s_rx_packet_buffer.set_gain_offset.gain );
    sx8724_offset( s_rx_packet_buffer.set_gain_offset.channel, s_rx_packet_buffer.set_gain_offset.offset );
    s_tx_packet_buffer.ack.code = 1;
    protocol_ack_create(&s_tx_packet_buffer.ack);
}

static void get_version( void )
{
    s_tx_packet_buffer.version_response.major = 0;
    s_tx_packet_buffer.version_response.minor = 1;
    protocol_version_response_create(&s_tx_packet_buffer.version_response);
}

void cmd_process( void )
{
    static uint8_t * const s_p_rx = (uint8_t*)&s_rx_packet_buffer;
    
    static const struct dispatch_table_t s_dispatch_table[] =
    {
        get_version,     // protocol_version_request_id
        set_gain_offset, // protocol_set_gain_offset_id
        
    };
    static const uint8_t * const s_p_tx = (const uint8_t*)&s_tx_packet_buffer;
    bool tx_in_progress = s_tx_ndx < s_tx_packet_buffer.hdr.length;
    if( tx_in_progress )
    {
        // sending transmission packet to uart as uart tx buffers allow
        s_tx_ndx += uart_tx_buf( &s_p_tx[s_tx_ndx], s_tx_packet_buffer.hdr.length-s_tx_ndx );
    }
    if( !s_pending_dispatch && s_rx_ndx < sizeof(s_rx_packet_buffer) )
    {
        // collect bytes of incoming packet if there's not a dispatch
        // pending and there's enough room
        if( uart_break_detected() )
        {
            s_rx_ndx = 0;
        }
        uint8_t bytes_received = uart_rx_buf( &s_p_rx[s_rx_ndx], sizeof(s_rx_packet_buffer)-s_rx_ndx );
        if( bytes_received )
        {
            s_rx_ndx += bytes_received;
            if( s_rx_ndx >= sizeof(protocol_header_t) )
            {
                if( s_rx_packet_buffer.hdr.length == s_rx_ndx )
                {
                    // all of the packet has been received
                    protocol_crc_t crc = *(protocol_crc_t*)&s_p_rx[s_rx_ndx-sizeof(protocol_crc_t)];
                    if( crc16(&s_rx_packet_buffer,s_rx_packet_buffer.hdr.length-sizeof(protocol_crc_t)) == crc )
                    {
                        // crc pass
                        if( s_rx_packet_buffer.hdr.id < ARRAY_SIZE(s_dispatch_table) )
                        {
                            // looks like a valid command
                            s_pending_dispatch = s_dispatch_table[s_rx_packet_buffer.hdr.id].p_func;
                        }
                    }
                }
            }
        }
    }
    if( s_pending_dispatch && !tx_in_progress )
    {
        // transmission buffer is available to be used
        s_tx_ndx = 0;
        s_pending_dispatch();
        s_rx_ndx = 0;
        s_pending_dispatch = NULL;
        s_tx_ndx += uart_tx_buf( &s_p_tx[s_tx_ndx], s_tx_packet_buffer.hdr.length-s_tx_ndx );
    }
}