#ifndef __UFS_CORE_H
#define __UFS_CORE_H
/**********************************************************************
 *
 *   UFS core header file
 *   Contain function definition and data structure required by the 
 *   UFS core and UFS_UTP layer
 *
 * Copyright (c) 2012-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **********************************************************************/

/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UfsCommonLib/ufs_core.h#2 $
  $DateTime: 2018/07/26 07:29:41 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     --------------------------------------------------- 
2018-07-25   sb      SW config to ignore LINKLOST indication impact on the UFS controller 
2017-06-06   jt      Add DAL config info in the host structure  
2017-05-22   jt      Changes for background initialization  
2017-03-20   jt      Add PA_PWRModeUserData definition
2016-10-19   jt      Add need_flags struct 
2016-08-18   jt      Add core_skip flag  
2016-07-28   jt      Add new geometry descriptor fields  
2015-03-26   rh      Add desc_rpmb in ufs_host_t
2015-02-18   rh      Add pre-init reading of WLUN_BOOT
2014-09-17   rh      Merge with UEFI driver changes
2014-01-08   rh      Adding report for ENH memory type 3/4 scale factor
2013-10-22   rh      Capture more debug information in the host structure
2013-09-18   rh      Adding write for bConfigDescrLock attribute
2013-09-10   rh      Adding generic UPIU write passthrough
2013-09-06   rh      Increase the size of the debug log buffer
2013-09-04   rh      Adding enhanced area capacity adjustment factor
2012-12-18   rh      Initial creation
=======================================================================*/

#include "ufs_osal.h"
#include "ufs_bsp.h"

#define UFS_WLUN_REPORT                0x81
#define UFS_WLUN_UFS_DEVICE            0xD0
#define UFS_WLUN_BOOT                  0xB0
#define UFS_WLUN_RPMB                  0xC4

// UPIU transaction specific defines

// FLAGS for indication of read or write
#define UPIU_FLAGS_READ                0x40
#define UPIU_FLAGS_WRITE               0x20
#define UPIU_FLAGS_ATTR_SIMPLE         0x00
#define UPIU_FLAGS_ATTR_ORDERED        0x01
#define UPIU_FLAGS_ATTR_HOQ            0x02

// Redefine the name for higher layer use
#define UFS_XFER_FLAGS_READ            UPIU_FLAGS_READ                
#define UFS_XFER_FLAGS_WRITE           UPIU_FLAGS_WRITE               
#define UFS_XFER_FLAGS_ATTR_SIMPLE     UPIU_FLAGS_ATTR_SIMPLE         
#define UFS_XFER_FLAGS_ATTR_ORDERED    UPIU_FLAGS_ATTR_ORDERED        
#define UFS_XFER_FLAGS_ATTR_HOQ        UPIU_FLAGS_ATTR_HOQ            

#define UPIU_FLAGS_OVERFLOW            0x40
#define UPIU_FLAGS_UNDERFLOW           0x20
#define UPIU_FLAGS_DATAMISMATCH        0x10

#define UPIU_CMDTYPE_SCSI              0x0
#define UPIU_CMDTYPE_NATIVE_UFS        0x1

#define UPIU_QUERY_OP_NOP              0x0
#define UPIU_QUERY_OP_READ_DESCRIPTOR  0x1
#define UPIU_QUERY_OP_WRITE_DESCRIPTOR 0x2
#define UPIU_QUERY_OP_READ_ATTRIBUTE   0x3
#define UPIU_QUERY_OP_WRITE_ATTRIBUTE  0x4
#define UPIU_QUERY_OP_READ_FLAG        0x5
#define UPIU_QUERY_OP_SET_FLAG         0x6
#define UPIU_QUERY_OP_CLEAR_FLAG       0x7
#define UPIU_QUERY_OP_TOGGLE_FLAG      0x8

