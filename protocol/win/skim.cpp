#include "stdafx.h"

extern "C"
{
#define PROTOCOL_FUNCTION_DECL inline
#pragma pack(1)
#include "protocol.h"
#pragma pack()
#define SKIM_BUILD_DLL
#include "skim.h"
};

typedef union _protocol_packet_buffer_t
{
	protocol_header_t			hdr;
	protocol_version_response_t	version_response;
}
protocol_packet_buffer_t;

#define CAST(t,p,v)	t * p = (t*)v

/*
extern "C" const protocol_dispatch_table_t g_protocol_dispatch_table[] =
{
	{ protocol_version_response_id, NULL }
};

*/

void SKIM_API version_request(void *pv, size_t * p_req_size, size_t * p_resp_size )
{
	if (pv)
	{
		CAST(protocol_version_request_t, p, pv);
		protocol_version_request_create(p);
	}
	if ( p_req_size )
		*p_req_size = sizeof(protocol_version_request_t);
	if (p_resp_size)
		*p_resp_size = sizeof(protocol_version_response_t);
}

bool SKIM_API version_response(void *pv_packet, float *p_version)
{
	bool ret = false;
	CAST(protocol_version_response_t, p, pv_packet);
	if (p->hdr.id == protocol_version_response_id)
	{
		if ( p_version )
			*p_version = (float)p->major + (float)p->minor / (float)100.0;
		ret = true;
	}
	return ret;
}

size_t SKIM_API get_maximum_packet_size(void)
{
	return sizeof(protocol_packet_buffer_t);
}