/*=============================================================================

FILE:         HalQdssTFunnel.c

DESCRIPTION:   
This file implements HAL functionalities for the Trace Funnel hardware block
within the QDSS system. The same HAL is to be used across any instance of the
funnel.

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*============================================================================
                          INCLUDE FILES
============================================================================*/

#include "HalQdssTFunnel.h"
#include "HwioQdssTFunnel.h"
#include "HalQdssTFunnelConfig.h"

#define HIGH 1
#define LOW  0

/**
* Configurable section which is specific to hardware implementation
* of QDSS in a chipset.
**/

#define HAL_QDSS_TFUNNEL_NUM_FUNNELS  QDSS_NUM_FUNNELS

static uint32 HAL_qdss_tfunnel_MappedBaseAddrs[
                                               HAL_QDSS_TFUNNEL_NUM_FUNNELS];

volatile uint32 QDSS_TFUNNEL_BASE_PTR;

//void HAL_qdss_tfunnel_NumFunnels(uint32 nNumFunnels)
//void HAL_qdss_tfunnel_MemAllocTable(uint32 pTableAddr)

//void HAL_qdss_tfunnel_HalInit(uint32 nBaseAddr, uint32 nFunnelId)
void HAL_qdss_tfunnel_HalInit(void)
{
  // Assign base addresses for each funnels
  HAL_qdss_tfunnel_MappedBaseAddrs[QDSS_IN_FUN0] = QDSS_IN_FUN0_BASE;
  HAL_qdss_tfunnel_MappedBaseAddrs[QDSS_IN_FUN1] = QDSS_IN_FUN1_BASE;
  HAL_qdss_tfunnel_MappedBaseAddrs[QDSS_MERG_FUN]= QDSS_MERG_FUN_BASE;
  return;
}

void HAL_qdss_tfunnel_UnlockAccess(uint32 nFunnelId)
{
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
    HWIO_OUT(QDSS_TFUNNEL_LOCKACCESS, 0xC5ACCE55);
  }
}

void HAL_qdss_tfunnel_UnlockAccessAll(void)
{
  int i;
  for (i = HAL_QDSS_TFUNNEL_NUM_FUNNELS - 1; i >= 0; i--)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[i];
    HWIO_OUT(QDSS_TFUNNEL_LOCKACCESS, 0xC5ACCE55);
  }
}

void HAL_qdss_tfunnel_LockAccessAll(void)
{
  int i;
  for (i = HAL_QDSS_TFUNNEL_NUM_FUNNELS - 1; i >= 0; i--)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[i];
    HWIO_OUT(QDSS_TFUNNEL_LOCKACCESS, ~0xC5ACCE55);
  }
}

void HAL_qdss_tfunnel_LockAccess(uint32 nFunnelId)
{
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
    HWIO_OUT(QDSS_TFUNNEL_LOCKACCESS, ~0xC5ACCE55);
  }
}


boolean HAL_qdss_tfunnel_isLocked(uint32 nFunnelId)
{
  QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
  return HWIO_INF(QDSS_TFUNNEL_LOCKSTATUS, LOCKGRANT);
}


boolean HAL_qdss_tfunnel_isPortEnabled(uint32 nPortId, uint32 nFunnelId)
{
  uint32 val = 0;

  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];

    val = HWIO_IN(QDSS_TFUNNEL_FUNCTL);
    val = (val & (1 << nPortId)) >> nPortId;
  }
  return val;
}

void HAL_qdss_tfunnel_EnablePort(uint32 nPortId, uint32 nFunnelId)
{
  uint32 val;

  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];

    val = HWIO_IN(QDSS_TFUNNEL_FUNCTL);
    val = (val & ~(0x1 << nPortId)) | (HIGH << nPortId);
    HWIO_OUT(QDSS_TFUNNEL_FUNCTL, val);
  }
}


#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_ALL_PORT_SHFT 0
void HAL_qdss_tfunnel_EnableAllPorts(uint32 nFunnelId)
{
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];

/* Enable all ports.
** Side effects of writing to a port-enable bit-field that does not exist needs to
be checked especially in Badger where a N-port funnel(N<8) will only contain N
fields in the register programming interface.
*/
    HWIO_OUTF(QDSS_TFUNNEL_FUNCTL, ENABLE_ALL_PORT, 0xff); /*Enable all 8-ports*/
  }
}

