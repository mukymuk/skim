#include "stdafx.h"
#include "protocol.h"
#define EXPORT	__declspec(dllexport)
#include "skim.h"
#include "mex.h"


void EXPORT request_version(void)
{
	//mexErrMsgIdAndTxt("skim:err", "ERROR");
	protocol_version_request_t ver;
	ver.hdr.id = PROTOCOL_ID_VERSION;
	ver.hdr.length = sizeof(ver);

}
