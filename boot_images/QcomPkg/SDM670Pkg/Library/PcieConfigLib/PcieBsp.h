/*
===========================================================================

  @file PcieBsp.h

  Copyright (c) 2015-2017, Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================
                              EDIT HISTORY

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 07/26/17  mk       Added support for iommu device handles
 05/05/17  tselvam  Included support to perform RMW on PHY init sequence
 04/10/17  tselvam  Included changes for SDM845
 02/10/17  tselvam  Included Version dependent PHY init sequence
                    Included new flag in devcfg to control io coherency
 08/10/16  tselvam  Included port PHY power down sequence and hw port index
                    part of portInfo_t struct
 07/28/16  tselvam  Updated comments
 07/08/16  tselvam  Updated to support PciHostBridgeDxe and restructured the
                    data structures as well bumped up the documentation
 06/13/16  tselvam  updated reset clock for MSM8998 PCIe bcr
 06/13/16  tselvam  MSM8998 port
 01/28/16  tselvam  File renamed to pcieBsp.h from PcieConfigData.h
                    Part of this moved all the data to xml file and
                    kept only the declerations of all data types
 01/26/16  tselvam  Fixed the copyright header
 01/25/16  tselvam  Suppress support for 11.ad
 01/08/16  tselvam  Included support for 11.ad and pcie1 port
 10/12/15  tselvam  KW review #5917: Array 'pcie20_wrapper_axi_bases' of size
                    3 may use index value(s) 3..7 at PCIe_ReadConfigData
 09/17/15  tselvam  Incorporated V3/V4 specific PHY changes and made changes to
                    version dependent PHY settings, so that it can be
                    extensible in future platforms
 08/20/15  tselvam  Changed the GPIO function for CLK REQ
 08/07/15  tselvam  Updated with new PHY seq for V3.0 h/w
 07/22/15  tselvam  New PHY sequence based on QMP PCIe 3-port PHY 14LPP
                    Version 5.0.
 06/15/15  tselvam  Removed unwanted macros and unified data structs and also
                    updated PHY sequence for V2 and V3 hardware.
                    Renamed the file to PcieConfigData.h from PcieHwioRegDef.h
 05/15/15  tselvam  First checkin as part of Unified code for multi platforms
 
===========================================================================
*/

#ifndef __PCIEBSP_H__
#define __PCIEBSP_H__

#include <Uefi.h>
#include "PcieHwioRegs.h"
#include <HALhwio.h>
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFITlmm.h>
#include <Protocol/EFIChipInfo.h>
#include <api/systemdrivers/icbarb.h>
#include <Library/DebugLib.h>
#include "msmhwiobase.h"

/*
 * Register setting special meaning macros
 */
#define PCIE_INVALID_REG_BASE                            (0xFFFFFFFFUL)
#define PCIE_INVALID_REG_SIZE                            (0x0UL)
#define PHY_REG_DELAY                                    (0x44454C41UL)
#define PHY_REG_NOP                                      (0x4E4F4F50UL)
#define PHY_REG_WRI                                      (0x57524954UL)
#define PHY_REG_REA                                      (0x52454144UL)
#define PHY_REG_RMW                                      (0x524D4F44UL)

/*----------------------------------------------------------------------------
 * Enum Definitions
 * -------------------------------------------------------------------------*/

/***************************************************************************//**
 * @enum pin_config_type
 * @brief GPIO pin config type
 *
 * Enum to choose between different types of GPIOs to be configured
 * type of this enum will be used to determine:
 *
 * @note
 * - Whether to call TLMM/PMIC lib functions to configure the GPIO
 * - Determine end of GPIO init sequence using NONE value
 * - Introduce delay in micro seconds in b/w the sequence
 ******************************************************************************/
typedef enum
{
  NONE = 0,    /**< invalid GPIO type */
  MSM_GPIO=1,  /**< TLMM GPIO type */
  PMIC_GPIO=2, /**< PMIC Gpio Type */
  PMIC_MPP=3,  /**< PMIC MPP type */
  DELAY=4      /**< Delay in micro seconds b/w GPIO init sequence */
}pin_config_type;

/***************************************************************************//**
 * @enum pcie_soc_version_number
 * @brief SoC version numbering ENUM
 *
 * Enum to describe major version number of the SoC
 *
 ******************************************************************************/
