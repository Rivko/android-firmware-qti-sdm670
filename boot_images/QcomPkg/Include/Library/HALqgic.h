#ifndef HAL_QGIC_H
#define HAL_QGIC_H
/*
===========================================================================

FILE:         HALqgic.h

DESCRIPTION:  
  This is the hardware abstraction layer interface for the Qualcomm
  Generic Interrupt Controller block.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
01/20/11   jz      Added Binary Point value.
07/26/07   gfr     Created.

===========================================================================
             Copyright (c) 2011 Qualcomm Technologies Incorporated.
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


/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */


/*
 * HAL_qgic_InterruptType
 *
 * Type definitions for QGIC interrupts.  This is a 32-bit number with
 * the following interpretation:
 *
 * 0-15:    SGI (software generated interrupts)
 * 16-31:   PPI (private peripheral interrupts)
 * 32-1021: SPI (shared peripheral interrupts)
 */
typedef uint32 HAL_qgic_InterruptType;


/*
 * HAL_QGIC_IRQ_*
 *
 * The macros are used to check the return value when acknowledging or
 * checking the highest priority interrupt.
 *
 *  HAL_QGIC_IRQ_SECURE_NA: Read is in secure mode, and only non-secure
 *                          interrupts are pending.
 *  HAL_QGIC_IRQ_NA:        No interrupts are pending.
 *  HAL_QGIC_IRQ_MAX:       Maximum number of interrupts.
 */
#define HAL_QGIC_IRQ_MAX         (1020)
#define HAL_QGIC_IRQ_SECURE_NA   (1022)
#define HAL_QGIC_IRQ_NA          (1023)


/*
 * HAL_qgic_PriorityType
 *
 * Type definitions for QGIC interrupt priority.  This is a 32-bit
 * number with a range from 0 (highest priority) to 255 (lowest priority).
 */
typedef uint32 HAL_qgic_PriorityType;


/*
 * HAL_QGIC_PRIORITY_*
 *
 * Macros to show the maximum and minimum priority levels supported by
 * the QGIC.
 *
 *  HAL_QGIC_PRIORITY_HIGHEST:    Highest priority level.
 *  HAL_QGIC_PRIORITY_LOWEST:     Lowest priority level.
 *  HAL_QGIC_PRIORITY_NULL:       Priority level that effectively disables
 *                                the interrupt.
 */
#define HAL_QGIC_PRIORITY_HIGHEST     (0)
#define HAL_QGIC_PRIORITY_LOWEST      (254)
#define HAL_QGIC_PRIORITY_NULL        (255)


/*
 * HAL_qgic_CPUIdType
 *
 * Type definitions for a QGIC CPU identifier.  This is a 32-bit number
 * indexing the CPU.
 */
typedef uint32 HAL_qgic_CPUIdType;


/*
 * HAL_qgic_TriggerType
 *
 * Enumeration of possible trigger types for an interrupt.
 *
 * HAL_QGIC_TRIGGER_LEVEL:   Interrupt is level sensitive.
 * HAL_QGIC_TRIGGER_EDGE:    Interrupt is edge sensitive.
 */
typedef enum
{
  HAL_QGIC_TRIGGER_LEVEL = 0,
  HAL_QGIC_TRIGGER_EDGE  = 1,

  HAL_ENUM_32BITS(QGIC_TRIGGER)
} HAL_qgic_TriggerType;


/*
 * HAL_qgic_SecureType
 *
 * The security type for an interrupt, either secure or not.
 */
typedef enum
{
  HAL_QGIC_NON_SECURE,
  HAL_QGIC_SECURE,

  HAL_ENUM_32BITS(QGIC_SECURE)
} HAL_qgic_SecureType;


/*
 * HAL_qgic_PortType
 *
 * Which interrupt port to the processor, either IRQ or FIQ.
 */
typedef enum
{
  HAL_QGIC_PORT_IRQ,
  HAL_QGIC_PORT_FIQ,

  HAL_ENUM_32BITS(QGIC_PORT)
} HAL_qgic_PortType;


