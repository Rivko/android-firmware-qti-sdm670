#ifndef __VSS_PARAMS_PRIVATE_IF_H__
#define __VSS_PARAMS_PRIVATE_IF_H__

/*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/vss_params_private_if.h#1 $
   $Author: pwbldsvc $
*/

#include "apr_comdef.h"

/****************************************************************************
 * CVD MODULE ID DEFINES                                                    *
 ****************************************************************************/

/**
  Sound device module ID.

  Parameters for this module ID are:
  #VSS_PARAM_RX_SOUND_DEVICE_ID
  #VSS_PARAM_TX_SOUND_DEVICE_ID
*/
#define VSS_MODULE_SOUND_DEVICE ( 0x000130B0 )


/**
  Parameter for specifying the Rx logical sound device ID.

  @comments
  The parameter data is 32 bit value.
*/
#define VSS_PARAM_RX_SOUND_DEVICE_ID ( 0x000130B1 )


/**
  Parameter for specifying the Tx logical sound device ID.

  @comments
  The parameter data is 32 bit value.
*/
#define VSS_PARAM_TX_SOUND_DEVICE_ID ( 0x000130B5 )

#endif /* __VSS_PARAMS_PRIVATE_IF_H__ */

