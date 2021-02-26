#ifndef USB_DCI_H
#define USB_DCI_H

/*=======================================================================*//**
 * @file        UsbDci.h
 * @author:     amirs
 * @date        14-July-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) Private defines, types and API.
 *
 * @details     The Chip-Idea USB Hardware is managed via descriptors called dQH and dTD.
 *              The dQH describes some of the endpoint configuration
 *              The dTD describes the transfer parameters.
 *
 * @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *              Chapter 7 Device Data Structures
 *
 *              Copyright (c) 2008-2014 QUALCOMM Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 *
*//*========================================================================*/

//===========================================================================
//
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
//
//
// when       who      what, where, why
// --------   ---      ----------------------------------------------------------
// 07/14/08   amirs    First Draft
// 08/08/08   amirs    First working DLOAD process
// 08/11/08   amirs    Add files and functions headers according to coding standard.
// 08/30/10   dpatnaik Port QHSUSB to RPM, add ULPI based charger detection
// 06/24/11   cching   Port QHSUSB to UEFI
// 05/12/14	  gothekar Move VBUS detection logic into target specifc SW
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "com_dtypes.h"       // common defines and types
#include "HALhwio.h"
#include <Library/UsbfnLib.h> // DCI API Prototypes
#include <Protocol/EFII2C.h>

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

//+++++++++++++++++++++++++++++++++
// DCI PRIVATE FLAGS
//+++++++++++++++++++++++++++++++++

// This flag should be enabled during development and disable for release
// to reduce the code size.
//#define QHSUSB_DEBUG_DCI_ENABLE

void qhsusb_loop_here(void); 

#ifdef QHSUSB_DEBUG_DCI_ENABLE
    #define DCI_ASSERT(val) dci_assert(val,__FILE__,__LINE__)
    #define DCI_INC_DEBUG_COUNTER(x) x++
    #define DCI_CHECK_CORE_ID(core_id) \
            if (core_id >= DCI_MAX_CORES)     \
            {                                 \
                DCI_ASSERT(QHSUSB_ERR_BAD_PARAM); \
                return(QHSUSB_ERR_BAD_PARAM); \
            }
#else
    #define DCI_ASSERT(val)   qhsusb_loop_here() 
    #define DCI_INC_DEBUG_COUNTER(x)
    #define DCI_CHECK_CORE_ID(core_id) \
            if(core_id >= DCI_MAX_CORES) \
            {                             \
                qhsusb_log(QHSUSB_CORE_ID_ERROR_LOG, 0, __LINE__); \
                DCI_ASSERT(QHSUSB_ERR_BAD_PARAM); \
            }     
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


// DCI Error Codes
#define DCI_OK                      0

#define DCI_SETUP_BUFFER_SIZE   8

#define DCI_MAX_CORES     1

#if (DCI_MAX_CORES > 1)
   #error The DCI has infrastructure to support 2 Cores at different base address.
   #error This was originally done to support either HSIC or HS-USB in boot (seletion in run-time by e-fuse).
   #error However, Multiple-Cores at the SAME TIME is not fully supported. A dQH and dTD is needed per core.
#endif

#define DCI_MAX_ENDPOINTS 3
#define DCI_MAX_DIRECTION 2  // RX=OUT=0,TX=IN=1

#define DCI_EP0_MAX_PACKET_SIZE  64
#define CI_USBFN20_ALIGNMENT_QUEUE_HEAD (0x800)

#define DCI_EP0     0

#define DIR_RX      0 // OUT Token
#define DIR_TX      1 // IN Token

// dQH defines for bitmask and shif
#define DQH_ZLT_BMSK                (1<<29)
#define DQH_IOS_BMSK                (1<<15)
#define DQH_MAX_PACKET_LEN_SHFT     16

// dTD defines for bitmask and shif
#define DTD_TERMINATE_BMSK          0x001
#define DTD_IOC_BMSK                (1<<15)

#define DTD_STATUS_BMSK                      0xFF
#define DTD_STATUS_ACTIVE_BMSK               (1<<7)
#define DTD_STATUS_HALTED_BMSK               (1<<6)
#define DTD_STATUS_DATA_BUFFER_ERROR_BMSK    (1<<5)
#define DTD_STATUS_TRANSACTION_ERROR_BMSK    (1<<3)

#define DTD_TOTAL_BYTES_SHFT        16

#define DTD_MAX_TRANSFER_16KB   (16*1024)

#define USB_REG_ID_VALUE    0x0042FA05

#define VAL_TO_BOOL(x)  ((boolean)((x) != 0))

#define DCI_MAX_TIMERS 2


// -----------------------------------------------------------------
// FSUSB PHY Registers accessed via I2C
// These are for NXP ISP1302 and Micrel MIC2555BML-0 FS xceivers
// -----------------------------------------------------------------
#define PHY_I2C_ADDR                        0x2C
#define PHY_INT_SOURCE_REG                  0x08
#define PHY_INT_SOURCE_REG_SESS_VLD_BMSK    0x02
#define PHY_INT_SOURCE_REG_VBUS_VLD_BMSK    0x01
#define PHY_OTG_CTL_REG1_SET                0x06
#define PHY_OTG_CTL_REG1_CLR                0x07
#define DM_PULLDOWN                         0x08
#define DP_PULLUP                           0x01