/*
 * HAL_qgic_BinaryPointType
 *
 * Which binary point register is used to calculate pre-emption for non-secure
 * interrupts.
 *
 *  HAL_QGIC_BINARY_POINT_MODE_BANKED:     Use non-secure BPR.
 *  HAL_QGIC_BINARY_POINT_MODE_RESTRICTED: Use secure binary point register.
 */
typedef enum
{
  HAL_QGIC_BINARY_POINT_MODE_BANKED,
  HAL_QGIC_BINARY_POINT_MODE_RESTRICTED,

  HAL_ENUM_32BITS(QGIC_BINARY_POINT_MODE)
} HAL_qgic_BinaryPointModeType;


/*
 * HAL_GQIC_BINARYPOINT_VALUE
 *
 * Macros to define a set of value for binary point in HAL_qgic_SetBinaryPoint.
 *
 *  HAL_QGIC_BINARYPOINT_NO_PREEMPTION: No pre-emption
 */
#define HAL_QGIC_BINARYPOINT_NO_PREEMPTION  (7)


/*
 * HAL_GQIC_TARGET_x
 *
 * Macros to define a set of targets for an SGI.  Used in HAL_qgic_Trigger.
 *
 *  HAL_QGIC_TARGET_CPU(x): Target the given CPU (0-7)
 *  HAL_GQIC_TARGET_OTHERS: Target all CPUs but the local one.
 *  HAL_GQIC_TARGET_SELF:   Target only the current CPU.
 */
#define HAL_QGIC_TARGET_CPU(x)  (1 << (x))
#define HAL_QGIC_TARGET_OTHERS  (0x10000000)
#define HAL_QGIC_TARGET_SELF    (0x20000000)
#define HAL_QGIC_TARGET_ALL     (HAL_QGIC_TARGET_OTHERS | HAL_QGIC_TARGET_SELF)


/*
 * HAL_qgic_DistributorInfoType
 *
 * Structure containing information about the distributor.
 *
 *  nRevision:           Revision of the distributor.
 *  nImplementor:        Implementor ID (0x70 for Qualcomm)
 *  nNumLockableSPI:     Number of lockable shared peripheral interrupts.
 *  bTrustZoneSupport:   Whether trust-zone support is present.
 *  nNumCPUInterfaces:   Number of CPU interfaces.
 *  nNumInterrupts:      Number of interrupts supported.
 */
typedef struct
{
  uint32  nRevision;
  uint32  nImplementor;
  uint32  nNumLockableSPI;
  uint32  nNumCPUInterfaces;
  uint32  nNumInterrupts;
  boolean bTrustZoneSupport;
} HAL_qgic_DistributorInfoType;

/*
 * HAL_qgic_BaseAddressType
 *
 * Structure containing qgic register base addresses
 *
 * nGICDBaseAddress: Distributor register definition base
 * nGICCBaseAddress: CPU interface Register definition base
 * nGICHBaseAddress: Hypervisor base address
 * nGICVBaseAddress: Virtualization cpu base address
*/
typedef struct
{
  UINTN nGICDBaseAddress;
  UINTN nGICCBaseAddress;
  UINTN nGICHBaseAddress;
  UINTN nGICVBaseAddress;
} HAL_qgic_BaseAddressType;


/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

void HAL_qgic_Init               ( HAL_qgic_BaseAddressType *pBaseAddr );
void HAL_qgic_Reset              ( void );
void HAL_qgic_Save               ( void );
void HAL_qgic_Restore            ( void );

void HAL_qgic_EnableDistributor    ( void );
void HAL_qgic_DisableDistributor   ( void );
void HAL_qgic_EnableDistributorNS  ( void );
void HAL_qgic_DisableDistributorNS ( void );
void HAL_qgic_GetDistributorInfo   ( HAL_qgic_DistributorInfoType *pmInfo );

