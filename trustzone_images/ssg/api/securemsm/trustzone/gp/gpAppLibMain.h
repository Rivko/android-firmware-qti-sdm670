#ifndef GPAPPLIBMAIN_H
#define GPAPPLIBMAIN_H

/*----------------------------------------------------------------------------
 * Include files
 * -------------------------------------------------------------------------*/
#include "tee_internal_api.h"

/*----------------------------------------------------------------------------
 * External Variable Declarations
 * -------------------------------------------------------------------------*/
extern const char* TA_APP_NAME;
extern const unsigned char TA_UUID[];

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
const char*
get_ta_app_name(
  void);

TEE_UUID
get_ta_uuid(
  void);

TEE_Identity
get_ta_identity(
  void);

#endif /* GPAPPLIBMAIN_H */