typedef enum
{
  Pcie_Qcom_SoC_Ver_ALL   = 0x00000000, /**< Applicable for all versions */
  Pcie_Qcom_SoC_Ver_1     = 0x00010000, /**< Qualcomm SoC version 1 */
  Pcie_Qcom_SoC_Ver_2     = 0x00020000, /**< Qualcomm SoC version 2 */
  Pcie_Qcom_SoC_Ver_3     = 0x00030000, /**< Qualcomm SoC version 3 */
  Pcie_Qcom_SoC_Ver_4     = 0x00040000, /**< Qualcomm SoC version 4 */
  Pcie_Qcom_SoC_Ver_5     = 0x00050000, /**< Qualcomm SoC version 5 */
  Pcie_Qcom_SoC_Ver_6     = 0x00060000, /**< Qualcomm SoC version 6 */
  Pcie_Qcom_SoC_Ver_7     = 0x00070000, /**< Qualcomm SoC version 7 */
  Pcie_Qcom_SoC_Ver_8     = 0x00080000, /**< Qualcomm SoC version 8 */
  Pcie_Qcom_SoC_Ver_9     = 0x00090000, /**< Qualcomm SoC version 9 */
  Pcie_Qcom_SoC_Ver_POLL  = 0x504F4C4C, /**< POLL sequence */
  Pcie_Qcom_SoC_Ver_STOP  = 0x53544F50, /**< STOP sequence */
}pcie_soc_version_number;

/***************************************************************************//**
 * @enum mem_reg_index
 * @brief Memory region indexer
 *
 * Enum to index array of memRegions_t type, to get access to
 * desired memory regions
 *
 * max_mem_reg should be used declare array to hold all the mem regions
 * Individial elements of the array can be accessed using
 * one of the valid reg index defined in this enum
 ******************************************************************************/
typedef enum
{
  dbi_reg=0,     /**< Access DBI register space */
  elbi_reg=1,    /**< Access ELBI register space */
  parf_reg=2,    /**< Access PARF register space */
  pcs_reg=3,     /**< Access PHY PCS register space */
  phy_reg=4,     /**< Access PHY register space */
  phy_rx_reg=5,  /**< Access Rx register space of PHY */
  phy_tx_reg=6,  /**< Access Tx register space of PHY */
  phy_misc_reg=7,/**< Access Misc PCS register space of PHY */
  qhp_l0_reg=8,  /**< Access QHP L0 register space */
  qhp_l1_reg=9,  /**< Access QHP L1 register space */
  axi_reg=10,    /**< Access AXI register space */
  invalid_reg=11,/**< Invalid reg base to end the PHY init sequence */
  max_mem_reg=12 /**< End value of loop contruct to walk memRegions_t array */
}mem_reg_index;

/***************************************************************************//**
 * Type Declarations
 ******************************************************************************/

/***************************************************************************//**
 * @struct memRegions_t
 * @brief Memory mapping details of various regions
 *
 * This data type will be be used throughout the library
 * where ever accessing a register space is necessary *
 * In current implementation an array of this type is created of
 * size max_mem_reg for each root port and are accessed
 * and assigned as dictated by mem_reg_index
 ******************************************************************************/
typedef struct
{
  UINT64 pa; /**< pysical address of memory region */
  UINT64 sz; /**< size of memory region */
}memRegions_t;

/***************************************************************************//**
 * @struct pciePhyRegSeq_t
 * @brief PHY initialization register sequence
 *
 * PHY Initialization value to be written to get the PHY
 * out of rest
 *
 * An array of this type will be defined and filled in with
 * PHY init register write sequence for common PHY & port PHY
 * (if available).
 *
 * REG_WRITE_CONDITIONAL macro defined within the project will be used
 * to walk through this base/offset/value sets and perform:
 *
 * @note
 * - Get the pa corresponding to the mem_reg_index value found in base
 * - add reg_offset to the pa obtained from step one
 * - write the reg_val (usually 1 byte) to the calculated register
 *
 * @note Example:
 *  { phy_tx_reg, HWIO_OFFS(PCIE_COM_POWER_DOWN_CONTROL), 0x01 }
 *
 * @par
 *  parsing above example element of this type array will get the
 *  base pointed by phy_tx_reg and offset it to
 *  PCIE_COM_POWER_DOWN_CONTROL regsier then write the value 0x01
 ******************************************************************************/
