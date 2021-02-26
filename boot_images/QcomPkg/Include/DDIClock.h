#ifndef DDICLOCK_H
#define DDICLOCK_H
/*
===========================================================================
*/
/**
  @file DDIClock.h
  @brief Public interface include file for accessing the clock device driver.

  The DDIClock.h file is the public API interface to the clock
  Device Access Library (DAL).
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The Clock_API_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and LaTeX. To edit or
      update any of the file/group text in the PDF, edit the
      Clock_API_mainpage.dox file or contact Tech Pubs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/DDIClock.h#2 $
  $DateTime: 2017/09/29 23:30:49 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  05/17/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALSys.h"
#include "com_dtypes.h" /* for "boolean" */
#include "ClockDefs.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/** @addtogroup clock_functions
@{ */

/**
  Defines the interface version.
 */
#define DALCLOCK_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

/** @} */ /* end_addtogroup clock_functions */

/*=========================================================================
      Interface Definitions
==========================================================================*/

/** @cond
*/

typedef struct DalClock DalClock;
struct DalClock
{
   struct DalDevice DalDevice;
   DALResult (*GetClockId)(DalDeviceHandle * _h,  const char * szClock,  ClockIdType * pnId);
   DALResult (*EnableClock)(DalDeviceHandle * _h,  ClockIdType  nClock);
   DALResult (*DisableClock)(DalDeviceHandle * _h,  ClockIdType  nClock);
   DALResult (*SetClockFrequency)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nFreq,  ClockFrequencyType  eMatch,  uint32 * pnResultFreq);
   DALResult (*SetClockDivider)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nDivider);
   DALResult (*InvertClock)(DalDeviceHandle * _h,  ClockIdType  nClock,  boolean  bInvert);
   DALResult (*ResetClock)(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockResetType  eReset);
   DALResult (*ResetClock_STM)(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockResetType  eReset);
   DALResult (*IsClockEnabled)(DalDeviceHandle * _h,  ClockIdType  nClock,  boolean * pbIsOn);
   DALResult (*IsClockOn)(DalDeviceHandle * _h,  ClockIdType  nClock,  boolean * pbIsOn);
   DALResult (*GetClockFrequency)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 * pnFrequencyHz);
   DALResult (*SelectExternalSource)(DalDeviceHandle * _h, ClockIdType nClock, uint32 nFreqHz, uint32 nSource, uint32 nDiv2x, uint32 nM, uint32 nN, uint32 n2D);
   DALResult (*GetPowerDomainId)(DalDeviceHandle * _h,  const char * szPowerDomain,  ClockPowerDomainIdType * pnPowerDomainId);
   DALResult (*EnablePowerDomain)(DalDeviceHandle * _h,  ClockPowerDomainIdType  nPowerDomainId);
   DALResult (*DisablePowerDomain)(DalDeviceHandle * _h,  ClockPowerDomainIdType  nPowerDomainId);
   DALResult (*EnableDCVS)(DalDeviceHandle * _h);
   DALResult (*EnableAVS)(DalDeviceHandle * _h);
   DALResult (*LoadNV)(DalDeviceHandle * _h);
   DALResult (*ProcessorSleep)(DalDeviceHandle * _h,  ClockSleepModeType  eMode,  uint32  nFlags);
   DALResult (*ProcessorRestore)(DalDeviceHandle * _h,  ClockSleepModeType  eMode,  uint32  nFlags);
   DALResult (*EnterLowPowerMode)(DalDeviceHandle * _h);
   DALResult (*ExitLowPowerMode)(DalDeviceHandle * _h);
   DALResult (*CalcClockFrequency)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 * pnFrequencyHz);
   DALResult (*ConfigClock)(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockConfigType  eConfig);
   DALResult (*SetUARTBaudClock)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 nDI, uint32 nFI, uint32 * pnFrequencyHz);
   DALResult (*SetVoltage)(DalDeviceHandle * _h,  ClockVRegIdType eVRegID, uint32 nVoltageUV, boolean *bIsLDOEnabled);
   DALResult (*LogState)(DalDeviceHandle * _h,  void *pULog,  uint32 nFlags);
   DALResult (*GetSourceId)(DalDeviceHandle * _h,  const char * szSource,  SourceIdType * pnId);
   DALResult (*EnableSource)(DalDeviceHandle * _h,  SourceIdType  nSource);
   DALResult (*DisableSource)(DalDeviceHandle * _h,  SourceIdType  nSource);
   DALResult (*SetSourceFrequency)(DalDeviceHandle * _h,  SourceIdType  nSource,  uint32  nFreq,  ClockFrequencyType  eMatch,  uint32 * pnResultFreq);
   DALResult (*GetSourceFrequency)(DalDeviceHandle * _h,  SourceIdType  nSource,  uint32 * pnFrequencyHz);
   DALResult (*ResetClockBranch)(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockResetType  eReset);
   DALResult (*GetDividerId)(DalDeviceHandle * _h,  const char * szDivider,  ClockDividerIdType * pnId);
   DALResult (*SetDivider)(DalDeviceHandle * _h,  ClockDividerIdType  nDividerId,  uint32  nDivider);
   DALResult (*LogClocksState)(DalDeviceHandle * _h,  uint32 nLogMask);
   DALResult (*DumpClocksState)(DalDeviceHandle * _h,  uint32 nLogMask);
   DALResult (*GetMaxPerfLevel)(DalDeviceHandle * _h, uint32 nCPU, uint32 *pnPerfLevel);
   DALResult (*GetMinPerfLevel)(DalDeviceHandle * _h, uint32 nCPU, uint32 *pnPerfLevel);
   DALResult (*GetCpuPerfLevelFrequency)(DalDeviceHandle * _h, uint32 nCPU, uint32 nPerfLevel, uint32 *pnFrequencyHz, uint32 *pnRequiredVoltage);
   DALResult (*SetCpuPerfLevel)(DalDeviceHandle * _h, uint32 nCPU, uint32 nPerfLevel, uint32 *pnFrequencyHz);
   DALResult (*GetCpuPerfLevel)(DalDeviceHandle * _h, uint32 nCPU, uint32 *pnPerfLevel);
};

