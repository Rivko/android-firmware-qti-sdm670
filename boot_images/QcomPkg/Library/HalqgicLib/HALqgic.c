/*
===========================================================================

FILE:         HALqgic.c

DESCRIPTION:  
  This is the hardware abstraction layer implementation for the Qualcomm
  Generic Interrupt Controller.

===========================================================================

                             Edit History


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


#include <HALhwio.h>
#include <HALqgic.h>
#include "HALqgicRegisters.h"



/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/*
 * Identifier string for this HAL.
 */
#define HAL_QGIC_VERSION_STRING "HAL_QGIC_V1"


/*
 * Macros to find a register address and field mask from an interrupt
 * number.  This is designed for the register arrays from [0..8].
 */
#define HAL_QGIC_INT2ADDR(addr, nInt)  HWIO_ADDRI(addr, ((nInt) >> 5))
#define HAL_QGIC_INT2MASK(nInt)        (1 << ((nInt) & 0x1F))


/*
 * Macros to check if an interrupt is SGI, PPI or SPI.
 */
#define HAL_QGIC_IS_SGI(nInt)  ((nInt) <= 15)
#define HAL_QGIC_IS_PPI(nInt)  ((nInt) >= 16 && (nInt) <= 31)
#define HAL_QGIC_IS_SPI(nInt)  ((nInt) >= 32 && (nInt) <= 1020)



/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  HAL_qgic_IsSGI
**
** ======================================================================== */

boolean HAL_qgic_IsSGI
(
  HAL_qgic_InterruptType nInterrupt
)
{
  return HAL_QGIC_IS_SGI(nInterrupt);

} /* END HAL_qgic_IsSGI */


/* ===========================================================================
**  HAL_qgic_IsPPI
**
** ======================================================================== */

boolean HAL_qgic_IsPPI
(
  HAL_qgic_InterruptType nInterrupt
)
{
  return HAL_QGIC_IS_PPI(nInterrupt);

} /* END HAL_qgic_IsPPI */


/* ===========================================================================
**  HAL_qgic_IsSPI
**
** ======================================================================== */

boolean HAL_qgic_IsSPI
(
  HAL_qgic_InterruptType nInterrupt
)
{
  return HAL_QGIC_IS_SPI(nInterrupt);

} /* END HAL_qgic_IsSPI */


/* ===========================================================================
**  HAL_qgic_Init
**
** ======================================================================== */

void HAL_qgic_Init
(
  HAL_qgic_BaseAddressType *pBaseAddr
)
{

  /*
   * Validate input pointer
  */

  if (pBaseAddr == NULL)
  {
    return;
  }
  
  /*
   * Assign register base
  */

  nHalGICDBaseAddress = (UINTN)pBaseAddr->nGICDBaseAddress;
  nHalGICCBaseAddress = (UINTN)pBaseAddr->nGICCBaseAddress; 
  nHalGICHBaseAddress = (UINTN)pBaseAddr->nGICHBaseAddress;
  nHalGICVBaseAddress = (UINTN)pBaseAddr->nGICVBaseAddress;

} /* END HAL_qgic_Init */


/* ===========================================================================
**  HAL_qgic_Reset
**
** ======================================================================== */

void HAL_qgic_Reset (void)
{
  UINTN n;

  /*
   * Reset HW to initial state.
   */

  HWIO_OUT(GICD_CTLR, 0);
  HWIO_OUT(GICD_CGCR, 0);
  HWIO_OUT(GICC_CTLR, 0);
  HWIO_OUT(GICC_PMR, 0);
  HWIO_OUT(GICC_BPR, 0);

  for (n = 0; n <= 11; n++)
  {
    HWIO_OUTI(GICD_ISRn, n, 0);
    HWIO_OUTI(GICD_ICENABLERn, n, 0xFFFFFFFF);
    HWIO_OUTI(GICD_ICPENDRn, n, 0xFFFFFFFF);
  }

  for (n = 0; n <= 95; n++)
  {
    HWIO_OUTI(GICD_IPRIORITYRn, n, 0);
    HWIO_OUTI(GICD_ITARGETSRn, n, 0);
  }

  for (n = 0; n <= 23; n++)
  {
    HWIO_OUTI(GICD_ICFGRn, n, 0);
  }

} /* END HAL_qgic_Reset */


/* ===========================================================================
**  HAL_qgic_EnableDistributor
**
** ======================================================================== */

