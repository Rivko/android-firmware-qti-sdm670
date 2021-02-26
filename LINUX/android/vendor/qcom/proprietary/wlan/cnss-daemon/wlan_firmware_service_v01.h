#ifndef WLFW_SERVICE_01_H
#define WLFW_SERVICE_01_H
/**
  @file wlan_firmware_service_v01.h

  @brief This is the public header file which defines the wlfw service Data structures.

  This header file defines the types and structures that were defined in
  wlfw. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed Mar  6 2019 (Spin 0)
   From IDL File: wlan_firmware_service_v01.idl */

/** @defgroup wlfw_qmi_consts Constant values defined in the IDL */
/** @defgroup wlfw_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup wlfw_qmi_enums Enumerated types used in QMI messages */
/** @defgroup wlfw_qmi_messages Structures sent as QMI messages */
/** @defgroup wlfw_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup wlfw_qmi_accessor Accessor for QMI service object */
/** @defgroup wlfw_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup wlfw_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define WLFW_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define WLFW_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define WLFW_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define WLFW_V01_MAX_MESSAGE_ID 0x0048
/**
    @}
  */


/** @addtogroup wlfw_qmi_consts
    @{
  */

/**  Maximum number of copy engine */
#define QMI_WLFW_MAX_NUM_CE_V01 12

/**  Maximum number of copy engine service */
#define QMI_WLFW_MAX_NUM_SVC_V01 24

/**  Maximum data size */
#define QMI_WLFW_MAX_DATA_SIZE_V01 6144

/**  Maximum number of calibration data files */
#define QMI_WLFW_MAX_NUM_CAL_V01 5

/**  Maximum string length */
#define QMI_WLFW_MAX_STR_LEN_V01 16

/**  Maximum number of shadow registers */
#define QMI_WLFW_MAX_NUM_SHADOW_REG_V01 24

/**  Maximum number of shadow registers version 2 */
#define QMI_WLFW_MAX_NUM_SHADOW_REG_V2_V01 36

/**  Maximum number of memory regions */
#define QMI_WLFW_MAX_NUM_MEMORY_REGIONS_V01 2

/**  Maximum number of memory configuration */
#define QMI_WLFW_MAX_NUM_MEM_CFG_V01 2

/**  Maxinum number of memory segment */
#define QMI_WLFW_MAX_NUM_MEM_SEG_V01 32

/**  Maximum timestamp length */
#define QMI_WLFW_MAX_TIMESTAMP_LEN_V01 32

/**  WLAN MAC address size */
#define QMI_WLFW_MAC_ADDR_SIZE_V01 6

/**  Maximum FW build ID length */
#define QMI_WLFW_MAX_BUILD_ID_LEN_V01 128

/**  Maximum Function Name length */
#define QMI_WLFW_FUNCTION_NAME_LEN_V01 128

/**  Maximum athdiag data size */
#define QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01 6144

/**  Maxinum number of GPIO */
#define QMI_WLFW_MAX_NUM_GPIO_V01 32
/**
    @}
  */

/** @addtogroup wlfw_qmi_enums
    @{
  */