typedef struct
{
  mem_reg_index base;  /**< mem_reg_index to select base address from
                        *   memRegions_t
                        */
  UINT32 opflag;   /**< flag used to identify the operation */
  UINT32 offset;   /**< register offset to be added to the base address */
  UINT32 mask;     /**< register mask to be cleared before writing value */
  UINT32 val;      /**< register init value to be written after POR */
} pciePhyRegSeq_t;

/***************************************************************************//**
 * @struct pciePhyAllVerSeq_t
 * @brief Version dependent PHY init sequences
 *
 * This structure is created to hold all possible PHY init sequences for
 * different versions of SoC
 ******************************************************************************/
typedef struct
{
  CONST EFIChipInfoVersionType version; /**< Version for following PHY init seq */
  CONST pciePhyRegSeq_t * phy_seq;      /**< PHY init sequence value */
} pciePhyAllVerSeq_t;

/***************************************************************************//**
 * @struct tlmmGpioParam_t
 * @brief TLMM GPIO settings
 *
 * This structure is created to capture all the configuration
 * parameter pertainig to configure a TLMM GPIO
 ******************************************************************************/
typedef struct
{
  UINT32 GpioNum;       /**< GPIO number */
  UINT32 Func;          /**< GPIO sub function to be selected from */
  UINT32 Direction;     /**< Input/Output direction to be configured */
  UINT32 Pull;          /**< Pull down/up settings */
  UINT32 DriveStrength; /**< drive strength */
  TLMM_ValueType value; /**< HIGH/LOW state if configured as Output */
}tlmmGpioParam_t;

/***************************************************************************//**
 * @struct pmicGpioParam_t
 * @brief PMIC GPIO settings
 *
 * This type is used to describe the properties of a PMIC GPIO
 * to be configured
 ******************************************************************************/
typedef struct
{
  UINT32 PmicIndex;  /**< PMIC index */
  EFI_PM_GPIO_WHICH_TYPE GpioNum; /**< PMIC GPIO number */
  EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE OutBuffConfig; /**< Buffer config CMOS,
                                                     *   OPen Drain NMOS/PMOS
                                                     */

  EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE VSrc; /**< Voltage source selection
                                         */
  EFI_PM_GPIO_SOURCE_CONFIG_TYPE Source; /**< PMIC source type */
  EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE BufferStrength; /**<
                                                              * drv strength
                                                              */
  BOOLEAN inversion; /**< inversion */
}pmicGpioParam_t;

/***************************************************************************//**
 * @union gpioConfigParam_t
 * @brief GPIO configuration parameter
 *
 * This union type is used to hold the any one of
 *
 * @note
 * - TLMM GPIO settings
 * - PMIC GPIO settings
 * - Delay in us
 *
 * to be configured
 ******************************************************************************/
typedef union
{
  tlmmGpioParam_t TlmmGpioParam; /**< TLMM GPIO description */
  pmicGpioParam_t PmicGpioParam; /**< PMIC GPIO description */
  UINTN DelayValue;              /**< Delay to be observed in us
                                  *   between the sequence
                                  */
}gpioConfigParam_t;

/***************************************************************************//**
 * @struct gpioPinConfig_t
 * @brief GPIO config information
 *
 * An array of this type will be created for each root port
 * to hold the sequence of TLMM/PMIC/Delay to be configured
 * before performing LTSSM
 ******************************************************************************/
typedef struct
{
  pin_config_type type;             /**< this enum will hold info how to access
                                     *   the union type. This type can be NONE
                                     *   to denote end of sequence
                                     */
  gpioConfigParam_t gpio_cfg_param; /**< Based on the type value, this field
                                     *   will hold one of TLMM/PMIC/Delay data
                                     */
}gpioPinConfig_t;

/***************************************************************************//**
 * @struct pcieLink_t
 * @brief PCIe link information
 *
 * This type will hold the critical link parameters
 ******************************************************************************/
typedef struct
{
  BOOLEAN trained;   /**< LTSSM success status TRUE/FALSE */
  UINT32  num_lanes; /**< Number of lanes present under each RP */
}pcieLink_t;

/***************************************************************************//**
 * @struct auxClkSettings_t
 * @brief PCIe aux clock frequency settings
 *
 * This type will hold frequency and clock names of aux clock to be set
 ******************************************************************************/
typedef struct
{
  UINT32 freq;       /**< Frequency to be set in Hz */
  CONST char * name; /**< Name of aux clock */
}auxClkSettings_t;

