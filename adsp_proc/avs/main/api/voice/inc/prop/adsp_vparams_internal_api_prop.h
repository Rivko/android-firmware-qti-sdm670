#ifndef __ADSP_VPARAMS_INTERNAL_API_PROP_H__
#define __ADSP_VPARAMS_INTERNAL_API_PROP_H__

/**
  @file adsp_vparams_internal_api_prop.h
  @brief This file contains module and parameter IDs for calibration purposes.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The Voice_mainpage.dox file contains all file/group
      descriptions that are in the output PDF generated using Doxygen and
      Latex. To edit or update any of the file/group text in the PDF, edit
      the Voice_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================

  Copyright (c) 2010, 2012-2013, 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/14   sa      Created this file for internal GUIDs
08/17/16   HB      Renaming of file.
===========================================================================*/


/** facing theshold param Id. payload provided the facing angle threshold set in the algorithm.
    This is read-only paramId and FW need to register to sensor's with this threshold.
    The maximum array size for this parameter is 4 bytes.
    facing angle is used for registering with the sensors for getting evets when the
    holding angle crosses this threshold

    @inputtable{Calibration_ID_VOICE_PARAM_FACING_THRESHOLD.tex}
*/
#define VOICE_PARAM_FACING_THRESHOLD         (0x00010E52)

/** ParamId to set holding patter reported by accelerometer.
    This is write-only paramId and FW set the value to algorithm when it is changed.
    The maximum array size for this parameter is 4 bytes.
    facing angle is used for registering with the sensors for getting evets when the
    holding angle crosses this threshold

    @inputtable{Calibration_ID_VOICE_PARAM_HOLDING_PATTERN.tex}
*/
#define VOICE_PARAM_HOLDING_PATTERN         (0x00010E53)

/** Identifies FNS module in TX path. This module is invoked from SMECNS for non stationary noise
 * suppression
 */

#define VOICE_MODULE_TX_EANS            (0x00010F22)

/** This module ID identifies the LTETTY TX module.
    This module does not support any param Ids:
*/
#define VOICE_MODULE_LTETTY_TX               (0x00010F25)

/** This module ID identifies the LTETTY RX module.
    This module does not support any param Ids:
*/
#define VOICE_MODULE_LTETTY_RX               (0x00010F26)

/** This module ID identifies the CTM TX module.
    This module does not support any param Ids:
*/
#define VOICE_MODULE_CTM_TX                  (0x00010F27)

/** This module ID identifies the CTM RX module.
    This module does not support any param Ids:
*/
#define VOICE_MODULE_CTM_RX                  (0x00010F28)

/** This module ID identifies the TTY ENCODER module.
    This module supports following paramids :

    VOC_PARAM_TTY_VOC_MODE
    VOICE_PARAM_TTY_ENC
*/
#define VOICE_MODULE_TTY_ENC                 (0x00010F2C)

/** This module ID identifies the DTMF GENERATION module.
    This module supports following paramids :

    DTMF_GEN_PARAM_SET_DTMF_GENERATION
*/
#define VOICE_MODULE_DTMF_GEN                (0x00010F2D)

/** TTY internal params.
  * To be sent by vocoders */

enum TTY_CDMA_VOCODER_TYPE {
   TTY_CDMA_V13K = 0,
   TTY_CDMA_EVRC,
   TTY_CDMA_FOURGV
};

/**  Parameter that identifies the 1x vocoder for which TTY is being initialized.
     the 1x vocoders use the param to intialize TTY.
     Values supported are defined by enum TTY_CDMA_VOCODER_TYPE
     Associated data structure: voc_param_tty_voc_mode_t */
#define VOC_PARAM_TTY_VOC_MODE               (0x00010E90)

/**< TTY mode based on voc_tty_modes_t */
typedef struct voc_param_tty_voc_mode_t voc_param_tty_voc_mode_t;

struct voc_param_tty_voc_mode_t
{
   uint32_t voc_type;
};

/**  Parameter used to get the tty struct pointer from TTY by the vocoders
     Associated data structure: voc_param_tty_enc_struct_t */
#define VOICE_PARAM_TTY_ENC                   (0x00010E91)

typedef struct voc_param_tty_enc_struct_t voc_param_tty_enc_struct_t;

struct voc_param_tty_enc_struct_t
{
   void  *tty_enc_struct_ptr;
};

#endif /* __ADSP_VPARAMS_INTERNAL_API_PROP_H__ */
