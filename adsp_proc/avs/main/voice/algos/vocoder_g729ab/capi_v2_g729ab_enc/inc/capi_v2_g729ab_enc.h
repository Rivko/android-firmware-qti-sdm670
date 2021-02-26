#ifndef CAPI_V2_G729AB_ENC_H_
#define CAPI_V2_G729AB_ENC_H_



/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "Elite_CAPI_V2.h"


#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus


/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

capi_v2_err_t capi_v2_g729ab_enc_get_static_properties (
   capi_v2_proplist_t *init_set_properties,
   capi_v2_proplist_t *static_properties);


capi_v2_err_t capi_v2_g729ab_enc_init (
   capi_v2_t*              _pif,
   capi_v2_proplist_t      *init_set_properties);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_G729AB_ENC_H_ */
