/*=========================================================================
File: HalQdssTFunnel.h   (was halqdss_tfunnel.h)

                       TRACE FUNNEL HAL

GENERAL DESCRIPTION
   The APIs contained in this file provide an interface to configure ALL trace
funnels in the QDSS hardware block in any chipset family. The QDSS block can
contain one or more trace funnels and the hardware HAL for any instance of the
funnel remains the same.
   The HAL funnel instance is identified by the funnel-id.

      Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*==========================================================================
 $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/HalQdssTFunnel.h#1 $
==========================================================================*/
#ifndef HALQDSSTFUNNEL_H_
#define HALQDSSTFUNNEL_H_

/*============================================================================
                          INCLUDE FILES
============================================================================*/

#include "HALcomdef.h"

/*============================================================================
                          PUBLIC FUNCTION
============================================================================*/

/*
Input: Number of funnels

Description:
   A QDSS based system can contain one or trace funnels that can all be
configured by the same HAL. Enumeration of the funnels at run time will be
performed over statically capturing this information at compile time.

*/
//void HAL_qdss_tfunnel_NumFunnels(uint32 nNumFunnels);
//void HAL_qdss_tfunnel_MemAllocTable(uint32 pTableAddr);
//void HAL_qdss_tfunnel_HalInit(uint32 nBaseAddr,uint32 nFunnelId);
void HAL_qdss_tfunnel_HalInit(void);


void HAL_qdss_tfunnel_UnlockAccess(uint32 nFunnelId);
void HAL_qdss_tfunnel_UnlockAccessAll(void);
void HAL_qdss_tfunnel_LockAccessAll(void);
void HAL_qdss_tfunnel_LockAccess(uint32 nFunnelId);

boolean HAL_qdss_tfunnel_isLocked(uint32 nFunnelId);

/*
Description:
   Return status of a specific port of a specific funnel.
*/
boolean HAL_qdss_tfunnel_isPortEnabled(uint32 nPortId, uint32 nFunnelId);


/*
Input:
   nPortId - Port identifier in the range of o to 7.
   nFunnelId - Unique funnel identifier in the range 0 to N-1, where N
indicates the number of trace funnels in the SoC
*/

void HAL_qdss_tfunnel_EnablePort(uint32 nPortId, uint32 nFunnelId);

/*
Function Name: HAL_qdss_tfunnel_EnableAllPorts

Input:
   nFunnelId - HAL funnel instance.

Description:
   Enables all ports in the funnel addressed by nFunnelId. When enabled, the
priority of all ports are set to their default state which is equal to their
corresponding port-id. For instance, Port 0's default priority is set to 0 and
Port 1's priority is set to 1, 0 being the highest priority.

Return: None
*/
void HAL_qdss_tfunnel_EnableAllPorts(uint32 nFunnelId);


boolean HAL_qdss_tfunnel_isFunnelEnabled(uint32 nFunnelId);

void HAL_qdss_tfunnel_DisablePort(uint32 nPortId, uint32 nFunnelId);
void HAL_qdss_tfunnel_DisableAllPorts(uint32 nFunnelId);

//void HAL_qdss_tfunnel_SetPortPriority(uint32 nPortId,uint32 nPriority,
//   uint32 nFunnelId);

/*
Description:
   Sets the priority of all 8-ports in ALL funnels to the lowest priority so
that the arbitration scheme follows a round-robin mechanism when selecting
ports.
*/
void HAL_qdss_tfunnel_SetAllPortsAllFunnelsPriorityEqual(void);


void HAL_qdss_tfunnel_SetClaimTag(uint32 nTag,uint32 nFunnelId);
void HAL_qdss_tfunnel_ClrClaimTag(uint32 nTag,uint32 nFunnelId);
uint32  HAL_qdss_tfunnel_ClaimTagImplementMask(uint32 nFunnelId);
uint32  HAL_qdss_tfunnel_GetClaimTag(uint32 nFunnelId);

void HAL_qdss_tfunnel_EnableAllPortsAllFunnels(void);
void HAL_qdss_tfunnel_DisableAllPortsAllFunnels(void);

void HAL_qdss_tfunnel_EnableStm(void);

#endif  /* HALQDSSTFUNNEL_H_ */

