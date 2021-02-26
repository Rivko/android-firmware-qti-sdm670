#ifndef __PCIELINK_H__
#define __PCIELINK_H__
/** @file PcieLink.h

  PCIe init function Header file.

  Copyright (c) 2014-2017, Qualcomm Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 05/22/17   tselvam  Updated prototype for PcieConfigLibUpdatePortInfo
 04/21/17   tselvam  Replaced POLL_UNTIL and REG_WRITE_CONDITIONAL mascros with
                     respective functions, as that is very easy to debug and
                     takes up less instruction in compiled version of the code
 10/07/16   tselvam  Created new API to Enable Clocks with a variable length
                     array of string as input
 10/07/16   tselvam  Changed POLL_UNTIL macro to use while loop instead of
                     a do while, so as to check for the condition even before
                     wasting with a unit of delay
 08/10/16   tselvam  Created new APIs to power down PHY and update AML variable
 07/28/16   tselvam  Included support to configure I/O port
 07/08/16   tselvam  Include new function to update the global port info
                     strcture based on the result of link training
 07/08/16   tselvam  code refactoring
 06/08/16   tselvam  read the device control 2 register based on cap walk &
                     merged the PERST and link training in one function
 02/08/16   tselvam  Added pcie xml config support
 06/15/15   tselvam  removed PCIe_ReadNumberofRP, since it is a static now.
                     Included support for pcie capability walk.
 05/13/15   tselvam  Unified code for multi platforms
 03/18/15   tselvam  Fixed Copyrights header
 03/16/15   tselvam  Included GCC BCR register variable
 06/16/14   hozefak  Add PcieInit protocol
 04/18/14   hozefak  First checkin for 8084
=============================================================================*/


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIClock.h>
#include "PcieBsp.h"

/* Macro Definitions */
#define TYPE0_BAR_TYPE_64BIT                             (0x00000002)
#define INIT_TIMEOUT                                     (150)
#define PCIE_PIPE_CLK_FREQUENCY                          (125000000)
#define PCIE_PIPE_CLK_SOURCE                             (2)
#define PCIE_PIPE_CLK_DIV                                (2)
#define PCIE_CLASS_CODE_BRIDGE_DEVICE                    (0x06)
#define PCIE_SUBCLASS_CODE_PCI_2_PCI_BRIDGE              (0x04)
#define PCIE_IO_SIZE                                     (0x10000)
#define PCIE_PHY_PLL_LOCK_DELAY_IN_MICRO_SEC             (1000)
#define PCIE_PHY_UP_TIMEOUT_COUNT_MAX_15_MILLI_SEC       (100)
#define PCIE_LINK_UP_TIMEOUT_COUNT_MAX_150_MILLI_SEC     (1000)
/* Max link speed */
#define CX_GEN1_MODE                                     (0x1)
#define CX_GEN2_MODE                                     (0x2)
#define CX_GEN3_MODE                                     (0x3)
/* PCIe ASPM Capability enables */
#define ASPM_L0s_DS_L1_DS                                (0x1)
#define ASPM_L0s_EN_L1_DS                                (0x1)
#define ASPM_L0s_DS_L1_EN                                (0x2)
#define ASPM_L0s_EN_L1_EN                                (0x3)
/* PCIe controller CTRL1 register values */
#define PLR_IATU_CTRL1_TYPE_CFG                          (4)
#define PLR_IATU_CTRL1_TYPE_MEMORY                       (0)
/* Bus address space */
#define PCIE_SINGLE_BUS_ADDR_SPACE_SIZE                  (0x100000)
#define PCIE_CONFIG_SPACE_LENGTH                         (0x1000)

/*
 * PCIe TYPE0/TYPE1 extended capability begin pointer
 */
#define PCI_CAPBILITY_POINTER_OFFSET                     (0x34)
#define EFI_PCIE_CAPABILITY_BASE_OFFSET                  (0x100)

/*
 * PCI capability IDs
 */
#define EFI_PCI_MSI_CAP_ID                               (0x05)
#define EFI_PCI_PCIE_CAP_ID                              (0x10)

/*
 * PCIe extended capability IDs
 */
#define EFI_PCIE_AER_CAP_ID                              (0x0001)
#define EFI_PCIE_VC_1_CAP_ID                             (0x0002)
#define EFI_PCIE_DEV_SN_CAP_ID                           (0x0003)
#define EFI_PCIE_PWR_BUD_CAP_ID                          (0x0004)
#define EFI_PCIE_LINK_DEC_1_CAP_ID                       (0x0005)
#define EFI_PCIE_LINK_DEC_2_CAP_ID                       (0x0006)
#define EFI_PCIE_RC_EC_EPA_CAP_ID                        (0x0007)
#define EFI_PCIE_MFVC_CAP_ID                             (0x0008)
#define EFI_PCIE_VC_2_CAP_ID                             (0x0009)
#define EFI_PCIE_RCRB_HEADER_CAP_ID                      (0x000A)
#define EFI_PCIE_VEN_SPECI_CAP_ID                        (0x000B)
#define EFI_PCIE_ACS_EXT_CAP_ID                          (0x000D)
#define EFI_PCIE_ARI_CAP_ID                              (0x000E)
#define EFI_PCIE_MCAST_CAP_ID                            (0x0012)
#define EFI_PCIE_RESIZE_BAR_CAP_ID                       (0x0015)
#define EFI_PCIE_DPA_EXT_CAP_ID                          (0x0016)
#define EFI_PCIE_TPH_REQ_CAP_ID                          (0x0017)
#define EFI_PCIE_LT_EXT_CAP_ID                           (0x0018)
#define EFI_PCIE_SEC_PCIE_EXT_CAP_ID                     (0x0019)
#define EFI_PCIE_LNR_EXT_CAP_ID                          (0x001C)
#define EFI_PCIE_DPC_EXT_CAP_ID                          (0x001D)
#define EFI_PCIE_L1SS_PM_CAP_ID                          (0x001E)
#define EFI_PCIE_PTM_CAP_ID                              (0x001F)