void HAL_qgic_EnableInterrupts    ( void );
void HAL_qgic_DisableInterrupts   ( void );
void HAL_qgic_EnableInterruptsNS  ( void );
void HAL_qgic_DisableInterruptsNS ( void );
void HAL_qgic_SetSecureDest       ( HAL_qgic_PortType ePort );
void HAL_qgic_SetSecureAckNS      ( boolean bSecureAckNS );
void HAL_qgic_SetPriorityMask     ( HAL_qgic_PriorityType nPriorityMask );
void HAL_qgic_SetBinaryPoint      ( uint32 nBinaryPoint );
void HAL_qgic_SetBinaryPointMode  ( HAL_qgic_BinaryPointModeType eBinaryPointMode );
void HAL_qgic_SetClockSecurity    ( HAL_qgic_SecureType eSecurity );

void HAL_qgic_AckInterrupt       ( HAL_qgic_InterruptType *pnInterrupt, HAL_qgic_CPUIdType *pnCPUId );
void HAL_qgic_EndOfInterrupt     ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_CPUIdType nCPUId );
void HAL_qgic_GetHighestPending  ( HAL_qgic_InterruptType *pnInterrupt, HAL_qgic_CPUIdType *pnCPUId );
void HAL_qgic_GetActivePriority  ( HAL_qgic_PriorityType *pnPriority );

void HAL_qgic_Enable             ( HAL_qgic_InterruptType nInterrupt );
void HAL_qgic_Disable            ( HAL_qgic_InterruptType nInterrupt );

void HAL_qgic_SetSecurity        ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_SecureType eSecurity );
void HAL_qgic_SetPriority        ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_PriorityType nPriority );
void HAL_qgic_SetTrigger         ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_TriggerType eTrigger );
void HAL_qgic_SetTargets         ( HAL_qgic_InterruptType nInterrupt, uint32 nTargets );

void HAL_qgic_SetSecurityAll     ( HAL_qgic_SecureType eSecurity );
void HAL_qgic_SetPriorityAll     ( HAL_qgic_PriorityType nPriority );

void HAL_qgic_SetPending         ( HAL_qgic_InterruptType nInterrupt );
void HAL_qgic_ClearPending       ( HAL_qgic_InterruptType nInterrupt );
void HAL_qgic_Generate           ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_SecureType eSecure, uint32 nTargets);

boolean HAL_qgic_IsEnabled       ( HAL_qgic_InterruptType nInterrupt );
boolean HAL_qgic_IsPending       ( HAL_qgic_InterruptType nInterrupt );
boolean HAL_qgic_IsActive        ( HAL_qgic_InterruptType nInterrupt );

boolean HAL_qgic_IsSGI           ( HAL_qgic_InterruptType nInterrupt );
boolean HAL_qgic_IsPPI           ( HAL_qgic_InterruptType nInterrupt );
boolean HAL_qgic_IsSPI           ( HAL_qgic_InterruptType nInterrupt );

void HAL_qgic_GetSecurity        ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_SecureType *peSecurity );
void HAL_qgic_GetPriority        ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_PriorityType *pnPriority );
void HAL_qgic_GetTrigger         ( HAL_qgic_InterruptType nInterrupt, HAL_qgic_TriggerType *peTrigger );


/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

HAL_qgic_Init( )

Description:
   This function is used to initialize the interrupt controller.  It must
   be called before any of the other functions will work.

===============================================================================

HAL_qgic_Reset( )

Description:
   This function is used to reset the interrupt controller to the hardware
   default state.  Once this is called, Init must be invoked before
   functionality is restored.

===============================================================================

HAL_qgic_EnableDistributor( )

Description:
   This function enables the distributor.  When invoked from a secure
   context it controls the secure interrupt processing side, and when
   invoked from a non-secure context it controls the non-secure side.
   The distributor must be enabled before any interrupts will work.

===============================================================================

HAL_qgic_DisableDistributor( )

