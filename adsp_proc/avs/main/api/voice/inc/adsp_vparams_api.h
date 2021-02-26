#ifndef __ADSP_VPARAMS_API_H__
#define __ADSP_VPARAMS_API_H__

/**
  @file adsp_vparams_api.h
  @brief This file contains module and parameter IDs for calibration purposes.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Voice_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2010, 2012-2013, 2016-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MaODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/16   HB       Moved proprietary information in other file.
07/21/16   sw      (Tech Pubs) Merged Doxygen comments from 8953.2.7; edited
                   Doxygen comments for 8998.2.8.
02/25/13   sw      (Tech Pubs) Updated Doxygen for 2.0 internal draft.
02/27/12   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/11/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
===========================================================================*/

#include "adsp_vparams_api_prop.h"
#include "gain_calib.h"
#include "hpcm_calib.h"
#include "voice_param_mod_enable_calib.h"
#include "tx_stream_limiter_calib.h"
#include "vol_calib.h"



/** @ingroup cal_param_ids
    Enables or disables a module.

    @vertspace{6} @inputtable{Cal_ID_0x00010E00.tex}
*/
/** Supports generic parameters that are not tied to a specific
    algorithm module in the voice preprocessor on the Tx path.
    This module ID is also used as the default identifier for the host PCM.
*/
#define VOICEPROC_MODULE_TX          (0x00010EF6)

/** Supports generic parameters that are not tied to a specific
    algorithm module in the voice preprocessor on the Rx path.
    This module ID is also used as the default identifier for the host PCM.
*/
#define VOICEPROC_MODULE_RX          (0x00010F06)

/** Supports generic parameters that are not tied to a specific
    algorithm module in the voice Tx stream.
*/
#define VOICESTREAM_MODULE_TX              (0x00010F07)

/** Supports generic parameters that are not tied to a specific
    algorithm module in the voice Rx stream.
*/
#define VOICESTREAM_MODULE_RX              (0x00010F14)

#endif /* __ADSP_VPARAMS_API_H__ */