typedef struct DalClockHandle DalClockHandle;
struct DalClockHandle
{
   uint32 dwDalHandleId;
   const DalClock * pVtbl;
   void * pClientCtxt;
   uint32 dwVtblen;
};

#define DAL_ClockDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALCLOCK_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/*=========================================================================
      Interface Declarations
==========================================================================*/

/** @addtogroup clock_functions
@{ */

/* =========================================================================
**  Function : DalClock_GetClockId
** =========================================================================*/
/**
  Looks up a clock ID for a specified clock name.

  This function takes the name of a clock and returns a handle that
  can be used for making requests on that clock. Usually, this function
  must be the first to be invoked prior to using any other clock functions.
  The string name of the clock can be found in the software frequency plan,
  e.g., cc_uart2_clk.

  @param _h        [in]  Handle to the DAL device.
  @param szClock   [in]  String name of the clock.
  @param pnId      [out] Pointer to the clock ID to be filled in.

  @return
  DAL_SUCCESS -- Clock was found and the ID was filled in. \n
  DAL_ERROR_NOT_FOUND -- Clock was not found on this target. \n
  DAL_ERROR_INVALID_PARAMETER -- Pointer to clock name or ID is null.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_GetClockId(DalDeviceHandle * _h,  const char * szClock,  ClockIdType * pnId)
{
   return ((DalClockHandle *)_h)->pVtbl->GetClockId( _h, szClock, pnId);
}


/* =========================================================================
**  Function : DalClock_EnableClock
** =========================================================================*/
/**
  Enables a clock.

  This function enables a specified clock.
  The clock is only turned on when the reference count goes from 0 to 1.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock to enable.

  @return
  DAL_SUCCESS -- Clock was enabled. \n
  DAL_ERROR_INTERNAL -- Unable to enable clock. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_EnableClock(DalDeviceHandle * _h,  ClockIdType  nClock)
{
   return ((DalClockHandle *)_h)->pVtbl->EnableClock( _h, nClock);
}


/* =========================================================================
**  Function : DalClock_DisableClock
** =========================================================================*/
/**
  Disables a clock.

  This function removes the client's vote to enable a specified clock.
  The clock will only be disabled when the reference count reaches 0.


  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock to disable.

  @return
  DAL_SUCCESS -- Call was successful (although the clock may still be enabled). \n
  DAL_ERROR_INTERNAL -- Unable to disable clock. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_DisableClock(DalDeviceHandle * _h,  ClockIdType  nClock)
{
   return ((DalClockHandle *)_h)->pVtbl->DisableClock( _h, nClock);
}


/* =========================================================================
**  Function : DalClock_SetClockFrequency
** =========================================================================*/
/**
  Sets the frequency of a clock.

  This function requests a frequency change for a specified clock. The
  driver selects the exact frequency based on the matching argument,
  which allows requesting a minimum or approximate frequency. There
  is no support for concurrent requests; the last request will be serviced
  and override any other minimum frequency requests.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock for which to set the frequency.
  @param nFreq   [in] Clock frequency in Hz, kHz, or MHz, depending on the
                      eMatch parameter.
  @param eMatch  [in] Type of match request.
  @param pnResultFreq [out] The programmed clock frequency in the same
                            units as nFreq; can be NULL if the resulting
                            frequency does not need to be checked. Note that
                            the result is truncated to the units requested.

  @return
  DAL_SUCCESS -- Requested clock frequency was found and programmed. \n
  DAL_ERROR_INTERNAL -- Unable to set clock config or invalid domain. \n
  DAL_ERROR_NOT_SUPPORTED -- A matching frequency was not found or domain
                             config is null. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid clock ID.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_SetClockFrequency(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nFreq,  ClockFrequencyType  eMatch,  uint32 * pnResultFreq)
{
   return ((DalClockHandle *)_h)->pVtbl->SetClockFrequency( _h, nClock, nFreq, eMatch, pnResultFreq);
}


/* =========================================================================
**  Function : DalClock_SetClockDivider
** =========================================================================*/
/**
  Sets the divider for a clock.

  This function programs the divider for a specified clock. This
  functionality only applies to a small subset of clocks that have
  independent dividers off the main domain.

  @param _h       [in] Handle to the DAL device.
  @param nClock   [in] Clock for which to set the divider.
  @param nDivider [in] Clock divider in logical units; 0 is invalid.

  @return
  DAL_SUCCESS -- Requested clock ID was valid. Divider value may or may not
  be correct. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_SetClockDivider(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nDivider)
{
   return ((DalClockHandle *)_h)->pVtbl->SetClockDivider( _h, nClock, nDivider);
}


/* =========================================================================
**  Function : DalClock_SetUARTBaudClock
** =========================================================================*/
/**
  Sets UART baud clock frequency.

  This function computes and sets UART clock frequency based on the input
  parameters. This function takes the source and divider values from the
  active configuration of the reference SIM clock, and computes the M and N
  values for the clock to set.
  The function should only be called to configure UART or GSBI UART clock
  frequency.  DalClock_SetClockFrequency should be called for other common
  clocks.

  @param _h            [in]  Handle to the DAL device.
  @param nClock        [in]  Clock to set.
  @param nDI           [in]  UIM baud rate adjustment factor
  @param nFI           [in]  UIM clock rate conversion factor
  @param pnFrequencyHz [out] Pointer to uint32 to fill in with the frequency.
                             A value of zero indicates that the frequency
                             could not be determined. This is an optional
                             parameter which can be null.

  @return
  DAL_SUCCESS -- Clock frequency was calculated successfully. \n
  DAL_ERROR_INTERNAL -- Unable to set clock config or invalid clock and
                        RefClock domain or config. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock or RefClock ID was invalid. \n
  DAL_ERROR_INSUFFICIENT_MEMORY -- Unable to allocate memory for clock config.

  @sideeffects
  During the calculation, the clock is enabled.

  @dependencies
  The clock being tested must be enabled or controllable from the current
  processor.

  @sa
  DalClock_SetClockFrequency
*/

