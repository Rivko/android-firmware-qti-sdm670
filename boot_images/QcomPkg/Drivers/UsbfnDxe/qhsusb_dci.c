//============================================================================
/**
 * @file        qhsusb_dci.c
 * @author      amirs
 * @date        14-July-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) implementation.
 *
 * @details     The QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface)
 *              acts like a HAL (Hardware Abtraction Layer) for Chip-Idea USB Hardware.
 *              The DCI should supply generic API that hides USB Hardware registers access from the upper layers.
 *              It should not be aware of Chapter-9 or specific function drivers (Mass-Storage , CDC , HID etc).
 *              It should be simple without any management algorithms.
 *
 *              The USB Device Core Driver (DCD) shall use the DCI to implement
 *              the USB protocol chapter 9 enumeration,
 *              and handling of special BUS signaling : Reset / Suspend / Resume.
 *
 * @note        This DCI is implementing the ChipIdea hardware core.
 *              The DCI depends on the USB Core (a.k.a LINK) connection to the PHY:
 *              When a 3-wired PHY is connected (Full-Speed only) then the LINK
 *              can not control the VBUS D+ wire via the Run/Stop bit.
 *
 * @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *
 *              Copyright (c) 2008 - 2014, QUALCOMM Technologies Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 *
 */
//============================================================================

// ===========================================================================
//
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
//
//
// when       who      what, where, why
// --------   ---      ----------------------------------------------------------
// 10/31/14   ar       Resolve blacklist security issue 
// 10/04/14   ck       Fix mass storage app issue in 64 bit UEFI
// 09/22/14   ar       Add parameter limit checks for security warnings
// 05/12/14	  gothekar Move VBUS detection logic into target specifc SW
// 04/30/14   ar       Cleanup for NDEBUG build  
// 08/17/11   sbryksin AHB/GEN_CONFIG settings changes for 8960
// 08/30/10   dpatnaik Port QHSUSB to RPM, add ULPI based charger detection
// 03/17/09   dhaval   Removed compiler warnings.
// 07/14/08   amirs    First Draft
// 08/08/08   amirs    First working DLOAD process
// 08/11/08   amirs    Add files and functions headers according to coding standard.
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "com_dtypes.h"       // common defines - basic types as uint32 etc
#include "Usbfn.h"
#include "UsbDci.h"           // dQH_t
#include "UsbUrb.h"           // URB structure - qhsusb_urb_t
#include "UsbRegOffset.h"     // For Multi-Core
#include "qhsusb_ch9.h"       //
#include "qhsusb_dcd.h"       // DCD struct qhsusb_dcd_dsc_device

#include <Library/UsbfnLib.h> // DCI API Prototypes
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include "Library/ArmLib.h"
#include <Protocol/EFII2C.h>
#include <Protocol/EFIUsbConfig.h>
#include <Protocol/EFIChipInfo.h>
#include <Protocol/EFIPmicSmbb.h>

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
// None

//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------

core_info_t dci_core_info[DCI_MAX_CORES];
UINTN dci_core_base_address[DCI_MAX_CORES] = {0}; // By default HS-USB Core

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------
#define USBFN_GSBI                    8

static qhsusb_dci_callback_table_t *dci_callback_table_ptr = NULL;
static uint8 dci_setup_buffer[DCI_SETUP_BUFFER_SIZE];  // 8 bytes buffer

// @note dQH and dTD are access by hardware and therefore volatile
//__align(2048) // Queue-Head MUST be 2K aligned
//static dQH_t dci_dqh_list[DCI_MAX_ENDPOINTS][DCI_MAX_DIRECTION];  // 2 ep * 2 dir
static dQH_t **dci_dqh_list = NULL;
#define DQH_LIST_ALIGN 2048
//__align(32) // dTD MUST be 32 bytes aligned
//static dTD_t dci_dtd_list[DCI_MAX_ENDPOINTS][DCI_MAX_DIRECTION][DCI_MAX_TDS+1];  // 4 ep * 2 dir * 4 TDs
static dTD_t ***dci_dtd_list = NULL;
#define DTD_ALIGN 32
#define DTD_PAGE_OFFSET 4096

static uint32 gDCI_MAX_TDS = 1;

//
// This is the dummy buffer used when the host sends data with size
// equal to DCI_MAX_TDS * 16 KB, i.e the max transfer size.
// in this case, the device does not know whether the host will send an zero
// length pkt to terminate the transfer. If the host does sends the zero
// length pkt, we need to accesp it.
//
static uint8* dummy_buffer_for_zero_length_pkt;
#define DUMMY_BUFFER_SIZE 1024

#ifdef QHSUSB_DEBUG_DCI_ENABLE
#pragma arm section rwdata = "USB_DEBUG_DATA", zidata = "USB_DEBUG_DATA"
    static uint32 dci_assert_val=0;
    static const char *dci_assert_filename_ptr=0;
    static int dci_assert_line=0;

// debug counters for events
    static uint32 dci_handle_setup_counter=0;
    static uint32 dci_handle_system_error_counter=0;
    static uint32 dci_handle_usb_error_counter=0;

    static uint32 dci_handle_port_change_counter=0;
    static uint32 dci_handle_usb_reset_counter=0;
    static uint32 dci_handle_suspend_counter=0;

    static uint32 dci_handle_transfer_complete_counter=0;

    static uint32 dci_transfer_request_counter=0;
#pragma arm section rwdata, zidata
#endif // QHSUSB_DEBUG_DCI_ENABLE

static boolean  dci_timer_is_running[DCI_MAX_TIMERS] = {FALSE,FALSE};

static uint8    dci_usb_address                      = 0;

static boolean  dci_start_enum                       = FALSE;

static boolean  dci_chg_det_supported                = FALSE;

static BOOLEAN  pre_vbus_valid                       = FALSE;

// Save the detected charger port type
static qhsusb_chg_port_type chg_port_type = QHSUSB_CHG_PORT_UNKNOWN;

//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------

static uint32 dci_init_dqh_list(uint32 core_id);
// Events handlers
static void dci_handle_setup(uint32 core_id);
static void dci_handle_transfer_complete(uint32 core_id);
static void dci_handle_usb_reset(uint32 core_id);
static void dci_handle_port_status_changed(uint32 core_id);
static void dci_handle_suspend(uint32 core_id);

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------

//============================================================================


/**
 * @function  qhsusb_dci_config
 *
 * @brief
 *    This function saves per target configuration
 *
 *
 * @param usbfn private data structure belongs to the usb fn device
 *
 *
 * @return
 *        None
 *
 */

static void qhsusb_dci_config( USBFN_DEV *usbfn )
{
    EFI_STATUS Status;
    EFI_QCOM_I2C_PROTOCOL *I2CProtocol = NULL;

    if (!usbfn)
    {
        DBG(EFI_D_ERROR, "ERROR: usbfnIAL_DEV is null\n");
        return;
    }

    // kai: caution: need to resolve UINTN and uint32
    Status = usbfn->UsbConfigProtocol->GetCoreBaseAddr (usbfn->UsbConfigProtocol,
                    usbfn->CoreType, (UINTN*)(&dci_core_base_address[0]));
    WRN_CHK ("Failed to get base adress");

    if (USB_CONFIG_HSUSB1 == usbfn->CoreType)
    {
      dci_chg_det_supported = TRUE;
      dci_core_info[0].dcd_context_ptr->speed = QHSUSB_DCD_SPEED_HIGH;
    }
    else
    {
      DBG(EFI_D_ERROR, "ERROR: Unknown Configuration.\n");
        dci_chg_det_supported = FALSE;
      dci_core_info[0].dcd_context_ptr->speed = QHSUSB_DCD_SPEED_UNKNOWN;
    }


//    //get i2c driver pointer in case of FSUSB
//    if (usbfn->ClientType == PLAT_CONFIG_CLIENT_FS_USB)
//    {
//        // Open the QCOM I2C Protocol
//        Status = gBS->LocateProtocol(&gQcomI2CProtocolGuid, NULL, (VOID **)&I2CProtocol);
//        if (EFI_ERROR (Status))
//        {
//            DEBUG(( EFI_D_ERROR, __FUNCTION__ "Unable to open I2C protocol\r\n"));
//        }
//    }
//
    dci_core_info[0].i2c_protocol = I2CProtocol;

}


/**
 * @function  qhsusb_dci_init
 *
 * @brief
 *    This function initializes the USB device controller according to the ChipIdea spec:
 *    1. Set Controller Mode in the USBMODE register to device mode.
 *    2. Allocate and Initialize device queue heads in system memory.
 *    3. Configure ENDPOINTLISTADDR Pointer.
 *    4. Enable the microprocessor interrupt associated with the USBHS
 *    5. Set Run/Stop bit to Run Mode.
 *
 *  @Note : Endpoint 0 is designed as a control endpoint only and does not need to be
 *      configured using ENDPTCTRL0 register.
 *      It is also not necessary to initially prime Endpoint 0
 *      because the first packet received will always be a setup packet.
 *      The contents of the first setup packet will require a response
 *      in accordance with USB device framework (Chapter 9) command set.
 *
 * @param core_id
 *
 * @param dcd_context_ptr
 *              A pointer to a DCD initernal context
 * @param callback_table_ptr
 *              A pointer to a callback table
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 8.1 "Device Controller Initialization"
 *      Chapter 8.4.3.1.1 Setup Packet Handling (Pre2.3 hardware)
 *
 */
