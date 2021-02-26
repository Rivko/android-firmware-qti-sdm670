#ifndef THERM_NPA_MITIGATE_H
#define THERM_NPA_MITIGATE_H

/*============================================================================
  FILE:         therm_npa_mitigate.h

  OVERVIEW:     Thermal NPA mitigation resources header file.

                Copyright (c) 2011-2014, 2017 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.qdsp6/2.1/power/thermal/src/target/therm_npa_mitigate.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*
===============================================================================

                             TYPE DEFINITION(S)

===============================================================================
*/

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
  @brief therm_npa_mitigate_init

  Thermal mitigate init function.
*/
void therm_npa_mitigate_init( void );


#ifdef __cplusplus
}
#endif

#endif /* THERM_NPA_MITIGATE_H */