// Query Response Code
#define UPIU_RESP_SUCCESS              0x0
#define UPIU_RESP_NOT_READABLE         0xf6
#define UPIU_RESP_NOT_WRITEABLE        0xf7
#define UPIU_RESP_ALREADY_WRITTEN      0xf8
#define UPIU_RESP_INVALID_LEN          0xf9
#define UPIU_RESP_INVALID_VAL          0xfa
#define UPIU_RESP_INVALID_SELECTOR     0xfb
#define UPIU_RESP_INVALID_INDEX        0xfc
#define UPIU_RESP_INVALID_IDN          0xfd
#define UPIU_RESP_INVALID_OPC          0xfe
#define UPIU_RESP_FAILED               0xff

// SCSI status value
#define UPIU_SCSI_S_GOOD               0x00
#define UPIU_SCSI_S_CHECK_CONDITION    0x02
#define UPIU_SCSI_S_BUSY               0x08
#define UPIU_SCSI_S_TASK_SET_FULL      0x28

// Query function define
#define UPIU_FNC_STD_READ              0x01
#define UPIU_FNC_STD_WRITE             0x81

// UFS device flag operation code
#define UPIU_FLAG_OP_READ              0x00
#define UPIU_FLAG_OP_SET               0x01
#define UPIU_FLAG_OP_CLEAR             0x02
#define UPIU_FLAG_OP_TOGGLE            0x04

#define DEFAULT_SECTOR_SIZE            4096

// DME attribute defines
// L1 - M-TX PHY 
#define TX_HSMODE_Capability                    0x01
#define TX_HSGEAR_Capability                    0x02
#define TX_PWMG0_Capability                     0x03
#define TX_PWMGEAR_Capability                   0x04
#define TX_PHY_MajorMinor_Release_Capability    0x0d

#define TX_MODE                                 0x21
#define TX_HSRATE_Series                        0x22
#define TX_HSGEAR                               0x23
#define TX_PWMGEAR                              0x24
#define TX_Amplitude                            0x25
#define TX_HS_SlewRate                          0x26
#define TX_SYNC_Source                          0x27
#define TX_HS_SYNC_LENGTH                       0x28
#define TX_HS_PREPARE_LENGTH                    0x29
#define TX_LS_PREPARE_LENGTH                    0x2a
#define TX_HIBERN8_Control                      0x2b
#define TX_LCC_Enable                           0x2c
#define TX_PWM_BURST_Closure_Extension          0x2d
#define TX_BYPASS_8B10B_Enable                  0x2e
#define TX_DRIVER_POLARITY                      0x2f
#define TX_LCC_Sequencer                        0x32
#define TX_Min_Active_Time                      0x33

#define TX_FSM_State                            0x41

#define DME_L1_TX_ITEM {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a, \
                        0x0b,0x0c,0x0d,0x0e,0x0f,0x21,0x22,0x23,0x24,0x25, \
                        0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e, \
                        0x2f,0x30,0x31,0x32,0x33,0x41}

// L1 - M-RX PHY
#define RX_HSMODE_Capability                    0x81
#define RX_HSGEAR_Capability                    0x82
#define RX_PWMG0_Capability                     0x83
#define RX_PWMGEAR_Capability                   0x84
#define RX_HS_SYNC_LENGTH_Capability            0x8b
#define RX_HS_PREPARE_LENGTH_Capability         0x8c
#define RX_LS_PREPARE_LENGTH_Capability         0x8b
#define RX_PWM_Burst_Closure_Length_Capability  0x8e
#define RX_PHY_MajorMinor_Release_Capability    0x90

#define RX_MODE                                 0xA1
#define RX_HSRATE_Series                        0xA2
#define RX_HSGEAR                               0xA3
#define RX_PWMGEAR                              0xA4
#define RX_LS_Terminated_Enable                 0xA5
#define RX_HS_Unterminated_Enable               0xA6
#define RX_Enter_HIBERN8                        0xA7
#define RX_BYPASS_8B10B_Enable                  0xA8

#define RX_FSM_State                            0xC1

#define DME_L1_RX_ITEM {0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a, \
                        0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x94,0x96, \
                        0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xC1}

