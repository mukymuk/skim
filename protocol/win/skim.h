#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef SKIM_BUILD_DLL
#define SKIM_API __declspec(dllexport)
#else
#define SKIM_API __declspec(dllimport)
#endif

	void SKIM_API version_request( void *pv, size_t * p_req_size, size_t * p_resp_size);
	size_t SKIM_API get_maximum_packet_size(void);
	bool SKIM_API version_response(void *pv_packet, float *p_version);

#ifdef __cplusplus
};
#endif
