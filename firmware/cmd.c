#include "global.h"
#include "protocol.h"
#include "cmd.h"
#include <stdarg.h>

static union skim_rsp_t s_response_buffer;

void cmd_init( void )
{
}

static uint8_t nak( enum skim_rsp_nak_t code )
{
    struct skim_rsp_nak_t * p_nak = &s_response_buffer.nak;
    p_nak->code = code;
    return sizeof(struct skim_rsp_nak_t); 
}

static uint8_t get_version( const void *pv_args )
{
    struct skim_rsp_version_t * p_version = &s_response_buffer.version;
    p_version->major = 0;
    p_version->minor = 1;
    return sizeof(struct skim_rsp_version_t);
}

static uint8_t get_gain_offset( const void *pv_args )
{
}


uint8_t cmd_dispatch( const void *pv_cmd )
{
    static uint8_t (* const s_dispatch_table[])(const void *) =
    {
        get_version,        // skim_cmd_id_version
        get_gain_offset,    // skim_cmd_id_gain
    };
    uint8_t response_len;
    const uint8_t * p_cmd = (const uint8_t *)pv_cmd;
    uint8_t cmd_id = *p_cmd;
    if( cmd_id < ARRAY_SIZE(s_dispatch_table) )
    {
        response_len = s_dispatch_table[cmd_id](&p_cmd[1]);
    }
    else
    {
        response_len = nak(skim_rsp_nak_unknown_command);
    }
    return response_len;
}