// L1.5 - PHY Adapter
#define PA_ActiveTxDataLanes                    0x1560
#define PA_ActiveRxDataLanes                    0x1580
#define PA_TxTrailingClocks                     0x1564
#define PA_PHY_Type                             0x1500
#define PA_AvailTxDataLanes                     0x1520
#define PA_AvailRxDataLanes                     0x1540
#define PA_MinRxTrailingClocks                  0x1543

#define PA_TxPWRStatus                          0x1567
#define PA_RxPWRStatus                          0x1582
#define PA_TxForceClock                         0x1562
#define PA_TxPWRMode                            0x1563

#define PA_MaxTxSpeedFast                       0x1521
#define PA_MaxTxSpeedSlow                       0x1522
#define PA_MaxRxSpeedFast                       0x1541
#define PA_MaxRxSpeedSlow                       0x1542
#define PA_TxLinkStartupHS                      0x1544

#define PA_Local_TX_LCC_Enable                  0x155e
#define PA_Peer_TX_LCC_Enable                   0x155f

#define PA_TxSpeedFast                          0x1565
#define PA_TxSpeedSlow                          0x1566
#define PA_RemoteVerInfo                        0x15A0

#define PA_TxGear                               0x1568
#define PA_TxTermination                        0x1569
#define PA_HSSeries                             0x156a
#define PA_PWRMode                              0x1571
#define PA_RxGear                               0x1583
#define PA_RxTermination                        0x1584
#define PA_MaxRxPWMGear                         0x1586
#define PA_MaxRxHSGear                          0x1587
#define PA_RxHSUnterminationCapability          0x15a5
#define PA_RxLSUnterminationCapability          0x15a6
#define PA_Hivern8Time                          0x15a7
#define PA_TActivate                            0x15a8
#define PA_LocalVerInfo                         0x15a9
#define PA_PACPFrameCount                       0x15c0
#define PA_PACPErrorCount                       0x15c1
#define PA_PHYTestControl                       0x15c2
#define PA_ConnectedTxDataLanes                 0x1561
#define PA_ConnectedRxDataLanes                 0x1581
#define PA_LogicalLaneMap                       0x15a1
#define PA_PWRModeUserData(x)                   (0x15b0 + x)

#define PA_SleepNoConfigTime                    0x15a2
#define PA_StallNoConfigTime                    0x15a3
#define PA_SaveConfigTime                       0x15a4

#define PA_VS_CONFIG_REG1                       0x9000

#define DME_VS_CFG                              0xD003

#define DME_L1x5_ITEM {0x1500,0x1520,0x1540,0x1543,0x1560, \
                       0x1561,0x1564,0x1567,0x1568,0x1569, \
                       0x156a,0x1571,0x1580,0x1581, \
                       0x1582,0x1583,0x1584,0x1586,0x1587, \
                       0x1590,0x1591,0x15a0,0x15a1,0x15a2, \
                       0x15a3,0x15a4,0x15a5,0x15a6,0x15a8, \
                       0x15a9,0x15b0,0x15b1,0x15b2,0x15b3, \
                       0x15b4,0x15b5,0x15b6,0x15b7,0x15b8, \
                       0x15b9,0x15ba,0x15bb,0x15c0,0x15c1, \
                       0x15c2}

// L2 - DataLink layer
#define DME_L2_ITEM   {0x2000,0x2001,0x2002,0x2003,0x2004, \
                       0x2005,0x2006,0x2040,0x2041,0x2042, \
                       0x2043,0x2044,0x2045,0x2046,0x2047, \
                       0x2060,0x2061,0x2062,0x2063,0x2064, \
                       0x2065,0x2066,0x2067}

// L3 - Network Layer
#define N_DeviceID                              0x3000
#define N_DeviceID_valid                        0x3001

#define DME_L3_ITEM   {0x3000,0x3001,0x3002,0x3021}

// L4 - Transport Layer
#define T_PeerDeviceID                          0x4021
#define T_PeerCPortID                           0x4022
#define T_TrafficClass                          0x4023
#define T_ConnectionState                       0x4020