Description:
   This function disables the distributor.  When invoked from a secure
   context it controls the secure interrupt processing side, and when
   invoked from a non-secure context it controls the non-secure side.
   If the distributor is disabled then no interrupts will be generated.

===============================================================================

HAL_qgic_EnableDistributorNS( )

Description:
   This function enables the non-secure distributor from secure mode.  It
   does nothing from the non-secure context.

===============================================================================

HAL_qgic_DisableDistributor( )

Description:
   This function disables the non-secure distributor from the secure mode.
   It does nothing from the non-secure context.

===============================================================================

HAL_qgic_GetDistributorInfo( )

Description:
   This function retrieves information about the distributor.

Return:
   *pmInfo - Structure containing details on the current distributor.

===============================================================================

HAL_qgic_EnableInterrupts( )

Description:
   This function enables the CPU interface to generate interrupts to the
   current core.  It must be invoked on each processor before any
   interrupt functionality will work.
   In secure context this enables secure interrupts.  In non-secure context
   it will enable non-secure interrupts.  Thus it generally will need to
   be invoked twice per CPU for full QGIC functionality.
   Note that the distributor must also be enabled for interrupts to be
   generated.

===============================================================================

HAL_qgic_DisableInterrupts( )

Description:
   This function disables the CPU interface to generate interrupts to the
   current core.
   In secure context this disables secure interrupts.  In non-secure context
   it will disable non-secure interrupts.

===============================================================================

HAL_qgic_EnableInterruptsNS( )

Description:
   This function allows the secure mode to enable the non-secure CPU
   interface to generate interrupts to the current core.
   It does nothing if invoked from the non-secure side.

===============================================================================

HAL_qgic_DisableInterruptsNS( )

Description:
   This function allows the secure mode to disable the non-secure CPU
   interface to generate interrupts to the current core.
   It does nothing if invoked from the non-secure side.

===============================================================================

HAL_qgic_SetSecureDest( )

Description:
   This function sets the destination port for secure interrupts.  This
   may be either the IRQ or FIQ port on the current processor.  It effects
   only the currently running core.
   This function can only be used in secure context.

Parameters:
   ePort - The port (IRQ or FIQ) to direct secure interrupts to.

===============================================================================

HAL_qgic_SetSecureAckNS( )

Description:
   This function sets whether or not an interrupt acknowledge from the
   secure domain will return a non-secure interrupt if that is the next
   highest pending one.
   This function can only be used in secure context.  By default secure
   acknowledge will not return a non-secure interrupt.

Parameters:
   bSecureAckNS - TRUE or FALSE depending on ack type desired.

===============================================================================

HAL_qgic_SetPriorityMask( )

Description:
   This function sets the priority mask for the current processor.  Any
   interrupts equal to or lower than the given mask value will not be
   seen by the processor.
   From non-secure context the maximum priority mask that can be set is
   128.

Parameters:
   nPriorityMask - The lowest priority interrupt that will be masked off.

===============================================================================

HAL_qgic_SetBinaryPoint( )

Description:
   This function sets the priority mask for the current processor.  This
   register is used to limit interrupts that can cause an interrupt request
   to the CPU based on the pending priority and the active priority.

Parameters:
   nBinaryPoint - The binary point register value.

===============================================================================

HAL_qgic_SetBinaryPointMode( )

Description:
   This function selects which binary point register is used for non-secure
   interrupts - either the non-secure BPR or the secure BPR.

   This function can only be invoked from the secure side.

Parameters:
   eBinaryPointMode - The binary point mode.

===============================================================================

HAL_qgic_SetClockSecurity( )

Description:
   This function sets the security mode for the QGIC clock control.  If set
   to non-secure then non-secure mode can set the clock override bits.

   This function can only be invoked from the secure side.

Parameters:
   eSecurity - The security mode.

===============================================================================

HAL_qgic_SetSecurity( )

Description:
   This function sets the secure or non-secure value for the given interrupt.
   It can only be used from a secure context.

