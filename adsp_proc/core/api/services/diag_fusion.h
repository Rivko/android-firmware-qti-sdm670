#ifndef DIAG_FUSION_H
#define DIAG_FUSION_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**==========================================================================

              DIAG_FUSION   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with lte modem.

  This file was AUTO GENERATED on: Wed May 25 2011

Copyright (c) 2002 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
/*=============================================================================
$Header: //components/rel/core.qdsp6/2.1/api/services/diag_fusion.h#1 $
=============================================================================*/

#include "diag.h"


/* Interfaces for the LTE modem */


/* Callback types for the functions */





/* Callback type diag_cmd_rsp */

  typedef void (*diag_cmd_rsp_fusion) (const byte *rsp, unsigned int length, 
                                void *param);


/*~ PARAM rsp VARRAY LENGTH length  */


  boolean diag_cmd_req_fusion (const byte *req, unsigned int length, 
                        const diag_cmd_rsp_fusion rsp_cb, void *param);
/*~ PARAM req VARRAY LENGTH length  */
/*~ FUNCTION diag_cmd_req_fusion
     CLNTNAME diag_cmd_req_fusion_on_modem  */



   #ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DIAG_FUSION_H */