#define DME_L4_ITEM   {0x4000,0x4001,0x4060,0x4061,0x4080, \
                       0x4080,0x4081,0x4082,0x4083,0x4084, \
                       0x4085,0x4086,0x40a1,0x40a2,0x40a3, \
                       0x40a4,0x40a5,0x40a6,0x40a7,0x40a8, \
                       0x40a9,0x40aa,0x40ab,0x4020,0x4021, \
                       0x4022,0x4023,0x4024,0x4025,0x4026, \
                       0x4027,0x4028,0x4029,0x402a,0x402b}

#define DME_DME_ITEM  {0x5000,0x5001,0x5002,0x5003,0x5004,0x5005}

#define UIC_ARG0_MIBattr(x,s)    (((x << 16) & 0xffff0000) | (s & 0xffff))

// JESD220B Table14.4 - Device Descriptor
typedef struct {
   uint8_t        bLength;
   uint8_t        bDescriptorType;
   uint8_t        bDevice;              
   uint8_t        bDeviceClass; 
   uint8_t        bDeviceSubClass;
   uint8_t        bProtocol;
   uint8_t        bNumberLU;
   uint8_t        bNumberWLU;
   uint8_t        bBootEnable;
   uint8_t        bDescrAccessEn;
   uint8_t        bInitPowerMode;
   uint8_t        bHighPriorityLUN;
   uint8_t        bSecureRemovalType;
   uint8_t        bSecurityLU;
   uint8_t        bBackgroundOpsTermLat;
   uint8_t        bInitActiveICCLevel;
   uint16_t       wSpecVersion;
   uint16_t       wManufactureDate;
   uint8_t        iManufacturerName;
   uint8_t        iProductName;
   uint8_t        iSerialNumber;
   uint8_t        iOemID;
   uint16_t       wManufacturerID;
   uint8_t        bUD0BaseOffset;
   uint8_t        bUDConfigPLength;
   uint8_t        bDeviceRTTCap;   
   uint16_t       wPeriodicRTCUpdate;
} ufs_desc_device_t;

// JESD220B Table 14.7 - Config Header and Config Device Desc. Params
typedef struct {
   uint8_t        bLength;
   uint8_t        bDescriptorType;
   uint8_t        bBootEnable;
   uint8_t        bDescrAccessEn;
   uint8_t        bInitPowerMode;
   uint8_t        bHighPriorityLUN;
   uint8_t        bSecureRemovalType;
   uint8_t        bInitActiveICCLevel;
   uint16_t       wPeriodicRTCUpdate;
} ufs_desc_config_device_t;

// JESD220B Table 14.8 - Config Unit Desc. Params
typedef struct {
   uint8_t        bLUEnable;
   uint8_t        bBootLunID;
   uint8_t        bLUWriteProtect;
   uint8_t        bMemoryType;
   uint32_t       dNumAllocUnits;
   uint8_t        bDataReliability;
   uint8_t        bLogicalBlockSize;
   uint8_t        bProvisioningType;
   uint16_t       wContextCapabilities;
} ufs_desc_config_unit_t;

// JESD220B Table 14.6 - Configuration Descriptor
typedef struct {
   ufs_desc_config_device_t   dev_cfg;
   ufs_desc_config_unit_t     unit_cfg[8];
} ufs_desc_config_t;

// JESD220B Table 14.9 - Geometry Descriptor
typedef struct {
   uint8_t        bLength;
   uint8_t        bDescriptorType;
   uint8_t        bMediaTechnology;
   uint64_t       qTotalRawDeviceCapacity;
   uint8_t        bMaxNumberLU;
   uint32_t       dSegmentSize;
   uint8_t        bAllocationUnitSize;
   uint8_t        bMinAddrBlockSize;
   uint8_t        bOptimalReadBlockSize;
   uint8_t        bOptimalWriteBlockSize;
   uint8_t        bMaxInBufferSize;
   uint8_t        bMaxOutBufferSize;
   uint8_t        bRPMB_ReadWriteSize;
   uint8_t        bDataOrdering;
   uint8_t        bMaxContexIDNumber;
   uint8_t        bSysDataTagUnitSize;
   uint8_t        bSysDataTagResSize;
   uint8_t        bSupportedSecRTypes;
   uint16_t       wSupportedMemoryTypes;
   uint32_t       dSystemCodeMaxNAllocU;
   uint16_t       wSystemCodeCapAdjFac;
   uint32_t       dNonPersistMaxNAllocU;
   uint16_t       wNonPersistCapAdjFac;
   uint32_t       dEnhanced1MaxNAllocU;
   uint16_t       wEnhanced1CapAdjFac;
   uint32_t       dEnhanced2MaxNAllocU;
   uint16_t       wEnhanced2CapAdjFac;
   uint32_t       dEnhanced3MaxNAllocU;
   uint16_t       wEnhanced3CapAdjFac;
   uint32_t       dEnhanced4MaxNAllocU;
   uint16_t       wEnhanced4CapAdjFac;
   uint32_t       dOptimalLogicalBlockSize;
} ufs_desc_geometry_t; 

