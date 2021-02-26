/*=============================================================================

                err_qdi_client.c

GENERAL DESCRIPTION
      ERR QDI Client Process Code that goes to main image

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2014 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_qdi_client.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/31/14   din     Added qdi calls for err on User PD.
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "err_qdi_client.h"
#include "erri.h"
#include "err_policy_pdr.h"

static int err_qdi_client_handle = -1;
extern boolean err_qdi_client_initialized;
extern void err_user_register_with_root(void);

/*===========================================================================

FUNCTION    ERR_QDI_CLIENT_INIT

DESCRIPTION
  Initialize the err client qdi service.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL void err_qdi_client_init(void) 
{
 
   err_qdi_client_handle = qurt_qdi_open(ERR_DRIVER_NAME);
   
   if(err_qdi_client_handle < 0)
   {
     ERR_FATAL("err_qdi_client_init :qdi_open failed\n", 0, 0, 0);
     return;
   }

   err_qdi_client_initialized = TRUE;

   err_qdi_invoke_send_pdr_info(err_policy_pdr_is_pdr_allowed());

   err_user_register_with_root();

}  /* err_qdi_client_init */


/*===========================================================================

FUNCTION    ERR_QDI_INVOKE_SEND_PDR_INFO

DESCRIPTION
  Sends Pid info after initialization.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/

err_qdi_e err_qdi_invoke_send_pdr_info
(
  boolean                       user_pdr_enabled
)
{
  return qurt_qdi_handle_invoke(err_qdi_client_handle, ERR_QDI_SEND_PDR_INFO,
		                user_pdr_enabled);

} /* err_qdi_invoke_send_pid_info*/

/*===========================================================================

FUNCTION    ERR_QDI_CLIENT_SEND_PDR_INFO

DESCRIPTION
  Initialize the err client qdi service.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void err_qdi_client_send_pdr_info
(
  boolean                       user_pdr_enabled
)
{
  if ( err_qdi_client_is_initialized())
    err_qdi_invoke_send_pdr_info(user_pdr_enabled);

}  /* err_qdi_client_send_pdr_info */

/*===========================================================================

FUNCTION    ERR_QDI_INVOKE_INFORM_PFR

DESCRIPTION
  Sends PFR after a crash . Sends the info in expception handler.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/

err_qdi_e err_qdi_invoke_inform_pfr
(
  uint32                        pfr_len,
  char *                        pfr
)
{
  return qurt_qdi_handle_invoke(err_qdi_client_handle, ERR_QDI_INFORM_PFR, pfr_len, pfr );

} /* err_qdi_invoke_inform_pfr */

/*===========================================================================

FUNCTION    ERR_QDI_INVOKE_ERR_HANDLING_DONE

DESCRIPTIONT
  Tells QDI on ROOT PD that err completion routine on User PD has completed.
  Root PD checks if PD restart is enabled. If so, it returns. Otherwise, 
  it brings the system down.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/

err_qdi_e err_qdi_invoke_err_handling_done(void)
{
  return qurt_qdi_handle_invoke(err_qdi_client_handle, ERR_QDI_ERR_HANDLING_DONE);

} /* err_qdi_invoke_err_handling_done */


/*===========================================================================

FUNCTION    ERR_QDI_INVOKE_COMMUNICATE_USER_CB

DESCRIPTIONT
  Sends the user Process callback info to user PD.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/

err_qdi_e err_qdi_invoke_communicate_user_cb
(
  void *        pre_STM_cb,
  void *        post_STM_cb,
  void *        normal_cb,
  void *        late_cb,
  void *        last_cb,
  void *        stack_addr, 
  uint32        stack_size
)
{
  return qurt_qdi_handle_invoke(err_qdi_client_handle, ERR_QDI_COMMUNICATE_USER_CB, 
                                pre_STM_cb, post_STM_cb, normal_cb, late_cb, last_cb, stack_addr, stack_size);

} /* err_qdi_invoke_communicate_user_cb */