typedef enum {
  WLFW_DRIVER_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_WLFW_MISSION_V01 = 0, /**<  Mission mode  */
  QMI_WLFW_FTM_V01 = 1, /**<  FTM mode  */
  QMI_WLFW_EPPING_V01 = 2, /**<  Epping mode  */
  QMI_WLFW_WALTEST_V01 = 3, /**<  WAL test mode  */
  QMI_WLFW_OFF_V01 = 4, /**<  OFF mode  */
  QMI_WLFW_CCPM_V01 = 5, /**<  CCPM unit test  */
  QMI_WLFW_QVIT_V01 = 6, /**<  QVIT mode  */
  QMI_WLFW_CALIBRATION_V01 = 7, /**<  CALIBRATION mode  */
  WLFW_DRIVER_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wlfw_driver_mode_enum_v01;
/**
    @}
  */

/** @addtogroup wlfw_qmi_enums
    @{
  */
typedef enum {
  WLFW_CAL_TEMP_ID_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_WLFW_CAL_TEMP_IDX_0_V01 = 0, /**<  temp 25C file  */
  QMI_WLFW_CAL_TEMP_IDX_1_V01 = 1, /**<  temp 45C file  */
  QMI_WLFW_CAL_TEMP_IDX_2_V01 = 2, /**<  temp 5C file  */
  QMI_WLFW_CAL_TEMP_IDX_3_V01 = 3, /**<  temp 65C file  */
  QMI_WLFW_CAL_TEMP_IDX_4_V01 = 4, /**<  temp -15C file  */
  WLFW_CAL_TEMP_ID_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wlfw_cal_temp_id_enum_v01;
/**
    @}
  */

/** @addtogroup wlfw_qmi_enums
    @{
  */
typedef enum {
  WLFW_PIPEDIR_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_WLFW_PIPEDIR_NONE_V01 = 0, 
  QMI_WLFW_PIPEDIR_IN_V01 = 1, /**<  Target to Host  */
  QMI_WLFW_PIPEDIR_OUT_V01 = 2, /**<  Host to Target  */
  QMI_WLFW_PIPEDIR_INOUT_V01 = 3, /**<  Bidirectional  */
  WLFW_PIPEDIR_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wlfw_pipedir_enum_v01;
/**
    @}
  */

/** @addtogroup wlfw_qmi_enums
    @{
  */
typedef enum {
  WLFW_MEM_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_WLFW_MEM_TYPE_MSA_V01 = 0, /**<  MSA memory  */
  QMI_WLFW_MEM_TYPE_DDR_V01 = 1, /**<  DDR memory  */
  QMI_WLFW_MEM_BDF_V01 = 2, /**<  BDF memory  */
  QMI_WLFW_MEM_M3_V01 = 3, /**<  M3 memory  */
  QMI_WLFW_MEM_CAL_V01 = 4, /**<  Calibration data memory  */
  QMI_WLFW_MEM_DPD_V01 = 5, /**<  DPD memory  */
  QMI_WLFW_MEM_QDSS_V01 = 6, /**<  QDSS trace memory  */
  WLFW_MEM_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wlfw_mem_type_enum_v01;
/**
    @}
  */

/** @addtogroup wlfw_qmi_enums
    @{
  */
typedef enum {
  WLFW_QDSS_TRACE_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_WLFW_QDSS_TRACE_OFF_V01 = 0, /**<  QDSS trace off  */
  QMI_WLFW_QDSS_TRACE_ON_V01 = 1, /**<  QDSS trace on  */
  WLFW_QDSS_TRACE_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wlfw_qdss_trace_mode_enum_v01;
/**
    @}
  */

/**  WLAN driver mode
 Calibration file ID
 Pipe direction
 Copy engine attribute flag values */
typedef uint32_t wlfw_ce_attr_mask_v01;
#define QMI_WLFW_CE_ATTR_FLAGS_V01 ((wlfw_ce_attr_mask_v01)0x00) 
#define QMI_WLFW_CE_ATTR_NO_SNOOP_V01 ((wlfw_ce_attr_mask_v01)0x01) /**<  NonSnooping  */
#define QMI_WLFW_CE_ATTR_BYTE_SWAP_DATA_V01 ((wlfw_ce_attr_mask_v01)0x02) /**<  Byte swap data  */
#define QMI_WLFW_CE_ATTR_SWIZZLE_DESCRIPTORS_V01 ((wlfw_ce_attr_mask_v01)0x04) /**<  Swizzle descriptors  */
#define QMI_WLFW_CE_ATTR_DISABLE_INTR_V01 ((wlfw_ce_attr_mask_v01)0x08) /**<  No interrupt on copy completion  */
#define QMI_WLFW_CE_ATTR_ENABLE_POLL_V01 ((wlfw_ce_attr_mask_v01)0x10) /**<  Poll for residue  */
/** WLAN FW mask */
typedef uint64_t wlfw_fw_mask_v01;
#define QMI_WLFW_ALREADY_REGISTERED_V01 ((wlfw_fw_mask_v01)0x01ull) /**<  Already registered  */
#define QMI_WLFW_FW_READY_V01 ((wlfw_fw_mask_v01)0x02ull) /**<  FW is ready  */
#define QMI_WLFW_MSA_READY_V01 ((wlfw_fw_mask_v01)0x04ull) /**<  MSA configuration is ready  */
#define QMI_WLFW_FW_MEM_READY_V01 ((wlfw_fw_mask_v01)0x08ull) /**<  Memory is ready  */
#define QMI_WLFW_FW_INIT_DONE_V01 ((wlfw_fw_mask_v01)0x10ull) /**<  FW initialization is done  */
/** WLAN dynamic feature mask */
typedef uint64_t wlfw_dynamic_feature_mask_v01;
#define QMI_WLFW_FW_REJUVENATE_V01 ((wlfw_dynamic_feature_mask_v01)0x01ull) /**<  FW rejuvenate is enabled  */
/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for copy engine configuration
 */
typedef struct {

  uint32_t pipe_num;

  wlfw_pipedir_enum_v01 pipe_dir;

  uint32_t nentries;

  uint32_t nbytes_max;

  wlfw_ce_attr_mask_v01 flags;
}wlfw_ce_tgt_pipe_cfg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for copy engine service configuration
 */
typedef struct {

  uint32_t service_id;

  wlfw_pipedir_enum_v01 pipe_dir;

  uint32_t pipe_num;
}wlfw_ce_svc_pipe_cfg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for shadow register
 */
typedef struct {

  uint16_t id;

  uint16_t offset;
}wlfw_shadow_reg_cfg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for shadow register version 2
 */
typedef struct {

  uint32_t addr;
}wlfw_shadow_reg_v2_cfg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for read index update memory information
 */
typedef struct {

  uint32_t base_addr_low;

  uint32_t base_addr_high;
}wlfw_rri_over_ddr_cfg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for CE MSI configuration
 */
typedef struct {

  uint16_t ce_id;

  uint16_t msi_vector;
}wlfw_msi_cfg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for memory region information
 */
typedef struct {

  uint64_t region_addr;

  uint32_t size;

  uint8_t secure_flag;
}wlfw_memory_region_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for memory configuration information
 */
typedef struct {

  uint64_t offset;

  uint32_t size;

  uint8_t secure_flag;
}wlfw_mem_cfg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for memory segment request information
 */
typedef struct {

  uint32_t size;

  wlfw_mem_type_enum_v01 type;

  uint32_t mem_cfg_len;  /**< Must be set to # of elements in mem_cfg */
  wlfw_mem_cfg_s_v01 mem_cfg[QMI_WLFW_MAX_NUM_MEM_CFG_V01];
}wlfw_mem_seg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for memory segment allocation information
 */
typedef struct {

  uint64_t addr;

  uint32_t size;

  wlfw_mem_type_enum_v01 type;

  uint8_t restore;
}wlfw_mem_seg_resp_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for RF chip information
 */
typedef struct {

  uint32_t chip_id;

  uint32_t chip_family;
}wlfw_rf_chip_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for RF board information
 */
typedef struct {

  uint32_t board_id;
}wlfw_rf_board_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for SOC information
 */
typedef struct {

  uint32_t soc_id;
}wlfw_soc_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_aggregates
    @{
  */
/**  structure for FW version information
 */
typedef struct {

  uint32_t fw_version;

  char fw_build_timestamp[QMI_WLFW_MAX_TIMESTAMP_LEN_V01 + 1];
}wlfw_fw_version_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends client interested indications */
typedef struct {

  /* Optional */
  /*  Subscribe option of wlfw_fw_ready_ind_msg */
  uint8_t fw_ready_enable_valid;  /**< Must be set to true if fw_ready_enable is being passed */
  uint8_t fw_ready_enable;

  /* Optional */
  /*  Subscribe option of wlfw_initiate_cal_download_ind_msg */
  uint8_t initiate_cal_download_enable_valid;  /**< Must be set to true if initiate_cal_download_enable is being passed */
  uint8_t initiate_cal_download_enable;

  /* Optional */
  /*  Subscribe option of wlfw_initiate_cal_update_ind_msg */
  uint8_t initiate_cal_update_enable_valid;  /**< Must be set to true if initiate_cal_update_enable is being passed */
  uint8_t initiate_cal_update_enable;

  /* Optional */
  /*  Subscribe option of wlfw_msa_ready_ind_msg */
  uint8_t msa_ready_enable_valid;  /**< Must be set to true if msa_ready_enable is being passed */
  uint8_t msa_ready_enable;

  /* Optional */
  /*  Subscribe option of wlfw_pin_connect_result_ind_msg */
  uint8_t pin_connect_result_enable_valid;  /**< Must be set to true if pin_connect_result_enable is being passed */
  uint8_t pin_connect_result_enable;

  /* Optional */
  /*  Unique client ID */
  uint8_t client_id_valid;  /**< Must be set to true if client_id is being passed */
  uint32_t client_id;

  /* Optional */
  /*  Subscribe option of wlfw_request_mem_ind_msg */
  uint8_t request_mem_enable_valid;  /**< Must be set to true if request_mem_enable is being passed */
  uint8_t request_mem_enable;

  /* Optional */
  /*  Subscribe option of wlfw_fw_mem_ready_ind_msg */
  uint8_t fw_mem_ready_enable_valid;  /**< Must be set to true if fw_mem_ready_enable is being passed */
  uint8_t fw_mem_ready_enable;

  /* Optional */
  /*  Subscribe option of wlfw_fw_init_done_ind_msg */
  uint8_t fw_init_done_enable_valid;  /**< Must be set to true if fw_init_done_enable is being passed */
  uint8_t fw_init_done_enable;

  /* Optional */
  /*  Subscribe option of wlfw_rejuvenate_ind_msg */
  uint8_t rejuvenate_enable_valid;  /**< Must be set to true if rejuvenate_enable is being passed */
  uint32_t rejuvenate_enable;

  /* Optional */
  /*  Subscribe option of wlfw_xo_cal_ind_msg */
  uint8_t xo_cal_enable_valid;  /**< Must be set to true if xo_cal_enable is being passed */
  uint8_t xo_cal_enable;

  /* Optional */
  /*  Subscribe option of wlfw_cal_done_ing_msg */
  uint8_t cal_done_enable_valid;  /**< Must be set to true if cal_done_enable is being passed */
  uint8_t cal_done_enable;

  /* Optional */
  /*  Subscribe option of wlfw_qdss_trace_req_mem_ind_msg */
  uint8_t qdss_trace_req_mem_enable_valid;  /**< Must be set to true if qdss_trace_req_mem_enable is being passed */
  uint8_t qdss_trace_req_mem_enable;

  /* Optional */
  /*  Subscribe option of wlfw_qdss_trace_save_ind_msg */
  uint8_t qdss_trace_save_enable_valid;  /**< Must be set to true if qdss_trace_save_enable is being passed */
  uint8_t qdss_trace_save_enable;

  /* Optional */
  /*  Subscribe option of wlfw_qdss_trace_free_ind_msg */
  uint8_t qdss_trace_free_enable_valid;  /**< Must be set to true if qdss_trace_free_enable is being passed */
  uint8_t qdss_trace_free_enable;
}wlfw_ind_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends client interested indications */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  FW Status Mask */
  uint8_t fw_status_valid;  /**< Must be set to true if fw_status is being passed */
  wlfw_fw_mask_v01 fw_status;
}wlfw_ind_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; indicates a client that calibration is done and FW can be turned on */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_fw_ready_ind_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; indicates a client that FW is ready with memory mapping on server side */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_msa_ready_ind_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; indicates a client that HAL-PHY done with Pin connect test on server side */
typedef struct {

  /* Optional */
  /*  Power Grid Test Result */
  uint8_t pwr_pin_result_valid;  /**< Must be set to true if pwr_pin_result is being passed */
  uint32_t pwr_pin_result;

  /* Optional */
  /*  IO Pin Test Result */
  uint8_t phy_io_pin_result_valid;  /**< Must be set to true if phy_io_pin_result is being passed */
  uint32_t phy_io_pin_result;

  /* Optional */
  /*  RF Pin Test Result */
  uint8_t rf_pin_result_valid;  /**< Must be set to true if rf_pin_result is being passed */
  uint32_t rf_pin_result;
}wlfw_pin_connect_result_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends WLAN mode */
typedef struct {

  /* Mandatory */
  /*  WLAN Driver Mode */
  wlfw_driver_mode_enum_v01 mode;

  /* Optional */
  /*  Hardware Debug option */
  uint8_t hw_debug_valid;  /**< Must be set to true if hw_debug is being passed */
  uint8_t hw_debug;
}wlfw_wlan_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends WLAN mode */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_wlan_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends WLAN driver's configurations */
typedef struct {

  /* Optional */
  /*  Host driver version */
  uint8_t host_version_valid;  /**< Must be set to true if host_version is being passed */
  char host_version[QMI_WLFW_MAX_STR_LEN_V01 + 1];

  /* Optional */
  /*  CE Configuration Information */
  uint8_t tgt_cfg_valid;  /**< Must be set to true if tgt_cfg is being passed */
  uint32_t tgt_cfg_len;  /**< Must be set to # of elements in tgt_cfg */
  wlfw_ce_tgt_pipe_cfg_s_v01 tgt_cfg[QMI_WLFW_MAX_NUM_CE_V01];

  /* Optional */
  /*  CE Service Configuration Information */
  uint8_t svc_cfg_valid;  /**< Must be set to true if svc_cfg is being passed */
  uint32_t svc_cfg_len;  /**< Must be set to # of elements in svc_cfg */
  wlfw_ce_svc_pipe_cfg_s_v01 svc_cfg[QMI_WLFW_MAX_NUM_SVC_V01];

  /* Optional */
  /*  Shadow Registers Information */
  uint8_t shadow_reg_valid;  /**< Must be set to true if shadow_reg is being passed */
  uint32_t shadow_reg_len;  /**< Must be set to # of elements in shadow_reg */
  wlfw_shadow_reg_cfg_s_v01 shadow_reg[QMI_WLFW_MAX_NUM_SHADOW_REG_V01];

  /* Optional */
  /*  Shadow Registers Version 2 Information */
  uint8_t shadow_reg_v2_valid;  /**< Must be set to true if shadow_reg_v2 is being passed */
  uint32_t shadow_reg_v2_len;  /**< Must be set to # of elements in shadow_reg_v2 */
  wlfw_shadow_reg_v2_cfg_s_v01 shadow_reg_v2[QMI_WLFW_MAX_NUM_SHADOW_REG_V2_V01];

  /* Optional */
  /*  Host memory address for read index update */
  uint8_t rri_over_ddr_cfg_valid;  /**< Must be set to true if rri_over_ddr_cfg is being passed */
  wlfw_rri_over_ddr_cfg_s_v01 rri_over_ddr_cfg;

  /* Optional */
  /*  MSI Config for copy engines */
  uint8_t msi_cfg_valid;  /**< Must be set to true if msi_cfg is being passed */
  uint32_t msi_cfg_len;  /**< Must be set to # of elements in msi_cfg */
  wlfw_msi_cfg_s_v01 msi_cfg[QMI_WLFW_MAX_NUM_CE_V01];
}wlfw_wlan_cfg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends WLAN driver's configurations */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_wlan_cfg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command asks for server's capabilities */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_cap_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command asks for server's capabilities */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  RF Chip Information */
  uint8_t chip_info_valid;  /**< Must be set to true if chip_info is being passed */
  wlfw_rf_chip_info_s_v01 chip_info;

  /* Optional */
  /*  RF Board Information */
  uint8_t board_info_valid;  /**< Must be set to true if board_info is being passed */
  wlfw_rf_board_info_s_v01 board_info;

  /* Optional */
  /*  SOC Information */
  uint8_t soc_info_valid;  /**< Must be set to true if soc_info is being passed */
  wlfw_soc_info_s_v01 soc_info;

  /* Optional */
  /*  FW Version Information */
  uint8_t fw_version_info_valid;  /**< Must be set to true if fw_version_info is being passed */
  wlfw_fw_version_info_s_v01 fw_version_info;

  /* Optional */
  /*  WLAN FW Build ID */
  uint8_t fw_build_id_valid;  /**< Must be set to true if fw_build_id is being passed */
  char fw_build_id[QMI_WLFW_MAX_BUILD_ID_LEN_V01 + 1];

  /* Optional */
  /*  Total MAC Number */
  uint8_t num_macs_valid;  /**< Must be set to true if num_macs is being passed */
  uint8_t num_macs;

  /* Optional */
  /*  Proper voltage for open loop CPR */
  uint8_t voltage_mv_valid;  /**< Must be set to true if voltage_mv is being passed */
  uint32_t voltage_mv;
}wlfw_cap_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends board data file to server */
typedef struct {

  /* Mandatory */
  /*  Validity of BDF Data */
  uint8_t valid;

  /* Optional */
  /*  File ID */
  uint8_t file_id_valid;  /**< Must be set to true if file_id is being passed */
  wlfw_cal_temp_id_enum_v01 file_id;

  /* Optional */
  /*  Total Size */
  uint8_t total_size_valid;  /**< Must be set to true if total_size is being passed */
  uint32_t total_size;

  /* Optional */
  /*  Segment ID */
  uint8_t seg_id_valid;  /**< Must be set to true if seg_id is being passed */
  uint32_t seg_id;

  /* Optional */
  /*  BDF Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_WLFW_MAX_DATA_SIZE_V01];

  /* Optional */
  /*  End Mark of a File */
  uint8_t end_valid;  /**< Must be set to true if end is being passed */
  uint8_t end;

  /* Optional */
  /*  Board Data File Type */
  uint8_t bdf_type_valid;  /**< Must be set to true if bdf_type is being passed */
  uint8_t bdf_type;
}wlfw_bdf_download_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends board data file to server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_bdf_download_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends calibration meta-data to server */
typedef struct {

  /* Mandatory */
  /*  Calibration Meta-data */
  uint32_t meta_data_len;  /**< Must be set to # of elements in meta_data */
  wlfw_cal_temp_id_enum_v01 meta_data[QMI_WLFW_MAX_NUM_CAL_V01];

  /* Optional */
  /*  PMIC XO Calibration Data */
  uint8_t xo_cal_data_valid;  /**< Must be set to true if xo_cal_data is being passed */
  uint8_t xo_cal_data;
}wlfw_cal_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends calibration meta-data to server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_cal_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; This command tells which calibration data to download */
typedef struct {

  /* Mandatory */
  /*  Calibration File ID */
  wlfw_cal_temp_id_enum_v01 cal_id;

  /* Optional */
  /*  Total Size */
  uint8_t total_size_valid;  /**< Must be set to true if total_size is being passed */
  uint32_t total_size;

  /* Optional */
  /*  Offset of cal data */
  uint8_t cal_data_location_valid;  /**< Must be set to true if cal_data_location is being passed */
  uint32_t cal_data_location;
}wlfw_initiate_cal_download_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends calibration data to server */
typedef struct {

  /* Mandatory */
  /*  Validity of Calibration Data */
  uint8_t valid;

  /* Optional */
  /*  File ID */
  uint8_t file_id_valid;  /**< Must be set to true if file_id is being passed */
  wlfw_cal_temp_id_enum_v01 file_id;

  /* Optional */
  /*  Total Size */
  uint8_t total_size_valid;  /**< Must be set to true if total_size is being passed */
  uint32_t total_size;

  /* Optional */
  /*  Segment ID */
  uint8_t seg_id_valid;  /**< Must be set to true if seg_id is being passed */
  uint32_t seg_id;

  /* Optional */
  /*  Calibration Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_WLFW_MAX_DATA_SIZE_V01];

  /* Optional */
  /*  End Mark of a File */
  uint8_t end_valid;  /**< Must be set to true if end is being passed */
  uint8_t end;

  /* Optional */
  /*  Offset of cal data */
  uint8_t cal_data_location_valid;  /**< Must be set to true if cal_data_location is being passed */
  uint32_t cal_data_location;
}wlfw_cal_download_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends calibration data to server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_cal_download_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; This command tells client to update local-stored calibration data */
typedef struct {

  /* Mandatory */
  /*  Calibration Data ID */
  wlfw_cal_temp_id_enum_v01 cal_id;

  /* Mandatory */
  /*  Total Size of Data */
  uint32_t total_size;

  /* Optional */
  /*  Offset of cal data */
  uint8_t cal_data_location_valid;  /**< Must be set to true if cal_data_location is being passed */
  uint32_t cal_data_location;
}wlfw_initiate_cal_update_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command pulls calibration data from server */
typedef struct {

  /* Mandatory */
  /*  Calibration Data ID */
  wlfw_cal_temp_id_enum_v01 cal_id;

  /* Mandatory */
  /*  Calibration Data Segment ID */
  uint32_t seg_id;
}wlfw_cal_update_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command pulls calibration data from server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  File ID */
  uint8_t file_id_valid;  /**< Must be set to true if file_id is being passed */
  wlfw_cal_temp_id_enum_v01 file_id;

  /* Optional */
  /*  Total Size */
  uint8_t total_size_valid;  /**< Must be set to true if total_size is being passed */
  uint32_t total_size;

  /* Optional */
  /*  Segment ID */
  uint8_t seg_id_valid;  /**< Must be set to true if seg_id is being passed */
  uint32_t seg_id;

  /* Optional */
  /*  Calibration Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_WLFW_MAX_DATA_SIZE_V01];

  /* Optional */
  /*  End Mark of a File */
  uint8_t end_valid;  /**< Must be set to true if end is being passed */
  uint8_t end;

  /* Optional */
  /*  Offset of cal data */
  uint8_t cal_data_location_valid;  /**< Must be set to true if cal_data_location is being passed */
  uint32_t cal_data_location;
}wlfw_cal_update_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends memory info to FW. */
typedef struct {

  /* Mandatory */
  /*  Physical Memory Address */
  uint64_t msa_addr;

  /* Mandatory */
  /*  Size of Memory in Bytes */
  uint32_t size;
}wlfw_msa_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends memory info to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Mandatory */
  /*  Memory Region Information */
  uint32_t mem_region_info_len;  /**< Must be set to # of elements in mem_region_info */
  wlfw_memory_region_info_s_v01 mem_region_info[QMI_WLFW_MAX_NUM_MEMORY_REGIONS_V01];
}wlfw_msa_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends memory map ready info to FW. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_msa_ready_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends memory map ready info to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_msa_ready_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends WLAN INI configuration to FW. */
typedef struct {

  /* Optional */
  /*  FW Debug Log Option */
  uint8_t enablefwlog_valid;  /**< Must be set to true if enablefwlog is being passed */
  uint8_t enablefwlog;
}wlfw_ini_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends WLAN INI configuration to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_ini_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends athdiag read command to FW. */
typedef struct {

  /* Mandatory */
  /*  Address Offset */
  uint32_t offset;

  /* Mandatory */
  /*  Memory Type */
  uint32_t mem_type;

  /* Mandatory */
  /*  Data Length */
  uint32_t data_len;
}wlfw_athdiag_read_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends athdiag read command to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01];
}wlfw_athdiag_read_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends athdiag write command to FW. */
typedef struct {

  /* Mandatory */
  /*  Address Offset */
  uint32_t offset;

  /* Mandatory */
  /*  Memory Type */
  uint32_t mem_type;

  /* Mandatory */
  /*  Data */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_WLFW_MAX_ATHDIAG_DATA_SIZE_V01];
}wlfw_athdiag_write_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends athdiag write command to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_athdiag_write_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends battery power to FW. */
typedef struct {

  /* Mandatory */
  /*  Battery Power in Micro Voltage */
  uint64_t voltage_uv;
}wlfw_vbatt_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends battery power to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_vbatt_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends WLAN MAC address to FW. */
typedef struct {

  /* Optional */
  /*  WLAN MAC address */
  uint8_t mac_addr_valid;  /**< Must be set to true if mac_addr is being passed */
  uint8_t mac_addr[QMI_WLFW_MAC_ADDR_SIZE_V01];
}wlfw_mac_addr_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends WLAN MAC address to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_mac_addr_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends WLAN host capabilities to server. */
typedef struct {

  /* Optional */
  /*  Number of Clients */
  uint8_t num_clients_valid;  /**< Must be set to true if num_clients is being passed */
  uint32_t num_clients;

  /* Optional */
  /*  WAKE MSI Base Data */
  uint8_t wake_msi_valid;  /**< Must be set to true if wake_msi is being passed */
  uint32_t wake_msi;

  /* Optional */
  /*  GPIO Configurations */
  uint8_t gpios_valid;  /**< Must be set to true if gpios is being passed */
  uint32_t gpios_len;  /**< Must be set to # of elements in gpios */
  uint32_t gpios[QMI_WLFW_MAX_NUM_GPIO_V01];

  /* Optional */
  /*  Whether NM Modem or Full Modem is Loaded */
  uint8_t nm_modem_valid;  /**< Must be set to true if nm_modem is being passed */
  uint8_t nm_modem;

  /* Optional */
  /*  BDF Download Support */
  uint8_t bdf_support_valid;  /**< Must be set to true if bdf_support is being passed */
  uint8_t bdf_support;

  /* Optional */
  /*  BDF Cache Support */
  uint8_t bdf_cache_support_valid;  /**< Must be set to true if bdf_cache_support is being passed */
  uint8_t bdf_cache_support;

  /* Optional */
  /*  M3 Image Download Support */
  uint8_t m3_support_valid;  /**< Must be set to true if m3_support is being passed */
  uint8_t m3_support;

  /* Optional */
  /*  M3 Image Cache Support */
  uint8_t m3_cache_support_valid;  /**< Must be set to true if m3_cache_support is being passed */
  uint8_t m3_cache_support;

  /* Optional */
  /*  Calibration Data Remote Filesys Support */
  uint8_t cal_filesys_support_valid;  /**< Must be set to true if cal_filesys_support is being passed */
  uint8_t cal_filesys_support;

  /* Optional */
  /*  Calibration Data Cache Support */
  uint8_t cal_cache_support_valid;  /**< Must be set to true if cal_cache_support is being passed */
  uint8_t cal_cache_support;

  /* Optional */
  /*  Calibration Is Done Or Not */
  uint8_t cal_done_valid;  /**< Must be set to true if cal_done is being passed */
  uint8_t cal_done;

  /* Optional */
  /*  Memory Bucket Selection */
  uint8_t mem_bucket_valid;  /**< Must be set to true if mem_bucket is being passed */
  uint32_t mem_bucket;

  /* Optional */
  /*  Memory Configuration Mode */
  uint8_t mem_cfg_mode_valid;  /**< Must be set to true if mem_cfg_mode is being passed */
  uint8_t mem_cfg_mode;

  /* Optional */
  /*  Duration Available For Best Effort Cold Calibration */
  uint8_t cal_duration_valid;  /**< Must be set to true if cal_duration is being passed */
  uint16_t cal_duration;
}wlfw_host_cap_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends WLAN host capabilities to server. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_host_cap_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; indicates client that server requests a chunk of memory */
typedef struct {

  /* Mandatory */
  /*  Memory Segment Info */
  uint32_t mem_seg_len;  /**< Must be set to # of elements in mem_seg */
  wlfw_mem_seg_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
}wlfw_request_mem_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends the requested memory to server. */
typedef struct {

  /* Mandatory */
  /*  Allocated Memory Information */
  uint32_t mem_seg_len;  /**< Must be set to # of elements in mem_seg */
  wlfw_mem_seg_resp_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
}wlfw_respond_mem_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends the requested memory to server. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_respond_mem_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; indicates a client that server has finished memory mapping */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_fw_mem_ready_ind_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; indicates client that FW initialization has completed */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_fw_init_done_ind_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; indicates a client that FW will start the rejuvenate process */
typedef struct {

  /* Optional */
  /*  Cause for Rejuvenation */
  uint8_t cause_for_rejuvenation_valid;  /**< Must be set to true if cause_for_rejuvenation is being passed */
  uint8_t cause_for_rejuvenation;

  /* Optional */
  /*  Request for Sub-System */
  uint8_t requesting_sub_system_valid;  /**< Must be set to true if requesting_sub_system is being passed */
  uint8_t requesting_sub_system;

  /* Optional */
  /*  Line Number */
  uint8_t line_number_valid;  /**< Must be set to true if line_number is being passed */
  uint16_t line_number;

  /* Optional */
  /*  WLAN rejuvenation requesting function name */
  uint8_t function_name_valid;  /**< Must be set to true if function_name is being passed */
  char function_name[QMI_WLFW_FUNCTION_NAME_LEN_V01 + 1];
}wlfw_rejuvenate_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command is send from client to server to ack rejuvenation. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_rejuvenate_ack_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command is send from client to server to ack rejuvenation. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_rejuvenate_ack_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends WLAN dynamic feature mask to FW. */
typedef struct {

  /* Optional */
  uint8_t mask_valid;  /**< Must be set to true if mask is being passed */
  wlfw_dynamic_feature_mask_v01 mask;
}wlfw_dynamic_feature_mask_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends WLAN dynamic feature mask to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Previous FW Feature Mask */
  uint8_t prev_mask_valid;  /**< Must be set to true if prev_mask is being passed */
  wlfw_dynamic_feature_mask_v01 prev_mask;

  /* Optional */
  /*  Current FW Feature Mask */
  uint8_t curr_mask_valid;  /**< Must be set to true if curr_mask is being passed */
  wlfw_dynamic_feature_mask_v01 curr_mask;
}wlfw_dynamic_feature_mask_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends the M3 information to server. */
typedef struct {

  /* Mandatory */
  /*  Physical Memory Address */
  uint64_t addr;

  /* Mandatory */
  /*  Size of Memory in Bytes */
  uint32_t size;
}wlfw_m3_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends the M3 information to server. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_m3_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; sets PMIC XO calibration data to client */
typedef struct {

  /* Mandatory */
  /*  PMIC XO Calibration Data */
  uint8_t xo_cal_data;
}wlfw_xo_cal_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; indicates calibration is done on FW side */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_cal_done_ind_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; This command tells client to allocate buffer for QDSS trace */
typedef struct {

  /* Mandatory */
  /*  Memory Segment Info */
  uint32_t mem_seg_len;  /**< Must be set to # of elements in mem_seg */
  wlfw_mem_seg_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
}wlfw_qdss_trace_req_mem_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends QDSS trace buffer address to server */
typedef struct {

  /* Mandatory */
  /*  Allocated Memory Information */
  uint32_t mem_seg_len;  /**< Must be set to # of elements in mem_seg */
  wlfw_mem_seg_resp_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
}wlfw_qdss_trace_mem_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends QDSS trace buffer address to server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_qdss_trace_mem_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; This command tells client to save QDSS trace data */
typedef struct {

  /* Mandatory */
  /*  QDSS Trace Data Source */
  uint32_t source;

  /* Mandatory */
  /*  Total Size of Data */
  uint32_t total_size;

  /* Optional */
  /*  Memory Segment Information */
  uint8_t mem_seg_valid;  /**< Must be set to true if mem_seg is being passed */
  uint32_t mem_seg_len;  /**< Must be set to # of elements in mem_seg */
  wlfw_mem_seg_resp_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];

  /* Optional */
  /* File Name */
  uint8_t file_name_valid;  /**< Must be set to true if file_name is being passed */
  char file_name[QMI_WLFW_MAX_STR_LEN_V01 + 1];
}wlfw_qdss_trace_save_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command pulls QDSS trace data from server */
typedef struct {

  /* Mandatory */
  /*  QDSS Trace Data Segment ID */
  uint32_t seg_id;
}wlfw_qdss_trace_data_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command pulls QDSS trace data from server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Total Size */
  uint8_t total_size_valid;  /**< Must be set to true if total_size is being passed */
  uint32_t total_size;

  /* Optional */
  /*  Segment ID */
  uint8_t seg_id_valid;  /**< Must be set to true if seg_id is being passed */
  uint32_t seg_id;

  /* Optional */
  /*  Calibration Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_WLFW_MAX_DATA_SIZE_V01];

  /* Optional */
  /*  End Mark of a File */
  uint8_t end_valid;  /**< Must be set to true if end is being passed */
  uint8_t end;
}wlfw_qdss_trace_data_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends QDSS trace configuration file to server */
typedef struct {

  /* Optional */
  /*  Total Size */
  uint8_t total_size_valid;  /**< Must be set to true if total_size is being passed */
  uint32_t total_size;

  /* Optional */
  /*  Segment ID */
  uint8_t seg_id_valid;  /**< Must be set to true if seg_id is being passed */
  uint32_t seg_id;

  /* Optional */
  /*  BDF Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QMI_WLFW_MAX_DATA_SIZE_V01];

  /* Optional */
  /*  End Mark of a File */
  uint8_t end_valid;  /**< Must be set to true if end is being passed */
  uint8_t end;
}wlfw_qdss_trace_config_download_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends QDSS trace configuration file to server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_qdss_trace_config_download_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends QDSS trace start/stop command to server */
typedef struct {

  /* Optional */
  /*  QDSS Trace Mode */
  uint8_t mode_valid;  /**< Must be set to true if mode is being passed */
  wlfw_qdss_trace_mode_enum_v01 mode;

  /* Optional */
  /*  Mode Options */
  uint8_t option_valid;  /**< Must be set to true if option is being passed */
  uint64_t option;
}wlfw_qdss_trace_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends QDSS trace start/stop command to server */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_qdss_trace_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Indication Message; This command frees QDSS trace memory */
typedef struct {

  /* Optional */
  /*  QDSS Memory Segment Information */
  uint8_t mem_seg_valid;  /**< Must be set to true if mem_seg is being passed */
  uint32_t mem_seg_len;  /**< Must be set to # of elements in mem_seg */
  wlfw_mem_seg_resp_s_v01 mem_seg[QMI_WLFW_MAX_NUM_MEM_SEG_V01];
}wlfw_qdss_trace_free_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends WLAN SHUTDOWN configuration to FW. */
typedef struct {

  /* Optional */
  /*  Shutdown config */
  uint8_t shutdown_valid;  /**< Must be set to true if shutdown is being passed */
  uint8_t shutdown;
}wlfw_shutdown_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends WLAN SHUTDOWN configuration to FW. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_shutdown_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command queries whether antenna switch is enabled or not */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wlfw_antenna_switch_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command queries whether antenna switch is enabled or not */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Antenna Switch Status */
  uint8_t antenna_valid;  /**< Must be set to true if antenna is being passed */
  uint64_t antenna;
}wlfw_antenna_switch_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Request Message; This command sends antenna grant value to FW */
typedef struct {

  /* Optional */
  /*  Antenna Granted */
  uint8_t grant_valid;  /**< Must be set to true if grant is being passed */
  uint64_t grant;
}wlfw_antenna_grant_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wlfw_qmi_messages
    @{
  */
/** Response Message; This command sends antenna grant value to FW */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}wlfw_antenna_grant_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_WLFW_ANTENNA_GRANT_V01 
//#define REMOVE_QMI_WLFW_ANTENNA_SWITCH_V01 
//#define REMOVE_QMI_WLFW_ATHDIAG_READ_V01 
//#define REMOVE_QMI_WLFW_ATHDIAG_WRITE_V01 
//#define REMOVE_QMI_WLFW_BDF_DOWNLOAD_V01 
//#define REMOVE_QMI_WLFW_CAL_DONE_IND_V01 
//#define REMOVE_QMI_WLFW_CAL_DOWNLOAD_V01 
//#define REMOVE_QMI_WLFW_CAL_REPORT_V01 
//#define REMOVE_QMI_WLFW_CAL_UPDATE_V01 
//#define REMOVE_QMI_WLFW_CAP_V01 
//#define REMOVE_QMI_WLFW_DYNAMIC_FEATURE_MASK_V01 
//#define REMOVE_QMI_WLFW_FW_INIT_DONE_IND_V01 
//#define REMOVE_QMI_WLFW_FW_MEM_READY_IND_V01 
//#define REMOVE_QMI_WLFW_FW_READY_IND_V01 
//#define REMOVE_QMI_WLFW_HOST_CAP_V01 
//#define REMOVE_QMI_WLFW_IND_REGISTER_V01 
//#define REMOVE_QMI_WLFW_INI_V01 
//#define REMOVE_QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01 
//#define REMOVE_QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01 
//#define REMOVE_QMI_WLFW_M3_INFO_V01 
//#define REMOVE_QMI_WLFW_MAC_ADDR_V01 
//#define REMOVE_QMI_WLFW_MSA_INFO_V01 
//#define REMOVE_QMI_WLFW_MSA_READY_V01 
//#define REMOVE_QMI_WLFW_MSA_READY_IND_V01 
//#define REMOVE_QMI_WLFW_PIN_CONNECT_RESULT_IND_V01 
//#define REMOVE_QMI_WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_V01 
//#define REMOVE_QMI_WLFW_QDSS_TRACE_DATA_V01 
//#define REMOVE_QMI_WLFW_QDSS_TRACE_FREE_IND_V01 
//#define REMOVE_QMI_WLFW_QDSS_TRACE_MEM_INFO_V01 
//#define REMOVE_QMI_WLFW_QDSS_TRACE_MODE_V01 
//#define REMOVE_QMI_WLFW_QDSS_TRACE_REQ_MEM_IND_V01 
//#define REMOVE_QMI_WLFW_QDSS_TRACE_SAVE_IND_V01 
//#define REMOVE_QMI_WLFW_REJUVENATE_ACK_V01 
//#define REMOVE_QMI_WLFW_REJUVENATE_IND_V01 
//#define REMOVE_QMI_WLFW_REQUEST_MEM_IND_V01 
//#define REMOVE_QMI_WLFW_RESPOND_MEM_V01 
//#define REMOVE_QMI_WLFW_SHUTDOWN_V01 
//#define REMOVE_QMI_WLFW_VBATT_V01 
//#define REMOVE_QMI_WLFW_WLAN_CFG_V01 
//#define REMOVE_QMI_WLFW_WLAN_MODE_V01 
//#define REMOVE_QMI_WLFW_XO_CAL_IND_V01 