/***************************************************************************//**
 * @enum TlpType_t
 * @brief TLP types
 *
 * This enumerator will define the different types of TLPs
 *
 * @note
 *   Reference: PCI Express base spec rev 3.0
 *   2.2.1. Common Packet Header Fields
 *   Table 2-3: Fmt[2:0] and Type[4:0] Field Encodings
 ******************************************************************************/
typedef enum {
  TlpTypeMRdWr=0x00,
  TlpTypeMRdLk=0x01,
  TlpTypeIORdWr=0x02,
  TlpTypeCfgRdWr0=0x04,
  TlpTypeCfgRdWr1=0x05,
} TlpType_t;

/***************************************************************************//**
 * @enum iATUIndex_t
 * @brief Address space identified for iATU
 *
 * viewport region index number for SNPS iATU configuration
 ******************************************************************************/
typedef enum
{
  OutMmem = 0,
  OutCfg = 1,
  OutIo = 2,
  InMem = 3,
  MaxAsRegions=4
} iATUIndex_t;

/*
 * Register offset inside capability block
 */
typedef enum
{
  cap_reg_offset = 0x04,
  control1_reg_offset = 0x08,
  control2_reg_offset = 0x0c
}CapBlockRegOffset_t;

typedef enum
{
  capability_id_offset = 0x00,
  next_pointer_offset = 0x01,
  pci_express_cap_reg_offset = 0x02,
  device_capabilities_offset = 0x04,
  device_control_offset = 0x8,
  device_status_offset = 0x0a,
  link_capabilities_offset = 0x0c,
  link_control_offset = 0x10,
  link_status_offset = 0x12,
  slot_capabilities_offset = 0x14,
  slot_control_offset = 0x18,
  slot_status_offset = 0x1a,
  root_control_offset = 0x1c,
  root_capabilities_offset = 0x1e,
  root_status_offset = 0x20,
  device_capabilities2_offset = 0x24,
  device_control2_offset = 0x28,
  device_status2_offset = 0x2a,
  link_capabilities2_offset = 0x2c,
  link_control2_offset = 0x30,
  link_status2_offset = 0x32,
  slot_capabilities2_offset = 0x34,
  slot_control2_offset = 0x38,
  slot_status2_offset = 0x3a
}PciExpressCapRegBlock_t;

/***************************************************************************//**
 * @fn PcieConfigLibInitializeConfigSpace
 * @brief Initialize Root Bridge Configuration Space
 *
 * Initialize the config space by setting up the base address,
 * setup the ATU's and check if the EP is connected
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibInitializeConfigSpace (UINT32 RootPortIndex);

/***************************************************************************//**
 * @fn PcieConfigLibTrainLink
 * @brief Perform Link Training
 *
 * This function will perform LTSSM_EN and do link training with connected
 * endpoint for the root port specified. This function will also update the
 * rootPort_t with available number of root ports.
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibTrainLink (UINT32 RootPortIndex);

/***************************************************************************//**
 * @fn PcieConfigLibInitializePHY
 * @brief Bring PHY out of reset
 *
 * This function will bring the PCIe PHY out of reset
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibInitializePHY (UINT32 RootPortIndex);

/***************************************************************************//**
 * @fn PcieConfigLibDeInitializePHY
 * @brief Power down PHY
 *
 * This function will power down port PHY
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibDeInitializePHY(UINT32 RootPortIndex);

/***************************************************************************//**
 * @fn PcieConfigLibSetupL1ss
 * @brief Configure L1ss
 *
 * This function will configure link parameters for L1ss
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibSetupL1ss (UINT32 RootPortIndex);

/***************************************************************************//**
 * @fn PcieConfigLibConfigureClocks
 * @brief Configure the PCIe clocks
 *
 * This function performs the following operations:
 *  o-> Enabled clock power domain
 *  o-> Resets and Enables all the clocks
 *  o-> sets the Aux clock frequency
 *
 * @param[in] void
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibConfigureClocks (UINT32 RootPortIndex);

/***************************************************************************//**
 * @fn PcieConfigLibGetPcieBspInfo
 * @brief returns pointer of pcieBsp_t struct
 *
 * PCIe configuration Library API to get the BSP info
 *
 * @param[in]  void
 *
 * @return Pointer to pcieBsp_t struct with BSP information fetched from Dev cfg
 ******************************************************************************/
