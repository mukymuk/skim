
#include "protocol_config.h"

////////////////////////////////////////////////////////////////////////////////
// DATA TYPES

typedef uint8_t protocol_id_t;
typedef uint8_t protocol_length_t;
typedef uint16_t protocol_crc_t;


#ifdef PROTOCOL_ENABLE_SEQUENCE
typedef PROTOCOL_ENABLE_SEQUENCE protocol_sequence_t;
#endif    

////////////////////////////////////////////////////////////////////////////////
// PACKET COMMON

typedef struct _protocol_header_t
{
    protocol_length_t   length;
    protocol_id_t       id;
#ifdef PROTOCOL_ENABLE_SEQUENCE
    protocol_sequence_t sequence;
#endif    
}
protocol_header_t;

typedef struct _protocol_nodata_t
{
    protocol_header_t           hdr;
    protocol_crc_t              crc;
}
protocol_nodata_t;

////////////////////////////////////////////////////////////////////////////////
// ID'S

#define protocol_version_request_id    ((protocol_id_t)0)
#define protocol_set_gain_offset_id     ((protocol_id_t)1)

#define protocol_ack_id                 ((protocol_id_t)0x80)
#define protocol_version_response_id   ((protocol_id_t)0x81)

////////////////////////////////////////////////////////////////////////////////
// ACK/NAK PACKET

typedef struct _protocol_ack_t
{
    protocol_header_t           hdr;
    uint8_t                     code;
    protocol_crc_t              crc;
}
protocol_ack_t;

////////////////////////////////////////////////////////////////////////////////
// VERSION PACKET

typedef protocol_nodata_t   protocol_version_request_t;

typedef struct _protocol_version_response_t
{
    protocol_header_t               hdr;
    uint8_t                         major;
    uint8_t                         minor;
    protocol_crc_t                  crc;
}
protocol_version_response_t;

////////////////////////////////////////////////////////////////////////////////
// SET PRESSURE GAIN/OFFSET PACKET

typedef struct _protocol_set_gain_offset_t
{
    protocol_header_t               hdr;
    uint8_t                         channel;
    float32_t                       gain;
    float32_t                       offset;
    protocol_crc_t                  crc;
}
protocol_set_gain_offset_t;


#ifndef PROTOCOL_FUNCTION_DECL
#define PROTOCOL_FUNCTION_DECL
#endif

#define PROTOCOL_CREATE_DECLARE(f)	void PROTOCOL_FUNCTION_DECL f##_create( f##_t *)

PROTOCOL_CREATE_DECLARE(protocol_version_request);
PROTOCOL_CREATE_DECLARE(protocol_version_response);
PROTOCOL_CREATE_DECLARE(protocol_ack);
PROTOCOL_CREATE_DECLARE(protocol_set_gain_offset);

/*
typedef struct _protocol_dispatch_table_t
{
	protocol_id_t	id;
	bool(*p_func)(const protocol_header_t*);
}
protocol_dispatch_table_t;
*/