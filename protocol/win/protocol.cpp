#include "stdafx.h"
#include "protocol.h"
#include "crc.h"

void APIENTRY protocol_version_request_create(protocol_version_request_t *p)
{
	p->hdr.id = protocol_version_request_id;
	p->hdr.length = sizeof(protocol_version_request_t);
	p->crc = crc16(p, sizeof(protocol_version_request_t) - sizeof(uint16_t));
}

