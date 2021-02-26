#ifndef HAL_GPIOINT_H
#define HAL_GPIOINT_H
/*
===========================================================================

FILE:         HALgpioint.h

DESCRIPTION:  
  This is the hardware abstraction layer interface for the GPIO interrupt
  controller block.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/hal/gpioint/inc/HALgpioint.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
12/17/07   gfr     Added new GPIO interrupts for 8650.
09/26/07   gfr     Added new GPIO interrupts for 7625 and 7800.
07/26/07   gfr     Created.

===========================================================================
       Copyright (c) 2007-2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif


/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */

#include <HALcomdef.h>
#include "HALgpiointInterface.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/*
 * HAL_GPIOINT
 *
 * Bare enumeration of supported GPIO interrupts.  Not all platforms
 * support all interrupts.
 * This is basically just here to ensure that HAL_GPIOINT_NUM is set to
 * the maximum possible number of GPIO interrupts.
 */
enum
{
  HAL_GPIOINT_0 = 0,
  HAL_GPIOINT_1,
  HAL_GPIOINT_2,
  HAL_GPIOINT_3,
  HAL_GPIOINT_4,
  HAL_GPIOINT_5,
  HAL_GPIOINT_6,
  HAL_GPIOINT_7,
  HAL_GPIOINT_8,
  HAL_GPIOINT_9,
  HAL_GPIOINT_10,
  HAL_GPIOINT_11,
  HAL_GPIOINT_12,
  HAL_GPIOINT_13,
  HAL_GPIOINT_14,
  HAL_GPIOINT_15,
  HAL_GPIOINT_16,
  HAL_GPIOINT_17,
  HAL_GPIOINT_18,
  HAL_GPIOINT_19,
  HAL_GPIOINT_20,
  HAL_GPIOINT_21,
  HAL_GPIOINT_22,
  HAL_GPIOINT_23,
  HAL_GPIOINT_24,
  HAL_GPIOINT_25,
  HAL_GPIOINT_26,
  HAL_GPIOINT_27,
  HAL_GPIOINT_28,
  HAL_GPIOINT_29,
  HAL_GPIOINT_30,
  HAL_GPIOINT_31,
  HAL_GPIOINT_32,
  HAL_GPIOINT_33,
  HAL_GPIOINT_34,
  HAL_GPIOINT_35,
  HAL_GPIOINT_36,
  HAL_GPIOINT_37,
  HAL_GPIOINT_38,
  HAL_GPIOINT_39,
  HAL_GPIOINT_40,
  HAL_GPIOINT_41,
  HAL_GPIOINT_42,
  HAL_GPIOINT_43,
  HAL_GPIOINT_44,
  HAL_GPIOINT_45,
  HAL_GPIOINT_46,
  HAL_GPIOINT_47,
  HAL_GPIOINT_48,
  HAL_GPIOINT_49,
  HAL_GPIOINT_50,
  HAL_GPIOINT_51,
  HAL_GPIOINT_52,
  HAL_GPIOINT_53,
  HAL_GPIOINT_54,
  HAL_GPIOINT_55,
  HAL_GPIOINT_56,
  HAL_GPIOINT_57,
  HAL_GPIOINT_58,
  HAL_GPIOINT_59,
  HAL_GPIOINT_60,
  HAL_GPIOINT_61,
  HAL_GPIOINT_62,
  HAL_GPIOINT_63,
  HAL_GPIOINT_64,
  HAL_GPIOINT_65,
  HAL_GPIOINT_66,
  HAL_GPIOINT_67,
  HAL_GPIOINT_68,
  HAL_GPIOINT_69,
  HAL_GPIOINT_70,
  HAL_GPIOINT_71,
  HAL_GPIOINT_72,
  HAL_GPIOINT_73,
  HAL_GPIOINT_74,
  HAL_GPIOINT_75,
  HAL_GPIOINT_76,
  HAL_GPIOINT_77,
  HAL_GPIOINT_78,
  HAL_GPIOINT_79,
  HAL_GPIOINT_80,
  HAL_GPIOINT_81,
  HAL_GPIOINT_82,
  HAL_GPIOINT_83,
  HAL_GPIOINT_84,
  HAL_GPIOINT_85,
  HAL_GPIOINT_86,
  HAL_GPIOINT_87,
  HAL_GPIOINT_88,
  HAL_GPIOINT_89,
  HAL_GPIOINT_90,
  HAL_GPIOINT_91,
  HAL_GPIOINT_92,
  HAL_GPIOINT_93,
  HAL_GPIOINT_94,
  HAL_GPIOINT_95,
  HAL_GPIOINT_96,
  HAL_GPIOINT_97,
  HAL_GPIOINT_98,
  HAL_GPIOINT_99,
  HAL_GPIOINT_100,
  HAL_GPIOINT_101,
  HAL_GPIOINT_102,
  HAL_GPIOINT_103,
  HAL_GPIOINT_104,
  HAL_GPIOINT_105,
  HAL_GPIOINT_106,
  HAL_GPIOINT_107,
  HAL_GPIOINT_108,
  HAL_GPIOINT_109,
  HAL_GPIOINT_110,
  HAL_GPIOINT_111,
  HAL_GPIOINT_112,
  HAL_GPIOINT_113,
  HAL_GPIOINT_114,
  HAL_GPIOINT_115,
  HAL_GPIOINT_116,
  HAL_GPIOINT_117,
  HAL_GPIOINT_118,
  HAL_GPIOINT_119,
  HAL_GPIOINT_120,
  HAL_GPIOINT_121,
  HAL_GPIOINT_122,
  HAL_GPIOINT_123,
  HAL_GPIOINT_124,
  HAL_GPIOINT_125,
  HAL_GPIOINT_126,
  HAL_GPIOINT_127,
  HAL_GPIOINT_128,
  HAL_GPIOINT_129,
  HAL_GPIOINT_130,
  HAL_GPIOINT_131,
  HAL_GPIOINT_132,
  HAL_GPIOINT_133,
  HAL_GPIOINT_134,
  HAL_GPIOINT_135,
  HAL_GPIOINT_136,
  HAL_GPIOINT_137,
  HAL_GPIOINT_138,
  HAL_GPIOINT_139,
  HAL_GPIOINT_140,
  HAL_GPIOINT_141,
  HAL_GPIOINT_142,
  HAL_GPIOINT_143,
  HAL_GPIOINT_144,
  HAL_GPIOINT_145,
  HAL_GPIOINT_146,
  HAL_GPIOINT_147,
  HAL_GPIOINT_148,
  HAL_GPIOINT_149,
  HAL_GPIOINT_150,
  HAL_GPIOINT_151,
  HAL_GPIOINT_152,
  HAL_GPIOINT_153,
  HAL_GPIOINT_154,
  HAL_GPIOINT_155,
  HAL_GPIOINT_156,
  HAL_GPIOINT_157,
  HAL_GPIOINT_158,
  HAL_GPIOINT_159,
  HAL_GPIOINT_160,
  HAL_GPIOINT_161,
  HAL_GPIOINT_162,
  HAL_GPIOINT_163,
  HAL_GPIOINT_164,
  HAL_GPIOINT_165,
  HAL_GPIOINT_166,
  HAL_GPIOINT_167,
  HAL_GPIOINT_168,
  HAL_GPIOINT_169,
  HAL_GPIOINT_170,
  HAL_GPIOINT_171,
  HAL_GPIOINT_172,
  HAL_GPIOINT_173,
  HAL_GPIOINT_174,
  HAL_GPIOINT_175,
  HAL_GPIOINT_176,
  HAL_GPIOINT_177,
  HAL_GPIOINT_178,
  HAL_GPIOINT_179,
  HAL_GPIOINT_180,
  HAL_GPIOINT_181,
  HAL_GPIOINT_NUM
};


