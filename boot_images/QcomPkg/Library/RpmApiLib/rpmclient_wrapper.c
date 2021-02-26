/*
===========================================================================
*/
/**
  @file RPM protocol Wrapper for RPM client API

  The file contains the RPM client API wrapper functions.
*/
/*
  ====================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  07/27/15   rli      Initial revision.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

/*
 * External header files.
 */
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIRpm.h>

/*=========================================================================
      Variables
==========================================================================*/
extern EFI_RPM_PROTOCOL *gRpmProtocol;

/* =========================================================================
**  Function : rpm_post_request
** =========================================================================*/
/*
  RPM Protocol wrapper function for rpm_post_request
*/
unsigned rpm_post_request(rpm_set_type set, rpm_resource_type resource, unsigned id, kvp_t *kvps)
{
   unsigned message_id = 0;
     if(gRpmProtocol) 
     {
       gRpmProtocol->RpmPostRequest(set, resource, id, kvps, &message_id);
     }
     else
     {
       DEBUG ((EFI_D_ERROR, "kvp_create:Rpm protocol is not initialized.\n"));
     }
  return message_id;
}

/* =========================================================================
**  Function : rpm_post_request
** =========================================================================*/
/*
  RPM Protocol wrapper function for rpm_post_request
*/
void rpm_churn_queue(unsigned message_id)
{
     if(gRpmProtocol) 
     {
       gRpmProtocol->RpmChurnQueue( message_id );
     }
     else
     {
       DEBUG ((EFI_D_ERROR, "kvp_create:Rpm protocol is not initialized.\n"));
     }
}