// JESD220B Table 14.10 - Unit Descriptor
typedef struct {
   uint8_t        bLength;
   uint8_t        bDescriptorType;
   uint8_t        bUnitIndex;
   uint8_t        bLUEnable;
   uint8_t        bBootLunID;
   uint8_t        bLUWriteProtect;
   uint8_t        bLUQueueDepth;
   uint8_t        bMemoryType;
   uint8_t        bDataReliability;
   uint8_t        bLogicalBlockSize;
   uint64_t       qLogicalBlockCount;
   uint32_t       dEraseBlockSize;
   uint8_t        bProvisioningType;
   uint64_t       qPhyMemResourceCount;
   uint16_t       wContextCapabilities;
   uint8_t        bLargeUnitGranularity_M1;
} ufs_desc_unit_t;

// JESD220B Table 14.11 - RPMB Unit Descriptor
typedef struct {
   uint8_t        bLength;
   uint8_t        bDescriptorType;
   uint8_t        bUnitIndex;
   uint8_t        bLUEnable;
   uint8_t        bBootLunID;
   uint8_t        bLUWriteProtect;
   uint8_t        bLUQueueDepth;
   uint8_t        bMemoryType;
   uint8_t        bLogicalBlockSize;
   uint64_t       qLogicalBlockCount;
   uint32_t       dEraseBlockSize;
   uint8_t        bProvisioningType;
   uint64_t       qPhyMemResourceCount;
} ufs_desc_rpmb_unit_t;
 
// Got the Descirptor?
typedef struct {
   uint8_t        has_desc_device;  
   uint8_t        has_desc_config;
   uint8_t        has_desc_geometry;   
   uint8_t        has_desc_unit[8];
   uint8_t        has_desc_rpmb_unit;
} ufs_has_desc_t;

// JESDB220B Table 14.19 - Flags
typedef struct {
   uint8_t        fDeviceInit;
   uint8_t        fPermanentWPEn;
   uint8_t        fPowerOnWPEn;
   uint8_t        fBackgroundOpsEn;
   uint8_t        fPurgeEnable;
   uint8_t        fPhyResourceRemoval;
   uint8_t        fBusyRTC;
   uint8_t        fPermanentlyDisableFwUpdate;
} ufs_flags_t;

// JESD220B Table 14.21 - Attributes
typedef struct {
   uint8_t        bBootLunEn;
   uint8_t        bCurrentPowerMode;
   uint8_t        bActiveICCLevel;
   uint8_t        bOutofOrderDataEn;
   uint8_t        bBackgroundOpStatus;
   uint8_t        bPurgeStatus;
   uint8_t        bMaxDataInSize;
   uint8_t        bMaxDataOutSize;
   uint32_t       dDynCapNeeded;
   uint8_t        bRefClkFreq;
   uint8_t        bConfigDescrLock;
   uint8_t        bMaxNumOfRTT;
   uint16_t       wExceptionEventControl;
   uint16_t       wExceptionEventStatus;
   uint32_t       dSecondsPassed;
   uint16_t       wContextConf;
   uint32_t       dCorrPrgBlkNum;
} ufs_attr_t;