/*
 * HAL_GPIOINT_NONE
 *
 * Value returned by GetPending to indicate no more interrupts pending.
 */
#define HAL_GPIOINT_NONE  0xFFFF


/*
 * HAL_gpioint_TriggerType
 *
 * Enumeration of possible trigger types for a GPIO interrupt.
 *
 * HAL_PIC_TRIGGER_HIGH:    Interrupt is level sensitive, positive polarity.
 * HAL_PIC_TRIGGER_LOW:     Interrupt is level sensitive, negative polarity.
 * HAL_PIC_TRIGGER_RISING:  Interrupt is edge sensitive, positive polarity.
 * HAL_PIC_TRIGGER_FALLING: Interrupt is edge sensitive, negative polarity.
 */
typedef enum
{
  HAL_GPIOINT_TRIGGER_HIGH     = 0,
  HAL_GPIOINT_TRIGGER_LOW      = 1,
  HAL_GPIOINT_TRIGGER_RISING   = 2,
  HAL_GPIOINT_TRIGGER_FALLING  = 3,
  HAL_GPIOINT_TRIGGER_DUAL_EDGE  = 4
} HAL_gpioint_TriggerType;

typedef enum
{
  HAL_GPIOINT_MSS_PROC,      
  HAL_GPIOINT_SPS_PROC,    
  HAL_GPIOINT_LPA_DSP_PROC,        
  HAL_GPIOINT_RPM_PROC,        
  HAL_GPIOINT_SC_PROC,
  HAL_GPIOINT_WCN_PROC,
  HAL_GPIOINT_DEVICE_DSP,
  HAL_GPIOINT_TURING_PROC,
  HAL_GPIOINT_NONE_PROC
} HAL_gpioint_ProcessorType;

