#ifndef __HALVCSINTERNAL_H__
#define __HALVCSINTERNAL_H__
/*
==============================================================================

FILE:         HALcpuInternal.h

DESCRIPTION:
  Internal definitions for the cpu HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/hal/src/HALvcsInternal.h#1 $

==============================================================================
            Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALvcs.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/


/* ============================================================================
**    Typedefs
** ==========================================================================*/


/*
 * HAL_vcs_LDOControlType
 *
 * Set of function pointers used to control an LDO.
 *
 * Config     - Program the LDO's config registers.
 * Enable     - Enable the given LDO.
 * Disable    - Disable the given LDO.
 * IsEnabled  - Read HW status.
 * SetVoltage - Set LDO voltage.
 * GetVoltage - Get LDO voltage.
 */
typedef struct HAL_vcs_LDOControlType
{
  void    (*Config)     ( HAL_vcs_LDODescType *pDesc, HAL_vcs_LDOConfigType *pConfig );
  void    (*Enable)     ( HAL_vcs_LDODescType *pDesc );
  void    (*Disable)    ( HAL_vcs_LDODescType *pDesc );
  boolean (*IsEnabled)  ( HAL_vcs_LDODescType *pDesc );
  void    (*SetVoltage) ( HAL_vcs_LDODescType *pDesc, uint32 nVoltageUV );
  void    (*GetVoltage) ( HAL_vcs_LDODescType *pDesc, uint32 *pVoltageUV, uint32 *pVoltageHW);
} HAL_vcs_LDOControlType;


/*
 * HAL_vcs_CoprocControlType
 *
 * Set of function pointers used to control a coproc.
 *
 * Enable    - Enable the coproc.
 * Disable   - Disable the coproc.
 * IsEnabled - Read HW status.
 */
typedef struct HAL_vcs_CoprocControlType
{
  void    (*Enable)    ( HAL_vcs_CoprocDescType *pDesc );
  void    (*Disable)   ( HAL_vcs_CoprocDescType *pDesc );
  boolean (*IsEnabled) ( HAL_vcs_CoprocDescType *pDesc );
} HAL_vcs_CoprocControlType;


/*
 * HAL_vcs_LDODescType
 *
 * Description of an LDO, including control functions.
 *
 * nAddr     - Base address register for this LDO.
 * pmControl - Pointer to the control functions for this source.
 */
typedef struct HAL_vcs_LDODescType
{
  uint32                  nAddr;
  HAL_vcs_LDOControlType *pmControl;
} HAL_vcs_LDODescType;


/*
 * HAL_vcs_CoprocDescType
 *
 * Description of a coproc, including control functions.
 *
 * nAddr     - Base address register for this coproc.
 * pmControl - Pointer to the control functions for this source.
 */
typedef struct HAL_vcs_CoprocDescType
{
  uint32                     nAddr;
  HAL_vcs_CoprocControlType *pmControl;
} HAL_vcs_CoprocDescType;


/*============================================================================

               FUNCTION DECLARATIONS FOR MODULE

============================================================================*/


void HAL_vcs_BusyWait
(
  uint32 nTimeUS
);


#endif /* !HALVCSINTERNAL_H */