Parameters:
   nInterrupt - Which interrupt to configure.
   eSecurity  - The security type.

===============================================================================

HAL_qgic_SetSecurityAll( )

Description:
   This function sets the secure or non-secure value for all interrupts.
   It can only be used from a secure context.  Since the default is for
   all interrupts to be secure it is expected this would only ever be
   called to set all interrupts to non-secure initially.

Parameters:
   eSecurity  - The security type.

===============================================================================

HAL_qgic_AckInterrupt( )

Description:
   This function is used to retrieve and acknowledge the highest priority
   pending interrupt.  The return value is:
     - HAL_QGIC_IRQ_NA if no more interrupts are pending,
     - HAL_QGIC_IRQ_SECURE_NA if the current processor is in secure mode
       and no more secure interrupts are available,
     - otherwise the interrupt number is returned.

Return:
   *pnInterrupt - Interrupt that was acknowledged and must now be processed.
   *pnCPUId     - For SGIs, this is the CPU that triggered the interrupt.
                  It is not valid for any other type of interrupt.

===============================================================================

HAL_qgic_EndOfInterrupt( )

Description:
   This function is used to inform the QGIC that interrupt processing is
   complete for the given interrupt.

Return:
   *pnInterrupt - Interrupt that was processed.
   *pnCPUId     - For SGIs, this is the CPU that triggered the interrupt
                  and should match was was returned in HAL_qgic_AckInterrupt.
                  It is not used for any other type of interrupt.

===============================================================================

HAL_qgic_GetHighestPending( )

Description:
   This function is used to check the highest priority pending
   interrupt.  See HAL_qgic_AckInterrupt for return value description.

Return:
   *pnInterrupt - Current highest priority pending interrupt.
   *pnCPUId     - For SGIs, this is the CPU that triggered the interrupt.
                  It is not valid for any other type of interrupt.

===============================================================================

HAL_qgic_GetActivePriority( )

Description:
   This function is used to get the highest priority of the interrupts
   currently being processed on the CPU.
   If secure interrupts are being processed, then non-secure use of this
   funtion will return '0' (highest priority) regardless of the priority
   of the secure interrupt.
   If no interrupts are active it will return HAL_QGIC_PRIORITY_NULL.

Return:
   *pnPriority - Current highest priority pending interrupt (subject to
                 security restraints, see description).

===============================================================================

HAL_qgic_Enable( )

Description:
   This function enables (unmasks) the given interrupt.

Parameters:
   nInterrupt - The interrupt to enable.

===============================================================================

HAL_qgic_Disable( )

Description:
   This function disables (masks) the given interrupt.

Parameters:
   nInterrupt - The interrupt to clear.

===============================================================================

HAL_qgic_SetTargets( )

Description:
   This function is used to set the target CPUs for the given interrupt.
   This only applies to SPIs, other interrupts will be ignored.

Parameters:
   nInterrupt - The interrupt to configure.
   nTargets   - The target CPUs for the interrupt.  Prepare by OR'ing the
                HAL_QGIC_TARGET_CPU(x) macro for each target CPU.

===============================================================================

HAL_qgic_SetTrigger( )

Description:
   This function is used to set the trigger type for the given interrupt.

Parameters:
   nInterrupt - The interrupt to set the trigger for.
   eTrigger   - The new trigger type for the interrupt.

===============================================================================

HAL_qgic_SetPriority( )

Description:
   This function is used to set the priority level for the given interrupt.

Parameters:
   nInterrupt - The interrupt to set the priority for.
   nPriority  - The new priority level for the interrupt.

===============================================================================

HAL_qgic_SetPriorityAll( )

Description:
   This function is used to set the priority level for all interrupts.  From
   non-secure mode this will only change priorities for non-secure interrupts.

Parameters:
   nPriority  - The new priority level for the interrupts.

===============================================================================

HAL_qgic_GetTrigger( )