static __inline DALResult
DalClock_SetUARTBaudClock(DalDeviceHandle * _h,  ClockIdType nClock, uint32 nDI, uint32 nFI, uint32 *pnFrequencyHz)
{
   return ((DalClockHandle *)_h)->pVtbl->SetUARTBaudClock( _h, nClock, nDI, nFI, pnFrequencyHz);
}


/* =========================================================================
**  Function : DalClock_InvertClock
** =========================================================================*/
/**
  Sets the Inversion state for a clock.

  This function allows inverting or uninverting a clock. This is rarely
  used. It is generally used just to overcome timing problems in the hardware.

  @param _h       [in] Handle to the DAL device.
  @param nClock   [in] Clock for which to set the Inversion state.
  @param bInvert  [in] Whether to invert the clock or not.

  @return
  DAL_SUCCESS -- Clock inversion was successfully applied. \n
  DAL_ERROR_NOT_SUPPORTED -- Function currently not supported. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid Clock ID.


  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_InvertClock(DalDeviceHandle * _h,  ClockIdType  nClock,  boolean  bInvert)
{
  return ((DalClockHandle *)_h)->pVtbl->InvertClock( _h, nClock, bInvert);
}


/* =========================================================================
**  Function : DalClock_ResetClock
** =========================================================================*/
/**
  Controls the reset assertion for a clock functional group.

  This function allows resetting hardware in the functional group of the
  specified clock, also known as a 'block-level' reset. All clocks within the
  same functional group share the same reset, so the reset only needs to be
  applied to a single clock within that group. The resetting is destructive
  to the hardware and requires reconfiguration after the reset is deasserted.

  Note:
    This is the preferred mechanism for resetting clocks and should always
    be used over DalClock_ResetClockBranch(), except in cases where the
    HW block-level reset is broken.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock for which to set the Reset state.
  @param eReset  [in] Type of reset: assert, deassert, or pulse.

  @return
  DAL_SUCCESS -- Clock reset was successfully applied. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_ResetClock(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockResetType  eReset)
{
   return ((DalClockHandle *)_h)->pVtbl->ResetClock( _h, nClock, eReset);
}


/* =========================================================================
**  Function : DalClock_ResetClock_STM
** =========================================================================*/
/**
  This API doesn't use the Mutex protection.
  This Function should be used only in STM (single thread mode)
  and error handler safe functions.

  Controls the reset assertion for a clock domain.

  This function allows resetting the hardware in a specified clock domain. The
  resetting is destructive to the hardware and requires reconfiguration
  after the reset is deasserted.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock for which to set the Reset state.
  @param eReset  [in] Type of reset: assert, deassert, or pulse.

  @return
  DAL_SUCCESS -- Clock reset was successfully applied. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_ResetClock_STM(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockResetType  eReset)
{
   return ((DalClockHandle *)_h)->pVtbl->ResetClock_STM( _h, nClock, eReset);
}


/* =========================================================================
**  Function : DalClock_ResetClockBranch
** =========================================================================*/
/**
  Controls the reset assertion for a clock branch.

  This function allows resetting the hardware on the specified clock branch. The
  resetting is destructive to the hardware and requires reconfiguration
  after the reset is deasserted.

  Note:
    This function should only be used in cases where the block-level reset
    asserted in DalClock_ResetClock() is broken.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock for which to set the Reset state.
  @param eReset  [in] Type of reset: assert, deassert, or pulse.

  @return
  DAL_SUCCESS -- Clock reset was successfully applied. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_ResetClockBranch(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockResetType  eReset)
{
   return ((DalClockHandle *)_h)->pVtbl->ResetClockBranch( _h, nClock, eReset);
}


/* =========================================================================
**  Function : DalClock_IsClockEnabled
** =========================================================================*/
/**
  Checks the enabled state of a clock.

  This function checks whether a specified clock is enabled or disabled. Note
  that this is not necessarily equivalent to the clock being active. A
  clock may be disabled by the local processor, but enabled by another one.
  Or if the hardware is misconfigured the clock may be enabled, but not
  actively toggling.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock to check.

  @return
  TRUE -- Clock is enabled. \n
  FALSE -- Clock is not enabled, or identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId(), DalClock_IsClockOn()
*/

static __inline boolean DalClock_IsClockEnabled
(
  DalDeviceHandle * _h,
  ClockIdType  nClock
)
{
  boolean bIsEnabled = FALSE;
  (void) ((DalClockHandle *)_h)->pVtbl->IsClockEnabled( _h, nClock, &bIsEnabled);
  return bIsEnabled;
}


/* =========================================================================
**  Function : DalClock_IsClockOn
** =========================================================================*/
/**
  Checks whether a clock is on.

  If the clock is on, it means the clock is not just enabled, but is
  actually toggling.

  @param _h        [in] Handle to the DAL device.
  @param nClock    [in] Clock to check.

  @return
  TRUE -- Clock is on. \n
  FALSE -- Clock is not on, or identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId(), DalClock_IsClockEnabled()
*/

static __inline boolean DalClock_IsClockOn
(
  DalDeviceHandle * _h,
  ClockIdType  nClock
)
{
  boolean bIsOn = FALSE;
  (void)((DalClockHandle *)_h)->pVtbl->IsClockOn( _h, nClock, &bIsOn);
  return bIsOn;
}


