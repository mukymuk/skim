#include <stdint.h>

#ifdef EXPORT_FCNS
#define EXPORTED_FUNCTION __declspec(dllexport)
#else
#define EXPORTED_FUNCTION __declspec(dllimport)
#endif

EXPORTED_FUNCTION void version_request( void * pv_dst, uint8_t length );
EXPORTED_FUNCTION double version_response( void * pv_dst, uint8_t length );