// Got the Attribute?
typedef struct {
   uint8_t        has_bBootLunEn;
   uint8_t        has_bCurrentPowerMode;
   uint8_t        has_bActiveICCLevel;
   uint8_t        has_bOutofOrderDataEn;
   uint8_t        has_bBackgroundOpStatus;
   uint8_t        has_bPurgeStatus;
   uint8_t        has_bMaxDataInSize;
   uint8_t        has_bMaxDataOutSize;
   uint8_t        has_dDynCapNeeded;
   uint8_t        has_bRefClkFreq;
   uint8_t        has_bConfigDescrLock;
   uint8_t        has_bMaxNumOfRTT;
   uint8_t        has_wExceptionEventControl;
   uint8_t        has_wExceptionEventStatus;
   uint8_t        has_dSecondsPassed;
   uint8_t        has_wContextConf;
   uint8_t        has_dCorrPrgBlkNum; 
}ufs_has_attr_t;

// Need the Flag?
typedef struct {
   uint8_t        need_fPermanentWPEn;
   uint8_t        need_fPowerOnWPEn; 
}ufs_need_flag_t;

typedef struct {
   uint8_t      max_pwm_gear;
   uint8_t      max_hs_gear;
   uint8_t      last_hs_rate;
   uint8_t      tx_avail_lanes;
   uint8_t      rx_avail_lanes;
   uint16_t     unipro_ver_local;
   uint16_t     unipro_ver_remote;
} ufs_mphy_caps_t;


typedef struct ufs_host_info {
   uint8_t              ufs_core_id;         /* UFS Core */
   uint8_t              is_initialized;      /* Is the device intialized */
   uintptr_t            ufs_hci_addr;        /* UFS HCI Base Address */
   uint64_t             ufs_req_list_phy;    /* Base addr of xfer request list */
   uint64_t             ufs_mgr_list_phy;    /* Base addr of management req list */
   uint8_t             *ufs_req_list_ptr;    /* Pointer to xfer request list */
   uint8_t             *ufs_mgr_list_ptr;    /* Pointer to management req list */

   ufs_mphy_caps_t      ufs_mphy_caps;
   uint8_t              has_caps;
   uint8_t              has_gear_switched;
   
   uint32_t             ufs_task_cnt;        /* Counter for the task tag generation */
   uint8_t              has_scsi_inquiry;    /* Got SCSI inquiry already? */

   /* Device parameter inforamtion */
   uint8_t              fDeviceInit_async;   /* Doing fDeviceInit asynchronously */
   uint8_t              fDeviceInit_sent;    /* Remember fDevicInit across LUs */     
   uint8_t              fDeviceInit_done;    /* Remember fDevicInit across LUs */     
   uint8_t              db_infinite_poll;    /* Poll doorbell without timeout */

	/* Non blocking operations */
   uint8_t              non_blocking_en;     /* Enable non-blocking on next xfer */
   uint8_t              xfer_pend;           /* A pending transfer is taking place */
   uint8_t              purge_set;           /* Purge enable flag set? */

   uint8_t              skip_gear_sw;        /* Don't perform gear switch */
   uint8_t              slow_xfer;           /* Platform that is running slow */
   uint8_t              core_skip;           /* Don't need to do these actions for XBL core */

   /* UFS Flags */
   ufs_need_flag_t      need_flags;          /* Need the UFS flags? */
   ufs_flags_t          flag;                /* All UFS Flags */

   /* UFS Attributes */
   ufs_has_attr_t       has_attr;            /* Got the UFS Attribute? */
   ufs_attr_t           attr;                /* All UFS Attributes */

   /* UFS Descriptors */
   ufs_has_desc_t       has;                 /* Got the UFS Descriptor? */
   ufs_desc_device_t    desc_dev;            /* Device descriptor */
   ufs_desc_config_t    desc_cfg;            /* Configuration descriptor */
   ufs_desc_unit_t      desc_unit[8];        /* Unit descriptor x 8 */
   ufs_desc_geometry_t  desc_geo;            /* Geometry descriptor */
   ufs_desc_rpmb_unit_t desc_rpmb;           /* RPMB unit descriptor */

   /* SCSI inquiry output */
   uint8_t              inquiry_vid[28];     /* Vendor ID string from INQUIRY */

   /* Debug */   
   uint32_t             dbg_is_val;          /* Last value of IS at failure */
   uint32_t             dbg_uecpa_val;       /* Last good value of UECPA */
   uint32_t             dbg_uecdl_val;       /* Last good value of UECDL */
   uint32_t             dbg_uecn_val;        /* Last good value of UECN */
   uint32_t             dbg_uect_val;        /* Last good value of UECT */
   uint32_t             dbg_uecdme_val;      /* Last good value of UECDME */
   uint32_t             dbg_pa_err_val;      /* Last good value of PA_ERR_CODE */
       
   uint32_t             next_failure;        /* Remember the last failed point */
   uint32_t             last_fail_id[8];     /* A debug code for last failure */
   uint32_t             failure_cnt;         /* Count number of failure since init */
 
   uint32_t             last_uic_cmd;        /* Last UIC command sent */
   uint32_t             ufs_uic_arg[3];      /* Result of the UIC command */  
   uint8_t              last_req_sense[32];  /* Store the current valid request sense value */
   uint8_t              last_scsi_status;    /* Status field for the last SCSI cmd */
   uint32_t             last_rpmb_status;    /* Status of the last RPMB operation */
   
   uint8_t              prdtCount;           /* PRDT Length */

   uint8_t              device_awake;        /* Sent NOP UPIU? */
   uint8_t              has_dal_cfg;         /* Got the DAL config attributes? */
   ufs_dal_config_item  dal_cfg;             /* Attributes from DAL config xml */
} ufs_host_t;

