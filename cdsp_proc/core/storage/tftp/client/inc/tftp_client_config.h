/***********************************************************************
 * tftp_client_config.h
 *
 * Short description.
 * Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/client/inc/tftp_client_config.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-05-31   rp    Port to SLPI image.
2014-12-05   dks   Target compilation and audio compilation fixes.
2014-06-04   rp    Switch to IPCRouter sockets.
2013-12-26   rp    Add tftp-client module.
2013-11-14   rp    Create

===========================================================================*/

#ifndef __TFTP_CLIENT_CONFIG_H__
#define __TFTP_CLIENT_CONFIG_H__

#include "tftp_config_i.h"

#ifndef TFTP_CLIENT_MAX_CLIENTS
  #define TFTP_CLIENT_MAX_CLIENTS    128
#endif

#ifdef TFTP_CLIENT_SERVER_INSTANCE_ID
  #error "TFTP Config Error : Below code should use this define."
#endif

#if defined (TFTP_MSM_BUILD)
  #if defined (TFTP_MPSS_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MSM_MPSS
  #elif defined (TFTP_ADSP_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MSM_ADSP
  #elif defined (TFTP_SLPI_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MSM_SLPI
  #endif
#elif defined (TFTP_MDM_BUILD)
  #if defined (TFTP_MPSS_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MDM_MPSS
  #elif defined (TFTP_ADSP_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MDM_ADSP
  #elif defined (TFTP_SLPI_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MDM_SLPI
  #elif defined (TFTP_TN_BUILD)
    #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_MDM_TN
  #endif
#elif defined (TFTP_GSS_BUILD)
  #define TFTP_CLIENT_SERVER_INSTANCE_ID TFTP_SERVER_INSTANCE_ID_APQ_GSS
#endif

#ifndef TFTP_CLIENT_SERVER_INSTANCE_ID
  #error "TFTP Config Error : Configure build type."
#endif

#endif /* __TFTP_CLIENT_CONFIG_H__ */
