#ifndef _AFE_SWMAD_H_
#define _AFE_SWMAD_H_

/*==============================================================================
  @file swmad.h
  @brief This file contains Public APIs for Software MAD (Mic-Activitiy 
         Detection) Module

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/prop/sw_mad.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------  
  06/19/17    sw        (Tech Pubs) Edited descriptions and values
==============================================================================*/

/** @h2xml_title1           {Software MAD Module}
    @h2xml_title_date       {January 03, 2017} */

#include "mmdefs.h"
#include "afe_common.h"

/*------------------------------------------------------------------------------
   Module ID
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Deprecated. Unique Module ID */
#define AFE_MODULE_ID_SW_MAD   0x0001022D

/** Deprecated.

    @h2xmlm_module      {"AFE_MODULE_ID_SW_MAD",AFE_MODULE_ID_SW_MAD}
    @h2xmlm_toolPolicy  {Calibration;RTC}
    @h2xmlm_description {Supports the Software Mic Activity Detection (MAD)
                         feature.}
    @h2xmlm_deprecated  {true}
    @{                  <-- Start of the Module --> */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/

/* Deprecated. Unique Paramter id */
#define AFE_PARAM_ID_SW_MAD_CFG             0x0001022E

/* Deprecated. Minor Version of this PARAM */
#define AFE_API_VERSION_SW_MAD_CONFIG       0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Deprecated. Structure definition for Parameter */
typedef struct afe_sw_mad_cfg_param_v1_t afe_sw_mad_cfg_param_v1_t;

/** Deprecated.

    @h2xmlp_parameter   {"AFE_PARAM_ID_SW_MAD_CFG", AFE_PARAM_ID_SW_MAD_CFG}
    @h2xmlp_description {Configures the software MAD algorithm.}
    @h2xmlp_version     {AFE_API_VERSION_SW_MAD_CONFIG} */
#include "adsp_begin_pack.h"
struct afe_sw_mad_cfg_param_v1_t
{
  uint32_t sw_mad_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_SW_MAD_CONFIG}
       @h2xmle_default     {AFE_API_VERSION_SW_MAD_CONFIG}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  int32_t sw_mad_threshQ23;
  /**< @h2xmle_description {Score threshold for the binary classifier.}
       @h2xmle_range       {-2147483648..2139095040}
       @h2xmle_dataFormat  {Q23}
       @h2xmle_default     {-511573} */

  int16_t sw_mad_gain_Q8;
  /**< @h2xmle_description {Gain that is applied to input samples.}
       @h2xmle_range       {-32768..32767}
       @h2xmle_dataFormat  {Q8}
       @h2xmle_default     {256} */

  int16_t reserved;
  /**< @h2xmle_description {Used for alignment; must be set to 0.}
       @h2xmle_readonly    {true}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Deprecated. Unique Parameter id */
#define AFE_PARAM_ID_SVM_MODEL          0x0001022F

/* Deprecated. Minor version of this PARAM */
#define AFE_API_VERSION_SVM_MODEL       0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Deprecated. Structure definition for Parameter */
typedef struct afe_svm_model_param_v1_t afe_svm_model_param_v1_t;

/** Deprecated.

    @h2xmlp_parameter   {"AFE_PARAM_ID_SVM_MODEL", AFE_PARAM_ID_SVM_MODEL}
    @h2xmlp_description {Registers the Sound Vector Model (SVM). This model
                         contains the unique sound characteristics or signatures
                         used by the algorithm hosted in the module.}
    @h2xmlp_version     {AFE_API_VERSION_SVM_MODEL} */
#include "adsp_begin_pack.h"
struct afe_svm_model_param_v1_t
{
  uint32_t svm_model_minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..AFE_API_VERSION_SVM_MODEL}
       @h2xmle_default     {AFE_API_VERSION_SVM_MODEL}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t model_size_in_bytes;
  /**< @h2xmle_description {Size of the SVM (in bytes).}
       @h2xmle_default     {880} */

  uint32_t svm_model_addr_lsw;
  /**< @h2xmle_description {Lower 32 bits of the 64-bit payload address for
                            the SVM.}
       @h2xmle_default     {0}
       @h2xmle_readonly    {true} */

  uint32_t svm_model_addr_msw;
  /**< @h2xmle_description {Upper 32 bits of the 64-bit payload address for
                            the SVM.}
       @h2xmle_default     {0}
       @h2xmle_readonly    {true} */

  uint32_t mem_map_handle;
  /**< @h2xmle_description {Unique identifier for an address.}
       @h2xmle_default     {0}
       @h2xmle_readonly    {true} */

  uint8_t blob[0];
  /**< @h2xmle_description {Represents the sound model. The size of this array
                           is the actual size of the sound model.}
       @h2xmle_elementType {rawData}
       @h2xmle_displayType {file}
       @h2xmle_defaultFile {swmad_default.bin} */

}
#include "adsp_end_pack.h"
;

/** @h2xml_Select         {afe_mod_enable_param_t}
    @h2xmlm_InsertParameter */ 

/** @} <-- End of the Module --> */

#endif /* _AFE_SWMAD_H_ */

