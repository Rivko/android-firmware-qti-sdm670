#ifndef ELITE_FWK_EXTNS_CODEC_INTERRUPT_H
#define ELITE_FWK_EXTNS_CODEC_INTERRUPT_H

/* ======================================================================== */
/**
@file Elite_fwk_extns_codec_interrupt.h

@brief frame work extensions for codec interrupts header file

  This file defines a framework extensions and corresponding private propeties
  needed for codec interrupts needed by modules.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/* =========================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
  ========================================================================== */
/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   11/23/14   sw      (Tech Pubs) Edited Doxygen markup/comments for 2.6.
   08/08/14   mk      Initial Version.
   ========================================================================= */

/** @cond OEM_only */

/** @weakgroup weakf_capiv2_ext_codec_interrupt_intro

The #FWK_EXTN_CDC_INTERRUPT framework extension enables support for codec
interrupt handling in CAPIv2 modules. This framework provides a utility used to
register and monitor the interrupts needed by the CAPIv2 modules. The Elite
framework reads the codec registers for interrupt-related information and
provides that information to the module.

@latexonly \subsection{Process Flow}

Figure \ref{fig:FrmwkExtCodecInt} illustrates the process flow for the Codec
Interrupt framework extension.

@endlatexonly

@fig_in_hdr{0.95,frmwk_ext_codec_interrupt_process_flow,Process flow for the Codec Interrupt framework extension,FrmwkExtCodecInt}

 -# The custom module uses #CAPI_V2_PARAM_ID_NUM_NEEDED_CDC_INT and 
    #CAPI_V2_PARAM_ID_CDC_INT_LIST to send codec interrupt information to 
    the Elite Framework.
 -# The Elite framework monitors the interrupts and uses codec 
    interrupt-related parameter IDs (see #CAPI_V2_PARAM_ID_CDC_INT_LIST) 
    to inform the custom module when interrupts are raised.
*/


#include "Elite_CAPI_V2_types.h"

/** @addtogroup capiv2_ext_codec_int
@{ */

/** Unique identifier used to represent a custom framework extension for
  codec interrupts.

  This extension supports the following parameters:
  - #CAPI_V2_PARAM_ID_NUM_NEEDED_CDC_INT
  - #CAPI_V2_PARAM_ID_CDC_INT_LIST @newpage
 */
#define FWK_EXTN_CDC_INTERRUPT 0x00010279

/** ID of the parameter used by a module to inform the Elite framework of
  the number of codec interrupts required by the module.

  @msgpayload{capi_v2_num_needed_cdc_int_t}
  @table{weak__capi__v2__num__needed__cdc__int__t}

  @par Example
  Caller:
  @code
  capi_v2_buf_t buf;
  capi_v2_num_needed_cdc_int_t num_cdc_int;

  buf.data_ptr = (int8_t *)&num_cdc_int.num_cdc_int;
  buf.max_data_len = sizeof(capi_v2_num_needed_cdc_int_t);

  @endcode
  @par
  Callee:
  @code
  capi_v2_cdc_int_t *cfg_ptr = (capi_v2_num_needed_cdc_int_t *)params_ptr->data_ptr;

  uint32_t cfg_size = sizeof(capi_v2_num_needed_cdc_int_t);

  cfg_ptr->num_cdc_int = 4;

  params_ptr->actual_data_len = cfg_size;
  @endcode @newpage
 */
#define CAPI_V2_PARAM_ID_NUM_NEEDED_CDC_INT 0x0001027A