// A data structure contain all the required information for 
// performing a command quest with or without data 
// Use with Command UPIU request
typedef struct ufs_command_request_info {
   struct ufs_host_info *phc;
   uint8_t        lun;              // Target LUN to issue xfer to
   uint8_t        flags;            // Read or write

   uint32_t       expected_len;     // Expected total length
   uint32_t       residual_len;     // Residual data count
   uint64_t       data_buf_phy;     // Physical buffer address
   uint8_t        status;           // SCSI command status
   uint8_t        req_sense[32];    // Request sense data responded
} ufs_cmd_req_t;

// For Querry UPIU request 
typedef struct ufs_attr_query_request {
   struct ufs_host_info *phc;
   uint8_t        idn;              // Identification field, input
   uint8_t        idx;              // Index field, input
   uint8_t        sel;              // Selection field, input
   uint32_t       val;              // Value field, in/out
   uint32_t       req_data_len;     // Request Length, used for descriptor
   uint8_t        *req_data_buf;    // Request Data, used for descriptor
   uint32_t       resp_data_len;    // Response Length, used for descriptor
   uint8_t        *resp_data_buf;   // Response Data, used for descriptor
} ufs_attr_req_t;


// Exposed function definition
// Issue a SCSI command 
int32_t ufs_do_scsi_command (ufs_cmd_req_t *cmdreq, uint8_t *cdb);

// Record the error code in the debugging buffer
void ufs_error_log(ufs_host_t *hba, uint32_t eid);

// Read UFS device descriptor, output stored in dest_buf
int32_t ufs_read_dev_descriptor (ufs_attr_req_t *preq);

// Send NOP UPIU to check if the device is ready
int32_t ufs_send_nop_ping (ufs_attr_req_t *preq);

// Write UFS attributes
int32_t ufs_write_attributes (ufs_attr_req_t *preq);

// Read UFS attributes
int32_t ufs_read_attributes (ufs_attr_req_t *preq);

// UFS device flag operation
int32_t ufs_dev_flag_op (ufs_attr_req_t *preq, uint8_t op);

// Write UFS device descriptor
int32_t ufs_write_dev_descriptor (ufs_attr_req_t *preq);

// Generic query request with write data payload
int32_t ufs_generic_write_query (ufs_attr_req_t *preq, uint8_t *osf, 
                                 uint32_t q_func);

// Poll the completion of pending transfer
int32_t ufs_poll_pending_xfer (ufs_host_t *hba, uint8_t resp_type);


#endif /* __UFS_CORE_H */