CONST pcieBsp_t * PcieConfigLibGetPcieBspInfo(VOID);

/***************************************************************************//**
 * @fn PcieConfigLibGetPciRootPortInfo
 * @brief returns pointer of rootPort_t struct
 *
 * PCIe configuration Library API to get the root port info
 *
 * @param[in]  void
 *
 * @return Pointer to rootPort_t struct with root port info
 ******************************************************************************/
rootPort_t * PcieConfigLibGetPciRootPortInfo(VOID);

/***************************************************************************//**
 * @fn PcieConfigLibTurnOffPortClocks
 * @brief Turn Off PCIe port Clocks
 *
 * This function will turn off all the port clocks
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibTurnOffPortClocks (UINT32 RootPortIndex);

/***************************************************************************//**
 * @fn PcieConfigLibTurnOffCommonClocks
 * @brief Turn Off PCIe Common Clocks
 *
 * This function will turn off all the common clocks if any
 *
 * @param[in] void 
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibTurnOffCommonClocks (VOID);

/***************************************************************************//**
 * @fn PcieConfigLibInitializeLDO
 * @brief Configure the PCIe LDOs
 *
 * Vote for the LDOs necessary for PCIe to be operational
 *
 * @param[in] void
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibInitializeLDO(VOID);

/***************************************************************************//**
 * @fn PcieConfigLibDeInitializeLDO
 * @brief Disable the PCIe LDOs
 *
 * Vote for the LDOs to be turned OFF
 *
 * @param[in] void
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibDeInitializeLDO(VOID);

/***************************************************************************//**
 * @fn PcieConfigLibConfigureGPIOs
 * @brief Configure the GPIOs
 *
 * Configure the GPIOs based on the input
 *
 * @param[in] Pointer to gpioPinConfig_t type
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibConfigureGPIOs(
    CONST gpioPinConfig_t * pGPIOConfigData);

/***************************************************************************//**
 * @fn PcieConfigLibUpdatePortInfo
 * @brief Update Root port info struct
 *
 * This function will be called from PcieLink.c : PcieConfigLibTrainLink
 * function, if link training failed  
 *
 * @param[in] None
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibUpdatePortInfo(VOID);

/***************************************************************************//**
 * @fn PcieConfigLibSetupOutboundiATU
 * @brief Sets up Outbound iATU
 *
 * Sets up iATU for specified root port with specified parameters
 *
 * @param[in] RootPortIndex, Root port index in portInfo_t struct
 * @param[in] Index, Index of the iATU to be configured
 * @param[in] Type, TLP type of the outbound window
 * @param[in] CpuAddrSpaceBase, iATU region base address
 * @param[in] CpuAddrSpaceLimit, iATU region limit address
 * @param[in] PciAddrSpaceBase, iATU translated target base address
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibSetupOutboundiATU (
  UINT32 RootPortIndex,
  iATUIndex_t Index,
  TlpType_t Type,
  UINT64 CpuAddrSpaceBase,
  UINT64 CpuAddrSpaceLimit,
  UINT64 PciAddrSpaceBase);

/***************************************************************************//**
 * @fn PcieConfigLibSetACPIPlatCallback
 * @brief Sets call back with ACPIPlatformDxe
 *
 * Sets up ACPI callback with ACPI Platform to update PRPx AML variable
 *
 * @param[in] HwPortIndex, Index of root port as defined by IPCAT
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibSetACPIPlatCallback (
  UINT32 HwPortIndex);

/***************************************************************************//**
 * @fn PcieConfigLibEnableClocks
 * @brief Enable the clocks
 *
 * Resets and Enabled clocks provided in clock descriptor
 *
 * @param[in] Pointer to EFI_CLOCK_PROTOCOL
 * @param[in] An array of const char * string list of clock names to Enable
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibEnableClocks (
        EFI_CLOCK_PROTOCOL  * pClockProtocol,
        CONST char ** EnableClockList);

/***************************************************************************//**
 * @fn PcieConfigLibPollBitMsk
 * @brief Poll for the status until the Condition is satisifed
 *
 * Polls the "Addr" for bit specified in "Mask" with an interval of "Delay" for
 * each iteration until the "Condition" specified for that bit is met
 *
 * @param[in] Root port index in portInfo_t struct
 * @param[in] UINT64 Addr - Address to be read register from 
 * @param[in] UINT32 Mask - Bit Mask to be used for checking the condition
 * @param[in] BOOLEAN Condition - TRUE/FALSE condition to be met for bit mask
 * @param[in] UINT32 Delay - Delay in micro seconds between each iteration
 *
 * @return TRUE if the condition is met else FALSE
 ******************************************************************************/
BOOLEAN PcieConfigLibPollBitMsk (
    UINT64 Addr,
    UINT32 Mask,
    BOOLEAN Condition,
    UINT32 Delay);

#endif/* #ifndef __PCIELINK_H__ */
