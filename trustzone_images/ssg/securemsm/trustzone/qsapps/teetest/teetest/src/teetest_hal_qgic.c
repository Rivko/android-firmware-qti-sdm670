/*
===========================================================================

FILE:         teetest_HALqgic.c

DESCRIPTION:
  This is the hardware abstraction layer implementation for the Qualcomm
  Generic Interrupt Controller.

===========================================================================

                             Edit History

$Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/teetest/teetest/src/teetest_hal_qgic.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/22/09   gfr     Fixed GICD_ICFGRn indexing.
04/08/09   gfr     Renamed GICD_IDR to GICD_IIDR.
12/11/08   gfr     Created.

===========================================================================
           Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include <stddef.h>
#include <stdint.h>
#include "teetest.h"
#include "teetest_hal_qgic.h"
#include "teetest_hal_hwio.h"
/*
 * QGIC Distributor section base address definitions
 */
const uintnt nHalGICDBaseAddress = HAL_QGIC_GICD_BASE_ADDR;

/*
 * QGIC CPU Interface section base address definitions
 */
const uintnt nHalGICCBaseAddress = HAL_QGIC_GICC_BASE_ADDR;

/*
 * QGIC Hypervisor section base address definitions
 */
const uintnt nHalGICHBaseAddress = HAL_QGIC_GICH_BASE_ADDR;

/*
 * QGIC Virtualization section base address definitions
 */
const uintnt nHalGICVBaseAddress = HAL_QGIC_GICV_BASE_ADDR;

/*
 * QGIC Redistributor section base address definitions.
 */
const uintnt nHalGICRBaseAddress = HAL_QGIC_GICR_BASE_ADDR;

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/*
 * Identifier string for this HAL.
 */
#define HAL_QGIC_VERSION_STRING "HAL_QGIC_V1"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

#ifdef TEETEST_GIC_V1
/* ===========================================================================
**  HAL_qgic_Generate
**
** ======================================================================== */
void HAL_qgic_Generate
(
  HAL_qgic_InterruptType nInterrupt,
  HAL_qgic_SecureType    eSecure,
  uint32_t               nTargets
)
{
  uint32_t nCmd;

  /*
   * Only send SGIs or the command may do bad things.
   */
  if (!HAL_QGIC_IS_SGI(nInterrupt))
  {
    return;
  }

  /*
   * Fill in the command value.
   */
  nCmd = nInterrupt;

  /*
   * Set the security attribute.
   */
  if (eSecure == HAL_QGIC_NON_SECURE)
  {
    nCmd |= HWIO_FMSK(GICD_SGIR, SATT);
  }

  /*
   * Fill in the targets.
   */
  if ((nTargets & HAL_QGIC_TARGET_OTHERS) &&
      (nTargets & HAL_QGIC_TARGET_SELF))
  {
    nCmd |= HWIO_FMSK(GICD_SGIR, T_LIST);
  }
  else if (nTargets & HAL_QGIC_TARGET_OTHERS)
  {
    nCmd |= HWIO_FVAL(GICD_SGIR, T_FILTER, 0x1);
  }
  else if (nTargets & HAL_QGIC_TARGET_SELF)
  {
    nCmd |= HWIO_FVAL(GICD_SGIR, T_FILTER, 0x2);
  }
  else
  {
    nCmd |= HWIO_FVAL(GICD_SGIR, T_LIST, nTargets);
  }

  /*
   * Trigger the interrupt.
   */
  HWIO_OUT(GICD_SGIR, nCmd);

} /* END HAL_qgic_Generate */

#endif

#ifdef TEETEST_GIC_V3

#define HAL_QGIC_SGI_REG(target,aff1,intid,aff2,mode,aff3) \
          ( ( (uint64_t)( target & 0xffff ) << 0 )  | \
            ( (uint64_t)( aff1 & 0xff )     << 16 ) | \
            ( (uint64_t)( intid & 0xf )     << 24 ) | \
            ( (uint64_t)( aff2 & 0xff )     << 32 ) | \
            ( (uint64_t)( mode & 0x1  )     << 40 ) | \
            ( (uint64_t)( aff3 & 0xff )     << 48 ) )

#define HAL_QGIC_AFF0(x) ( ( x >> 0  ) & 0xff )
#define HAL_QGIC_AFF1(x) ( ( x >> 8  ) & 0xff )
#define HAL_QGIC_AFF2(x) ( ( x >> 16 ) & 0xff )
#define HAL_QGIC_AFF3(x) ( ( x >> 24 ) & 0xff )

/* ===========================================================================
**  HALGICv3_GetCPUID
**
** ======================================================================== */
static uint32_t HALGICv3_GetCPUID(void)
{
  uintnt nVal;
  #ifdef TEETEST64
    asm volatile ("mrs  %0, MPIDR_EL1" : "=r" (nVal) );
  #else
    asm volatile ("mrc  p15, 0, %0, c0, c0, 5" : "=r" (nVal) );
  #endif
  return (uint32_t)(nVal & 0xffffff);
}

/* ===========================================================================
**  HALGICv3_WriteSGI
**
** ======================================================================== */
static void HALGICv3_WriteSGI(uint64_t nVal)
{
  #ifdef TEETEST64
    asm volatile ("msr  ICC_SGI1R_EL1, %0"     :: 
      "r" (nVal) 
    );
  #else
    asm volatile ("mcrr p15, 0, %0, %1, c12 "  :: 
      "r" ((uint32_t) (nVal) & 0xFFFFFFFF), 
      "r" ((uint32_t)(nVal >> 32) & 0xFFFFFFFF)
    );
  #endif  
  return;
}

void HAL_qgic_Generate
(
  HAL_qgic_InterruptType nInterrupt,
  HAL_qgic_SecureType    eSecure,
  uint32_t                 nTargets
)
{
  uint64_t nCmd = 0;
  uint64_t nMode = 0;
  uint64_t nCpuId = 0;
  uint64_t nTargetMask = 0;

  /*
   * Only send SGIs or the command may do bad things.
   */
  if (!HAL_QGIC_IS_SGI(nInterrupt))
  {
    return;
  }

  /*
   * Fill in the targets.
   */
  if (nTargets & HAL_QGIC_TARGET_OTHERS)
  {
    /*
     * Set to 1-to-N mode
     */
    nMode = 1;
  }
  else if (nTargets & HAL_QGIC_TARGET_SELF)
  {
    nCpuId = HALGICv3_GetCPUID();
    nTargetMask = 0x1 << HAL_QGIC_AFF0(nCpuId);
  }
  else
  {
    /* Not supported for teetest*/
    return;
  }

  nCmd = HAL_QGIC_SGI_REG(nTargetMask,
                          HAL_QGIC_AFF1(nCpuId),
                          nInterrupt,
                          HAL_QGIC_AFF2(nCpuId),
                          nMode,
                          HAL_QGIC_AFF3(nCpuId));

  /*
   * Trigger the interrupt.
   */
  if (eSecure == HAL_QGIC_NON_SECURE)
  {
    /* Not supported in teetest */
    return;
  }
  else
  {
    HALGICv3_WriteSGI(nCmd);
  }

} /* END HAL_qgic_Generate */

#endif




