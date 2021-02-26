/**

@file   HALnoc.c

@brief  Base file for HAL NOC.
        Intended to be used to check core revision, and call the correct
        implementation - mostly empty now as there is only one core
        revision.
        
===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/HALnoc.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2015/06/13   sds     Add ExtControl support
2011/08/15   sds     Created 

===========================================================================
          Copyright (c) 2011-2015 QUALCOMM Technologies, Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALnoc.h"
#include "HALnocHwio.h"
#include "HALnocHwioGeneric.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define LIMIT(val, field) (val)>(field##_BMSK >> field##_SHFT)?\
                                (field##_BMSK >> field##_SHFT):val

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** 
@copydoc HAL_noc_Init
*/
void HAL_noc_Init
(
  HAL_noc_InfoType *info
)
{
  /* Nothing to do. */
  (void)info;
}

/**
@copydoc HAL_noc_Reset
*/
void HAL_noc_Reset
(
  HAL_noc_InfoType *info
)
{
  /* Unimplemented */
  (void)info;
}

/**
@copydoc HAL_noc_Save
*/
void HAL_noc_Save
(
  HAL_noc_InfoType *info
)
{
  /* Unimplemented */
  (void)info;
}

/**
@copydoc HAL_noc_Restore
*/
void HAL_noc_Restore
(
  HAL_noc_InfoType *info
)
{
  /* Unimplemented */
  (void)info;
}

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_noc_SetDefaultPriority
*/
void HAL_noc_SetDefaultPriority
(
  HAL_noc_InfoType *info,
  uint32_t          master_port,
  uint32_t          priority
)
{
  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters )
  {
    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_MAINCTL_LOW, master_port, DFLTPRIORITY, priority);
  }
}

/**
@copydoc HAL_noc_SetLimiter
*/
void HAL_noc_SetLimiter
(
  HAL_noc_InfoType *   info,
  uint32_t             master_port,
  HAL_noc_LimiterType *config
)
{
  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters &&
      NULL != config )
  {
    uint32_t bandwidth = LIMIT(config->bandwidth,NOC_QOS_LIMITBW_LOW_BANDWIDTH);
    uint32_t saturation = LIMIT(config->saturation,NOC_QOS_LIMITBW_LOW_SATURATION);

    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_LIMITBW_LOW, master_port, BANDWIDTH, bandwidth);
    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_LIMITBW_LOW, master_port, SATURATION, saturation);
  }
}

/**
@copydoc HAL_noc_EnableLimiter
*/
void HAL_noc_EnableLimiter
(
  HAL_noc_InfoType *info,
  uint32_t          master_port,
  bool              enable
)
{
  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters )
  {
    uint32_t val = enable?1:0;

    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_MAINCTL_LOW, master_port, BWLIMITEN, val);
  }
}

/**
@copydoc HAL_noc_SetRegulator
*/
void HAL_noc_SetRegulator
(
  HAL_noc_InfoType *     info,
  uint32_t               master_port,
  HAL_noc_RegulatorType *config
)
{
  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters &&
      NULL != config)
  {
    uint32_t bandwidth  = LIMIT(config->bandwidth,NOC_QOS_REGUL0BW_LOW_BANDWIDTH);
    uint32_t saturation = LIMIT(config->saturation,NOC_QOS_REGUL0BW_LOW_SATURATION);

    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_REGUL0CTL_LOW, master_port, HIGHPRIORITY, config->high_priority);
    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_REGUL0CTL_LOW, master_port, LOWPRIORITY, config->low_priority);
    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_REGUL0BW_LOW, master_port, BANDWIDTH, bandwidth);
    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_REGUL0BW_LOW, master_port, SATURATION, saturation);
  }
}

