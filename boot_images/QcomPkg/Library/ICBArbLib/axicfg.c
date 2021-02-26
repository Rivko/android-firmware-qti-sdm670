/*=============================================================================

@file   axicfg.c

@brief  Internal source file for bus configuration

when         who     what, where, why
----------   ---     --------------------------------------------------------
2016/02/22   sds     Branch for Hoya restructure.
2011/12/05   dj      Created file

===========================================================================
            Copyright (c) 2011-2016 QUALCOMM Technologies, Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "ddiaxicfg.h"
#include "axicfgos.h"
#include "ddiaxicfgint.h"
#include "axicfgint.h"
#include "ul_i.h"

/* -----------------------------------------------------------------------
** Public functions
** ----------------------------------------------------------------------- */
/**
  @copydoc AxiCfg_SetQosPolicy
 */
AxiCfg_ReturnCodeType AxiCfg_SetQosPolicy
(
  ICBId_MasterType      eMaster,
  AxiCfg_QosPolicyType *pQosPolicy
)
{
  AxiCfg_ReturnCodeType ret = AXICFG_ERROR_INVALID;
  if( AxiCfgInt_Init() )
  {
    icb_master_def_type *master = ul_find_master(eMaster);

    /* Find the associated master and make sure it's locally owned. */
    if( NULL != master && master->is_locally_owned )
    {
      ret = AxiCfgInt_SetQosPolicy( master, pQosPolicy );
    }
  }
  return ret;
}

/* -----------------------------------------------------------------------
** Deprecated functions
** ----------------------------------------------------------------------- */
/**
  @brief Configure the clock gating for the indicated master.

  @param[in] eMaster    The master port identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetMasterClockGating( ICBId_MasterType eMaster, AxiCfg_ClockGatingType * pGating)
{
  (void)eMaster;
  (void)pGating;

  /* Deprecated. */
}

/**
  @brief Configure the clock gating for the indicated slave.

  @param[in] eSlave     The slave way identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetSlaveClockGating( ICBId_SlaveType eSlave, AxiCfg_ClockGatingType * pGating)
{
  (void)eSlave;
  (void)pGating;
}

/**
  @copydoc AxiCfg_HaltMasterPort
 */
AxiCfg_ReturnCodeType AxiCfg_HaltMasterPort( ICBId_MasterType eMaster )
{
  (void)eMaster;

  return AXICFG_ERROR_UNSUPPORTED; 
}

/**
  @copydoc AxiCfg_HaltMasterPort_OnError
 */
AxiCfg_ReturnCodeType AxiCfg_HaltMasterPort_OnError( ICBId_MasterType eMaster )
{
  (void)eMaster;

  return AXICFG_ERROR_UNSUPPORTED; 
}

/**
  @copydoc AxiCfg_UnhaltMasterPort
 */
AxiCfg_ReturnCodeType AxiCfg_UnhaltMasterPort( ICBId_MasterType eMaster )
{
  (void)eMaster;

  return AXICFG_ERROR_UNSUPPORTED; 
}

/**
  @copydoc AxiCfg_UnhaltMasterPort_OnError
 */
AxiCfg_ReturnCodeType AxiCfg_UnhaltMasterPort_OnError( ICBId_MasterType eMaster )
{
  (void)eMaster;

  return AXICFG_ERROR_UNSUPPORTED; 
}
