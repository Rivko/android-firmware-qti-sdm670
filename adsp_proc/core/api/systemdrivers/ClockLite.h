#ifndef __CLOCKLITE_H__
#define __CLOCKLITE_H__
/*
  ===========================================================================
*/
/**
  @file ClockLite.h
  @brief  Public interface for a lightweight, non-DAL API, for clocks that
          must be available prior to the DAL clock driver being initialized.
  ===========================================================================

*/

/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/ClockLite.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/14   FRV     Initial
  ===========================================================================
*/


#include "DALSys.h"


/*=========================================================================
      Interface Declarations
==========================================================================*/


/** @addtogroup clock_lite_functions
@{ */


/* =========================================================================
**  Function : Clock_EnablePRNG
** =========================================================================*/
/**
  Enable the PRNG clock.

  This function enables the PRNG clock using either the DAL clock driver
  if it has initialized and is available or a lite version driver otherwise.
 
  @return
  DAL_SUCCESS -- PRNG clock enabled successfully.
  DAL_ERROR -- PRNG clock not enabled successfully.
*/

DALResult Clock_EnablePRNG(void);


/* =========================================================================
**  Function : Clock_DisablePRNG
** =========================================================================*/
/**
  Disable the PRNG clock.

  This function disables the PRNG clock using either the DAL clock driver
  if it has initialized and is available or a lite version driver otherwise.

  @return
  DAL_SUCCESS -- PRNG clock disabled successfully.
  DAL_ERROR -- PRNG clock not disabled successfully.
*/

DALResult Clock_DisablePRNG(void);


/** @} */ /* end_addtogroup clock_lite_functions */

#endif /* __CLOCKLITE_H__ */