boolean HAL_qdss_tfunnel_isFunnelEnabled(uint32 nFunnelId)
{
  boolean ret = FALSE;
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
    if (HWIO_IN(QDSS_TFUNNEL_FUNCTL) & HWIO_FMSK(QDSS_TFUNNEL_FUNCTL, ENABLE_ALL_PORT))
    {
      ret = TRUE;
    }
  }
  return ret;
}

void HAL_qdss_tfunnel_DisablePort(uint32 nPortId, uint32 nFunnelId)
{
  uint32 val;
  nPortId = nPortId & 0x7;
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];

    val = HWIO_IN(QDSS_TFUNNEL_FUNCTL);
    val = (val & ~(0x1 << (HWIO_SHFT(QDSS_TFUNNEL_FUNCTL, ENABLE_SLAVE_PORT_1) * nPortId)))
        | (LOW << (HWIO_SHFT(QDSS_TFUNNEL_FUNCTL, ENABLE_SLAVE_PORT_1) * nPortId));

    HWIO_OUT(QDSS_TFUNNEL_FUNCTL, val);
  }
}

void HAL_qdss_tfunnel_DisableAllPorts(uint32 nFunnelId)
{
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
    HWIO_OUTF(QDSS_TFUNNEL_FUNCTL, ENABLE_ALL_PORT, 0x0); /*Disable all 8-ports*/
  }
}

void HAL_qdss_tfunnel_SetPortPriority(uint32 nPortId, uint32 nPriority,
                                      uint32 nFunnelId)
{
  uint32 val;
  nPortId &= 0x7; /*truncate value to 3 bits*/
  nPriority &= 0x7; /*truncate value to 3 bits*/

  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
    val = HWIO_IN(QDSS_TFUNNEL_PRICTL);
    val = (val & ~(0x7 << (HWIO_SHFT(QDSS_TFUNNEL_PRICTL, PRIORITY_PORT1) * nPortId)))
        | (nPriority << (HWIO_SHFT(QDSS_TFUNNEL_PRICTL, PRIORITY_PORT1) * nPortId));

    HWIO_OUT(QDSS_TFUNNEL_FUNCTL, val);
  }
}

/* FUNCTION HAL_qdss_tfunnel_SetClaimTag*/
void HAL_qdss_tfunnel_SetClaimTag(uint32 nTag, uint32 nFunnelId)
{
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
    HWIO_OUTF(QDSS_TFUNNELCLAIMSET, CLAIMSET, nTag);
  }
}

/* FUNCTION HAL_qdss_tfunnel_ClrClaimTag*/
void HAL_qdss_tfunnel_ClrClaimTag(uint32 nTag, uint32 nFunnelId)
{
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
    HWIO_OUTF(QDSS_TFUNNELCLAIMCLR, CLAIMCLR, nTag);
  }
}

uint32 HAL_qdss_tfunnel_ClaimTagImplementMask(uint32 nFunnelId)
{
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
  }
  return HWIO_FMSK(QDSS_TFUNNELCLAIMSET, CLAIMSET);
}

/* FUNCTION HAL_qdss_tfunnel_GetClaimTag */
uint32 HAL_qdss_tfunnel_GetClaimTag(uint32 nFunnelId)
{
  if (nFunnelId < HAL_QDSS_TFUNNEL_NUM_FUNNELS)
  {
    QDSS_TFUNNEL_BASE_PTR = HAL_qdss_tfunnel_MappedBaseAddrs[nFunnelId];
  }

  return HWIO_INF(QDSS_TFUNNELCLAIMCLR, CLAIMCLR);
}

void HAL_qdss_tfunnel_EnableAllPortsAllFunnels(void)
{
  int i;
  for (i = HAL_QDSS_TFUNNEL_NUM_FUNNELS - 1; i >= 0; i--)
  {
    HAL_qdss_tfunnel_EnableAllPorts(i);
  }
}


void HAL_qdss_tfunnel_DisableAllPortsAllFunnels(void)
{
  int i;
  for (i = HAL_QDSS_TFUNNEL_NUM_FUNNELS - 1; i >= 0; i--)
  {
    HAL_qdss_tfunnel_DisableAllPorts(i);
  }
}

void HAL_qdss_tfunnel_EnableStm()
{
  HAL_qdss_tfunnel_UnlockAccessAll();

  //Enable STM port of the In funnel for STM
  HAL_qdss_tfunnel_EnablePort(QDSS_IN_FUN_STM_PORT, QDSS_STM_IN_FUN);

  //Enable the Merge funnel port for the above in funnel
  HAL_qdss_tfunnel_EnablePort(QDSS_STM_IN_FUN, QDSS_MERG_FUN);

  HAL_qdss_tfunnel_LockAccessAll();
}