//============================================================================
uint32 qhsusb_dci_init(uint32 core_id, struct qhsusb_dcd_dsc_device* dcd_context_ptr,
                       qhsusb_dci_callback_table_t *callback_table_ptr)
{

    uint32 i,j;
  uint32      qstatus       = QHSUSB_OK;
  uint32      ep_list_addr  = 0;


  if(IS_NOT_32_BIT_ADDR(dci_core_base_address[0])) USB_ASSERT_GOTO(FALSE, EXIT);

    dci_locate_pmic_protocol_type();

    DCI_CHECK_CORE_ID(core_id);

    if (dcd_context_ptr->init)
    {
       qhsusb_dci_disconnect (core_id);
       // stall for at least 1 millisecond before reset
       // the RS bit.
       gBS->Stall(1000);
       qhsusb_dci_connect (core_id);
     goto EXIT;
    }
    dcd_context_ptr->init = TRUE;

    //  Save the Callbcak Table Pointer and DCD context
    dci_callback_table_ptr = callback_table_ptr;
    dci_core_info[core_id].dcd_context_ptr = dcd_context_ptr;

    // Set core/speed/platform configuration
    qhsusb_dci_config(dcd_context_ptr->usbfn);


    // 1 Set Controller Mode in the USBMODE register to device mode.
    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBMODE,USB_OTG_HS_USBMODE_CM_BMSK,USB_OTG_HS_USBMODE_CM_DEVICE_CONTROLLER_DEFAULT_FOR_DEVICE_ONLY_CONTROLLER_FVAL);

    // 2. Allocate and Initialize device queue heads in system memory.
    // 2.1  calculate the largest transfer size amount the pipe.
    for(i = 0; i < DCD_MAX_EP; ++i){
      for(j = 0; j < DCD_MAX_DIR; ++j){
        uint32 tds = UFN_MAX_SIZE_TO_TDS(dcd_context_ptr->max_transfer_size[i][j]);
        if(gDCI_MAX_TDS < tds){
          gDCI_MAX_TDS = tds;
        }
      }
    }
  qstatus = dci_init_dqh_list(core_id);
  if(QHSUSB_OK != qstatus){
      goto EXIT;
    }

    // 3. Configure ENDPOINTLISTADDR Pointer.

    ArmDataMemoryBarrier();

  ep_list_addr = (UINT32) ((UINTN) (dci_dqh_list[DCI_EP0]));
  DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPOINTLISTADDR, ep_list_addr);

    // 4. Enable the microprocessor interrupt associated with the USBHS
    // From 3.6.3 USBINTR :
    //      The USB Status register (USBSTS) still shows interrupt sources even if they are
    //      disabled by the USBINTR register, allowing polling of interrupt events by the software.
    DCI_HWIO_OUT(core_id,USB_OTG_HS_USBINTR,0);   // disable interrupts

    // Use SETUP TripWire instead of Lockout mechanism
    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBMODE,USB_OTG_HS_USBMODE_SLOM_BMSK, USB_OTG_HS_USBMODE_SLOM_SETUP_LOCKOUTS_OFF_FVAL);

    // Disable Stream Mode
    // DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBMODE,USB_OTG_HS_USBMODE_SDIS_BMSK, USB_OTG_HS_USBMODE_SDIS_STREAM_DISABLE_FVAL);

    //Initialize platform specific stuff (AHB settings, chicken bits, etc)
    qhsusb_dci_init_platform_specific(core_id);

    // Select PHY Type (PORTSC reg) - see dci_init_phy()

    // Make sure the 16 MSBs of this register are 0s (CI Ref code)
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENPDTSETUPSTAT,0);

    // 5. Set Run/Stop bit to Run Mode. Dont do this we will set R/S bit after charger detection.
    // Note: This might be not-relevant when using an external PHY.
    // If we are a HID device we will do charger detection after this and set R/S bit.
    if(qhsusb_dci_get_enum_flag() == TRUE)
    {
        DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RS_BMSK,USB_OTG_HS_USBCMD_RS_RUN_FVAL);
    }

    // =======================================================================
    // NOTE:
    //      Endpoint#0 dQH is configured in dci_init_dqh_list() and some HW is config by default
    //      It is the upper layers responsibility to call qhsusb_dci_config_endpoint() for non-control endpoints.
    // =======================================================================

    // Verify the HW is ok by checking the ID
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    {
        uint32 hwid = DCI_HWIO_IN(core_id,USB_OTG_HS_ID);

        if (hwid != USB_REG_ID_VALUE)
        {
      qstatus = QHSUSB_ERR_HW_FAILURE;
            DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
        }
    }
    #endif // QHSUSB_DEBUG_DCI_ENABLE
EXIT:
  return (qstatus);
}

//============================================================================
/**
 * @function    dci_init_dqh_list
 * @brief       Init the dQH and dTD list with default values.
 *
 * @param core_id
 *
 * @return  none.
 *
 * @see qhsusb_dci_init
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 8.5.1 Queue Head Initialization
 *
 */
//============================================================================
//
// ToDo: The controller initialization chain should return error message if
// the contrller cannot be initialized
//
static uint32 dci_init_dqh_list(uint32 core_id)
{
    uint32 ep,dir,td;
    uint32 status = QHSUSB_OK;

    // Allocate Memory for QHead
    dci_dqh_list = (dQH_t **) AllocateZeroPool(DCI_MAX_ENDPOINTS * sizeof(dQH_t *));
    if(NULL == dci_dqh_list){
      DBG(EFI_D_ERROR,"Failed to allocate memory for QHead");
      status = QHSUSB_ERR_OUT_OF_RESOURCES;
      goto EXIT;
    }

  dci_dqh_list[0] = (dQH_t *) UncachedAllocateAlignedZeroPool(
      DCI_MAX_ENDPOINTS * DCI_MAX_DIRECTION * sizeof(dQH_t),
      DQH_LIST_ALIGN);
    if(NULL == dci_dqh_list[0]){
      DBG(EFI_D_ERROR,"Failed to allocate memory for QHead");
      status = QHSUSB_ERR_OUT_OF_RESOURCES;
      goto EXIT;
    }
  if(IS_NOT_32_BIT_ADDR(dci_dqh_list[0]) || 
     !IS_ALIGNED(dci_dqh_list[0], DQH_LIST_ALIGN)) {
      status = QHSUSB_ERR_BAD_PARAM;
      USB_ASSERT_GOTO(FALSE, EXIT);
    }

    for(ep = 1; ep < DCI_MAX_ENDPOINTS; ++ep){
      dci_dqh_list[ep] = dci_dqh_list[ep-1] + DCI_MAX_DIRECTION;
    }

    //
    // Address for QHead
    //
    for(ep = 0; ep < DCI_MAX_ENDPOINTS; ++ep){
      for(dir = 0; dir < DCI_MAX_DIRECTION; ++dir){
        DBG(EFI_D_POOL,"dci_dqh_list[%d][%d]=>0x%x",ep,dir, &dci_dqh_list[ep][dir]);
      }
    }

    // Create dTDs
    dci_dtd_list = (dTD_t ***) AllocateZeroPool( DCI_MAX_ENDPOINTS * sizeof(dTD_t **));
    if(NULL == dci_dtd_list){
      DBG(EFI_D_ERROR,"Failed to allocate memory for dTd list");
      status =  QHSUSB_ERR_OUT_OF_RESOURCES;
      goto EXIT;
    }

    dci_dtd_list[0] = (dTD_t **) AllocateZeroPool( DCI_MAX_ENDPOINTS * DCI_MAX_DIRECTION * sizeof( dTD_t *));
    if(NULL == dci_dtd_list[0]){
      DBG(EFI_D_ERROR,"Failed to allocate memory for dTd list");
      status =  QHSUSB_ERR_OUT_OF_RESOURCES;
      goto EXIT;
    }
  
  dci_dtd_list[0][0] = (dTD_t *) UncachedAllocateAlignedZeroPool( 
    DCI_MAX_ENDPOINTS * DCI_MAX_DIRECTION * (gDCI_MAX_TDS) * sizeof(dTD_t),
    DTD_ALIGN);
    if(NULL == dci_dtd_list[0][0]){
      DBG(EFI_D_ERROR,"Failed to allocate memory for dTd list");
      status =  QHSUSB_ERR_OUT_OF_RESOURCES;
      goto EXIT;
    }
  if(IS_NOT_32_BIT_ADDR(dci_dtd_list[0][0]) ||
     !IS_ALIGNED(dci_dtd_list[0][0], DTD_ALIGN)){
      status = QHSUSB_ERR_BAD_PARAM;
      USB_ASSERT_GOTO(FALSE, EXIT);
    }

    for ( dir = 1  ;  dir < DCI_MAX_DIRECTION  ;  ++dir ) {
      dci_dtd_list[0][dir] = dci_dtd_list[0][dir-1] + (gDCI_MAX_TDS);
    }
    for ( ep = 1  ;  ep < DCI_MAX_ENDPOINTS  ;  ++ep ) {
      dci_dtd_list[ep] = dci_dtd_list[ep-1] + DCI_MAX_DIRECTION;
      dci_dtd_list[ep][0] = dci_dtd_list[ep-1][DCI_MAX_DIRECTION-1] + (gDCI_MAX_TDS);
      for ( dir = 1  ;  dir < DCI_MAX_DIRECTION  ;  ++dir ) {
        dci_dtd_list[ep][dir] = dci_dtd_list[ep][dir-1] + (gDCI_MAX_TDS);
      }
    }

    //
    // Address for dTDs: since the number of dTDs can go very large,
    // depending on the max transfer size, only the first 4 are
    // printed, which should be enough for infer other dTDs address
    //
#define MAX_TDS_PRINTED 4
    for(ep = 0; ep < DCI_MAX_ENDPOINTS; ++ep){
      for(dir = 0; dir < DCI_MAX_DIRECTION; ++dir){
        for(td = 0; td < gDCI_MAX_TDS && td < MAX_TDS_PRINTED; ++td){
          DBG(EFI_D_POOL,"dci_dtd_list[%d][%d][%d]=>0x%x",ep,dir,td,&dci_dtd_list[ep][dir][td]);
        }
      }
    }

    // NOTE: dQH and dTD are not relevant before endpoints are primed.
    //       setup packet has deticated 8 bytes buffer in the dQH

  dummy_buffer_for_zero_length_pkt = UncachedAllocateAlignedZeroPool(
                                          DUMMY_BUFFER_SIZE,
                                          USBFN_DEFAULT_BUFFER_ALIGN);
    if(NULL == dummy_buffer_for_zero_length_pkt){
      DBG(EFI_D_ERROR,"Failed to allocate dummy buffer.");
      status =  QHSUSB_ERR_OUT_OF_RESOURCES;
      goto EXIT;
    }
  if(IS_NOT_32_BIT_ADDR(dummy_buffer_for_zero_length_pkt) || 
     !IS_ALIGNED(dummy_buffer_for_zero_length_pkt, USBFN_DEFAULT_BUFFER_ALIGN)) {
      status = QHSUSB_ERR_BAD_PARAM;
      USB_ASSERT_GOTO(FALSE, EXIT);
    }

    DBG(EFI_D_POOL,"dummy_buffer_for_zero_length_pkt=>0x%x",
        dummy_buffer_for_zero_length_pkt
        );

    for (ep=0;ep<DCI_MAX_ENDPOINTS;ep++)
    {
        for (dir=0;dir<DCI_MAX_DIRECTION;dir++)
        {
            dci_dqh_list[ep][dir].next_dtd_ptr = (UINT32) (UINTN) &dci_dtd_list[ep][dir][0];
            for (td = 0; td < gDCI_MAX_TDS; td++)
            { /* TDs are not linked at this stage */
              dci_dtd_list[ep][dir][td].next_dtd_ptr = DTD_TERMINATE_BMSK;
            }
        }
    }

    // Endpoint#0 RX max packet length is 64 bytes - for full-speed
    dci_dqh_list[DCI_EP0][DIR_RX].mult_zlt_packetlen_ios =
        (DCI_EP0_MAX_PACKET_SIZE << DQH_MAX_PACKET_LEN_SHFT);
    // Endpoint#0 TX max packet length is 64 bytes - for full-speed
    dci_dqh_list[DCI_EP0][DIR_TX].mult_zlt_packetlen_ios =
        (DCI_EP0_MAX_PACKET_SIZE << DQH_MAX_PACKET_LEN_SHFT);

    // Enable Endpoint#0 IOS ( Interrupt-On-Setup )
    dci_dqh_list[DCI_EP0][DIR_RX].mult_zlt_packetlen_ios |= DQH_IOS_BMSK;

EXIT:
    //
    // Deallocate the resources if something goes wrong
    //
    if(QHSUSB_OK != status){
      if(dci_dqh_list){
        if(dci_dqh_list[0]){
          UncachedFreeAlignedPool((void *)(dci_dqh_list[0]));
        }
        FreePool(dci_dqh_list);
      }
      if(dci_dtd_list){
        if(dci_dtd_list[0]){
          if(dci_dtd_list[0][0]){
            UncachedFreeAlignedPool((void *)(dci_dtd_list[0][0]));
          }
          FreePool(dci_dtd_list[0]);
        }
        FreePool(dci_dtd_list);
      }
      if(dummy_buffer_for_zero_length_pkt){
        UncachedFreeAlignedPool(dummy_buffer_for_zero_length_pkt);
      }
    }
    return status;
}