/***************************************************************************//**
 * @struct pcieClkDesc_t
 * @brief PCIe clock description
 *
 * This type will hold the names of clocks in following category
 *
 *  o Reset Clocks
 *  o Extern Clocks
 *  o GCC Clocks
 *
 * each will be a variable array of strings ending with a NULL pointer to
 * end of clock names
 ******************************************************************************/
typedef struct
{
  CONST char ** reset;    /**< Clock name names used for reset */
  CONST char ** external; /**< Clock names used for external clocks */
  CONST char ** gcc;      /**< Clock names used for GCC controlled clocks */
}pcieClkDesc_t;

/***************************************************************************//**
 * @struct pcieIcbClkBW_t
 * @brief PCIe ICB Clock bandwidth vote settings
 *
 * This type will hold:
 *
 *  o ICM master and slave names
 *  o required bandwidth settings
 *  o npa client handle
 *
 * Above in is true for per port settings
 ******************************************************************************/
typedef struct
{
  CONST char                   * npa_client_name;    
  npa_client_handle              npa_hdl;
  CONST ICBArb_MasterSlaveType * ms_list;
  CONST UINT32                   ms_size;
  CONST ICBArb_RequestType     * req_list;
  CONST UINT32                   req_size;
}pcieIcbClkBW_t;

/***************************************************************************//**
 * @struct portInfo_t
 * @brief Root port information
 *
 * This type is used to hold the collective information
 * on each root port.
 ******************************************************************************/
typedef struct
{
  CONST pcieClkDesc_t * clk;        /**< struct that holds multiple array of
                                     *   strings to denote the the names of
                                     *   port clocks to be controller by GCC
                                     *   clock APIs
                                     */
  CONST char *  pwr;                /**< null terminated string representing
                                     *   power domain name of this port
                                     */
  pcieIcbClkBW_t * icb_bw;    /**< ICB bandwidth request for PCIE */
  CONST auxClkSettings_t * aux_clk; /**< Aux clock frequency and name */
  CONST gpioPinConfig_t * gpio_cfg; /**< base pointer of an array of type
                                     *   gpioPinConfig_t. This array will
                                     *   have the sequence of GPIOs to be
                                     *   be configured before performing
                                     *   LTSSM
                                     */
  CONST memRegions_t * mem;         /**< base pointer of an array of type
                                     *   memRegions_t, which will have all
                                     *   the regions physical address filled
                                     *   during compile time
                                     */
  CONST pciePhyAllVerSeq_t * port_phy_init_seq; /**< port PHY init sequence */
  CONST pciePhyRegSeq_t * port_phy_deinit_seq; /**< port PHY de-init sequence */
  pcieLink_t * link;                /**< base pointer of the type variable
                                     *   pcieLink_t, which will hold the
                                     *   following info during compile time:
                                     *     o-> num_lanes
                                     *   and the other members of this type
                                     *   should be populated during runtime
                                     */
  CONST UINT32 hw_rp_index;         /**< Index of this root port as per
                                     *    IPCAT
                                     */
  CONST BOOLEAN io_coherency_en;   /**< IO Coherency enable Flag for this RP
                                     */  
  CONST char * iommu_hdl;           /**< Device handle to be provided to the 
                                     *    IoMMU driver
                                     */
}portInfo_t;

/***************************************************************************//**
 * @struct rootPort_t
 * @brief Root port information
 *
 * This type is used to hold the collective information
 * of all root port. This structure will the base pointer
 * of an array of type portInfo_t with number of root port
 * information as well
 ******************************************************************************/

typedef struct
{
  portInfo_t * port_info; /**< pointer to an array of all
                           *   port_info struct
                           */
  UINT32       num_rp;    /**< number of RPs present, can be
                           *   calculated from the size of
                           *   the above array
                           */
}rootPort_t;

/***************************************************************************//**
 * @struct pcieBsp_t
 * @brief Parent data structure of PCIe bsp
 *
 * This type is used to hold complete information
 * on all the root ports, PHY init sequence,
 * common PHY clock and power domain settings
 ******************************************************************************/
typedef struct
{
  CONST pcieClkDesc_t * phy_clk;             /**< null-terminated common PHY
                                              *   clock name
                                              */
  CONST pciePhyAllVerSeq_t * phy_init_seq;   /**< common PHY init sequence */
  CONST rootPort_t      * rp_info;           /**< root port information */
}pcieBsp_t;

#endif /* __PCIEBSP_H__ */