/** ID of the parameter used by the module to inform the Elite framework of
  the list of codec interrupts to be monitored through this parameter.

  @msgpayload{capi_v2_cdc_int_t}
  @table{weak__capi__v2__cdc__int__t}

  @par Example
  Caller:
  @code
  capi_v2_buf_t buf;
  capi_v2_cdc_int_t *cdc_int_ptr;
  capi_v2_num_needed_cdc_int_t num_cdc_int;

  buf.data_ptr = (int8_t *)cdc_int_ptr;
  buf.max_data_len = sizeof(capi_v2_cdc_int_t) * num_cdc_int.num_cdc_int;

  @endcode
  @par
  Callee:
  @code
  capi_v2_num_needed_cdc_int_t num_cdc_int;
  capi_v2_cdc_int_t *cfg_ptr = (capi_v2_cdc_int_t *)params_ptr->data_ptr;

  uint32_t cfg_size = sizeof(capi_v2_cdc_int_t) * num_cdc_int.num_cdc_int;

  cfg_ptr[0].cdc_int = (uint32_t)CAPI_V2_PARAM_ID_CDC_INT_SPKR_AUDIO_CLIP;
  cfg_ptr[1].cdc_int = (uint32_t)CAPI_V2_PARAM_ID_CDC_INT_SPKR2_AUDIO_CLIP;
  cfg_ptr[2].cdc_int = (uint32_t)CAPI_V2_PARAM_ID_CDC_INT_VBAT_ATTACK;
  cfg_ptr[3].cdc_int = (uint32_t)CAPI_V2_PARAM_ID_CDC_INT_VBAT_RELEASE;

  params_ptr->actual_data_len = cfg_size;
  @endcode
 */
#define CAPI_V2_PARAM_ID_CDC_INT_LIST 0x0001027B

/** ID of the Microphone Activity Detection (MAD) audio interrupt. */
#define CAPI_V2_PARAM_ID_CDC_INT_MAD_AUDIO 0x0001027C

/** ID of the MAD beacon interrupt. */
#define CAPI_V2_PARAM_ID_CDC_INT_MAD_BEACON 0x0001027D

/** ID of the MAD ultrasound interrupt. */
#define CAPI_V2_PARAM_ID_CDC_INT_MAD_ULT 0x0001027E

/** ID of the left speaker clip interrupt. */
#define CAPI_V2_PARAM_ID_CDC_INT_SPKR_AUDIO_CLIP 0x0001027F

/** ID of the battery voltage (VBAT) attack interrupt. */
#define CAPI_V2_PARAM_ID_CDC_INT_VBAT_ATTACK 0x00010280

/** ID of the VBAT release interrupt. */
#define CAPI_V2_PARAM_ID_CDC_INT_VBAT_RELEASE 0x00010281

/** ID of the right speaker clip interrupt. */
#define CAPI_V2_PARAM_ID_CDC_INT_SPKR2_AUDIO_CLIP 0x00010282


/** Contains information on PCM levels that caused clipping in the codec.
*/
typedef struct capi_v2_cdc_clip_pcm_info_t
{
  capi_v2_buf_t cdc_clip_pcm_level_buf;  /**< Buffer of PCM levels. */
}/** @cond */capi_v2_cdc_clip_pcm_info_t/** @endcond */; 

typedef struct capi_v2_num_needed_cdc_int_t capi_v2_num_needed_cdc_int_t;

/** @weakgroup weak_capi_v2_num_needed_cdc_int_t
@{ */
struct capi_v2_num_needed_cdc_int_t
{
   uint32_t num_cdc_int;  /**< Number of codec interrupts to monitor. */
};
/** @} *//* end_weakgroup weak_capi_v2_num_needed_cdc_int_t */


typedef struct capi_v2_cdc_int_t capi_v2_cdc_int_t;

/** @weakgroup weak_capi_v2_cdc_int_t
@{ */
struct capi_v2_cdc_int_t
{
   uint32_t cdc_int;
   /**< Codec interrupt to monitor.

        @values
        - #CAPI_V2_PARAM_ID_CDC_INT_MAD_AUDIO
        - #CAPI_V2_PARAM_ID_CDC_INT_MAD_BEACON
        - #CAPI_V2_PARAM_ID_CDC_INT_MAD_ULT
        - #CAPI_V2_PARAM_ID_CDC_INT_SPKR_AUDIO_CLIP
        - #CAPI_V2_PARAM_ID_CDC_INT_VBAT_ATTACK
        - #CAPI_V2_PARAM_ID_CDC_INT_VBAT_RELEASE 
        - #CAPI_V2_PARAM_ID_CDC_INT_SPKR2_AUDIO_CLIP @tablebulletend */
};
/** @} *//* end_weakgroup weak_capi_v2_cdc_int_t */

/** @} *//* end_addtogroup capiv2_ext_codec_int */

/** @endcond *//* OEM_only */

#endif /* #ifndef ELITE_FWK_EXTNS_CODEC_INTERRUPT_H */
