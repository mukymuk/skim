#include "protocol.h"
#include "crc.h"

#define INIT_HDR(t,p)	(p)->hdr.id = t##_id; (p)->hdr.length = sizeof(t##_t); (p)->crc = crc16(p, sizeof(t##_t) - sizeof(uint16_t) )
#define PROTOCOL_CREATE(f)	void PROTOCOL_FUNCTION_DECL f##_create( f##_t *p) { INIT_HDR(f,p); }


PROTOCOL_CREATE(protocol_version_request);
PROTOCOL_CREATE(protocol_version_response);
PROTOCOL_CREATE(protocol_ack);
PROTOCOL_CREATE(protocol_set_gain_offset);

/*
extern const protocol_dispatch_table_t * const g_protocol_dispatch_table;

bool protocol_dispatch(void *pv_packet)
{
	bool ret = false;
	const protocol_header_t *p_hdr = (const protocol_header_t*)pv_packet;
	const uint16_t *p_crc = (const uint16_t *)&((const uint8_t*)pv_packet)[p_hdr->length];
	if (*p_crc == crc16(pv_packet, p_hdr->length - sizeof(uint16_t)))
	{
		const protocol_dispatch_table_t * p = g_protocol_dispatch_table;
		while (p->p_func)
		{
			if (p->id == p_hdr->id)
			{
				ret = p->p_func(p_hdr);
				break;
			}
			p++;
		}
		ret = true;
	}
	return ret;
}
*/