/* =========================================================================
**  Function : DalClock_GetClockFrequency
** =========================================================================*/
/**
  Gets the frequency of a clock.

  This function retrieves the current frequency of a clock in Hz. This
  is independent of whether or not the clock is running. Note that only clocks
  controlled on the local processor can have the frequency retrieved.

  @param _h            [in]  Handle to the DAL device.
  @param nClock        [in]  Clock to check.
  @param pnFrequencyHz [out] Pointer to uint32 to fill in with the frequency.
                             A value of zero indicates that the frequency
                             could not be determined.

  @return
  DAL_SUCCESS -- Clock was valid. The frequency may or may not have been
                 retrieved (see function description). \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier or frequency was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_GetClockFrequency(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 * pnFrequencyHz)
{
   return ((DalClockHandle *)_h)->pVtbl->GetClockFrequency( _h, nClock, pnFrequencyHz);
}


/* =========================================================================
**  Function : DalClock_SelectExternalSource
** =========================================================================*/
/**
  Selects an external source for a clock.

  This function selects an external source for a clock and configures
  the clock generator dividers. This is only necessary for clock domains
  that can be derived from a source outside the clock block such that
  DalClock_SetClockFrequency() cannot be used.
  Note that the caller must specify a non-zero value for the source clock's
  frequency if the caller expects the clock driver to provide voltage scaling
  support.

  @param _h        [in] Handle to the DAL device.
  @param nClock    [in] Clock to configure.
  @param nFreqHz   [in] Source frequency in Hz. The value '0' indicates that
                        voltage scaling is not expected for this source.
  @param nSource   [in] Source to use. This value should match the MUX input
                        on which the external source is connected.
                        @note1 If the specified clock does not support the
                        requested external source, this request selects the first
                        source (in most cases XO) available to the clock.
  @param nDiv2x    [in] Integer divider to use (2x the desired divider).
  @param nM        [in] The M value for any M/N counter (0 to bypass).
                        Not all clock domains have M/N counters; these
                        values will be ignored in such cases.
  @param nN        [in] The N value for any M/N counter.
  @param n2D       [in] Twice the D value for any M/N counter. The raw value
                        is doubled to allow for half-integer duty cycles.

  @return
  DAL_SUCCESS -- Clock was valid, and external source was selected. \n
  DAL_ERROR_INTERNAL -- Invalid domain. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid clock identifier or external source.

  @dependencies
  Any voltage requirements for the clock must be specified using the nFreqHz
  parameter.

  @sa
  DalClock_SetClockFrequency()
*/

static __inline DALResult
DalClock_SelectExternalSource(DalDeviceHandle * _h, ClockIdType nClock, uint32 nFreqHz, uint32 nSource, uint32 nDiv2x, uint32 nM, uint32 nN, uint32 n2D)
{
   return ((DalClockHandle *)_h)->pVtbl->SelectExternalSource( _h, nClock, nFreqHz, nSource, nDiv2x, nM, nN, n2D);
}


/* =========================================================================
**  Function : DalClock_ConfigClock
** =========================================================================*/
/**
  Configures a clock.

  This function configures a clock based on predefined configuration modes.

  @param _h        [in] Handle to the DAL device.
  @param nClock    [in] Clock to check.
  @param eConfig   [in] Clock configuration.

  @return
  DAL_SUCCESS -- Call was successful.\n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid. \n
  DAL_ERROR_OUT_OF_RANGE_PARAMETER -- Configuration is not supported/invalid.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_ConfigClock(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockConfigType  eConfig)
{
  return ((DalClockHandle *)_h)->pVtbl->ConfigClock( _h, nClock, eConfig);
}


/* =========================================================================
**  Function : DalClock_GetSourceId
** =========================================================================*/
/**
  Looks up a source ID for a specified source name.

  This function takes the name of a source and returns a handle that
  can be used for making requests on that source. Usually, this function
  must be the first to be invoked prior to using any other source functions.
  The string name of the source can be found in the software frequency plan,
  e.g., xo.

  @param _h        [in]  Handle to the DAL device.
  @param szSource  [in]  String name of the source.
  @param pnId      [out] Pointer to the source ID to be filled in.

  @return
  DAL_SUCCESS -- Source was found and the ID was filled in. \n
  DAL_ERROR_NOT_FOUND -- Source was not found on this target. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid source name or ID.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_GetSourceId(DalDeviceHandle * _h,  const char * szSource,  SourceIdType * pnId)
{
   return ((DalClockHandle *)_h)->pVtbl->GetSourceId( _h, szSource, pnId);
}


/* =========================================================================
**  Function : DalClock_EnableSource
** =========================================================================*/
/**
  Enables a source.

  This function enables a specified source.
  The source is only turned on when the first reference count goes from 0 to 1.

  Note: The clock driver flags sources whose voltage and source requirements
  are always externally satisfied. In this case the clock driver does not
  make these requests and this API is non re-entrant. Clients MUST
  avoid making concurrent calls into this API for these sources.
  If the source requires calibration upon enabling, the client must ensure
  any increased voltage requirements during calibration are also met.

  @param _h       [in] Handle to the DAL device.
  @param nSource  [in] Source to enable.

  @return
  DAL_SUCCESS -- Source was enabled. \n
  DAL_ERROR_INTERNAL -- Unable to enable source. \n
  DAL_ERROR_INVALID_PARAMETER -- Source identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetSourceId()
*/

static __inline DALResult
DalClock_EnableSource(DalDeviceHandle * _h,  SourceIdType  nSource)
{
   return ((DalClockHandle *)_h)->pVtbl->EnableSource( _h, nSource);
}


/* =========================================================================
**  Function : DalClock_DisableSource
** =========================================================================*/
/**
  Disables a source.

  This function removes the client's vote to enable a specified source.
  The source will only be disabled when the reference counts reach 0.

  Note: The clock driver flags sources whose voltage and source requirements
  are always externally satisfied. In this case the clock driver does not
  make these requests and this API is non re-entrant. Clients MUST
  avoid making concurrent calls into this API for these sources.

  @param _h       [in] Handle to the DAL device.
  @param nSource  [in] Source to disable.

  @return
  DAL_SUCCESS -- Call was successful (although the source may still be enabled). \n
  DAL_ERROR_INTERNAL -- Unable to disable source. \n
  DAL_ERROR_INVALID_PARAMETER -- Source identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetSourceId()
*/

static __inline DALResult
DalClock_DisableSource(DalDeviceHandle * _h,  SourceIdType  nSource)
{
   return ((DalClockHandle *)_h)->pVtbl->DisableSource( _h, nSource);
}