//============================================================================
/**
 * @function qhsusb_dci_poll_events
 * @brief   Check for any pending events (interrupts are disabled) and call the relevant callbacks.
 *
 * @param core_id
 *
 * @return  none.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 8.4.3.1.2 Setup Packet Handling (2.3 hardware and later)
 *      Chapter 3.6.2 USBSTS
 *
 */
//============================================================================
void qhsusb_dci_poll_events(uint32 core_id)
{
    uint32          usbsts                   = DCI_HWIO_IN(core_id,USB_OTG_HS_USBSTS);
    uint32          setup_status             = DCI_HWIO_IN(core_id,USB_OTG_HS_ENPDTSETUPSTAT);    
    BOOLEAN         curr_vbus_valid          = FALSE;    

    DCI_CHECK_CORE_ID(core_id);

    // Clear status bits
    DCI_HWIO_OUT(core_id,USB_OTG_HS_USBSTS,usbsts);

    // Handle Cable Attach / Detach status change
    struct qhsusb_dcd_dsc_device *dcd_context_ptr = dci_core_info[core_id].dcd_context_ptr;

    curr_vbus_valid = dci_is_vbus_valid();

    if (pre_vbus_valid != curr_vbus_valid) {
      DBG(EFI_D_INFO, "vbus %d -> %d", pre_vbus_valid, curr_vbus_valid);
      pre_vbus_valid = curr_vbus_valid;
      dci_callback_table_ptr->usb_cable_status_change_callback_fn(dcd_context_ptr, curr_vbus_valid);
    }

    // USB Reset
    // When the device controller detects a USB Reset and enters the default state,
    // this bit will be set to a one
    if (usbsts & USB_OTG_HS_USBSTS_URI_BMSK)
    {
        DBG(EFI_D_INFO, "Reset");
        DCI_INC_DEBUG_COUNTER(dci_handle_usb_reset_counter);
        dci_handle_usb_reset(core_id);
    }

    // Device Suspend
    // When a device controller enters a suspend state from an active state, this bit will be set to a one.
    if (usbsts & USB_OTG_HS_USBSTS_SLI_BMSK)
    {
        DBG(EFI_D_INFO, "Suspend");
        DCI_INC_DEBUG_COUNTER(dci_handle_suspend_counter);
        dci_handle_suspend(core_id);
    }

    // Port Change Detect = Resume / Speed
    // The Device Controller sets this bit to a one when it detects resume signaling or the port
    // controller enters the full or highspeed operational state.
    // When the port controller exits the full  or highspeed operation states due to Reset or Suspend events,
    // the notification mechanisms are the USB Reset Received bit and the DCSuspend bits respectively
    if (usbsts & USB_OTG_HS_USBSTS_PCI_BMSK)
    {
        DBG(EFI_D_INFO, "Port change");
        DCI_INC_DEBUG_COUNTER(dci_handle_port_change_counter);
        dci_handle_port_status_changed(core_id);
    }

    // System Error
    // In the AMBA implementation, this bit will be set to "1" when
    // an Error response is seen by the master interface (HRESP[1:0]=ERROR).
    if (usbsts & USB_OTG_HS_USBSTS_SEI_BMSK)
    {
        DBG(EFI_D_ERROR, "System error");
        DCI_INC_DEBUG_COUNTER(dci_handle_system_error_counter);
        DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
    }

    // USB Error Interrupt (USBERRINT)
    // When completion of a USB transaction results in an error condition,
    // this bit is set by the Host/Device Controller.
    // This bit is set along with the USBINT bit,
    // if the TD on which the error interrupt occurred also had its interrupt on complete (IOC) bit set
    if (usbsts & USB_OTG_HS_USBSTS_UEI_BMSK)
    {
        DBG(EFI_D_ERROR, "USB error");
        DCI_INC_DEBUG_COUNTER(dci_handle_usb_error_counter);
        DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
    }

    // USB Interrupt (USBINT) = Transfer Completed
    // This bit is set by the Host/Device Controller when the cause of an interrupt
    // is a completion of a USB transaction where the Transfer Descriptor (TD) has an interrupt on complete (IOC) bit set.
    if (usbsts & USB_OTG_HS_USBSTS_UI_BMSK)
    {
        DBG(EFI_D_INFO, "Transfer complete");
        DCI_INC_DEBUG_COUNTER(dci_handle_transfer_complete_counter);
        dci_handle_transfer_complete(core_id);
    }

    // Setup Received
    if (setup_status & 0x01)
    {
        DBG(EFI_D_INFO, "Setup");
        DCI_INC_DEBUG_COUNTER(dci_handle_setup_counter);
        dci_handle_setup(core_id);
    }

}

// ===========================================================================
/**
 * @function    dci_handle_setup
 *
 * @brief       This function handles setup receive event
 *
 * @details
 *      After receiving an interrupt and inspecting ENDPTSETUPSTAT to determine that a setup packet was received
 *      on a particular pipe:
 *       1. Write '1' to clear corresponding bit ENDPTSETUPSTAT.
 *       2. Write ‘1’ to Setup Tripwire (SUTW) in USBCMD register.
 *       3. Duplicate contents of dQH.SetupBuffer into local software byte array.
 *       4. Read Setup TripWire (SUTW) in USBCMD register. (if set continue; if cleared goto 2)
 *       5. Write '0' to clear Setup Tripwire (SUTW) in USBCMD register.
 *       6. Process setup packet using local software byte array copy and execute status/handshake phases.
 *       7. Before priming for status/handshake phases ensure that ENDPTSETUPSTAT is ‘0’.
 *
 *
 *       A poll loop should be used to wait until ENDPTSETUPSTAT transitions to ‘0’ after step 1) above and
 *      before priming for the status/handshake phases.
 *       In core versions 3.2 and later, the time from writing a ‘1’ to ENDPTSETUPSTAT and reading back a
 *      ‘0’ is very short (~12 us) so a poll loop in the DCD will not be harmful.
 *       In core versions 3.1 and earlier, the time from writing a ‘1’ to ENDPTSETUPSTAT and reading back a
 *      ‘0’ may vary according to the type of traffic on the bus up to nearly a 1ms, however the it is absolutely
 *      necessary to ensure ENDPTSETUPSTAT has transitioned to ‘0’ after step 1) and before priming for the
 *      status/handshake phases.
 *
 *  @note : After receiving a new setup packet the status and/or handshake phases
 *      may still be pending from a previous control sequence. These should be
 *      flushed & deallocated before linking a new status and/or handshake dTD for
 *      the most recent setup packet
 *
 *  @note : A setup transfer does not use a dTD
 *      but instead stores the incoming data from a setup packet in an 8byte buffer within the dQH
 *
 * @param core_id
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *     Chapter 8.4.3.1.2 Setup Packet Handling (2.3 hardware and later)
 *     Chapter 8.5.2 Operational Model For Setup Transfers
 *
 */
// ===========================================================================
static void dci_handle_setup(uint32 core_id)
{
    uint32 usbcmd=0;
    uint32 setup_status=0;

    DCI_CHECK_CORE_ID(core_id);
    
    //  1. Write '1' to clear corresponding bit ENDPTSETUPSTAT.
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENPDTSETUPSTAT,0x01);
    do
    {
        setup_status = DCI_HWIO_INM(core_id,USB_OTG_HS_ENPDTSETUPSTAT,0x01);
    } while( setup_status != 0);

    do
    {
        //  2. Write ‘1’ to Setup Tripwire (SUTW) in USBCMD register.
        DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_SUTW_BMSK,USB_OTG_HS_USBCMD_SUTW_SET_FVAL);
        //  3. Duplicate contents of dQH.SetupBuffer into local software byte array.
        CopyMem((uint8*)dci_setup_buffer, (uint8*)dci_dqh_list[DCI_EP0][DIR_RX].setup_buffer, sizeof(dci_setup_buffer));
        //  4. Read Setup TripWire (SUTW) in USBCMD register. (if set continue; if cleared goto 2)
        usbcmd = DCI_HWIO_INM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_SUTW_BMSK);
    } while(usbcmd == 0);

    //  5. Write '0' to clear Setup Tripwire (SUTW) in USBCMD register.
    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_SUTW_BMSK,USB_OTG_HS_USBCMD_SUTW_CLR_FVAL);
    //  6. Process setup packet using local software byte array copy and execute status/handshake phases.
    //  7. Before priming for status/handshake phases ensure that ENDPTSETUPSTAT is ‘0’.

    // Call the DCD callback
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    if(dci_callback_table_ptr->setup_callback_fn != NULL)
    #endif // QHSUSB_DEBUG_DCI_ENABLE
    {
       /**
        * VK: @todo get actual core index
        */
       uint32 core_index = core_id;
       void *dcd_context_ptr = dci_core_info[core_index].dcd_context_ptr;

       dci_callback_table_ptr->setup_callback_fn(dcd_context_ptr, dci_setup_buffer);
    }
}

// ===========================================================================
/**
 * @function    dci_handle_transfer_complete
 *
 * @brief       This function handles transfer complete event.
 *
 * @details
 *      After a dTD has been initialized and the associated endpoint primed
 *      the device controller will execute the transfer upon the hostinitiated request.
 *      The DCD will be notified with a USB interrupt if the Interrupt On Complete bit was set or alternately,
 *      the DCD can poll the endpoint complete register to find when the dTD had been executed.
 *
 *      After a dTD has been executed, DCD can check the status bits to determine success or failure
 *
 * @param core_id
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 8.6.4 Transfer Completion
 *      Chapter 3.6.22 ENDPTCOMPLETE
 */
