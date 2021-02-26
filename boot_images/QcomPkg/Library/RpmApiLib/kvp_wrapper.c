/*
===========================================================================
*/
/**
  @file RPM protocol Wrapper for KVP functions

  The file contains the KVP API wrapper functions.
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
      Type Definitions
==========================================================================*/


/*=========================================================================
      Variables
==========================================================================*/
extern EFI_RPM_PROTOCOL *gRpmProtocol;

/* =========================================================================
**  Function : kvp_create
** =========================================================================*/
/*
  RPM Protocol wrapper function for kvp_create
*/
kvp_t *kvp_create(unsigned buffer_size)
{
   kvp_t* msg = NULL;
     if(gRpmProtocol) 
     {
       gRpmProtocol->KvpCreate(buffer_size, &msg);
     }
     else
     {
       DEBUG ((EFI_D_ERROR, "kvp_create:Rpm protocol is not initialized.\n"));
     }
  return msg;
}

/* =========================================================================
**  Function : kvp_destroy
** =========================================================================*/
/*
  RPM Protocol wrapper function for kvp_destroy
*/
void kvp_destroy(kvp_t *self)
{
     if(gRpmProtocol) 
     {
       gRpmProtocol->KvpDestroy(self);
     }
     else
     {
       DEBUG ((EFI_D_ERROR, "kvp_destroy:Rpm protocol is not initialized.\n"));
     }
}

/* =========================================================================
**  Function : kvp_put
** =========================================================================*/
/*
  RPM Protocol wrapper function for kvp_put
*/
void kvp_put(kvp_t *self, unsigned  type, unsigned  length, const void  *value)
{
     if(gRpmProtocol) 
     {
       gRpmProtocol->KvpPut(self, type, length, value);
     }
     else
     {
       DEBUG ((EFI_D_ERROR, "kvp_put:Rpm protocol is not initialized.\n"));
     }
}

/* =========================================================================
**  Function : kvp_clear
** =========================================================================*/
/*
  RPM Protocol wrapper function for kvp_clear
*/
void kvp_clear(kvp_t *self)
{
     if(gRpmProtocol) 
     {
       gRpmProtocol->KvpClear(self);
     }
     else
     {
       DEBUG ((EFI_D_ERROR, "kvp_clear:Rpm protocol is not initialized.\n"));
     }
}