/* =========================================================================
**  Function : DalClock_SetSourceFrequency
** =========================================================================*/
/**
  Sets the frequency of a source.

  This function requests a frequency change for a specified source. The
  driver selects the exact frequency based on the matching argument,
  which allows requesting a minimum or approximate frequency. There
  is no support for concurrent requests; the last request will be serviced
  and override any other minimum frequency requests.
  An error will be returned if the source is being used at the time of request.

  Note: The clock driver flags sources whose voltage and source requirements
  are always externally satisfied. In this case the clock driver does not
  make these requests and this API is non re-entrant. Clients MUST
  avoid making concurrent calls into this API for these sources.

  @param _h       [in] Handle to the DAL device.
  @param nSource  [in] Source for which to set the frequency.
  @param nFreq    [in] Source frequency in Hz, kHz, or MHz, depending on the
                       eMatch parameter.
  @param eMatch   [in] Type of match request.
  @param pnResultFreq [out] The programmed source frequency in the same
                            units as nFreq; can be NULL if the resulting
                            frequency does not need to be checked. Note that
                            the result is truncated to the units requested.

  @return
  DAL_SUCCESS -- Requested source frequency was found and programmed. \n
  DAL_ERROR_INTERNAL -- The source is currently being used. \n
  DAL_ERROR_NOT_SUPPORTED -- A matching frequency was not found. \n
  DAL_ERROR_INVALID_PARAMETER -- Source identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetSourceId()
*/

static __inline DALResult
DalClock_SetSourceFrequency(DalDeviceHandle * _h,  SourceIdType  nSource,  uint32  nFreq,  ClockFrequencyType  eMatch,  uint32 * pnResultFreq)
{
   return ((DalClockHandle *)_h)->pVtbl->SetSourceFrequency( _h, nSource, nFreq, eMatch, pnResultFreq);
}


/* =========================================================================
**  Function : DalClock_GetSourceFrequency
** =========================================================================*/
/**
  Gets the frequency of a source.

  This function retrieves the current frequency of a Source in Hz. This
  is independent of whether or not the source is running. Note that only sources
  that are known/viewable by the local processor can have the frequency
  retrieved.

  Note: The clock driver flags sources whose voltage and source requirements
  are always externally satisfied. In this case this API is non re-entrant.
  Clients MUST avoid making concurrent calls into this API for these sources.

  @param _h            [in]  Handle to the DAL device.
  @param nSource       [in]  Source to check.
  @param pnFrequencyHz [out] Pointer to uint32 to fill in with the frequency.
                             A value of zero indicates that the frequency
                             could not be determined.

  @return
  DAL_SUCCESS -- Source was valid. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid source identifier or frequency .

  @dependencies
  None.

  @sa
  DalClock_GetSourceId()
*/

static __inline DALResult
DalClock_GetSourceFrequency(DalDeviceHandle * _h,  SourceIdType  nSource,  uint32 * pnFrequencyHz)
{
   return ((DalClockHandle *)_h)->pVtbl->GetSourceFrequency( _h, nSource, pnFrequencyHz);
}


/* =========================================================================
**  Function : DalClock_GetDividerId
** =========================================================================*/
/**
  Looks up a divider ID for a specified divider name.

  This function takes the name of a divider and returns a handle that
  can be used for making requests on that divider. Usually, this function
  must be the first to be invoked prior to using any other divider functions.
  The string name of the divider can be found in the software frequency plan,
  e.g., cc_uart2_clk.

  @param _h        [in]  Handle to the DAL device.
  @param szDivider [in]  String name of the divider.
  @param pnId      [out] Pointer to the divider ID to be filled in.

  @return
  DAL_SUCCESS -- Divider was found and the ID was filled in. \n
  DAL_ERROR_NOT_FOUND -- Divider was not found on this target. \n
  DAL_ERROR_INVALID_PARAMETER -- Pointer to divider name or ID is null.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_GetDividerId(DalDeviceHandle * _h,  const char * szDivider,  ClockDividerIdType * pnId)
{
   return ((DalClockHandle *)_h)->pVtbl->GetDividerId( _h, szDivider, pnId);
}


/* =========================================================================
**  Function : DalClock_SetDivider
** =========================================================================*/
/**
  Sets the divider divide value.

  This function programs the divide value for a specified divider.

  @param _h         [in] Handle to the DAL device.
  @param nDividerId [in] Divider to set.
  @param nDivider   [in] Divide value in logical units; 0 is invalid.

  @return
  DAL_SUCCESS -- Requested divider ID was valid. Divider value may or may not
                 be correct. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetDividerId()
*/

static __inline DALResult
DalClock_SetDivider(DalDeviceHandle * _h,  ClockDividerIdType  nDividerId,  uint32  nDivider)
{
   return ((DalClockHandle *)_h)->pVtbl->SetDivider( _h, nDividerId, nDivider);
}



/** @} */ /* end_addtogroup clock_functions */

/** @addtogroup power_domain_functions
@{ */

/* =========================================================================
**  Function : DalClock_GetPowerDomainId
** =========================================================================*/
/**
  Gets the power domain identifier.

  This function looks up and returns the identifier for a specified power
  domain. This identifier is used for the power domain control functions.
  The string used to get the identifier can be found in the software frequency
  plan, e.g., VDD_GRP.

  @param _h                [in]  Handle to the DAL device.
  @param szPowerDomain     [in]  Power domain name.
  @param pnPowerDomainId   [out] Pointer to the ID to be filled in.

  @return
  DAL_SUCCESS -- Power domain was valid, and the ID was filled in. \n
  DAL_ERROR_NOT_FOUND -- Power domain not found. \n
  DAL_ERROR_INVALID_PARAMETER -- Power domain name or ID was invalid.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_GetPowerDomainId(DalDeviceHandle * _h,  const char * szPowerDomain,  ClockPowerDomainIdType * pnPowerDomainId)
{
   return ((DalClockHandle *)_h)->pVtbl->GetPowerDomainId( _h, szPowerDomain, pnPowerDomainId);
}


/* =========================================================================
**  Function : DalClock_EnablePowerDomain
** =========================================================================*/
/**
  Enables a power domain.

  @param _h               [in] Handle to the DAL device.
  @param nPowerDomainId   [in] Power domain to enable.

  @return
  DAL_SUCCESS -- Power domain was enabled. \n
  DAL_ERROR_INVALID_PARAMETER -- Power domain identifier was invalid.

  @dependencies
  None.

  @sideeffects
  Any clocks inside the power domain are enabled during the enable
  sequence. The state is restored afterward.

  @sa
  DalClock_GetPowerDomainId(), DalClock_DisablePowerDomain()
*/