/*Service Message Definition*/
/** @addtogroup wlfw_qmi_msg_ids
    @{
  */
#define QMI_WLFW_IND_REGISTER_REQ_V01 0x0020
#define QMI_WLFW_IND_REGISTER_RESP_V01 0x0020
#define QMI_WLFW_FW_READY_IND_V01 0x0021
#define QMI_WLFW_WLAN_MODE_REQ_V01 0x0022
#define QMI_WLFW_WLAN_MODE_RESP_V01 0x0022
#define QMI_WLFW_WLAN_CFG_REQ_V01 0x0023
#define QMI_WLFW_WLAN_CFG_RESP_V01 0x0023
#define QMI_WLFW_CAP_REQ_V01 0x0024
#define QMI_WLFW_CAP_RESP_V01 0x0024
#define QMI_WLFW_BDF_DOWNLOAD_REQ_V01 0x0025
#define QMI_WLFW_BDF_DOWNLOAD_RESP_V01 0x0025
#define QMI_WLFW_CAL_REPORT_REQ_V01 0x0026
#define QMI_WLFW_CAL_REPORT_RESP_V01 0x0026
#define QMI_WLFW_CAL_DOWNLOAD_REQ_V01 0x0027
#define QMI_WLFW_CAL_DOWNLOAD_RESP_V01 0x0027
#define QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01 0x0028
#define QMI_WLFW_CAL_UPDATE_REQ_V01 0x0029
#define QMI_WLFW_CAL_UPDATE_RESP_V01 0x0029
#define QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01 0x002A
#define QMI_WLFW_MSA_READY_IND_V01 0x002B
#define QMI_WLFW_PIN_CONNECT_RESULT_IND_V01 0x002C
#define QMI_WLFW_MSA_INFO_REQ_V01 0x002D
#define QMI_WLFW_MSA_INFO_RESP_V01 0x002D
#define QMI_WLFW_MSA_READY_REQ_V01 0x002E
#define QMI_WLFW_MSA_READY_RESP_V01 0x002E
#define QMI_WLFW_INI_REQ_V01 0x002F
#define QMI_WLFW_INI_RESP_V01 0x002F
#define QMI_WLFW_ATHDIAG_READ_REQ_V01 0x0030
#define QMI_WLFW_ATHDIAG_READ_RESP_V01 0x0030
#define QMI_WLFW_ATHDIAG_WRITE_REQ_V01 0x0031
#define QMI_WLFW_ATHDIAG_WRITE_RESP_V01 0x0031
#define QMI_WLFW_VBATT_REQ_V01 0x0032
#define QMI_WLFW_VBATT_RESP_V01 0x0032
#define QMI_WLFW_MAC_ADDR_REQ_V01 0x0033
#define QMI_WLFW_MAC_ADDR_RESP_V01 0x0033
#define QMI_WLFW_HOST_CAP_REQ_V01 0x0034
#define QMI_WLFW_HOST_CAP_RESP_V01 0x0034
#define QMI_WLFW_REQUEST_MEM_IND_V01 0x0035
#define QMI_WLFW_RESPOND_MEM_REQ_V01 0x0036
#define QMI_WLFW_RESPOND_MEM_RESP_V01 0x0036
#define QMI_WLFW_FW_MEM_READY_IND_V01 0x0037
#define QMI_WLFW_FW_INIT_DONE_IND_V01 0x0038
#define QMI_WLFW_REJUVENATE_IND_V01 0x0039
#define QMI_WLFW_REJUVENATE_ACK_REQ_V01 0x003A
#define QMI_WLFW_REJUVENATE_ACK_RESP_V01 0x003A
#define QMI_WLFW_DYNAMIC_FEATURE_MASK_REQ_V01 0x003B
#define QMI_WLFW_DYNAMIC_FEATURE_MASK_RESP_V01 0x003B
#define QMI_WLFW_M3_INFO_REQ_V01 0x003C
#define QMI_WLFW_M3_INFO_RESP_V01 0x003C
#define QMI_WLFW_XO_CAL_IND_V01 0x003D
#define QMI_WLFW_CAL_DONE_IND_V01 0x003E
#define QMI_WLFW_QDSS_TRACE_REQ_MEM_IND_V01 0x003F
#define QMI_WLFW_QDSS_TRACE_MEM_INFO_REQ_V01 0x0040
#define QMI_WLFW_QDSS_TRACE_MEM_INFO_RESP_V01 0x0040
#define QMI_WLFW_QDSS_TRACE_SAVE_IND_V01 0x0041
#define QMI_WLFW_QDSS_TRACE_DATA_REQ_V01 0x0042
#define QMI_WLFW_QDSS_TRACE_DATA_RESP_V01 0x0042
#define QMI_WLFW_SHUTDOWN_REQ_V01 0x0043
#define QMI_WLFW_SHUTDOWN_RESP_V01 0x0043
#define QMI_WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_REQ_V01 0x0044
#define QMI_WLFW_QDSS_TRACE_CONFIG_DOWNLOAD_RESP_V01 0x0044
#define QMI_WLFW_QDSS_TRACE_MODE_REQ_V01 0x0045
#define QMI_WLFW_QDSS_TRACE_MODE_RESP_V01 0x0045
#define QMI_WLFW_QDSS_TRACE_FREE_IND_V01 0x0046
#define QMI_WLFW_ANTENNA_SWITCH_REQ_V01 0x0047
#define QMI_WLFW_ANTENNA_SWITCH_RESP_V01 0x0047
#define QMI_WLFW_ANTENNA_GRANT_REQ_V01 0x0048
#define QMI_WLFW_ANTENNA_GRANT_RESP_V01 0x0048
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro wlfw_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type wlfw_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define wlfw_get_service_object_v01( ) \
          wlfw_get_service_object_internal_v01( \
            WLFW_V01_IDL_MAJOR_VERS, WLFW_V01_IDL_MINOR_VERS, \
            WLFW_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

