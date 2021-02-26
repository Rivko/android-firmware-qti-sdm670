#ifndef _AFE_COMMON_H_
#define _AFE_COMMON_H_

/*==============================================================================
  @file afe_common.h
  @brief This file contains Public APIs for AFE module 

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/afe_common.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  06/19/17    sw        (Tech Pubs) Edited descriptions
  04/28/17   unni       Created File
==============================================================================*/

/** @h2xml_title1           {Public Common APIs}
    @h2xml_title_date       {April 28, 2017} */

#include "mmdefs.h"

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define AFE_PARAM_ID_ENABLE   0x00010203

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct afe_mod_enable_param_t afe_mod_enable_param_t;

/** @h2xmlp_parameter   {"AFE_PARAM_ID_ENABLE", AFE_PARAM_ID_ENABLE}
    @h2xmlp_description {Enables a specified module.} */
#include "adsp_begin_pack.h"
struct afe_mod_enable_param_t
{
  uint16_t enable;
  /**< @h2xmle_description {Enables the module.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_default     {1} */

  uint16_t reserved;
  /**< @h2xmle_description {Used for alignment; must be set to 0.}
       @h2xmle_readonly    {true}
       @h2xmle_default     {0} */
}
#include "adsp_end_pack.h"
;

#endif /* _AFE_COMMON_H_ */
