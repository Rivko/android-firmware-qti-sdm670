/*=============================================================================

                err_qdi_client.h  --  Header File

GENERAL DESCRIPTION
      ERR QDI Layer Header File

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2007 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_qdi_client.h#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/31/14   din      Added qdi calls for err on User PD.
=============================================================================*/
#ifndef ERR_QDI_CLIENT_H
#define ERR_QDI_CLIENT_H

#include "err_qdi.h"

/*==============================================================================

                           EXTERN FUNCTIONS

=============================================================================*/

/*===========================================================================

FUNCTION    ERR_QDI_CLIENT_IS_INITIALIZED

DESCRIPTION
  Initialize the err client qdi service.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE
  
SIDE EFFECTS
  None

===========================================================================*/
boolean err_qdi_client_is_initialized(void);

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
);

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
);

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

err_qdi_e err_qdi_invoke_err_handling_done(void);

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
);

#endif /* ERR_QDI_CLIENT_H */