/**
@copydoc HAL_noc_SetRegulatorMode
*/
void HAL_noc_SetRegulatorMode
(
  HAL_noc_InfoType *         info,
  uint32_t                   master_port,
  HAL_noc_RegulatorModeType *mode
)
{
  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters &&
      NULL != mode )
  {
    uint32_t rden = mode->read?1:0;
    uint32_t wren = mode->write?1:0;

    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_REGUL0CTL_LOW, master_port, RDEN, rden);
    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_REGUL0CTL_LOW, master_port, WREN, wren);
  }
}

/**
@copydoc HAL_noc_EnableUrgencyForwarding
*/
void HAL_noc_EnableUrgencyForwarding
(
  HAL_noc_InfoType *info,
  uint32_t          master_port,
  bool              enable
)
{
  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters )
  {
    uint32_t val = enable?1:0;

    NOC_OUTFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_MAINCTL_LOW, master_port, SLVURGMSGEN, val);
  }
}

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_noc_GetDefaultPriority
*/
bool HAL_noc_GetDefaultPriority
(
  HAL_noc_InfoType *info,
  uint32_t          master_port,
  uint32_t *        priority
)
{
  bool ret = false;

  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters &&
      NULL != priority )
  {
    *priority = 
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
                QOS_MAINCTL_LOW, master_port, DFLTPRIORITY);
    ret = true;
  }

  return ret;
}

/**
@copydoc HAL_noc_GetLimiter
*/
bool HAL_noc_GetLimiter
(
  HAL_noc_InfoType *   info,
  uint32_t             master_port,
  HAL_noc_LimiterType *config
)
{
  bool ret = false;

  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters )
  {
    config->bandwidth =
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
               QOS_LIMITBW_LOW, master_port, BANDWIDTH);
    config->saturation =
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
               QOS_LIMITBW_LOW, master_port, SATURATION);
    ret = true;
  }

  return ret;
}

/**
@copydoc HAL_noc_IsLimiterEnabled
*/
bool HAL_noc_IsLimiterEnabled
(
  HAL_noc_InfoType *info,
  uint32_t          master_port
)
{
  bool ret = false;

  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters )
  {
    ret =
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
               QOS_MAINCTL_LOW, master_port, BWLIMITEN) == 1;
  }

  return ret;
}

/**
@copydoc HAL_noc_GetRegulator
*/
bool HAL_noc_GetRegulator
(
  HAL_noc_InfoType *     info,
  uint32_t               master_port,
  HAL_noc_RegulatorType *config
)
{
  bool ret = false;

  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters )
  {
    config->high_priority =
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
               QOS_REGUL0CTL_LOW, master_port, HIGHPRIORITY);
    config->low_priority =
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
               QOS_REGUL0CTL_LOW, master_port, LOWPRIORITY);
    config->bandwidth =
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
               QOS_REGUL0BW_LOW, master_port, BANDWIDTH);
    config->saturation =
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
               QOS_REGUL0BW_LOW, master_port, SATURATION);
    ret = true;
  }

  return ret;
}

/**
@copydoc HAL_noc_GetRegulatorMode
*/
bool HAL_noc_GetRegulatorMode
(
  HAL_noc_InfoType *         info,
  uint32_t                   master_port,
  HAL_noc_RegulatorModeType *mode
)
{
  bool ret = false;

  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters &&
      NULL != mode )
  {
    mode->read = 
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_REGUL0CTL_LOW, master_port, RDEN) == 1;
    mode->write = 
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
              QOS_REGUL0CTL_LOW, master_port, WREN) == 1;
    ret = true;
  }

  return ret;
}

/**
@copydoc HAL_noc_IsUrgencyForwardingEnabled
*/
bool HAL_noc_IsUrgencyForwardingEnabled
(
  HAL_noc_InfoType *info,
  uint32_t          master_port
)
{
  bool ret = false;

  /* Verify input parameters. */
  if( NULL != info &&
      master_port < info->num_masters )
  {
    ret = 
      NOC_INFI(info->base_addr, info->qos_offset, info->qos_size,
               QOS_MAINCTL_LOW, master_port, SLVURGMSGEN);
  }

  return ret;
}

