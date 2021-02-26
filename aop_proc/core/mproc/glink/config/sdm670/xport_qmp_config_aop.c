/*===========================================================================

            GLink QMP transport Configuration Structures

=============================================================================

  @file
    xport_qmp_config.c

    Contains structures to be used in Glink QMP trasnport configuration.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/mproc/glink/config/sdm670/xport_qmp_config_aop.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/17   vr      Initial version.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "xport_qmp_config.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

const xport_qmp_config_type xport_qmp_config[GLINK_NUM_REMOTE_HOSTS] =
{
  { 
    /* AOP->APSS */
      "apps",
      "aop_apps",             /* Logical Channel name    */
      1*1024,                 /* Mailbox region size     */    
      100+4,                  /* Local mailbox size      */
      0,
      2,                      /* Incoming interrupt      */
      0x01300000              /* Physical address        */
  },
  {
      /* AOP->CDSP */
      "cdsp",
      "aop_cdsp",             /* Logical Channel name    */
      1*1024,                 /* Mailbox region size     */    
      100+4,                  /* Local mailbox size      */
      0x12,
      18,                     /* Incoming interrupt      */
      0x01330000              /* Physical address        */
  },
  {
      /* AOP->ADSP */
      "slpi",
      "aop_slpi",             /* Logical Channel name    */
      1*1024,                 /* Mailbox region size     */    
      100+4,                  /* Local mailbox size      */
      0x8,
      8,                      /* Incoming interrupt      */
      0x01340000              /* Physical address        */
  },
  {
      /* AOP->MPSS */
      "mpss",
      "aop_mpss",             /* Logical Channel name    */
      1*1024,                 /* Mailbox region size     */    
      100+4,                  /* Local mailbox size      */
      0xA,
      10,                     /* Incoming interrupt      */
      0x01310000              /* Physical address        */
  }
};

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      xport_qmp_get_config
===========================================================================*/
/**
  Return the configuration (remote subsystem) of given index.

  @param[in]  ind    Index.

  @return   Configuration.
*/
/*=========================================================================*/
const xport_qmp_config_type* xport_qmp_get_config(uint32 ind)
{
  if (ind >= GLINK_NUM_REMOTE_HOSTS)
  {
    return NULL;
  }

  return &xport_qmp_config[ind];
}

/*===========================================================================
  FUNCTION      xport_qmp_alloc_mailbox
===========================================================================*/
/**
  Allocates memory for mailbox communication

  @param[in]  ind    Index.

  @return   Mailbox memory
*/
/*=========================================================================*/
void *xport_qmp_alloc_mailbox(uint32 ind)
{
  if (ind >= GLINK_NUM_REMOTE_HOSTS)
  {
    return NULL;
  }

  return (void *) (uint32)(xport_qmp_config[ind].mailbox_desc_start);
}

