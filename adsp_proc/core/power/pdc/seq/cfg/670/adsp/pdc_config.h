#ifndef _PDC_CONFIG_H_
#define _PDC_CONFIG_H_
/*==============================================================================
  FILE:         pdc_config.h
  
  OVERVIEW:     This file contains target specific definitions related to the
                PDC sequences
 
  DEPENDENCIES: None

                Copyright (c) 2017-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/seq/cfg/670/adsp/pdc_config.h#2 $
$DateTime: 2018/01/22 08:39:02 $
==============================================================================*/

/*==========================================================================
 *                          DEFINITIONS
 *=========================================================================*/
/* PDC offsets */
#define SSC_PDC_SSC_GDSC_NOAOSS          0x17
#define SSC_PDC_SSC_GDSC_AOSS            0x0
#define SSC_PDC_SSC_GDSC_AOSS_NOTIMER    0x2b

#define SSC_PDC_NUMSEQWORDS           24
#define SSC_PDC_NUMBRANCHES           4

#endif /* _PDC_CONFIG_H_ */