/*
 * HAL_gpioint_GroupType
 */
typedef enum
{
  HAL_GPIOINT_GROUP1 = 0,
  HAL_GPIOINT_GROUP2 = 1,
} HAL_gpioint_GroupType;

typedef struct
{
  void * pTargetData;
} HAL_gpioint_ContextType;

typedef struct
{
  HAL_gpioint_ProcessorType eProcessor;
  uint32*  nGPIOIntBase;
  uint32*  nGPIOIntBasePhys;
  uint32   nHalTargetProcVersion;
}HAL_gpioint_TargetDataType;

/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

void HAL_gpioint_Init            ( HAL_gpioint_ContextType * pTarget );
void HAL_gpioint_Reset           ( void );
void HAL_gpioint_Save            ( void );
void HAL_gpioint_Restore         ( void );

void HAL_gpioint_Enable          ( uint32 nGPIO );
void HAL_gpioint_Disable         ( uint32 nGPIO ); 
void HAL_gpioint_Clear           ( uint32 nGPIO );
void HAL_gpioint_EnableRawStatus ( uint32 nGPIO );
void HAL_gpioint_GetPending      ( HAL_gpioint_GroupType eGroup, uint32 *pnGPIO );
void HAL_gpioint_GetNumber       ( uint32 *pnNumber );
void HAL_gpioint_GetTrigger      ( uint32 nGPIO, HAL_gpioint_TriggerType *peTrigger );
void HAL_gpioint_GetGroup        ( uint32 nGPIO, HAL_gpioint_GroupType *peGroup);

void HAL_gpioint_SetTrigger      ( uint32 nGPIO, HAL_gpioint_TriggerType eTrigger );

boolean HAL_gpioint_IsSupported  ( uint32 nGPIO );
boolean HAL_gpioint_IsPending    ( uint32 nGPIO );
boolean HAL_gpioint_IsEnabled    ( uint32 nGPIO );
boolean HAL_gpioint_IsSet        ( uint32 nGPIO);
void HAL_gpioint_SetTargetProcessor      ( HAL_gpioint_ProcessorType eProcId, uint32 nGpio, HAL_gpioint_InterruptType eInterrupt );
void HAL_gpioint_SetDirConnIntrPolarity  ( HAL_gpioint_TriggerType eTrigger, uint32 nGPIO );
void HAL_gpioint_SetDirConnIntr          ( uint32 nGPIO, HAL_gpioint_InterruptType eDirConnIntr );
void HAL_gpioint_DisableDirConnIntr      ( uint32 nGPIO );
void HAL_gpioint_GetTargetProcessor      ( uint32 nGpio,HAL_gpioint_ProcessorType *peProcId);
void HAL_gpioint_TriggerInterrupt        ( uint32 nGPIO);
/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

HAL_gpioint_Init( )

Description:
   This function is used to initialize the GPIO interrupt controller.  It
   must be called before any of the other functions will work.

===============================================================================

HAL_gpioint_Reset( )

Description:
   This function is used to reset the GPIO interrupt controller to the
   hardware default state.  Once this is called, Init must be invoked before
   functionality is restored.

===============================================================================

HAL_gpioint_GetPending( )

Description:
   This function is used to retrieve the next pending GPIO interrupt in 
   the given group.
   The GPIO number is returned, or HAL_GPIOINT_NONE if no more GPIO
   interrupts are pending.

Parameters:
   eGroup - Which GPIO group to check

Return:
   *pnGPIO - Next pending GPIO, or HAL_GPIOINT_NONE

===============================================================================

HAL_gpioint_GetNumber( )

Description:
   This function returns the number of supported GPIO interrupts on this
   platform, which may be less than HAL_GPIOINT_NUM.

Return:
   *pnNumber - The number of interrupts supported.

===============================================================================

HAL_gpioint_GetGroup( )