// ===========================================================================
static void dci_handle_transfer_complete(uint32 core_id)
{

  uint8 ep;
  uint8 dir;
  uint32 ep_complete = 0;
  uint32  ep_stat     = 0;
  dTD_t *dtd_ptr = NULL;

  uint32 remain_bytes = 0;
  uint32 status = 0;

  qhsusb_urb_t *urb_ptr;

  boolean client_call_back;

  DCI_CHECK_CORE_ID(core_id);
  
  ep_complete = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTCOMPLETE);
  ep_stat     = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTSTAT);
  (void)ep_stat; // Make compiler happy

  DBG(EFI_D_INFO,"ep_complete 0x%x, ep_state 0x%x",ep_complete, ep_stat);

  // Clear the indication for the next transfers
  // Note: The variable ep_complete holds the current status
  DCI_HWIO_OUT(core_id, USB_OTG_HS_ENDPTCOMPLETE, ep_complete);

  for (ep = 0; ep < DCI_MAX_ENDPOINTS; ep++)
  {
    for (dir = DIR_RX; dir <= DIR_TX; dir++)
    {
      uint32 mask = (1 << (ep + dir * USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT));
      client_call_back = FALSE;

      if ((ep_complete & mask))
      {

        boolean short_pkt = FALSE;

        // step 1. Extract info from dQH and dTD
        urb_ptr = (void*) (UINTN) dci_dtd_list[ep][dir][0].urb_ptr;
        if( NULL == urb_ptr ) {
          DBG(EFI_D_ERROR,"\n dci_dtd_list[%d][%d][0].urb_ptr is NULL. \n",ep,dir);
          return;
        }

        urb_ptr->transfer_status = QHSUSB_URB_STATUS_COMPLETE_OK;

        //
        // calculate how many bytes have been transferred (TX).
        //
        // process as many as dTDs as possible
        do {

          if(urb_ptr->current_dTD_index > urb_ptr->max_dTD_index) break;

          uint32  data_size_dtd = 0; // how many bytes received in the current dTD.
          dtd_ptr = &dci_dtd_list[ep][dir][urb_ptr->current_dTD_index];
          status  = (dtd_ptr->size_ioc_mult_sts) & DTD_STATUS_BMSK;

          //
          // only process the dTD if it finishes without error
          //
          if(0 == status){
            remain_bytes = (dtd_ptr->size_ioc_mult_sts) >> DTD_TOTAL_BYTES_SHFT;
            data_size_dtd = ((urb_ptr->transfer_length_dTD)[urb_ptr->current_dTD_index] - remain_bytes);
            if(data_size_dtd != DTD_MAX_TRANSFER_16KB){
              short_pkt = TRUE;
            }
            urb_ptr->actual_length += data_size_dtd;

            DBG(EFI_D_INFO,"Process dci_dtd_list[%d][%d][%d] @ 0x%p, data processed %d, total data processed %d",
                ep,dir,urb_ptr->current_dTD_index, urb_ptr, data_size_dtd, urb_ptr->actual_length);

            urb_ptr->current_dTD_index += 1;
          } else if (0x80 == status) {
            urb_ptr->transfer_status = QHSUSB_URB_STATUS_ACTIVE;
            DBG(EFI_D_INFO,"Process dci_dtd_list[%d][%d][%d] @ 0x%p. Xfer is active",
                ep,dir,urb_ptr->current_dTD_index, urb_ptr);
          } else {
            urb_ptr->transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
            DBG(EFI_D_INFO,"Process dci_dtd_list[%d][%d][%d] @ 0x%p. dTD status = 0x%x",
                ep,dir,urb_ptr->current_dTD_index, urb_ptr, status);
          }
        } while (0 == status && FALSE == short_pkt && urb_ptr->actual_length != urb_ptr->transfer_length);

        // The Xfer ends in one of the following conditions:
        // (1) A short pkt
        // (2) All of the buffer has been used.
        if(TRUE == short_pkt || urb_ptr->actual_length == urb_ptr->transfer_length || QHSUSB_URB_STATUS_COMPLETE_ERROR == urb_ptr->transfer_status){
#define MAX_WAIT_CNT_RETYR 1000
          uint32 wait_cnt = 0;

          uint32 endptflush, endptstat, endptcmplt;

          uint32 ep_stat_mask = (1 << (ep + dir * USB_OTG_HS_ENDPTSTAT_ETBR_SHFT));

          endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,ep_stat_mask);

          DBG(EFI_D_INFO, "END REASON: short pkt %d, urb_ptr->actual_length %d, urb_ptr->transfer_length %d, urb_ptr->transfer_status %d",
              short_pkt,
              urb_ptr->actual_length,
              urb_ptr->transfer_length,
              urb_ptr->transfer_status);

          // It's possible that the client xfer has finished but endptstat is not zero.
          // For instance, if less dTDs are used than the ones
          // chained in the hardware, endptstat will remain one until the endpt has been
          // flushed.
          if(ep_stat_mask){
            DBG(EFI_D_INFO,"EP %d, DIR %d: Force dTDs to be retired",ep,dir);
            do
            {
              // 1. Write a ‘1’ to the corresponding bit(s) in ENDPTFLUSH.
              DCI_HWIO_OUT(0,USB_OTG_HS_ENDPTFLUSH, (1<<ep));

              // 2. Wait until all bits in ENDPTFLUSH are ‘0’.
              do
              {
                endptflush = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTFLUSH,(1<<ep));
              } while (endptflush != 0);

              // 3. Read ENDPTSTAT to ensure that for all endpoints commanded to be flushed, that the corresponding
              endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,ep_stat_mask);
            }while(endptstat != 0);
          }

          do {
            wait_cnt++;
            endptcmplt = DCI_HWIO_INM(core_id, USB_OTG_HS_ENDPTCOMPLETE, mask);
            if(endptcmplt) {
              DCI_HWIO_OUT(core_id, USB_OTG_HS_ENDPTCOMPLETE, endptcmplt);
            }
          } while(endptcmplt && wait_cnt < MAX_WAIT_CNT_RETYR);

          client_call_back = TRUE;

          DBG(
              EFI_D_INFO,
              "dTD completes successfully. "
              "ep %d, "
              "dir %d, "
              "is short pkt %d, "
              "current dTD index %d, "
              "inform user total %d bytes received",
              ep,
              dir,
              short_pkt,
              urb_ptr->current_dTD_index,
              urb_ptr->actual_length);
        }

        if(TRUE == client_call_back) {

          // step 3: call the URB callbacks
          if (urb_ptr->complete_callback != NULL) {
            urb_ptr->complete_callback(urb_ptr);
          }
          dci_dtd_list[ep][dir][0].urb_ptr = 0;
        }
      } // complete detected
    } // for dir
  } // for ep
}


// ===========================================================================
/**
 * @function    dci_handle_usb_reset
 *
 * @brief       This function handles USB Reset event.
 *
 * @details
 *      A bus reset is used by the host to initialize downstream devices.
 *      When a bus reset is detected, the DEVICE CONTROLLER will:
 *       1. Renegotiate its attachment speed,
 *       2. Reset the device address to 0,
 *       3. Notify the DCD by interrupt (assuming the USB Reset Interrupt Enable is set).
 *
 *      After a reset is received, all endpoints (except endpoint 0) are disabled
 *      and any primed transactions will be cancelled by the device controller.
 *
 *      The DCD must perform the following tasks when a reset is received:
 *       1. Clear all setup token semaphores by reading the ENDPTSETUPSTAT register and writing the same value back to the ENDPTSETUPSTAT register.
 *       2. Clear all the endpoint complete status bits by reading the ENDPTCOMPLETE register and writing the same value back to the ENDPTCOMPLETE register.
 *       3. Cancel all primed status by waiting until all bits in the ENDPTPRIME are 0 and then writing 0xFFFFFFFF to ENDPTFLUSH.
 *       4. Read the reset bit in the PORTSCx register and make sure that it is still active.
 *       5. Free all allocated dTDs because they will no longer be executed by the device controller.
 *
 * @param core_id
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *       Chapter 8.2.1 Bus Reset
 */
// ===========================================================================
static void dci_handle_usb_reset(uint32 core_id)
{

    uint32 tmp=0;
    //int ep,dir;

    //if (dci_handle_setup_counter == 0)
    //{
    //    return; // 1st reset during power-up.
    //}

    DCI_CHECK_CORE_ID(core_id);

    //  1. Clear all setup token semaphores by reading the ENDPTSETUPSTAT register and writing the same value back to the ENDPTSETUPSTAT register.
    tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_ENPDTSETUPSTAT);
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENPDTSETUPSTAT,tmp);

    //  2. Clear all the endpoint complete status bits by reading the ENDPTCOMPLETE register and writing the same value back to the ENDPTCOMPLETE register.
    tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTCOMPLETE);
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTCOMPLETE,tmp);

    //  3. Cancel all primed status by waiting until all bits in the ENDPTPRIME are 0 and then writing 0xFFFFFFFF to ENDPTFLUSH.
    do
    {
        tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTPRIME);
    } while (tmp != 0);
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTFLUSH,0xFFFFFFFF);

    //  4. Read the reset bit in the PORTSCx register and make sure that it is still active.
    tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_PORTSC);
    if ((tmp & USB_OTG_HS_PORTSC_PR_BMSK) == 0) // PR=Port-Reset
    {
        DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
    }

    //  5. Optional: Free all allocated dTDs because they will no longer be executed by the device controller.
    // NOTE: dQH and dTD are not relevant before endpoints are primed.
    //       setup packet has dedicated 8 bytes buffer in the dQH
    //ZeroMem(dci_dtd_list, sizeof(dci_dtd_list));

    //for (ep=0;ep<DCI_MAX_ENDPOINTS;ep++)
    //    for (dir=0;dir<DCI_MAX_DIRECTION;dir++)
    //{
        // currently for simplicity - only one dTD per dQH - no chain yet.
    //    dci_dtd_list[ep][dir].next_dtd_ptr = DTD_TERMINATE_BMSK;
    //}

   {  // Call DCD callback
      uint32 core_index = core_id;
      void *dcd_context_ptr = dci_core_info[core_index].dcd_context_ptr;

      dci_callback_table_ptr->usb_reset_callback_fn(dcd_context_ptr);
   }
   //dci_handle_setup_counter = 0;
}

// ===========================================================================
/**
 * @function    dci_handle_port_status_changed
 *
 * @brief       This function handles USB port status changed.
 *
 * @details     This function notify the DCD about USB-Resume signal or Speed-Detection.
 *
 * @param core_id
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *       Chapter 3.6.16 PORTSCx
 */
// ===========================================================================
static void dci_handle_port_status_changed(uint32 core_id)
{
    DCI_CHECK_CORE_ID(core_id);
  
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    if(dci_callback_table_ptr->port_status_changed_callback_fn != NULL)
    #endif // QHSUSB_DEBUG_DCI_ENABLE
    {
       uint32 core_index = core_id;
       void *dcd_context_ptr = dci_core_info[core_index].dcd_context_ptr;

       dci_callback_table_ptr->port_status_changed_callback_fn(dcd_context_ptr);
    }
}

// ===========================================================================
/**
 * @function    dci_handle_suspend
 *
 * @brief       This function handles USB suspend event
 *
 * @details     This function notify the DCD.
 *
 * @param core_id
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *       Chapter 3.6.16 PORTSCx
 */
// ===========================================================================
static void dci_handle_suspend(uint32 core_id)
{
    // Notify DCD
    dci_handle_port_status_changed(core_id);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_set_address
 *
 * @brief       This function set the device address
 *
 * @details
 *      USBADRA Device Address Advance. Default=0.
 *      When this bit is ‘0’, any writes to USBADR are instantaneous.
 *      When this bit is written to a ‘1’ at the same time or before USBADR is written,
 *      the write to the USBADR field is staged and held in a hidden register.
 *      After an IN occurs on endpoint 0 and is ACKed, USBADR will be loaded from the holding register.
 *
 *      Hardware will automatically clear this bit on the following conditions:
 *      1) IN is ACKed to endpoint 0. (USBADR is updated from staging register).
 *      2) OUT/SETUP occur to endpoint 0. (USBADR is not updated).
 *      3) Device Reset occurs (USBADR is reset to 0).
 *
 *      Note: After the status phase of the SET_ADDRESS descriptor, the DCD has 2 ms to program the USBADR field.
 *      This mechanism will ensure this specification is met when the DCD can not write of the device address within 2ms from the SET_ADDRESS status
 *      phase. If the DCD writes the USBADR with USBADRA=1 after the SET_ADDRESS data phase (before the prime of the status phase),
 *      the USBADR will be programmed instantly at the correct time and meet the 2ms USB requirement
 *
 * @param core_id
 *
 * @param address
 *      New Device Address.
 *
 * @return  none.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 3.6.6.2 Device Controller (USB DEVICEADDR)
 */