static __inline DALResult
DalClock_EnablePowerDomain(DalDeviceHandle * _h,  ClockPowerDomainIdType  nPowerDomainId)
{
  return ((DalClockHandle *)_h)->pVtbl->EnablePowerDomain( _h, nPowerDomainId);
}


/* =========================================================================
**  Function : DalClock_DisablePowerDomain
** =========================================================================*/
/**
  Disables a power domain.

  @param _h               [in] Handle to the DAL device.
  @param nPowerDomainId   [in] Power domain to disable.

  @return
  DAL_SUCCESS -- Power domain was disabled. \n
  DAL_ERROR_INVALID_PARAMETER -- Power domain identifier was invalid.

  @dependencies
  Any bus interfaces for this power domain must be disabled prior to
  collapsing the core or the bus may lock up.

  @sideeffects
  Any clocks inside the power domain are enabled during the collapse
  sequence.

  @sa
  DalClock_GetPowerDomainId(), DalClock_EnablePowerDomain()
*/

static __inline DALResult
DalClock_DisablePowerDomain(DalDeviceHandle * _h,  ClockPowerDomainIdType  nPowerDomainId)
{
  return ((DalClockHandle *)_h)->pVtbl->DisablePowerDomain( _h, nPowerDomainId);
}

/** @} */ /* end_addtogroup power_domain_functions */

/** @addtogroup misc_functions
@{ */

/* =========================================================================
**  Function : DalClock_EnableDCVS
** =========================================================================*/
/**
  Enables the dynamic clock and voltage scaling.

  This function enables the DCVS processing for system resources such as
  CPU, buses, and memories. The DCVS driver must invoke this function when
  all system resource requirements have been configured.

  @param _h       [in] Handle to the DAL device.

  @return
  Always returns DAL_SUCCESS.

  @dependencies
  All system resource requests must be set prior to enabling the DCVS.
*/

static __inline DALResult
DalClock_EnableDCVS(DalDeviceHandle * _h)
{
   return ((DalClockHandle *)_h)->pVtbl->EnableDCVS( _h);
}


/* =========================================================================
**  Function : DalClock_EnableAVS
** =========================================================================*/
/**
  Controls the Adaptive Voltage Scaling state.

  This function enables the AVS hardware block for the current processor.
  Only the current processor's AVS is enabled via this function. In a
  multicore system, each processor must invoke the function.

  @param _h       [in] Handle to the DAL device.

  @return
  DAL_SUCCESS -- AVS control succeeded. \n
  DAL_ERROR -- AVS control failed.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_EnableAVS(DalDeviceHandle * _h)
{
   return ((DalClockHandle *)_h)->pVtbl->EnableAVS( _h);
}


/* =========================================================================
**  Function : DalClock_LoadNV
** =========================================================================*/
/**
  Loads the clock driver nonvolatile data.

  This function triggers the driver to load configuration information from
  NV storage. It must be invoked once this storage is ready to be used.

  @param _h       [in] Handle to the DAL device.

  @return
  Always returns DAL_SUCCESS.

  @dependencies
  NV must be initialized.
*/

static __inline DALResult
DalClock_LoadNV(DalDeviceHandle * _h)
{
  return ((DalClockHandle *)_h)->pVtbl->LoadNV( _h);
}


/* =========================================================================
**  Function : DalClock_ProcessorSleep
** =========================================================================*/
/**
  Puts the processor in the Sleep mode.

  This function configures the processor clocks for Sleep mode
  and, depending on the environment, may execute the appropriate
  Stand by and Wait For Interrupt (SWFI) instruction.

  @param _h        [in] Handle to the DAL device.
  @param eMode     [in] Type of sleep to enter.
  @param nFlags    [in] Mode-specific flags.

  @return
  DAL_SUCCESS -- Processor clocks configured for sleep mode. \n
  DAL_ERROR_INTERNAL -- Error executing this function. \n
  DAL_ERROR_NOT_SUPPORTED -- Function not supported.

  @dependencies
  None.

  @sa
  DalClock_ProcessorRestore()
*/

static __inline DALResult
DalClock_ProcessorSleep(DalDeviceHandle * _h,  ClockSleepModeType  eMode,  uint32  nFlags)
{
  return ((DalClockHandle *)_h)->pVtbl->ProcessorSleep( _h, eMode, nFlags);
}


/* =========================================================================
**  Function : DalClock_ProcessorRestore
** =========================================================================*/
/**
  Restores the processor clock state after sleep.

  This function restores the processor clocks after a power collapse.

  @param _h        [in] Handle to the DAL device.
  @param eMode     [in] Type of sleep to leave; usually
                        DALCLOCK_SLEEP_MODE_POWER_COLLAPSE().
  @param nFlags    [in] Mode-specific flags; usually 0.

  @return
  DAL_SUCCESS -- Processor clocks restored. \n
  DAL_ERROR_INTERNAL -- Error executing this function.

  @dependencies
  DalClock_ProcessorSleep() must have been called previously.

  @sa
  DalClock_ProcessorSleep()
*/

static __inline DALResult
DalClock_ProcessorRestore(DalDeviceHandle * _h,  ClockSleepModeType  eMode,  uint32  nFlags)
{
   return ((DalClockHandle *)_h)->pVtbl->ProcessorRestore( _h, eMode, nFlags);
}

/* =========================================================================
**  Function : DalClock_EnterLowPowerMode
** =========================================================================*/
/**
  Enters the low power mode.

  This function is used to enter a low power state that facilitates
  rapid battery charging.

  @param _h       [in] Handle to the DAL device.

  @return
  DAL_SUCCESS -- Operation succeeded.\n
  DAL_ERROR -- Operation failed.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_EnterLowPowerMode(DalDeviceHandle * _h)
{
   return ((DalClockHandle *)_h)->pVtbl->EnterLowPowerMode( _h);
}


/* =========================================================================
**  Function : DalClock_ExitLowPowerMode
** =========================================================================*/
/**
  Exits the low power mode.

  This function restores the state of the SoC to the state it was in before
  it entered a low power state that facilitates rapid battery charging.

  @param _h       [in] Handle to the DAL device.

  @return
  DAL_SUCCESS -- Operation succeeded.\n
  DAL_ERROR -- Operation failed.

  @dependencies
  DalClock_EnterLowPowerMode() must have been called previously.
*/

