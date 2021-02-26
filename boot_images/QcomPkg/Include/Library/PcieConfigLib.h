/** @file PcieConfigLib.h

  PCIe driver header files with protocols defined

  Copyright (c) 2012-2016, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 07/28/16   tselvam  Included I/O port access support
 07/08/16   tselvam  Included new APIs to interface with PciHostBrdgeDxe
 06/08/16   tselvam  Included support to calculate RPMask during runtime
 01/11/16   tselvam  Register for PCIe ExitBootServices callback
 05/13/15   tselvam  Unified code for multi platforms
 01/26/15   tselvam  First checkin for 8996
 04/18/14   hozefak  First checkin for 8994
=============================================================================*/
#ifndef PCIECONFIGLIB_H
#define PCIECONFIGLIB_H 

typedef enum {
  PciRbResTypeCfg = 0,
  PciRbResTypeIo = 1,
  PciRbResTypeMem = 2,
  PciRbResTypeBus = 3,
  PciRbResTypeMax = 4
}ROOT_PORT_RES_TYPE;

typedef struct {
  UINT64 Base;
  UINT64 Start;
  UINT64 End;
} PCI_ROOT_BRIDGE_RESOURCE_APERTURE;

/***************************************************************************//**
 * @fn PcieConfigLibEnableRootPorts
 * @brief Entry Point for PCIe Config Library. Enumeration starts here.
 *
 * Function to initialize the PCIe hardware for this SoC and enumerate
 * the connected EndPoints
 *
 * @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 * @param[in] SystemTable    A pointer to the EFI System Table.
 *
 * @retval EFI_SUCCESS       The entry point is executed successfully.
 * @retval other             Appropriate error number.
 ******************************************************************************/
EFI_STATUS
EFIAPI
PcieConfigLibEnableRootPorts(VOID);

/***************************************************************************//**
 * @fn PcieConfigLibGetNumberOfHostBridges
 * @brief returns number of host bridges to enable
 *
 * PCIe configuration Library API to get total number of Host Bridges
 *
 * @param[in]  void
 *
 * @return unsigned 32-bit value, of number of root ports present
 ******************************************************************************/
UINT32
EFIAPI
PcieConfigLibGetNumberOfHostBridges(VOID);

/***************************************************************************//**
 * @fn PcieConfigLibGetNumberOfRootBridges
 * @brief returns number of root bridges to enable
 *
 * PCIe configuration Library API to get total number of root bridges per
 * the supplied host bridge index
 *
 * @param[in] UINT32 HostBridgeIndex
 *
 * @return unsigned 32-bit value, of number of root bridges for this
 *         host bridge
 ******************************************************************************/
UINT32
EFIAPI
PcieConfigLibGetNumberOfRootBridges(UINT32 HostBridgeIndex);

/***************************************************************************//**
 * @fn PcieConfigLibGetResourceAperture
 * @brief returns memory and bus resource aperture
 *
 * PCIe configuration Library API to get the Bus and Memory Aperture
 * corresponding to the Segment number passed
 *
 * @param[in]  UINT32 SegmentNumber
 * @param[out] A pointer to PCI_ROOT_BRIDGE_RESOURCE_APERTURE
 *
 * @retval EFI_SUCCESS       The entry point is executed successfully.
 * @retval other             Appropriate error number.
 ******************************************************************************/
EFI_STATUS
EFIAPI
PcieConfigLibGetResourceAperture(
    UINT32 SegmentNumber,
    PCI_ROOT_BRIDGE_RESOURCE_APERTURE * ResAperture);

/***************************************************************************//**
 * @fn PcieConfigLibPciRead8
 * @brief Read one byte from config space
 *
 * Reads an 8 bit value from PCI configuration address
 *
 * @param[in] PciAddress     Address to read from
 *
 * @retval 8 bit value read.
 ******************************************************************************/
UINT8 EFIAPI PcieConfigLibPciRead8 (IN UINTN PciAddress);

/***************************************************************************//**
 * @fn PcieConfigLibPciRead16
 * @brief Read one word from config space
 *
 * Reads a 16 bit value from PCI configuration address
 *
 * @param[in] PciAddress     Address to read from
 *
 * @retval 8 bit value read.
 ******************************************************************************/
UINT16 EFIAPI PcieConfigLibPciRead16 (IN UINTN PciAddress);

/***************************************************************************//**
 * @fn PcieConfigLibPciRead32
 * @brief Read one double word from config space
 *
 * Reads a 32 bit value from PCI configuration address
 *
 * @param[in] PciAddress     Address to read from
 *
 * @retval 8 bit value read.
 ******************************************************************************/
UINT32 EFIAPI PcieConfigLibPciRead32 (IN UINTN PciAddress);

/***************************************************************************//**
 * @fn PcieConfigLibPciWrite8
 * @brief Write one byte to config space
 *
 * Wtites a given 8-bit balue to a PCI config space address
 *
 * @param[in] PciAddress     Address to read from
 * @param[in] Value          8 bit value to write
 *
 * @retval 8 bit value written.
 ******************************************************************************/
UINT8 EFIAPI PcieConfigLibPciWrite8 (IN UINTN PciAddress, IN UINT8 Value);

/***************************************************************************//**
 * @fn PcieConfigLibPciWrite16
 * @brief Write one word to config space
 *
 * Writes a given 16 bit value to a PCI configuration address
 *
 * @param[in] PciAddress     Address to read write to
 * @param[in] Value          16 bit value to write
 *
 * @retval 16 bit value written.
 ******************************************************************************/
UINT16 EFIAPI PcieConfigLibPciWrite16 (IN UINTN PciAddress, IN UINT16 Value);

/***************************************************************************//**
 * @fn PcieConfigLibPciWrite32
 * @brief Write one double word to config space
 *
 * Writes a given 32 bit value to a PCI configuration address
 *
 * @param[in] PciAddress     Address to write to
 * @param[in] Value          32 bit value to write
 *
 * @retval 32 bit value written.
 ******************************************************************************/
UINT32 EFIAPI PcieConfigLibPciWrite32 (IN UINTN PciAddress, IN UINT32 Value);

#endif/* #ifndef PCIECONFIGLIB_H*/