// ===========================================================================
void qhsusb_dci_set_address(uint32 core_id, uint8 address)
{
    uint32 val=0;

    //DCI_CHECK_CORE_ID(core_id);

    val = (address << USB_OTG_HS_DEVICEADDR_USBADR_SHFT) | USB_OTG_HS_DEVICEADDR_USBADRA_BMSK;
    DCI_HWIO_OUT(core_id,USB_OTG_HS_DEVICEADDR,val);

    if (dci_usb_address==0)
    {
      /* debug code */
      /* this was added to remove warning issued by compiler
         as this variable set but not used anywhere in code */
    }

    dci_usb_address = address; // debug
}

// ===========================================================================
/**
 * @function    qhsusb_dci_stall_endpoint
 *
 * @brief       This function stall an endpoint
 *
 * @details
 *      There are two occasions where the device controller may need to return to the host a STALL.
 *
 *      The first occasion is the FUNCTIONAL stall,
 *      which is a condition set by the DCD as described in the USB 2.0 device framework (chapter 9).
 *      A functional stall is only used on noncontrol endpoints and can be enabled in the device
 *      controller by setting the endpoint stall bit in the ENDPTCTRLx register
 *      associated with the given endpoint and the given direction.
 *
 *      In a functional stall condition, the device controller will continue to return STALL responses to all
 *      transactions occurring on the respective endpoint and direction until the endpoint stall bit is cleared by the DCD.
 *
 *      A PROTOCOL stall, unlike a function stall, is used on control endpoints is automatically cleared by the device
 *      controller at the start of a new control transaction (setup phase).
 *      When enabling a protocol stall, the DCD should enable the stall bits (both directions) as a pair.
 *      A single write to the ENDPTCTRLx register can ensure that both stall bits are set at the same instant.
 *
 * @note : Any write to the ENDPTCTRLx register during operational mode must preserve the endpoint type field (i.e. perform a readmodifywrite).
 *
 * @param core_id
 *
 * @param ep
 *          endpoint number
 *
 * @param dir
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *   Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *   Chapter 8.3.2 Stalling
 */
