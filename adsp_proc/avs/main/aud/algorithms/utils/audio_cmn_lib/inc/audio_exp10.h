/*========================================================================*/
/**
@file audio_exp10.h

This file contains declaration for exponential routine.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Copyright (c) 2008, 2010, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/audio_exp10.h#1 $

when       who          what, where, why
--------   ---          -------------------------------------------------------
7/10/06  Prapti Patel   Created file.
10/20/10   sw,kli       (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */

#ifndef _EXP10_H_
#define _EXP10_H_

#include "AudioComdef.h"

/*=============================================================================
      Function Declarations 
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup math_operations
@{ */

/**
  Exponential function. It computes 10^x for a given value of x.

  @param[in] x   Exponent.

  @return
  int32_t 10^x.
  
  @dependencies
  None.
*/
int32_t exp10_fixed(int32_t x); 

/** @} */ /* end_addtogroup math_operations */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*_EXP10_H_*/

