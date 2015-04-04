
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
    protocol_id_t       id;
    protocol_length_t   length;
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

#define PROTOCOL_ID_VERSION    ((protocol_id_t)0)

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