// ===========================================================================
uint32 qhsusb_dci_stall_endpoint(uint32 core_id, uint8 ep, uint8 dir)
{
    uint32 status=DCI_OK;
    uint32 mask=0;

    DCI_CHECK_CORE_ID(core_id);
    if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
    {
        /* No need to error out. Just don't perform the STALL*/
        DBG(EFI_D_WARN,"Invalid parameter, skipping operation");
        return(status); 
    }

    if (ep == 0)
    {
        mask =  USB_OTG_HS_ENDPTCTRLn_TXS_BMSK     // TXS = TX Endpoint Stall
              | USB_OTG_HS_ENDPTCTRLn_RXS_BMSK;    // RXS = RX Endpoint Stall
    }
    else
    {
        if (dir)
            mask = USB_OTG_HS_ENDPTCTRLn_TXS_BMSK; // TXS = TX Endpoint Stall
        else
            mask = USB_OTG_HS_ENDPTCTRLn_RXS_BMSK; // RXS = RX Endpoint Stall

    }

    DCI_HWIO_OUTMI(core_id,USB_OTG_HS_ENDPTCTRLn,ep,mask,mask);

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_unstall_endpoint
 *
 * @brief       This function unstall a stalled endpoint
 *
 * @details
 *      There are two occasions where the device controller may need to return to the host a STALL.
 *
 *      The first occasion is the FUNCTIONAL stall,
 *      which is a condition set by the DCD as described in the USB 2.0 device framework (chapter 9).
 *      A functional stall is only used on noncontrol endpoints and can be enabled in the device
 *      controller by setting the endpoint stall bit in the ENDPTCTRLx register
 *      associated with the given endpoint and the given direction.
 *
 *      In a functional stall condition, the device controller will continue to return STALL responses to all
 *      transactions occurring on the respective endpoint and direction until the endpoint stall bit is cleared by the DCD.
 *
 *      A PROTOCOL stall, unlike a function stall, is used on control endpoints is automatically cleared by the device
 *      controller at the start of a new control transaction (setup phase).
 *      When enabling a protocol stall, the DCD should enable the stall bits (both directions) as a pair.
 *      A single write to the ENDPTCTRLx register can ensure that both stall bits are set at the same instant.
 *
 * @note : Any write to the ENDPTCTRLx register during operational mode must preserve the endpoint type field (i.e. perform a readmodifywrite).
 *
 * @note : Reset Data-Toggle as mentioned in Chapter 8.3.3.1 Data Toggle Reset
 *
 * @param core_id
 *
 * @param ep
 *          endpoint number
 *
 * @param dir
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @see qhsusb_dci_stall_endpoint
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *   Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *   Chapter 8.3.2 Stalling
 *   Chapter 8.3.3.1 Data Toggle Reset
 */
// ===========================================================================
uint32 qhsusb_dci_unstall_endpoint(uint32 core_id, uint8 ep, uint8 dir)
{
    uint32 status=DCI_OK;
    uint32 mask=0;

    DCI_CHECK_CORE_ID(core_id);
    if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
    {
        /* No need to error out. Just don't perform the UNSTALL*/
        DBG(EFI_D_WARN,"Invalid parameter, skipping operation");
        return(status); 
    }

    if (ep == 0)
    {
        mask =  USB_OTG_HS_ENDPTCTRLn_TXS_BMSK     // TXS = TX Endpoint Stall
              + USB_OTG_HS_ENDPTCTRLn_RXS_BMSK;     // RXS = RX Endpoint Stall
    }
    else
    {
        if (dir)
        {
            mask = USB_OTG_HS_ENDPTCTRLn_TXS_BMSK  // TXS = TX Endpoint Stall
                 + USB_OTG_HS_ENDPTCTRLn_TXR_BMSK; // TXR = TX Data Toggle Reset

        }
        else
        {
            mask = USB_OTG_HS_ENDPTCTRLn_RXS_BMSK  // RXS = RX Endpoint Stall
                 + USB_OTG_HS_ENDPTCTRLn_RXR_BMSK; // RXR = RX Data Toggle Reset
        }
    }

    DCI_HWIO_OUTMI(core_id,USB_OTG_HS_ENDPTCTRLn,ep,mask,(mask & (USB_OTG_HS_ENDPTCTRLn_TXR_BMSK|USB_OTG_HS_ENDPTCTRLn_RXR_BMSK)));

    return(status);
}

boolean qhsusb_dci_get_endpoint_stall_state(uint32 core_id, uint8 ep, uint8 dir)
{
    uint32 mask=0;

    DCI_CHECK_CORE_ID(core_id);

    if (dir)
        mask = USB_OTG_HS_ENDPTCTRLn_TXS_BMSK; // TXS = TX Endpoint Stall
    else
        mask = USB_OTG_HS_ENDPTCTRLn_RXS_BMSK; // RXS = RX Endpoint Stall

    return DCI_HWIO_INI(core_id,USB_OTG_HS_ENDPTCTRLn,ep) & mask ? TRUE : FALSE;
}

// ===========================================================================
/**
 * @function    qhsusb_dci_config_endpoint
 *
 * @brief       This function configures the endpoint
 *
 * @details     This function set the ENDPTCTRLX register and the relevant dQH.
 *
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 *
 * @param core_id
 *
 * @param ep
 *          endpoint number
 * @param dir
 *          endpoint direction (1=TX,0=RX)
 * @param ep_type
 *          endpoint type:
 *          QHSUSB_EP_TYPE_CONTROL      0
 *          QHSUSB_EP_TYPE_ISO          1
 *          QHSUSB_EP_TYPE_BULK         2
 *          QHSUSB_EP_TYPE_INTERRUPT    3
 * @param max_packet_length
 *          maximum packet length - depends on the connection speed and the Hardware FIFOs.
 * @param zero_length_transfer
 *          if set to TRUE, and the transfer length is a multiply of the max_packet_length
 *          then a zero lenfth packet is added.
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *        Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *        Chapter 8.3.1 Endpoint Initialization
 */
// ===========================================================================
uint32 qhsusb_dci_config_endpoint(uint32 core_id, uint8 ep, uint8 dir,
                                  uint8 ep_type, uint16 max_packet_length,
                                  boolean zero_length_transfer)
{
    uint32 status=DCI_OK;
    uint32 ep_control = 0;
    uint32 mask = 0;

    if (ep == 0)
    {
        return(DCI_OK);  // can't change ep#0 type or packet length
    }

    if ((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION) || (core_id >= DCI_MAX_CORES) )
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    if (dir == QHSUSB_EP_DIR_TX)
    {
        ep_control =    USB_OTG_HS_ENDPTCTRLn_TXE_BMSK // TXE = TX Endpoint Enable
                      + USB_OTG_HS_ENDPTCTRLn_TXR_BMSK // TXR = TX Data Toggle Reset
                      + (ep_type << USB_OTG_HS_ENDPTCTRLn_TXT_SHFT); // TXT = TX Endpoint Type
        mask = 0xffff0000;
    }
    else
    {
        ep_control =    USB_OTG_HS_ENDPTCTRLn_RXE_BMSK // RXE = RX Endpoint Enable
                      + USB_OTG_HS_ENDPTCTRLn_RXR_BMSK // RXR = RX Data Toggle Reset
                      + (ep_type << USB_OTG_HS_ENDPTCTRLn_RXT_SHFT); // RXT = RX Endpoint Type
        mask = 0x0000ffff;
    }

    // Currently Hard-coded FS 64 bytes
    //dci_dqh_list[ep][dir].mult_zlt_packetlen_ios = (max_packet_length << DQH_MAX_PACKET_LEN_SHFT);
    dci_dqh_list[ep][dir].mult_zlt_packetlen_ios = (max_packet_length << DQH_MAX_PACKET_LEN_SHFT);
    if (zero_length_transfer == TRUE) // allow
    {
        //dci_dqh_list[ep][dir].mult_zlt_packetlen_ios &= (~DQH_ZLT_BMSK); // ZLT=0=Enable-Zero-Length-Transfer
        dci_dqh_list[ep][dir].mult_zlt_packetlen_ios &= (~DQH_ZLT_BMSK); // ZLT=0=Enable-Zero-Length-Transfer
    }
    else
    {
        //dci_dqh_list[ep][dir].mult_zlt_packetlen_ios |= DQH_ZLT_BMSK; // ZLT=1=Disable-Zero-Length-Transfer
        dci_dqh_list[ep][dir].mult_zlt_packetlen_ios |= DQH_ZLT_BMSK; // ZLT=1=Disable-Zero-Length-Transfer
    }

    DCI_HWIO_OUTMI(core_id,USB_OTG_HS_ENDPTCTRLn,ep,mask,ep_control);

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_unconfig_endpoint
 *
 * @brief       This function de-configures the endpoint
 *
 * @details     This function set the dQH with Max-Packet-Size=0,
 *              which results bus error when the host send IN/OUT tokens
 *
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 *
 * @note        caution: if one endpoint direction is enabled and the paired endpoint of opposite
 *              direction is disabled
 *              then the unused direction type must be changed from the default controltype
 *              to any other type (ie. bulktype).
 *              leaving an unconfigured endpoint control will cause undefined behavior
 *              for the data pid tracking on the active endpoint/direction.
 *
 * @param core_id
 *
 * @param ep
 *          endpoint number
 *
 * @param dir
 *          endpoint direction (1=TX,0=RX)
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @see qhsusb_dci_config_endpoint
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *        Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *        Chapter 8.3.1 Endpoint Initialization
 */
// ===========================================================================
uint32 qhsusb_dci_unconfig_endpoint(uint32 core_id, uint8 ep, uint8 dir)
{
    uint32 status=DCI_OK;

    if ((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION) || (core_id >= DCI_MAX_CORES) || !dci_dqh_list)
    {
        status = QHSUSB_ERR_BAD_PARAM;
    }
    else
    {
        // @note: set max-packet-length to zero disable any data packet from the host.
        dci_dqh_list[ep][dir].mult_zlt_packetlen_ios = 0;

        // @note: ep#0 RX & TX are always enabled.
        if (ep > DCI_EP0)
        {
            uint32 mask = 0;
            uint32 value = 0;

            if (dir == QHSUSB_EP_DIR_TX)
            {
                mask = USB_OTG_HS_ENDPTCTRLn_TXE_BMSK; // TXE = TX Endpoint Enable
            }
            else
            {
                mask = USB_OTG_HS_ENDPTCTRLn_RXE_BMSK; // RXE = RX Endpoint Enable
            }

            DCI_HWIO_OUTMI(core_id,USB_OTG_HS_ENDPTCTRLn,ep,mask,value);
        }
    }

    return(status);
}

// ===========================================================================
/**
 * @function    prepare_TDs_for_transfer
 *
 * @brief   This function prepare the TDs for each transfer
 *
 * @note    The number of TDs is limited to DCI_MAX_TDS
 *
 * @param ep
 *          endpoint number
 *
 * @param dir
 *          endpoint direction (1=TX,0=RX)
 *
 * @param buffer_addr
 *          pointer to buffer
 *
 * @param byte_count
 *          number of bytes to send (dir)
 *          or
 *          maximum number of bytes to receive (rx)
 *
 * @return None.
 */
// ===========================================================================
static void prepare_TDs_for_transfer(uint8 ep, uint8 dir, uint32 buffer_addr, uint32 byte_count)
{
  uint32 total_bytes_for_TD;
  uint32 td=0;
  dTD_t *dtd_ptr;
  dTD_t *prev_td;
  qhsusb_urb_t *urb_ptr;

  urb_ptr = (void *) (UINTN) dci_dtd_list[ep][dir][0].urb_ptr;

  do
  {
    total_bytes_for_TD = MIN (byte_count,DTD_MAX_TRANSFER_16KB);
    /*Initialize first 7 DWords to 0. */
    ZeroMem((uint8*)&dci_dtd_list[ep][dir][td],sizeof(dci_dtd_list[ep][dir][td]));
    dtd_ptr = &dci_dtd_list[ep][dir][td];


    (urb_ptr->transfer_length_dTD)[td] = total_bytes_for_TD;

    /* Fill in total bytes and initialize the status field with the active bit set
       to “1” and all remaining status bits set to “0”. */
    dtd_ptr->size_ioc_mult_sts =
      (total_bytes_for_TD << DTD_TOTAL_BYTES_SHFT) + DTD_STATUS_ACTIVE_BMSK;
    /* link the previous TD with this new one */
    if (td > 0)
    {
      prev_td = &(dci_dtd_list[ep][dir][td-1]);
      prev_td->next_dtd_ptr |= (uint32) (UINTN) dtd_ptr;
    }

    //
    // Set IOC for each dTD
    //
    /* Set the interrupt on complete */
    dtd_ptr->size_ioc_mult_sts |= DTD_IOC_BMSK;


    /*Fill in buffer pointer page 0 and the current offset to point to the start of the data buffer. */
    dtd_ptr->buffer0_ptr = buffer_addr;
    /* Initialize buffer pointer page 1 through page 4 to be one greater than each of the previous buffer pointer. */
    dtd_ptr->buffer1_ptr = dtd_ptr->buffer0_ptr+4*1024;
    dtd_ptr->buffer2_ptr = dtd_ptr->buffer1_ptr+4*1024;
    dtd_ptr->buffer3_ptr = dtd_ptr->buffer2_ptr+4*1024;
    dtd_ptr->buffer4_ptr = dtd_ptr->buffer3_ptr+4*1024;
    byte_count -= total_bytes_for_TD;
    buffer_addr += total_bytes_for_TD;
  } while ((byte_count > 0) && (++td < gDCI_MAX_TDS));

  /* Set the terminate bit to “1”. */
  dtd_ptr->next_dtd_ptr = DTD_TERMINATE_BMSK;

  /* Set the max dTD index */
  urb_ptr->max_dTD_index = td;

  //
  // Testing Code. Leave in on purpose
  //
  /*if(1 == dir && 1 == ep ){
    for(td = 0; td < gDCI_MAX_TDS; td++){
      if(td == gDCI_MAX_TDS || dci_dtd_list[ep][dir][td].size_ioc_mult_sts & DTD_IOC_BMSK){
      DBG(EFI_D_ERROR,
          "\ndci_dtd_list[%d][%d][%d] = 0x%x\n"
          "next_dtd_ptr = 0x%x\n"
          "size_ioc_mult_sts = 0x%x\n"
          "buffer0_ptr = 0x%x\n"
          "buffer1_prt = 0x%x\n"
          "buffer2_ptr = 0x%x\n"
          "buffer3_prt = 0x%x\n"
          "buffer4_ptr = 0x%x\n"
          "urb_ptr = 0x%x\n",
          ep,dir,td,
          &(dci_dtd_list[ep][dir][td]),
          dci_dtd_list[ep][dir][td].next_dtd_ptr,
          dci_dtd_list[ep][dir][td].size_ioc_mult_sts,
          dci_dtd_list[ep][dir][td].buffer0_ptr,
          dci_dtd_list[ep][dir][td].buffer1_ptr,
          dci_dtd_list[ep][dir][td].buffer2_ptr,
          dci_dtd_list[ep][dir][td].buffer3_ptr,
          dci_dtd_list[ep][dir][td].buffer4_ptr,
          dci_dtd_list[ep][dir][td].urb_ptr);
      }
    }
  }*/

}
// ===========================================================================
/**
 * @function    transfer_data_request
 *
 * @brief       This function initate rx/dir data transfer
 *
 * @details
 *
 *  The behaviors of the device controller for interrupt and bulk endpoints are identical.
 *  All valid IN and OUT transactions to bulk pipes will handshake with a NAK unless the endpoint had been primed.
 *  Once the endpoint has been primed, data delivery will commence.
 *
 *  A dTD will be retired by the device controller when the packets described in the transfer descriptor have
 *  been completed.
 *
 *  Each dTD describes N packets to be transferred.
 *    With Zero Length Termination (ZLT) = 0
 *       N = INT(Number Of Bytes/Max. Packet Length) + 1
 *    With Zero Length Termination (ZLT) = 1
 *       N = MAXINT(Number Of Bytes/Max. Packet Length)
 *
 *  Bit    | Status Field Description
 *  =================================
 *   7     |  Active.
 *   6     |  Halted.
 *   5     |  Data Buffer Error.
 *   3     |  Transaction Error.
 *   4,2,0 |  Reserved.
 *
 * @param core_id
 *
 * @param ep
 *          endpoint number
 * @param dir
 *          endpoint direction (1=TX,0=RX)
 * @param buffer_ptr
 *          pointer to buffer
 * @param byte_count
 *          number of bytes to send (dir)
 *          or
 *          maximum number of bytes to receive (rx)
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *   Chapter 3.6.19 ENDPTPRIME
 *   Chapter 8.4.1.1 Priming Transmit Endpoints
 *   Chapter 8.4.2 Interrupt/Bulk Endpoint Operational Model
 *   Chapter 8.6.2 Building a Transfer Descriptor
 *   Chapter 8.6.3 Executing A Transfer Descriptor
 */
// ===========================================================================
static uint32 transfer_data_request(uint32 core_id, uint8 ep, uint8 dir, uint32 buffer_ptr,uint32 byte_count )
{
    uint32 buffer_addr = buffer_ptr;
    dQH_t *dqh_ptr = &dci_dqh_list[ep][dir];
    dTD_t *dtd_ptr = &dci_dtd_list[ep][dir][0];
    uint32 mask = (1<<(ep+dir*USB_OTG_HS_ENDPTPRIME_PETB_SHFT));
    uint32 endptstat;
    uint32 urb_ptr = dtd_ptr->urb_ptr;

    DCI_CHECK_CORE_ID(core_id);
    
    // 8.4.3.1.2 Setup Packet Handling (2.3 hardware and later) :
    //   7. Before priming for status/handshake phases ensure that ENDPTSETUPSTAT is ‘0’.
    // NOTE: This is a rare case.
    //      The USB 2.0 spec allow the device the following timeout to responde:
    //      - no data pahse - up to 50 milisec
    //      - with data pahse - up to 500 milisec
    if (ep==0)
    {
        uint32 setup_status = DCI_HWIO_INM(core_id,USB_OTG_HS_ENPDTSETUPSTAT,0x01);
        if (setup_status)
        {
            DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
            return(QHSUSB_ERR_SETUP_PENDING);
        }
    }

    /*max DTD_MAX_TRANSFER_16KB*gDCI_MAX_TDS transfer*/
    if (byte_count > DTD_MAX_TRANSFER_16KB*(gDCI_MAX_TDS))
    {
      DBG (EFI_D_ERROR, "Requested Transfer is too large. Max is set to %d", DTD_MAX_TRANSFER_16KB*(gDCI_MAX_TDS));
      DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
      return(QHSUSB_ERR_BAD_PARAM);
    }

    //
    // Check if the endpoint is already primed
    //
    endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,mask);
    if(endptstat) // check if the endopint is active, if there is a pending transfer.
    {
        DCI_ASSERT(QHSUSB_ERR_INVALID_OPERATION);
        DBG (EFI_D_ERROR, "The endpoint has active transfer. Please cancel the transfer");
        return(QHSUSB_ERR_INVALID_OPERATION);
    }
    prepare_TDs_for_transfer(ep, dir, buffer_addr, byte_count);

    // 1. Write dQH next pointer AND dQH terminate bit to 0 as a single DWord operation.
    dqh_ptr->next_dtd_ptr = (uint32) (UINTN) &dci_dtd_list[ep][dir][0];

    // 2. Clear active & halt bit in dQH (in case set from a previous error).
    dqh_ptr->size_ioc_int_sts = 0;
    dci_dtd_list[ep][dir][0].urb_ptr = urb_ptr;

    // 3. Prime endpoint by writing ‘1’ to correct bit position in ENDPTPRIME.

    ArmDataMemoryBarrier();
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTPRIME,mask);

    // 4. Verify Endpoint was primed
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    {
        uint32 tmp=0;

        //! @todo add timeout or retry count
        do
        {
            tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTSTAT);
        } while ( (tmp & mask) == 0);


    }
    #endif // QHSUSB_DEBUG_DCI_ENABLE

    DBG (EFI_D_INFO, "queued URB on ep %1x, dir %1x, byte count 0x%x", ep, dir, byte_count);

    return(DCI_OK);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_transfer_request
 *
 * @brief       This function initate data transfer according to URB.
 *
 * @details
 *              This function calls transfer_data_request() and updtae the URB.
 *
 * @note        The USB is a Master-Slave protocol where the Device is the Slave.
 *              The Device's data is actually sent/received on the USB bus
 *              only when the Host send SETUP/IN/OUT token.
 *              Therefore, the caller only start a REQUEST.
 *              A callback is called upon complete/error detection.
 *              The caller May poll the status in the URB.
 *
 * @param urb_ptr
 *              A pointer to URB structure.
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @see transfer_data_request
 *
 */