static __inline DALResult
DalClock_ExitLowPowerMode(DalDeviceHandle * _h)
{
   return ((DalClockHandle *)_h)->pVtbl->ExitLowPowerMode( _h);
}

/** @} */ /* end_addtogroup misc_functions */

/** @addtogroup test_functions
@{ */

/* =========================================================================
**  Function : DalClock_CalcClockFrequency
** =========================================================================*/
/**
  Calculates the frequency of a clock.

  This function calculates the current frequency of a clock in Hz using
  hardware counting circuits. This functionality  depends on the presence of
  this hardware support, as well as being available on the current processor.
  It also requires that the clock can be enabled, which must be possible from
  the current environment.

  The frequency calculation is accurate to within a few kHz.
  The calculation takes a long time (at least a few milliseconds) and
  is intended only for testing purposes, not any real modes of operation.

  @param _h            [in]  Handle to the DAL device.
  @param nClock        [in]  Clock to check.
  @param pnFrequencyHz [out] Pointer to uint32 to fill in with the frequency.
                             A value of zero indicates that the frequency
                             could not be determined.

  @return
  DAL_SUCCESS -- Clock frequency was calculated successfully. \n
  DAL_ERROR_INVALID_PARAMETER -- Clock identifier was invalid, or frequency
                                 pointer was null.

  @sideeffects
  During the calculation, the clock is enabled.

  @dependencies
  The clock being tested must be enabled or controllable from the current
  processor.

  @sa
  DalClock_GetClockFrequency()
*/

static __inline DALResult
DalClock_CalcClockFrequency(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 * pnFrequencyHz)
{
   return ((DalClockHandle *)_h)->pVtbl->CalcClockFrequency( _h, nClock, pnFrequencyHz);
}

/* =========================================================================
**  Function : DalClock_GetMaxPerfLevel
** =========================================================================*/
/**
  Returns the maximum perf level supported for the given CPU Core or Cluster.

  @param _h            [in]  Handle to the DAL device.
  @param nCPU          [in]  CPU Core or Cluster ID.
  @param pnPerfLevel   [out] Pointer to the UINT32 to be filled
                             in with the maximum perf level supported
                             for this Core or Cluster.

  @return
  DAL_SUCCESS -- function completed successfully. \n
  DAL_ERROR -- parameter was invalid.

*/
static __inline DALResult
DalClock_GetMaxPerfLevel(DalDeviceHandle * _h, uint32 nCPU, uint32  *pnPerfLevel)
{
   return ((DalClockHandle *)_h)->pVtbl->GetMaxPerfLevel( _h, nCPU, pnPerfLevel);
}


/* =========================================================================
**  Function : DalClock_GetMinPerfLevel
** =========================================================================*/
/**
  Returns the minimum perf level supported for the given CPU Core or Cluster.

  @param _h            [in]  Handle to the DAL device.
  @param nCPU          [in]  CPU Core or Cluster ID.
  @param pnPerfLevel   [out] Pointer to the UINT32 to be filled
                             in with the minimum perf level supported
                             for this Core or Cluster.

  @return
  DAL_SUCCESS -- function completed successfully. \n
  DAL_ERROR -- parameter was invalid.

*/
static __inline DALResult
DalClock_GetMinPerfLevel(DalDeviceHandle * _h, uint32 nCPU, uint32  *pnPerfLevel)
{
   return ((DalClockHandle *)_h)->pVtbl->GetMinPerfLevel( _h, nCPU, pnPerfLevel);
}


/* =========================================================================
**  Function : DalClock_GetCpuPerfLevelFrequency
** =========================================================================*/
/**
  Returns the frequency and voltage associated with the given perf level for
  the given CPU Core or Cluster.

  @param _h                [in]  Handle to the DAL device.
  @param nCPU              [in]  CPU Core or Cluster ID.
  @param nPerfLevel        [in]  Perf Level under query.
  @param pnFrequencyHz     [out] Pointer to the associated frequency.
  @param pnRequiredVoltage [out] Pointer to the associated voltage.

  @return
  DAL_SUCCESS -- function completed successfully. \n
  DAL_ERROR -- parameter was invalid.

*/
static __inline DALResult
DalClock_GetCpuPerfLevelFrequency(DalDeviceHandle * _h, uint32 nCPU, uint32 nPerfLevel,
                                  uint32 *pnFrequencyHz, uint32 *pnRequiredVoltage)
{
   return ((DalClockHandle *)_h)->pVtbl->GetCpuPerfLevelFrequency( _h, nCPU, nPerfLevel, pnFrequencyHz, pnRequiredVoltage);
}


/* =========================================================================
**  Function : DalClock_SetCpuPerfLevel
** =========================================================================*/
/**
  Sets the desired perf level for the given CPU Core or Cluster.

  @param _h                [in]  Handle to the DAL device.
  @param nCPU              [in]  CPU Core or Cluster ID.
  @param nPerfLevel        [in]  Desired Perf Level.
  @param pnFrequencyHz     [out] Pointer to the actual frequency achieved.

  @return
  DAL_SUCCESS -- function completed successfully. \n
  DAL_ERROR -- parameter was invalid.

*/
static __inline DALResult
DalClock_SetCpuPerfLevel(DalDeviceHandle * _h, uint32 nCPU, uint32 nPerfLevel, uint32 *pnFrequencyHz)
{
   return ((DalClockHandle *)_h)->pVtbl->SetCpuPerfLevel( _h, nCPU, nPerfLevel, pnFrequencyHz);
}