// Simple Read/Write register
#define DCI_HWIO_IN(core, usb_reg)                      in_dword((dci_core_base_address[(core)]+(usb_reg)))
#define DCI_HWIO_OUT(core, usb_reg, val)                out_dword((dci_core_base_address[(core)]+(usb_reg)),(val))
// Read/Write register with Mask
#define DCI_HWIO_INM(core, usb_reg, mask)               ( DCI_HWIO_IN((core), (usb_reg)) & (mask))
#define DCI_HWIO_OUTM(core, usb_reg, mask, val)         DCI_HWIO_OUT((core), (usb_reg), ((DCI_HWIO_IN((core), (usb_reg)) & (~(mask))) | ((val) & (mask))))
// Read/Write register with Index
#define DCI_HWIO_OUTMI(core, usb_reg, index, mask, val) DCI_HWIO_OUTM((core), ((usb_reg)+(index)*4), (mask), (val))
#define DCI_HWIO_INI(core, usb_reg, index)              DCI_HWIO_IN((core), ((usb_reg)+(index)*4))

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

/** @struct dTD = Device-Transfer-Descriptor , Total 32 bytes
 * @note should be 32 byte aligned
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 7.2 Endpoint Transfer Descriptor (dTD)
 *
 */
typedef  volatile struct dTD_Tag
{
   uint32   next_dtd_ptr;        // Next dTD Pointer (32 byte aligned) + Terminate bit
   // Note: avoid bit-fields-struct for multiple-fields 32 bits
   uint32   size_ioc_mult_sts;    // Total bytes (16..30), IOC (15), MULT (10..11), STS (0-7)

   uint32   buffer0_ptr;         // Buffer pointer Page 0 (12-31)
   uint32   buffer1_ptr;         // Buffer pointer Page 1 (12-31)
   uint32   buffer2_ptr;         // Buffer pointer Page 2 (12-31)
   uint32   buffer3_ptr;         // Buffer pointer Page 3 (12-31)
   uint32   buffer4_ptr;         // Buffer pointer Page 4 (12-31)

   uint32  urb_ptr;             // Points to software managment info
} dTD_t;

/** @struct dQH = Device-Queue-Head , Total 64 bytes
 * @note should be 64 byte aligned
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 7.1 Endpoint Queue Head (dQH)
 *
 */
typedef volatile struct
{
   // Note: avoid bit-fields-struct for multiple-fields 32 bits
   uint32   mult_zlt_packetlen_ios;    // Mult (31..30), Zlt (29), max_packet_length (26..16), ios (15)

   uint32   current_dtd_ptr;           // current dtd pointer

   // overlay transfer = Shadow copied by the USB Hardware from the current dTD.
   uint32   next_dtd_ptr;        // Next dTD Pointer (32 byte aligned) + Terminate bit
   // Note: avoid bit-fields-struct for multiple-fields 32 bits
   uint32   size_ioc_int_sts;    // Total bytes (16..30), IOC (15), INT (8), STS (0-7)

   uint32   buffer0_ptr;           // Buffer pointer Page 0 (12-31) + offset (0..11) = full address.
   uint32   buffer1_ptr;           // Buffer pointer Page 1 (12-31)
   uint32   buffer2_ptr;           // Buffer pointer Page 2 (12-31)
   uint32   buffer3_ptr;           // Buffer pointer Page 3 (12-31)
   uint32   buffer4_ptr;           // Buffer pointer Page 4 (12-31)
   // end of transfer_overlay

   uint32   reserved1;
   uint8    setup_buffer[8];        // 8 bytes of setup data that follows the setup pid
   uint32   reserved2[4];           // for 64 bytes alignment
} dQH_t;


typedef enum
{
    DCI_PHY_TYPE_PMIC_3_WIRE    = 1,
    DCI_PHY_TYPE_ULPI           = 2,
    DCI_PHY_TYPE_FSIC           = 3,
    DCI_PHY_TYPE_HSIC           = 4,

    // TBD

    DCI_PHY_TYPE_MAX
} phy_type_t;

struct qhsusb_dcd_dsc_device;

// Note: For Multi-Core Support
typedef struct
{
    uint32 base_address;
    phy_type_t phy_type;
    struct qhsusb_dcd_dsc_device*  dcd_context_ptr;    // For DCD internal use (ask by Vladimir)
    EFI_QCOM_I2C_PROTOCOL *i2c_protocol;               //pointer to i2c driver
} core_info_t;

typedef enum
{
    QHSUSB_CHG_PHY_MODE_NORMAL,
    QHSUSB_CHG_PHY_MODE_NONDRIVE
} qhsusb_phy_mode_type;
//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------
void dci_assert(uint32 val,const char* filename_ptr,int line);

//----------------------------------------------------------------------------
// Target Specific Function Declarations and Documentation
//----------------------------------------------------------------------------
void dci_init_phy(uint32 core_id);

void dci_dcache_inval(uint32 *addr, uint32 length); /* DCache Invalidate */
void dci_dcache_flush(uint32 *addr, uint32 length); /* DCache Clean & Invalidate */


//=============================================================================
/**
 * Detects charger port type
 *
 * Should be called after usb stack initialization
 *
 * @param core_id
 *
 *
 *
 * @return Charger port type as qhsusb_chg_port_type.
 */
//=============================================================================
qhsusb_chg_port_type dci_detect_charger_type(uint32 core_id);

//============================================================================

/**
 * @function  dci_locate_pmic_protocol_type
 *
 * @brief target specific pmic initialization
 *
 * * @param none
 *              
 * * @return none 
 *
 */
//============================================================================
void dci_locate_pmic_protocol_type(void);

//============================================================================

/**
 * @function  is_dci_vbus_valid
 *
 * @brief Target specific Vbus detrection based on PMIC type
 *
 * * @param none
 *              
 * * @return Boolean,   
*
 */
//============================================================================
BOOLEAN dci_is_vbus_valid(void);

// ------------------------------------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------------------------------------

extern UINTN dci_core_base_address[DCI_MAX_CORES];
extern core_info_t dci_core_info[DCI_MAX_CORES];


#endif // USB_DCI_H
