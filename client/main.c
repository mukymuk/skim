#include <stdint.h>
#include "protocol.h"
#include "skimcom.h"
#include "crc.h"

EXPORTED_FUNCTION void version_request( void * pv_dst, uint8_t length )
{
	protocol_version_request_t packet;
	packet.hdr.id = PROTOCOL_ID_VERSION;
	packet.hdr.length = sizeof(packet);
	packet.crc = crc16(&packet,sizeof(packet)-sizeof(uint16_t));
}

EXPORTED_FUNCTION double version_response( void * pv_dst, uint8_t length )
{
	return 0;
}