Description:
   This function is used to get the current trigger type for the given
   interrupt.

Parameters:
   nInterrupt - The interrupt to get the trigger for.

Return:
   *peTrigger - The trigger type for the interrupt.

===============================================================================

HAL_qgic_GetSecurity( )

Description:
   This function is used to get the current security type for the given
   interrupt.

Parameters:
   nInterrupt - The interrupt to check.

Return:
   *peSecurity - The security type for the interrupt.

===============================================================================

HAL_qgic_GetPriority( )

Description:
   This function is used to get the priority for the given interrupt.

Parameters:
   nInterrupt - The interrupt to check.

Return:
   *pnPriority - The priority for the interrupt.

===============================================================================

HAL_qgic_SetPending( )

Description:
   This function will mark the given interrupt as pending in the
   distributor.  It only applies to SPIs and PPIs.  For triggering SGIs
   use HAL_qgic_Trigger.
   For PPIs only the current CPU is effected.

Parameters:
   nInterrupt - The interrupt to set as pending.

===============================================================================

HAL_qgic_ClearPending( )

Description:
   This function will clear the given interrupt as pending in the
   distributor.  It only applies to SPIs and PPIs.  Clearing SGIs
   requires AckInterrupt/EndOfInterrupt.
   For PPIs the state for the current CPU is returned.

Parameters:
   nInterrupt - The interrupt to clear as pending.

===============================================================================

HAL_qgic_IsPending( )

Description:
   This function checks if a given interrupt is pending to be serviced.
   This does not mean that it will be serviced, as it may be disabled.  In
   addition if either the CPU interface or distributor are disabled then
   interrupts may be pending but never go active.

Parameters:
   nInterrupt - The interrupt to check.

Return:
   boolean - TRUE if the interrupt is pending.

===============================================================================

HAL_qgic_IsActive( )

Description:
   This function checks if the given interrupt is active, i.e. currently
   being processed.  For SGIs and PPIs the result applies only to the local
   CPU.

Parameters:
   nInterrupt - The interrupt to check.

Return:
   boolean - TRUE if the interrupt is active.

===============================================================================

HAL_qgic_IsEnabled( )

Description:
   This function returns if the given interrupt is enabled or not.

Parameters:
   nInterrupt - The interrupt to check.

Return:
   boolean - TRUE if the interrupt is enabled.

===============================================================================

HAL_qgic_IsPPI( )

Description:
   This function returns if the given interrupt is a private peripheral
   interrupt.

Parameters:
   nInterrupt - The interrupt to check.

Return:
   boolean - TRUE if the interrupt is a PPI.

===============================================================================

HAL_qgic_IsSPI( )

Description:
   This function returns if the given interrupt is a shared peripheral
   interrupt.

Parameters:
   nInterrupt - The interrupt to check.

Return:
   boolean - TRUE if the interrupt is a SPI.

===============================================================================

HAL_qgic_IsSGI( )

Description:
   This function returns if the given interrupt is a software generated
   interrupt.

Parameters:
   nInterrupt - The interrupt to check.

Return:
   boolean - TRUE if the interrupt is a SGI.

===============================================================================

HAL_qgic_Save( )

Description:
   This function saves the current hardware context.

===============================================================================

HAL_qgic_Restore( )

Description:
   This function restores the hardware context saved in HAL_qgic_Save.

===============================================================================

HAL_qgic_Generate( )

Description:
   This function causes an SGI to fire on the given CPUs.  It only works on
   SGIs (software-generated-interrupts) which are numbers 0-15.

Parameters:
   nInterrupt - Which interrupt to generate.
   eSecure    - Whether to fire a secure or non-secure interrupt.  Only
                a secure context can fire a secure interrupt.
   nTargets   - Bitmask of which targets should get the interrupt.  See
                HAL_QGIC_TARGET_x macros for how to construct this.

===============================================================================
*/

#ifdef __cplusplus
}
#endif

#endif /* HAL_QGIC_H */