void HAL_qgic_EnableDistributor (void)
{
  HWIO_OUTF(GICD_CTLR, ENABLE, 0x1);

} /* END HAL_qgic_EnableDistributor */


/* ===========================================================================
**  HAL_qgic_DisableDistributor
**
** ======================================================================== */

void HAL_qgic_DisableDistributor (void)
{
  HWIO_OUTF(GICD_CTLR, ENABLE, 0x0);

} /* END HAL_qgic_DisableDistributor */


/* ===========================================================================
**  HAL_qgic_EnableDistributorNS
**
** ======================================================================== */

void HAL_qgic_EnableDistributorNS (void)
{
  HWIO_OUTF(GICD_CTLR, ENABLE_NS, 0x1);

} /* END HAL_qgic_EnableDistributorNS */


/* ===========================================================================
**  HAL_qgic_DisableDistributor
**
** ======================================================================== */

void HAL_qgic_DisableDistributorNS (void)
{
  HWIO_OUTF(GICD_CTLR, ENABLE_NS, 0x0);

} /* END HAL_qgic_DisableDistributorNS */


/* ===========================================================================
**  HAL_qgic_SetSecurity
**
** ======================================================================== */

void HAL_qgic_SetSecurity
(
  HAL_qgic_InterruptType nInterrupt,
  HAL_qgic_SecureType    eSecure
)
{
  UINTN nAddr;
  uint32 nMask;
  uint32 nVal;

  nAddr = HAL_QGIC_INT2ADDR(GICD_ISRn, nInterrupt);
  nMask = HAL_QGIC_INT2MASK(nInterrupt);

  nVal = inpdw(nAddr);

  if (eSecure == HAL_QGIC_SECURE)
  {
    nVal &= ~nMask;
  }
  else
  {
    nVal |= nMask;
  }

  outpdw(nAddr, nVal);

} /* END HAL_qgic_SetSecurity */


/* ===========================================================================
**  HAL_qgic_SetSecurityAll
**
** ======================================================================== */

void HAL_qgic_SetSecurityAll
(
  HAL_qgic_SecureType eSecure
)
{
  UINTN n;
  uint32 nVal;

  /*
   * Figure out value to program
   */
  if (eSecure == HAL_QGIC_SECURE)
  {
    nVal = 0;
  }
  else
  {
    nVal = 0xFFFFFFFF;
  }

  /*
   * Set all the security the same.
   */
  for (n = 0; n <= 11; n++)
  {
    HWIO_OUTI(GICD_ISRn, n, nVal);
  }

} /* END HAL_qgic_SetSecurityAll */


/* ===========================================================================
**  HAL_qgic_Enable
**
** ======================================================================== */

void HAL_qgic_Enable
(
  HAL_qgic_InterruptType nInterrupt
)
{
  UINTN nAddr;
  uint32 nMask;

  nAddr = HAL_QGIC_INT2ADDR(GICD_ISENABLERn, nInterrupt);
  nMask = HAL_QGIC_INT2MASK(nInterrupt);

  outpdw(nAddr, nMask);

} /* END HAL_qgic_Enable */


/* ===========================================================================
**  HAL_qgic_Disable
**
** ======================================================================== */

void HAL_qgic_Disable
(
  HAL_qgic_InterruptType nInterrupt
)
{
  UINTN nAddr;
  uint32 nMask;

  nAddr = HAL_QGIC_INT2ADDR(GICD_ICENABLERn, nInterrupt);
  nMask = HAL_QGIC_INT2MASK(nInterrupt);

  outpdw(nAddr, nMask);

} /* END HAL_qgic_Disable */


/* ===========================================================================
**  HAL_qgic_IsEnabled
**
** ======================================================================== */