// ===========================================================================
uint32 qhsusb_dci_transfer_request(qhsusb_urb_t* urb_ptr)
{
    uint32  status=0;
    uint8   ep=0;
    boolean dir=FALSE;
    uint32  buffer_ptr;
    uint32  byte_count = 0;
    uint32  core_id=urb_ptr->core_id;
    uint32  endpoint_control=0;
    uint32  mask=0;

    DCI_INC_DEBUG_COUNTER(dci_transfer_request_counter);

    // 1. Check Signature
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    if (urb_ptr->signature != QHSUSB_URB_SIGNATURE)
    {
        return(QHSUSB_ERR_URB_SIGNATURE);
    }
    #endif // QHSUSB_DEBUG_DCI_ENABLE

    DCI_CHECK_CORE_ID(core_id);

    // 2. extract parameters
    //usb 2.0 spec: bmRequestType 1 Bitmap Characteristics of request:
    //D7: Data transfer direction
    //0 = Host-to-device
    //1 = Device-to-host

    ep = urb_ptr->endpoint_address & 0x0F;
    dir = VAL_TO_BOOL(urb_ptr->endpoint_address & 0x80);
    buffer_ptr = urb_ptr->transfer_buffer_ptr;
    byte_count = urb_ptr->transfer_length;

    if (ep >= DCI_MAX_ENDPOINTS)
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    // Verify the endpoint is configured
    endpoint_control = DCI_HWIO_INI(core_id,USB_OTG_HS_ENDPTCTRLn,ep);
    mask = (dir == DIR_TX) ? USB_OTG_HS_ENDPTCTRLn_TXE_BMSK : USB_OTG_HS_ENDPTCTRLn_RXE_BMSK;

    if ( (endpoint_control & mask) == 0)
    {
      return(QHSUSB_ERR_INVALID_OPERATION);
    }

    dci_dtd_list[ep][dir][0].urb_ptr = (uint32) (UINTN) urb_ptr;
    status = transfer_data_request(core_id,ep,dir,buffer_ptr,byte_count);

    if (status == QHSUSB_URB_STATUS_REUSABLE)
    {
        // Mark as active
        urb_ptr->transfer_status = QHSUSB_URB_STATUS_ACTIVE;
        urb_ptr->actual_length   = 0;

        // Bug fixed: Don't modify DTD when it's in flight.
        // Set dTD to point at the URB
        // Currently one dTD per dQH.
        // Move the following line to above
        //dci_dtd_list[ep][dir].urb_ptr = (uint32)urb_ptr;

    }
    else
    {
        dci_dtd_list[ep][dir][0].urb_ptr = 0;
        urb_ptr->transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
    }

    return(status);
}
// ===========================================================================

// ===========================================================================
/**
 * @function    qhsusb_dci_cancel_transfer
 *
 * @brief       This function terminates an active transfers
 *
 * @details
 *
 * @param core_id
 *
 * @param ep
 *          endpoint number
 * @param dir
 *          endpoint direction (1=TX,0=RX)
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @see qhsusb_dci_shutdown
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 8.6.5 Flushing/Depriming an Endpoint
 */
// ===========================================================================
uint32 qhsusb_dci_cancel_transfer(uint32 core_id,uint8 ep,uint8 dir)
{
    uint32 mask,endptstat,endptflush;

    DCI_CHECK_CORE_ID(core_id);

    if (!dci_dqh_list || ep >= DCI_MAX_ENDPOINTS || dir >= DCI_MAX_DIRECTION )
      return 0;

    mask = (1<<(ep+dir*USB_OTG_HS_ENDPTSTAT_ETBR_SHFT));

    dQH_t *dqh_ptr = NULL;
    dTD_t *dtd_ptr = NULL;
    qhsusb_urb_t    *urb_ptr = NULL;

    dqh_ptr = &dci_dqh_list[ep][dir];
    dtd_ptr = (void*) (UINTN) dqh_ptr->current_dtd_ptr;

    if(dtd_ptr) //dtd_ptr can be NULL if there is no URB submitted.
    {
       urb_ptr = (void*) (UINTN) dci_dtd_list[ep][dir][0].urb_ptr;
       dci_dtd_list[ep][dir][0].urb_ptr = 0;
    }

    do
    {
        // 1. Write a ‘1’ to the corresponding bit(s) in ENDPTFLUSH.
        DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTFLUSH,mask);

        // 2. Wait until all bits in ENDPTFLUSH are ‘0’.
        do
        {
            endptflush = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTFLUSH,mask);
        } while (endptflush != 0);

        // 3. Read ENDPTSTAT to ensure that for all endpoints commanded to be flushed, that the corresponding
        endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,mask);

    }while(endptstat != 0);

    if(urb_ptr &&
       urb_ptr->transfer_status == QHSUSB_URB_STATUS_ACTIVE) // This only applies to URBs in flight.
    {
        urb_ptr->transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
         // 3. call the URB callbacks
          if(urb_ptr->complete_callback != NULL)
           {
               urb_ptr->complete_callback(urb_ptr);
          }
    }

    return(0);
}
// ===========================================================================

// ===========================================================================
/**
 * @function    qhsusb_dci_shutdown
 *
 * @brief       This function cancel any active transfer.
 *
 * @note        This function is called at the end of the DLOAD program before jumping to ARMPROG.
 *              Since ep#1 rx is still active , it need to be canceled.
 *
 * @param core_id
 *
 * @return status
 *
 *
 * @see qhsusb_dci_cancel_transfer
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 8.6.5 Flushing/Depriming an Endpoint
 */
// ===========================================================================
EFI_STATUS qhsusb_dci_shutdown(uint32 core_id)
{
    uint8 ep;
    boolean  dir;
    EFI_STATUS Status = EFI_SUCCESS;

    DCI_CHECK_CORE_ID(core_id);

    for (ep=0;ep<DCI_MAX_ENDPOINTS;ep++)
    {
        for (dir=0;dir<DCI_MAX_DIRECTION;dir++)
        {
            qhsusb_dci_cancel_transfer(core_id,ep,dir);
        }
    }

    // de-configure all endpoints
    // When Max-Packet-Len = 0, the host will not get a NAK but a bus error.
    for (ep=0;ep<DCI_MAX_ENDPOINTS;ep++)
    {
        for (dir=0;dir<DCI_MAX_DIRECTION;dir++)
        {
            qhsusb_dci_unconfig_endpoint(core_id,ep,dir);
        }
    }

    if(dci_dqh_list){
      if(dci_dqh_list[0]){
        UncachedFreeAlignedPool((void *)(dci_dqh_list[0]));
      }
      FreePool(dci_dqh_list);
    }
    if(dci_dtd_list){
      if(dci_dtd_list[0]){
        if(dci_dtd_list[0][0]){
          UncachedFreeAlignedPool((void *)(dci_dtd_list[0][0]));
        }
        FreePool(dci_dtd_list[0]);
      }
      FreePool(dci_dtd_list);
    }
    if (dummy_buffer_for_zero_length_pkt) {
      UncachedFreeAlignedPool(dummy_buffer_for_zero_length_pkt);
    }

    pre_vbus_valid = FALSE;

    return Status;
}
// ===========================================================================

#ifdef QHSUSB_DEBUG_DCI_ENABLE
// ===========================================================================
/**
 * @function    dci_assert
 *
 * @brief   This function log error detected.
 *
 * @note    This function should be use for debug purpose only.
 *
 * @param   val
 *              0 = ok , otherwise error.
 *
 * @return  None.
 *
 */
// ===========================================================================
void dci_assert(uint32 val,const char* filename_ptr,int line)
{
    if ((dci_assert_line==0) || (dci_assert_filename_ptr==0))
    {
      /* debug code */
      /* this was added to remove warning issued by compiler
         as these variables are set but not used anywhere in code */
    }
    if (val)
    {
        if (dci_assert_val == 0) // hold the first error found
        {
            dci_assert_val = val;
            dci_assert_filename_ptr = filename_ptr;
            dci_assert_line = line;
        }
    }
}
#endif // QHSUSB_DEBUG_DCI_ENABLE

// ------------------------------------------------------------------------------------------------
// Non implemented APIs
// ------------------------------------------------------------------------------------------------

// ===========================================================================
/**
 * @function    qhsusb_dci_start_timer
 *
 * @brief   Start one of the USB Core general-purpose timers
 *
 * @note    Time#0 is for free run, Timer#1 is for short delays.
 *
 * @param core_id
 *
 * @param timer
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @param repeat
 *          1=Repeat , 0=One-Shot
 *
 * @return status
 *              0 = ok , otherwise error.
 */
// ===========================================================================
uint32  qhsusb_dci_start_timer(uint32 core_id,uint8 timer, boolean repeat)
{
    uint32 status=DCI_OK;
    uint32 control=0;

    if ((timer >= DCI_MAX_TIMERS) || (core_id >= DCI_MAX_CORES))
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    if ( dci_timer_is_running[timer] )
    {
        return(QHSUSB_ERR_TIMER_IS_RUNNING);
    }

    control = USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK | USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK;

    if (repeat)
    {
        control |= USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK;
    }

    if (timer == 0)
    {
        // Load Maximum Value (0xffffff =~ 16 seconds)
        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER0LD,USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);

        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER0CTRL,control);
    }
    else
    {
        // Load Maximum Value (16 seconds)
        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER1LD,USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);

        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER1CTRL,control);
    }

    dci_timer_is_running[timer] = TRUE;

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_stop_timer
 *
 * @brief   Stop one of the USB Core general-purpose timers
 *
 * @note    Time#0 is for free run, Timer#1 is for short delays.
 *
 * @param core_id
 *
 * @param timer
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @return status
 *              0 = ok , otherwise error.
 */