Description:
   This function returns the group for the given GPIO interrupt.

Parameters:
   nGPIO - The GPIO interrupt to check.

Return:
   *peGroup - The group of the given interrupt.

===============================================================================

HAL_gpioint_Enable( )

Description:
   This function enables (unmasks) the given interrupt.

Parameters:
   nGPIO - The interrupt to enable.

===============================================================================

HAL_gpioint_Disable( )

Description:
   This function disables (masks) the given interrupt.

Parameters:
   nGPIO - The interrupt to enable.

===============================================================================

HAL_gpioint_Clear( )

Description:
   This function clears the given interrupt.

Parameters:
   nGPIO - The interrupt to enable.

===============================================================================

HAL_gpioint_SetTrigger( )

Description:
   This function is used to set the trigger type for the given interrupt.
   It should be invoked with interrupts locked.

Parameters:
   nGPIO    - The interrupt to set the trigger for.
   eTrigger - The new trigger type for the interrupt.

===============================================================================

HAL_gpioint_GetTrigger( )

Description:
   This function is used to get the current trigger type for the given
   interrupt.

Parameters:
   nGPIO - The GPIO interrupt to get the trigger for.

Return:
   *peTrigger - The trigger type for the interrupt.

===============================================================================

HAL_gpioint_IsPending( )

Description:
   This function checks if a given interrupt is waiting to be serviced.

Parameters:
   nGPIO - The GPIO interrupt to check.

Return:
   boolean - TRUE if the interrupt is pending.

===============================================================================

HAL_gpioint_IsEnabled( )

Description:
   This function returns if the given interrupt is enabled or not.

Parameters:
   nGPIO - The GPIO interrupt to check.

Return:
   boolean - TRUE if the interrupt is enabled.

===============================================================================

HAL_gpioint_IsSet( )

Description:
   This function returns if the given interrupt is fired or not.Regardless of it being enabled.

Parameters:
   nGPIO - The GPIO interrupt to check.

Return:
   boolean - TRUE if the interrupt is fired.

===============================================================================

HAL_gpioint_IsSupported( )

Description:
   This function returns if the given interrupt is supported on this platform.

Parameters:
   nGPIO - The GPIO interrupt to check.

Return:
   boolean - TRUE if the interrupt is supported.

===============================================================================

HAL_gpioint_Save( )

Description:
   This function saves the current hardware context.

===============================================================================

HAL_gpioint_Restore( )

Description:
   This function restores the hardware context saved in HAL_gpioint_Save.

===============================================================================

HAL_gpioint_SetTargetProcessor( )

Description:
   This function sets the target processor for the specified gpio interrupt.
Parameters:
   nGPIO - The GPIO interrupt to check.
   nProcId - The processor id to route the interrupt to.
   eInterrupt - The type of interrupt either a summary or a direct connect.
Return:
   None.

===============================================================================

HAL_gpioint_GetTargetProcessor( )

Description:
   This function gets the target processor for the specified gpio interrupt.
Parameters:
   nGpio - The GPIO interrupt to check the processor for.
   peProcId - The pointer passed in by the tramp driver this is populated by 
              the hal with the processor id to whom the interrupt is routed.
Return:
  None

===============================================================================
HAL_gpioint_SetDirConnIntr( )

Description:
   This function sets the Direct connect interrupt for the specified gpio.
Parameters:
   nGPIO - The GPIO number to associate with the direct connect interrupt.
   HAL_gpioint_InterruptType nDirConnIntr the direct connect interrupt number.
Return:
   None.

===============================================================================
HAL_gpioint_SetDirConnIntrPolarity( )

Description:
   This function sets the Direct connect interrupt polarity.
Parameters:
   HAL_gpioint_TriggerType  eTrigger - Polarity can be set from the Interrupt 
   trigger. 
   uint32 nGPIO - The Gpio number.
Return:
   None.

===============================================================================
HAL_gpioint_DisableDirConnIntr( )

Description:
   This function clears the Direct connect interrupt for the specified gpio.
   The gpio then defaults to the summary interrupt type.
Parameters:
   nGPIO - The GPIO number whos direct connect interrupt needs to be disabled.
Return:
   None.

===============================================================================
HAL_gpioint_EnableRawStatus( )

Description:
   In order for any SUMMARY GPIO interrupt to work the raw status bit of the cfg
register needs to be updated.
Parameters:
   nGPIO - The GPIO number whos interrupt needs to be enabled.
Return:
   None.

===============================================================================
*/

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIOINT_H */
