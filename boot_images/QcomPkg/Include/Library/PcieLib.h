#ifndef PCIE_LIB_H
#define PCIE_LIB_H
/** @file PcieLib.h
   
  This file exports specialized chip specific code for PCIe protocol.

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/09/12   jc      Target reset, clock, power, and GPIO support.
 10/24/11   jc      API change to use bus:device:function parameters.
 10/04/11   jc      Initial revision of UEFI PCIe Driver

=============================================================================*/

/*=========================================================================
      Include Files
==========================================================================*/

/*=========================================================================
      Macros
==========================================================================*/

/* Default address increment between endpoint device BAR assignments */
#define PCIELIB_BAR_AXI_INC     0x200000

/*=========================================================================
      Functions
==========================================================================*/

/**
  EFI_STATUS PcieLibEnablePower()

  @brief
  This API is called by the client code to initialize the PCIe library.
  All PCIe controller cores and associated logic will be powered and reset,
  but no PCIe configuration (register accesses) will occur.  This API
  is provided for a staged initialization to allow non-blocking settling time
  for external power and clock sources.

  A staged initialization consists of the following operations:

  PcieLibEnablePower();
  start_time = timetick_get();
  <Perform other boot operations>
  PcieLibGetSettlingTimeUsec(&settling_usec);
  current_time = timetick_get();
  if (USEC(current_time - start_time) < settling_usec)
  {
    <Delay remainder of settling time>
  }
  PcieLibInit();

  @retval EFI_SUCCESS               Success
  @retval EFI_INVALID_PARAMETER     Invalid Parameters passed to the API
  @retval EFI_DEVICE_ERROR          Core Initialization failed

 */
EFI_STATUS PcieLibEnablePower(void);

/**
  EFI_STATUS PcieLibGetSettlingTimeUsec()

  @brief
  This API is called by the client code to obtain the required external clock
  and power settling time.

  @param[out] Usec  A pointer location to be filled by the
                    driver with the required external clock
                    and power settling time (microeconds).

  @retval EFI_SUCCESS               Success

 */
EFI_STATUS PcieLibGetSettlingTimeUsec
(
  OUT UINT32 *Usec
);

/**
  EFI_STATUS PcieLibInit()

  @brief
  This API is called by the client code to initialize the PCIe library.
  All PCIe controller cores and associated logic will be powered and enabled,
  and all devices on the PCIe bus(es) will be enumerated.

  NOTE: If PcieLibEnablePower() has not been called, then this API will
        perform that function and a blocking wait for the required
        settling time.  Conversely, if PcieLibEnablePower() has already
        been called then this API assumes the required settling time has
        already elapsed.

  A staged initialization consists of the following operations:

  PcieLibEnablePower();
  start_time = timetick_get();
  <Perform other boot operations>
  PcieLibGetSettlingTimeUsec(&settling_usec);
  current_time = timetick_get();
  if (USEC(current_time - start_time) < settling_usec)
  {
    <Delay remainder of settling time>
  }
  PcieLibInit();

  @retval EFI_SUCCESS               Success
  @retval EFI_INVALID_PARAMETER     Invalid Parameters passed to the API
  @retval EFI_DEVICE_ERROR          Core Initialization failed

 */
EFI_STATUS PcieLibInit(void);

/**
  EFI_STATUS PcieLibGetConfigBaseAddr()

  @brief
  This API is called by the client code to obtain the base address of the
  specified PCIe device's configuration register space in the PCIe controller
  core's memory mapped I/O range.

  @param[in]  Bus       Index of PCIe controller core

  @param[in]  Device    Index of PCIe endpoint device

  @param[in]  Function  Index of PCIe endpoint device function

  @param[out] BaseAddr  A pointer location to be filled by the
                        driver with the base address of the PCIe
                        core's memory mapped I/O range.

  @retval EFI_SUCCESS               Success
  @retval EFI_INVALID_PARAMETER     Invalid Parameters passed to the API
  @retval EFI_DEVICE_ERROR          Core Initialization failed

 */
EFI_STATUS PcieLibGetConfigBaseAddr
(
  IN UINT32 Bus,
  IN UINT32 Device,
  IN UINT32 Function,
  OUT UINT32 **BaseAddr
);

/**
  EFI_STATUS PcieLibGetIoBaseAddr()

  @brief
  This API is called by the client code to obtain the base address of the
  specified PCIe device's memory region in the PCIe controller core's
  memory mapped I/O range.

  @param[in]  Bus       Index of PCIe controller core
  @param[in]  Bus       Index of PCIe controller core

  @param[in]  Device    Index of PCIe endpoint device

  @param[in]  Function  Index of PCIe endpoint device function

  @param[in]  BARIndex  Index of PCIe endpoint device base address

  @param[out] BaseAddr  A pointer location to be filled by the
                        driver with the base address of the PCIe
                        core's memory mapped I/O range for the
                        specified device's memory region.

  @retval EFI_SUCCESS               Success
  @retval EFI_INVALID_PARAMETER     Invalid Parameters passed to the API
  @retval EFI_DEVICE_ERROR          Core Initialization failed

 */
EFI_STATUS PcieLibGetIoBaseAddr
(
  IN UINT32 Bus,
  IN UINT32 Device,
  IN UINT32 Function,
  IN UINT32 BARIndex,
  OUT UINT32 **BaseAddr
);

#endif   /* PCIE_LIB_H */