boolean HAL_qgic_IsEnabled
(
  HAL_qgic_InterruptType nInterrupt
)
{
  UINTN nAddr;
  uint32 nMask;

  nAddr = HAL_QGIC_INT2ADDR(GICD_ISENABLERn, nInterrupt);
  nMask = HAL_QGIC_INT2MASK(nInterrupt);

  if (inpdw(nAddr) & nMask)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* END HAL_qgic_IsEnabled */


/* ===========================================================================
**  HAL_qgic_SetPending
**
** ======================================================================== */

void HAL_qgic_SetPending
(
  HAL_qgic_InterruptType nInterrupt
)
{
  UINTN nAddr;
  uint32 nMask;

  nAddr = HAL_QGIC_INT2ADDR(GICD_ISPENDRn, nInterrupt);
  nMask = HAL_QGIC_INT2MASK(nInterrupt);

  outpdw(nAddr, nMask);

} /* END HAL_qgic_SetPending */


/* ===========================================================================
**  HAL_qgic_ClearPending
**
** ======================================================================== */

void HAL_qgic_ClearPending
(
  HAL_qgic_InterruptType nInterrupt
)
{
  UINTN nAddr;
  uint32 nMask;

  nAddr = HAL_QGIC_INT2ADDR(GICD_ICPENDRn, nInterrupt);
  nMask = HAL_QGIC_INT2MASK(nInterrupt);

  outpdw(nAddr, nMask);

} /* END HAL_qgic_ClearPending */


/* ===========================================================================
**  HAL_qgic_IsPending
**
** ======================================================================== */

boolean HAL_qgic_IsPending
(
  HAL_qgic_InterruptType nInterrupt
)
{
  UINTN nAddr;
  uint32 nMask;

  nAddr = HAL_QGIC_INT2ADDR(GICD_ISPENDRn, nInterrupt);
  nMask = HAL_QGIC_INT2MASK(nInterrupt);

  if (inpdw(nAddr) & nMask)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* END HAL_qgic_IsPending */


/* ===========================================================================
**  HAL_qgic_IsActive
**
** ======================================================================== */

boolean HAL_qgic_IsActive
(
  HAL_qgic_InterruptType nInterrupt
)
{
  UINTN nAddr;
  uint32 nMask;

  /*
   * Get the address and mask for this interrupt.
   */
  nAddr = HAL_QGIC_INT2ADDR(GICD_IACTIVERn, nInterrupt);
  nMask = HAL_QGIC_INT2MASK(nInterrupt);

  /*
   * Check the active bit.
   */
  if (inpdw(nAddr) & nMask)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* END HAL_qgic_IsActive */


/* ===========================================================================
**  HAL_qgic_SetPriority
**
** ======================================================================== */

void HAL_qgic_SetPriority
(
  HAL_qgic_InterruptType nInterrupt,
  HAL_qgic_PriorityType  nPriority
)
{
  UINTN nAddr;
  uint32 nMask;
  UINTN nShift;
  uint32 nVal;

  /*
   * Get the address and mask for this interrupt.
   */
  nAddr  = HWIO_ADDRI(GICD_IPRIORITYRn, nInterrupt / 4);
  nShift = (nInterrupt & 0x3) * 8;
  nMask  = 0xFF << nShift;

  /*
   * Mask in new priority.
   */
  nVal = inpdw(nAddr);
  nVal &= ~nMask;
  nVal |= (nPriority << nShift) & nMask;

  /*
   * Program the new priority.
   */
  outpdw(nAddr, nVal);

} /* END HAL_qgic_SetPriority */

/* ===========================================================================
**  HAL_qgic_GetPriority
**
** ======================================================================== */

void HAL_qgic_GetPriority
(
  HAL_qgic_InterruptType nInterrupt,
  HAL_qgic_PriorityType  *pnPriority
)
{
  UINTN nAddr;
  UINTN nShift;
  uint32 nVal;

  /*
   * Get the address and mask for this interrupt.
   */
  nAddr  = HWIO_ADDRI(GICD_IPRIORITYRn, nInterrupt / 4);
  nShift = (nInterrupt & 0x3) * 8;

  /*
   * Read the priority.
   */
  nVal = inpdw(nAddr);
  *pnPriority = nVal >> nShift;
  *pnPriority &= 0xFF;

} /* END HAL_qgic_GetPriority */


/* ===========================================================================
**  HAL_qgic_SetPriorityAll
**
** ======================================================================== */

void HAL_qgic_SetPriorityAll
(
  HAL_qgic_PriorityType  nPriority
)
{
  UINTN n;
  uint32 nVal;

  /*
   * Setup new priority for all interrupts.
   */
  nVal =
    (nPriority << 0)  | (nPriority << 8) |
    (nPriority << 16) | (nPriority << 24);

  /*
   * Program the new priority.
   */
  for (n = 0; n <= 95; n++)
  {
    HWIO_OUTI(GICD_IPRIORITYRn, n, nVal);
  }

} /* END HAL_qgic_SetPriorityAll */


/* ===========================================================================
**  HAL_qgic_SetTargets
**
** ======================================================================== */

void HAL_qgic_SetTargets
(
  HAL_qgic_InterruptType nInterrupt,
  uint32                 nTargets
)
{
  UINTN nAddr;
  uint32 nMask;
  uint32 nShift;
  uint32 nVal;
  
  /*
   * Get the address and mask for this interrupt.
   */
  nAddr  = HWIO_ADDRI(GICD_ITARGETSRn, nInterrupt / 4);
  nShift = (nInterrupt & 0x3) * 8;
  nMask  = 0xFF << nShift;

  /*
   * Mask in new target.
   */
  nVal = inpdw(nAddr);
  nVal &= ~nMask;
  nVal |= (nTargets << nShift) & nMask;

  /*
   * Program the new target CPU.
   */
  outpdw(nAddr, nVal);

} /* END HAL_qgic_SetTargets */


/* ===========================================================================
**  HAL_qgic_SetTrigger
**
** ======================================================================== */

void HAL_qgic_SetTrigger
(
  HAL_qgic_InterruptType nInterrupt,
  HAL_qgic_TriggerType   eTrigger
)
{
  UINTN nAddr;
  uint32 nMask;
  uint32 nVal;
  boolean bIsEnabled;

  /*
   * Get the address and mask for this interrupt.
   */
  nAddr = HWIO_ADDRI(GICD_ICFGRn, nInterrupt >> 4);
  nMask = (0x1 << ((nInterrupt & 0xF) * 2 + 1));

  /*
   * Get current configuration
   */
  nVal = inpdw(nAddr);

  /*
   * Determine new configuration based on interrupt type and trigger.
   */
  if (eTrigger == HAL_QGIC_TRIGGER_EDGE)
  {
    nVal |= nMask;
  }
  else
  {
    nVal &= ~nMask;
  }

  /*
   * Disable the interrupt while programming the type.
   */
  bIsEnabled = HAL_qgic_IsEnabled(nInterrupt);
  if (bIsEnabled)
  {
    HAL_qgic_Disable(nInterrupt);
  }

  /*
   * Program the new trigger.
   */
  outpdw(nAddr, nVal);

  /*
   * Re-enable the interrupt if necessary.
   */
  if (bIsEnabled)
  {
    HAL_qgic_Enable(nInterrupt);
  }

} /* END HAL_qgic_SetTrigger */


/* ===========================================================================
**  HAL_qgic_GetTrigger
**
** ======================================================================== */

void HAL_qgic_GetTrigger
(
  HAL_qgic_InterruptType  nInterrupt,
  HAL_qgic_TriggerType   *peTrigger
)
{
  UINTN nAddr;
  uint32 nMask;

  /*
   * Get the address and mask for this interrupt.
   */
  nAddr = HWIO_ADDRI(GICD_ICFGRn, nInterrupt >> 4);
  nMask = (0x1 << ((nInterrupt & 0xF) * 2 + 1));

  /*
   * Determine new configuration based on interrupt type and trigger.
   */
  if (inpdw(nAddr) & nMask)
  {
    *peTrigger = HAL_QGIC_TRIGGER_EDGE;
  }
  else
  {
    *peTrigger = HAL_QGIC_TRIGGER_LEVEL;
  }

} /* END HAL_qgic_GetTrigger */


/* ===========================================================================
**  HAL_qgic_Generate
**
** ======================================================================== */

void HAL_qgic_Generate
(
  HAL_qgic_InterruptType nInterrupt,
  HAL_qgic_SecureType    eSecure,
  uint32                 nTargets
)
{
  UINTN nCmd;

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


/* ===========================================================================
**  HAL_qgic_EnableInterrupts
**
** ======================================================================== */

void HAL_qgic_EnableInterrupts (void)
{
  HWIO_OUTF(GICC_CTLR, ENABLE, 1);

} /* END HAL_qgic_EnableInterrupts */


/* ===========================================================================
**  HAL_qgic_DisableInterrupts
**
** ======================================================================== */

void HAL_qgic_DisableInterrupts (void)
{
  HWIO_OUTF(GICC_CTLR, ENABLE, 0);

} /* END HAL_qgic_DisableInterrupts */


/* ===========================================================================
**  HAL_qgic_EnableInterruptsNS
**
** ======================================================================== */

void HAL_qgic_EnableInterruptsNS (void)
{
  HWIO_OUTF(GICC_CTLR, ENABLE_NS, 1);

} /* END HAL_qgic_EnableInterruptsNS */


/* ===========================================================================
**  HAL_qgic_DisableInterrupts
**
** ======================================================================== */

void HAL_qgic_DisableInterruptsNS (void)
{
  HWIO_OUTF(GICC_CTLR, ENABLE_NS, 0);

} /* END HAL_qgic_DisableInterrupts */


/* ===========================================================================
**  HAL_qgic_SetSecureDest
**
** ======================================================================== */

void HAL_qgic_SetSecureDest
(
  HAL_qgic_PortType ePort
)
{
  if (ePort == HAL_QGIC_PORT_IRQ)
  {
    HWIO_OUTF(GICC_CTLR, S_DEST, 0);
  }
  else if (ePort == HAL_QGIC_PORT_FIQ)
  {
    HWIO_OUTF(GICC_CTLR, S_DEST, 1);
  }

} /* END HAL_qgic_SetSecureDest */


/* ===========================================================================
**  HAL_qgic_SetSecureAckNS
**
** ======================================================================== */

void HAL_qgic_SetSecureAckNS
(
  boolean bSecureAckNS
)
{
  if (bSecureAckNS)
  {
    HWIO_OUTF(GICC_CTLR, S_ACK, 1);
  }
  else
  {
    HWIO_OUTF(GICC_CTLR, S_ACK, 0);
  }

} /* END HAL_qgic_SetSecureAckNS */


/* ===========================================================================
**  HAL_qgic_SetPriorityMask
**
** ======================================================================== */

void HAL_qgic_SetPriorityMask
(
  HAL_qgic_PriorityType nPriorityMask
)
{
  /*
   * Set the mask in hardware.
   */
  HWIO_OUTM(GICC_PMR, 0xFF, nPriorityMask);

} /* END HAL_qgic_SetPriorityMask */


/* ===========================================================================
**  HAL_qgic_SetBinaryPoint
**
** ======================================================================== */

void HAL_qgic_SetBinaryPoint
(
  uint32 nBinaryPoint
)
{

  /*
   * Set the BPR.
   */
  HWIO_OUT(GICC_BPR, nBinaryPoint);

} /* END HAL_qgic_SetBinaryPoint */


/* ===========================================================================
**  HAL_qgic_SetBinaryPointMode
**
** ======================================================================== */

void HAL_qgic_SetBinaryPointMode
(
  HAL_qgic_BinaryPointModeType eBinaryPointMode
)
{
  /*
   * Set the BPR mode.
   */
  switch (eBinaryPointMode)
  {
    case HAL_QGIC_BINARY_POINT_MODE_BANKED:
      HWIO_OUTF(GICC_CTLR, SBPR, 0);
      break;

    case HAL_QGIC_BINARY_POINT_MODE_RESTRICTED:
      HWIO_OUTF(GICC_CTLR, SBPR, 1);
      break;

    default:
      break;
  }

} /* END HAL_qgic_SetBinaryPointMode */


/* ===========================================================================
**  HAL_qgic_SetClockSecurity
**
** ======================================================================== */

void HAL_qgic_SetClockSecurity
(
  HAL_qgic_SecureType    eSecure
)
{
  /*
   * We would like to use the proper field name, but the field name
   * (GICD_CGCR) is the same as a register, which causes our register macro
   * expansion to fail.  I.e. we get HWIO_RPM_GICD_ANSACR_RPM_GICD_CGCR_FMSK.
   */
  if (eSecure == HAL_QGIC_SECURE)
  {
    /* HWIO_OUTF(GICD_ANSACR, GICD_CGCR, 0); */
    HWIO_OUTM(GICD_ANSACR, 0x1, 0);
  }
  else
  {
    /* HWIO_OUTF(GICD_ANSACR, GICD_CGCR, 1); */
    HWIO_OUTM(GICD_ANSACR, 0x1, 1);
  }

} /* END HAL_qgic_SetClockSecurity */


/* ===========================================================================
**  HAL_qgic_AckInterrupt
**
** ======================================================================== */

void HAL_qgic_AckInterrupt
(
  HAL_qgic_InterruptType *pnInterrupt,
  HAL_qgic_CPUIdType     *pnCPUId
)
{
  uint32 nVal;

  /*
   * Read ack register from HW.  This is a command read and so should only
   * be done once.
   */
  nVal = HWIO_IN(GICC_IAR);

  /*
   * Parse out the interrupt number.
   */
  *pnInterrupt =
    (nVal & HWIO_FMSK(GICC_IAR, INT_ID)) >> HWIO_SHFT(GICC_IAR, INT_ID);

  /*
   * Parse out the CPU Id (only valid for SGIs).
   */
  *pnCPUId =
    (nVal & HWIO_FMSK(GICC_IAR, CPU_ID)) >> HWIO_SHFT(GICC_IAR, CPU_ID);

} /* END HAL_qgic_AckInterrupt */


/* ===========================================================================
**  HAL_qgic_EndOfInterrupt
**
** ======================================================================== */

void HAL_qgic_EndOfInterrupt
(
  HAL_qgic_InterruptType nInterrupt,
  HAL_qgic_CPUIdType     nCPUId
)
{
  uint32 nVal;

  /*
   * Prepare the value to write to the EOI register.
   */
  nVal =
   HWIO_FVAL(GICC_EOIR, INT_ID, nInterrupt) |
   HWIO_FVAL(GICC_EOIR, CPU_ID, nCPUId);

  /*
   * Signal end-of-interrupt.
   */
  HWIO_OUT(GICC_EOIR, nVal);

} /* END HAL_qgic_EndOfInterrupt */


/* ===========================================================================
**  HAL_qgic_GetHighestPending
**
** ======================================================================== */

void HAL_qgic_GetHighestPending
(
  HAL_qgic_InterruptType *pnInterrupt,
  HAL_qgic_CPUIdType     *pnCPUId
)
{
  uint32 nVal;

  /*
   * Read register from HW.
   */
  nVal = HWIO_IN(GICC_HPPIR);

  /*
   * Parse out the interrupt number.
   */
  *pnInterrupt =
    (nVal & HWIO_FMSK(GICC_HPPIR, INT_ID)) >> HWIO_SHFT(GICC_HPPIR, INT_ID);

  /*
   * Parse out the CPU Id (only valid for SGIs).
   */
  *pnCPUId =
    (nVal & HWIO_FMSK(GICC_HPPIR, CPU_ID)) >> HWIO_SHFT(GICC_HPPIR, CPU_ID);
 
} /* END HAL_qgic_GetHighestPending */


/* ===========================================================================
**  HAL_qgic_GetActivePriority
**
** ======================================================================== */

void HAL_qgic_GetActivePriority
(
  HAL_qgic_PriorityType *pnPriority
)
{
  /*
   * Read priority from register.
   */
  *pnPriority = HWIO_INF(GICC_RPR, VAL);

} /* END HAL_qgic_GetActivePriority */


/* ===========================================================================
**  HAL_qgic_Save
**
** ======================================================================== */

void HAL_qgic_Save (void)
{
  /*
   * Not implemented yet.
   */

} /* END HAL_qgic_Save */


/* ===========================================================================
**  HAL_qgic_Restore
**
** ======================================================================== */

void HAL_qgic_Restore (void)
{
  /*
   * Not implemented yet.
   */

} /* END HAL_qgic_Restore */


/* ===========================================================================
**  HAL_qgic_GetDistributorInfo
**
** ======================================================================== */

void HAL_qgic_GetDistributorInfo
(
  HAL_qgic_DistributorInfoType *pmInfo
)
{
  /*
   * Read identity from hardware.
   */
  pmInfo->nRevision = HWIO_INF(GICD_IIDR, REVISION);
  pmInfo->nImplementor = HWIO_INF(GICD_IIDR, IMPLEMENTOR);

  /*
   * Read type information from hardware.
   */
  pmInfo->nNumLockableSPI = HWIO_INF(GICD_TYPER, LSPI);
  pmInfo->nNumCPUInterfaces = HWIO_INF(GICD_TYPER, CPU_NUM) + 1;
  pmInfo->bTrustZoneSupport = HWIO_INF(GICD_TYPER, TZ);

  /*
   * Get the max number of interrupts.
   */
  pmInfo->nNumInterrupts = (HWIO_INF(GICD_TYPER, IT_LINES) + 1) * 32;
  if (pmInfo->nNumInterrupts > HAL_QGIC_IRQ_MAX)
  {
    pmInfo->nNumInterrupts = HAL_QGIC_IRQ_MAX;
  }

} /* END HAL_qgic_GetDistributorInfo */


