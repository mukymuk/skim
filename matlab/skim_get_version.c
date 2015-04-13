#include "mex.h"
#include <stdint.h>
#include "..\protocol\protocol.h"
#include "..\protocol\crc.c"

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (nrhs != 2) 
	{
		mexErrMsgIdAndTxt("skim:skim_get_version:nrhs", "Two inputs required.");
	}
	if (nlhs != 1)
	{
		mexErrMsgIdAndTxt("skim:skim_get_version:nlhs", "One output required.");
	}
	protocol_version_request_t version_request;
	version_request.hdr.id = PROTOCOL_ID_VERSION;
	version_request.hdr.length = sizeof(version_request);
	version_request.crc = 0;
	version_request.crc = crc16(&version_request,sizeof(version_request));

	//mxCreateDoubleScalar
}