/* =========================================================================
**  Function : DalClock_GetCpuPerfLevel
** =========================================================================*/
/**
  Gets the current perf level of the given CPU Core or Cluster.

  @param _h                [in]  Handle to the DAL device.
  @param nCPU              [in]  CPU Core or Cluster ID.
  @param pnPerfLevel       [out] Pointer to the current Perf Level.

  @return
  DAL_SUCCESS -- function completed successfully. \n
  DAL_ERROR -- parameter was invalid.

*/
static __inline DALResult
DalClock_GetCpuPerfLevel(DalDeviceHandle * _h, uint32 nCPU, uint32 *pnPerfLevel)
{
   return ((DalClockHandle *)_h)->pVtbl->GetCpuPerfLevel( _h, nCPU, pnPerfLevel);
}


/* =========================================================================
**  Function : DalClock_SetVoltage
** =========================================================================*/
/**
  Sets the voltage of the requested rail based on feedback from the CPR engine.
  Not all rails may be adjustable on each sub-system image.

  @param _h            [in]  Handle to the DAL device.
  @param eVRegID       [in]  VReg ID to specify which rail to adjust.
  @param nVoltageUV    [in]  Voltage (microvolts) to set.
  @param bIsLDOEnabled [in]  Boolean to indicate if the LDO is enabled.

  @return
  DAL_SUCCESS -- Voltage adjustment was applied to the request voltage rail.
  DAL_ERROR -- Rail identifier was invalid, or the voltage adjustment
               could not take place due to another outstanding voltage
               adjustment by the clock driver.

  @sideeffects
  Some CPU/BUS performance changes and voltage related activities may take
  longer during this voltage adjustment call due to this resource mgmt.

  @dependencies

  @sa
  DalClock_CPRAdjustVoltage()
*/

static __inline DALResult
DalClock_SetVoltage(DalDeviceHandle * _h, ClockVRegIdType eVRegId, int32 nVoltageUV, boolean *pbIsLDOEnabled)
{
   return ((DalClockHandle *)_h)->pVtbl->SetVoltage( _h, eVRegId, nVoltageUV, pbIsLDOEnabled);
}


/* =========================================================================
**  Function : DalClock_LogState
** =========================================================================*/
/**
  Writes every clock and power domain's status to ULOG.

  This function iterates over all clocks and power domains in the driver
  context and writes their status to the ULOG handle.
  Clock status includes Enabled, On, Reset, Frequency, and Source enum.
  Power domain status includes Enabled, and On.

  @param _h       [in]  Handle to the DAL device.
  @param pULog    [out] Pointer to the ULOG handle.
  @param nFlags   [in]  Flags used to enable certain operations such as
                        clock frequency calculation.

  @return
  DAL_SUCCESS -- Clock and Power domain statuses written to ULOG correctly \n
  DAL_ERROR -- ULOG writes failed.

  @dependencies
  Clocks and power domains must be initialized in the driver context.
*/

static __inline DALResult
DalClock_LogState(DalDeviceHandle * _h, void *pULog,  uint32 nFlags)
{
   return ((DalClockHandle *)_h)->pVtbl->LogState( _h, pULog, nFlags);
}


/* =========================================================================
**  Function : DalClock_LogClocksState
** =========================================================================*/
/**
  This API is called at Exit Boot Service.
  Writes every clock and power domain's status to ULOG/UART based on
  the EFI Non volatile BDSClockLogState inESB.

  This function iterates over all clocks and power domains in the driver
  context and writes their status to the ULOG handle.
  Clock status includes Enabled, On, Reset, Frequency, and Source enum.
  Power domain status includes Enabled, and On.

  @param _h       [in]  Handle to the DAL device.
  @param nLogMask [in]  used to select clock Characteristic to be dumpped
                        0                               : not dump the Clock frequency
                        CLOCK_LOG_STATE_CLOCK_FREQUENCY : dump the Clock frequency

  @return
  DAL_SUCCESS -- Clock and Power domain statuses written to ULOG correctly \n
  DAL_ERROR -- ULOG writes failed.

  @dependencies
  Clocks and power domains must be initialized in the driver context.
*/

static __inline DALResult
DalClock_LogClocksState(DalDeviceHandle * _h,  uint32 nLogMask)
{
   return ((DalClockHandle *)_h)->pVtbl->LogClocksState( _h, nLogMask);
}

/* =========================================================================
**  Function : DalClock_DumpClocksState
** =========================================================================*/
/**
  This API is called via EFI Test protocol.

  Writes every clock and power domain's status to ULOG/UART based on
  the setting of BDSClockLogState in BDS menu

  This function iterates over all clocks and power domains in the driver
  context and writes their status to the ULOG handle.
  Clock status includes Enabled, On, Reset, Frequency, and Source enum.
  Power domain status includes Enabled, and On.

  @param _h       [in]  Handle to the DAL device.
  @param nLogMask [in]  used to select clock Characteristic to be dumpped
                        0                               : not dump the Clock frequency
                        CLOCK_LOG_STATE_CLOCK_FREQUENCY : dump the Clock frequency

  @return
  DAL_SUCCESS -- Clock and Power domain statuses written to ULOG correctly \n
  DAL_ERROR -- ULOG writes failed.

  @dependencies
  Clocks and power domains must be initialized in the driver context.
*/

static __inline DALResult
DalClock_DumpClocksState(DalDeviceHandle * _h,  uint32 nLogMask)
{
   return ((DalClockHandle *)_h)->pVtbl->DumpClocksState( _h, nLogMask);
}
/** @} */ /* end_addtogroup misc_functions */

/** @addtogroup test_functions
@{ */


/* =========================================================================
**  Function : Clock_Init
** =========================================================================*/
/**
  Performs clock initialization in limited environments.

  This function is invoked in certain images that do not support the
  full DAL clock driver. It is primarily used in early boot loader
  stages. This function is not actually a DAL function and does not require
  the DAL framework.

  @return
  TRUE -- Clock_Init was successful. \n
  FALSE -- Critical failure; boot should not proceed.

  @dependencies
  Varies based on the image under consideration; usually, there are
  no dependencies.
*/

boolean Clock_Init (void);

/** @} */ /* end_addtogroup boot_functions */


#endif /* !DDICLOCK_H */