// ===========================================================================
uint32  qhsusb_dci_stop_timer(uint32 core_id,uint8 timer)
{
    uint32 status=DCI_OK;

    if ((timer >= DCI_MAX_TIMERS) || (core_id >= DCI_MAX_CORES))
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    if (timer == 0)
    {
        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER0CTRL,USB_OTG_HS_GPTIMER0CTRL_GTPRUN_TIMER_STOP_FVAL);
    }
    else
    {
        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER1CTRL,USB_OTG_HS_GPTIMER0CTRL_GTPRUN_TIMER_STOP_FVAL);
    }

    dci_timer_is_running[timer] = FALSE;

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_get_time_usec
 *
 * @brief   Get the elapsed time in micro seconds (usec)
 *
 * @note    If the timer is in repeat mode it is the caller responsibility to handle rollover situation.
 *
 * @param core_id
 *
 * @param timer
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @return  time
 *          elapsed time in micro seconds
 */
// ===========================================================================
uint32  qhsusb_dci_get_time_usec(uint32 core_id,uint8 timer)
{
    uint32 value=0;

    if ((timer >= DCI_MAX_TIMERS) || (core_id >= DCI_MAX_CORES))
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    if (timer == 0)
    {
        value = DCI_HWIO_INM(core_id,USB_OTG_HS_GPTIMER0CTRL,USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);
    }
    else if (timer == 1)
    {
        value = DCI_HWIO_INM(core_id,USB_OTG_HS_GPTIMER1CTRL,USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);
    }

    value = USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK - value;

    return(value);
}

//============================================================================
/**
 * @function  qhsusb_dci_get_speed
 *
 * @brief Report the USB connection speed.
 *
 * @Note : Although the device is high-speed, the Host might be full-speed.
 *
 * @Note : The USB HW can be forced to Full-Speed.
 *
 * @param core_id
 *
 * @return Speed
 *              QHSUSB_DEVICE_SPEED_FULL
 *              QHSUSB_DEVICE_SPEED_LOW
 *              QHSUSB_DEVICE_SPEED_HIGH
 *              QHSUSB_DEVICE_SPEED_UNKNOWN
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *       Chapter 3.6.16 PORTSCx
 *
 */
//============================================================================
uint32  qhsusb_dci_get_speed(uint32 core_id)
{
    uint32 speed;
  
    DCI_CHECK_CORE_ID(core_id);

    speed = DCI_HWIO_INM(core_id,USB_OTG_HS_PORTSC,USB_OTG_HS_PORTSC_PSPD_BMSK) >> USB_OTG_HS_PORTSC_PSPD_SHFT;
    return(speed);
}

//============================================================================
/**
 * @function  qhsusb_dci_is_suspended
 *
 * @brief report if the device is in suspend mode
 *
 * @param core_id
 *
 * @return is_suspended
 *         1=Suspended,0=Not-Suspended
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *       Chapter 3.6.16 PORTSCx
 *
 */
//============================================================================
boolean qhsusb_dci_is_suspended(uint32 core_id)
{
    uint32 susp;

    DCI_CHECK_CORE_ID(core_id);

    susp = DCI_HWIO_INM(core_id,USB_OTG_HS_PORTSC,USB_OTG_HS_PORTSC_SUSP_BMSK);
    susp = VAL_TO_BOOL(susp);
    return((boolean)susp);
}

//============================================================================
/**
 * @function  qhsusb_dci_get_line_state
 *
 * @brief Read line state from portsc register
 *
 *
 * @param core_id
 *           USB core id
 *
 * @return uint8
 *           line state
 *
 * @ref
 *
 *
 */
//============================================================================
uint32 qhsusb_dci_get_line_state(uint32 core_id)
{
    DCI_CHECK_CORE_ID(core_id);
    return DCI_HWIO_INM(core_id, USB_OTG_HS_PORTSC, USB_OTG_HS_PORTSC_LS_BMSK) >> USB_OTG_HS_PORTSC_LS_SHFT;
}

//============================================================================
/**
 * @function  qhsusb_dci_set_runstop_bit
 *
 * @brief Set the Run Stop bit in the USB controller
 *
 *
 * @param core_id
 *           USB core id
 *
 * @return none
 *
 *
 * @ref
 *
 */
 //============================================================================
void qhsusb_dci_set_runstop_bit(uint32 core_id)
{
    DCI_CHECK_CORE_ID(core_id);
    DCI_HWIO_OUTM(core_id, USB_OTG_HS_USBCMD, USB_OTG_HS_USBCMD_RS_BMSK, USB_OTG_HS_USBCMD_RS_RUN_FVAL);
}

//============================================================================
/**
 * @function  qhsusb_dci_ulpi_read
 *
 * @brief write to the ulpi register
 *
 * @Note : For performing tasks that require ULPI reads eg. charger detection
 *
 * @param ulpi_reg
 *           ULPI register from which to read
 *
 * @return uint8 ULPI register value
 *
 * @ref
 *
 *
 */
//============================================================================
uint32 qhsusb_dci_ulpi_read(uint32 core_id, uint32 ulpi_reg)
{
    uint32 ulpi_cmd = 0;
    uint32 read_val = 0;
    uint32 loop_count = 0;
    uint32 temp = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;

    DCI_CHECK_CORE_ID(core_id);
    
    ulpi_cmd = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK +
               ( ulpi_reg << USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT );

    DCI_HWIO_OUT(core_id, USB_OTG_HS_ULPI_VIEWPORT, ulpi_cmd);

    /* poll until the run bit transitions to 0. */
    while((temp == USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK) && (loop_count++ < QHSUSB_UPLI_LOOP_COUNT))
    {
        read_val = DCI_HWIO_IN(core_id, USB_OTG_HS_ULPI_VIEWPORT) ;
        temp = read_val & USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;
        qhsusb_dci_delay_ms(1); /* poll for 1ms */
    }

    return (read_val & USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK) >> USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT;

}

// ===========================================================================
/**
 * @function    qhsusb_dci_ulpi_write
 *
 * @brief   Writes to UPLI view port register
 *
 *
 * @param core_id
 *
 * @param ulpi_reg
 *          ULPI register name
 *
 * @param data
 *          Data to write to the ulpi register
 *
 * @param mask
 *          Mask to be used with specified data
 *
 * @return  none
 *
 */
// ===========================================================================
void qhsusb_dci_ulpi_write(uint32 core_id, uint32 ulpi_reg, uint8 data, uint8 mask)
{
    uint32 ulpi_cmd = 0;
    uint8  write_data = 0;
    uint32 loop_count = 0;
    uint32 temp = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;

    DCI_CHECK_CORE_ID(core_id);
    
    if(mask != 0xFF)
    {
        write_data = qhsusb_dci_ulpi_read(core_id, ulpi_reg);

        write_data &= (~mask);
        write_data |= (mask & data);
    }
    else
    {
        write_data = data;
    }

    ulpi_cmd = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK +
               USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK  +
               ( ulpi_reg << USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT ) +
               ( write_data );

    DCI_HWIO_OUT(core_id, USB_OTG_HS_ULPI_VIEWPORT, ulpi_cmd);

    //poll until the run bit transitions to 0.
    while((temp == USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK) && (loop_count++ < QHSUSB_UPLI_LOOP_COUNT))
    {
        temp = DCI_HWIO_INM(core_id,USB_OTG_HS_ULPI_VIEWPORT,USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK);
        qhsusb_dci_delay_ms(1); /* poll for 1ms */
    }

}


//=============================================================================
/**
 * @function  qhsusb_dci_detect_charger_type
 *
 * Detects charger port type
 *
 * Should be called after usb stack initialization
 *
 * @param core_id
 *
 *
 *
 * @return Charger port type.
 */
//=============================================================================
qhsusb_chg_port_type qhsusb_dci_detect_charger_type(uint32 core_id)
{
    DCI_CHECK_CORE_ID(core_id);
  
    if(dci_chg_det_supported)
    {
        chg_port_type = dci_detect_charger_type(core_id);
        return chg_port_type;
    }
    else
    {
        qhsusb_dci_connect(core_id);
        return QHSUSB_CHG_PORT_INVALID;
    }
}

//=============================================================================
/**
 * @function  qhsusb_dci_get_charger_type
 *
 * return the detected charger port type
 *
 * Should be called after qhsusb_dci_detect_charger_type
 *
 * @param core_id
 *
 *
 *
 * @return Charger port type.
 */
//=============================================================================
qhsusb_chg_port_type qhsusb_dci_get_charger_type(uint32 core_id)
{
    return chg_port_type;
}

//=============================================================================
/**
 * @function  qhsusb_dci_set_enum_flag
 *
 * Stores whether R/S bit should be set during dci_init sequence
 *
 *
 *
 * @param true or false
 *
 *
 *
 * @return None
 */
//=============================================================================
void qhsusb_dci_set_enum_flag(boolean start_enum)
{
    dci_start_enum = start_enum;
}

//=============================================================================
/**
 *
 * @function  qhsusb_dci_get_enum_flag
 *
 * Returns whether R/S bit should be set during dci_init sequence
 *
 *
 * @param None
 *
 *
 *
 * @return boolean
 */
//=============================================================================
boolean qhsusb_dci_get_enum_flag(void)
{
    return dci_start_enum;
}

// ===========================================================================
/**
 * @function    qhsusb_dci_write_scratch
 *
 * @brief   Write scratch data on USB_OTG_HS_GPTIMER0LD and USB_OTG_HS_GPTIMER1LD
 *
 * @note    The max value of GPTIMER0LD and GPTIMER1LD is 0xffffff (24 bits).
 *
 * @param core_id
 *
 */
// ===========================================================================
void qhsusb_dci_write_scratch(uint32 core_id,uint32 data0,uint32 data1 )
{
    DCI_CHECK_CORE_ID(core_id);
  
    qhsusb_dci_stop_timer(core_id, QHSUSB_TIMER_0_FREE_RUN);
    qhsusb_dci_stop_timer(core_id, QHSUSB_TIMER_1_ONE_SHOT);
   
    DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER0LD,data0);
    DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER1LD,data1);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_read_scratch
 *
 * @brief   Read scratch data on USB_OTG_HS_GPTIMER0LD and USB_OTG_HS_GPTIMER1LD
 *
 * @note    The max value of GPTIMER0LD and GPTIMER1LD is 0xffffff.
 *
 * @param core_id
 *
 * @param data0_ptr - data0 pointer (24 bits) - to return value
 *
 * @param data1_ptr - data1 pointer (24 bits) - to return value
 *
 */
// ===========================================================================
void qhsusb_dci_read_scratch(uint32 core_id,uint32 *data0_ptr,uint32 *data1_ptr )
{
    DCI_CHECK_CORE_ID(core_id);
  
    uint32 data0 = DCI_HWIO_IN(core_id,USB_OTG_HS_GPTIMER0LD);
    uint32 data1 = DCI_HWIO_IN(core_id,USB_OTG_HS_GPTIMER1LD);

    *data0_ptr = data0;
    *data1_ptr = data1;
}

// ===========================================================================
/**
 * @function    qhsusb_loop_here
 * 
 * @brief   This API loops here incase of any error in parameter checks
 * 
 * @note    This API will just loop in case of DCI_ASSERT being used here          
 *  
 * @param   none
 *
 */
// ===========================================================================
void qhsusb_loop_here(void)
{
    static volatile uint32 loop_here = 0x1; 
    while(loop_here);
}

