#ifndef QDSS_DIAG_CMD_H
#define QDSS_DIAG_CMD_H

/*=============================================================================

FILE:         qdss_diag_cmd.h

DESCRIPTION:

================================================================================
              Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "diagpkt.h"
#include "diagcmd.h"

#include "qdss_constants.h"

#define QDSSDIAG_PROCESSOR_SENSOR     0x0800  // Sensor Root
#define QDSS_DIAG_PROC_ID             QDSSDIAG_PROCESSOR_SENSOR


#define QDSS_QUERY_STATUS              0x00
#define QDSS_TRACE_SINK                0x01
#define QDSS_FILTER_ETM                0x02
#define QDSS_FILTER_STM                0x03
#define QDSS_FILTER_HWEVENT_ENABLE     0x04

#define QDSS_TMC_CMD                   0x30
#define QDSS_FILTER_HWEVENT_CONFIGURE  0x31
#define QDSS_HWEVENT_SET_DETECT_MASK   0x32
#define QDSS_HWEVENT_SET_TRIGGER_MASK  0x33

#define QDSS_MAP_CTI                   0x34
#define QDSS_UNMAP_CTI                 0X35
#define QDSS_RESET_CTI                 0x36
#define QDSS_QUERY_CTI                 0x37
#define QDSS_TPDM_CMD                  0x46
#define QDSS_TPDA_CMD                  0x47

#define QDSS_TEST_CMD                  0xfd
#define QDSS_AGENT_TEST                0xfe


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  uint8 cmdCode;        /**< Diag Message ID */
  uint8 subsysId;       /**< Subsystem ID (DIAG_SUBSYS_QDSS)*/
  uint16 subsysCmdCode; /**< Subsystem command code */
} qdss_diag_pkt_hdr;

/*-------------------------------------------------------------------------*/
/**
  @brief Response packet: Generic reponse with result.

  Common packet when only returning a result.
 */

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
} qdss_diag_pkt_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr; /**< Header */
  uint8 result;          /**< See QDSS_CMDRESP_... definitions */
} qdss_diag_pkt_rsp;

/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
} qdss_query_status_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8 trace_sink;
  uint8 stm_enabled;
  uint8 hw_events_enabled;
} qdss_query_status_rsp;

/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8 trace_sink;
} qdss_trace_sink_req;

typedef qdss_diag_pkt_rsp qdss_trace_sink_rsp; //generic response


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8  state;
} qdss_filter_etm_req;

typedef qdss_diag_pkt_rsp qdss_filter_etm_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8  state;
} qdss_filter_stm_req;

typedef qdss_diag_pkt_rsp qdss_filter_stm_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8  state;
} qdss_filter_hwevents_req;

typedef qdss_diag_pkt_rsp qdss_filter_hwevents_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint16   id;
  uint16   reserved; //keep the args 32 bit aligned
  uint8  args[1];
} qdss_tmc_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint16   id;
  uint16   result;
  uint8  args[1];
} qdss_tmc_rsp;

#define QDSS_TMC_READ_START         1
#define QDSS_TMC_READ               2
#define QDSS_TMC_READ_END           3
#define QDSS_ETR_SET_BYTE_COUNTER   4
#define QDSS_ETR_SET_TIMER          5

typedef PACK(struct)
{
   uint32 counter;
}qdss_etr_set_byte_counter_req;

typedef PACK(struct)
{
   uint32 timer_ms;
}qdss_etr_set_timer_req;


typedef PACK(struct)
{
   uint8 reserved;
}qdss_tmc_no_args;

typedef qdss_tmc_no_args qdss_etr_set_byte_counter_rsp;
typedef qdss_tmc_no_args qdss_tmc_read_start_req;
typedef qdss_tmc_no_args qdss_tmc_read_start_rsp;
typedef qdss_tmc_no_args qdss_tmc_read_end_req;
typedef qdss_tmc_no_args qdss_tmc_read_end_rsp;
typedef qdss_tmc_no_args qdss_etr_set_timer_rsp;

typedef PACK(struct)
{
   uint16 version;
   uint16 seq;
   uint32 len;
}qdss_tmc_read_req;

//DIAG caps response allocations at 4096 bytes
//We must leave 20 bytes of our header and 30 bytes of diag bookkeeping
//So the current maximum is 4046.

#define QDSS_TMC_READ_BLOCK_SIZE 4000

typedef PACK(struct)
{
   uint16 version;
   uint16 seq;
   uint32 len;
   uint32 checksum;
   uint8  data[QDSS_TMC_READ_BLOCK_SIZE];
}qdss_tmc_read_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint32 register_addr;
  uint32 register_value;
} qdss_filter_hwevents_configure_req;

typedef qdss_diag_pkt_rsp qdss_filter_hwevents_configure_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint32 hwe_mask;
} qdss_filter_hwevents_set_detect_mask_req;

typedef qdss_diag_pkt_rsp qdss_filter_hwevents_set_detect_mask_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint32 hwe_mask;
} qdss_filter_hwevents_set_trigger_mask_req;

typedef qdss_diag_pkt_rsp qdss_filter_hwevents_set_trigger_mask_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  char  block[CTI_BLOCK_SIZE];
  uint8 type;
  uint8 trigger;
  uint8 channel;
} qdss_map_cti_req;

typedef qdss_diag_pkt_rsp qdss_map_cti_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  char block[CTI_BLOCK_SIZE];
  uint8 type;
  uint8 trigger;
  uint8 channel;
} qdss_unmap_cti_req;

typedef qdss_diag_pkt_rsp qdss_unmap_cti_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  char block[CTI_BLOCK_SIZE];
} qdss_reset_cti_req;

typedef qdss_diag_pkt_rsp qdss_reset_cti_rsp;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  char block[CTI_BLOCK_SIZE];
  uint8 type;
} qdss_query_cti_req;

/*-------------------------------------------------------------------------*/
/*
 * ARMv8 will support up to 32 triggers.
 * If a target supports less than 32, only use 'length' elements.
 */
#define MAX_TRIGGERS 32
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8 result;
  uint8 length;
  uint32 mappings[MAX_TRIGGERS]; /* Actual size is length */
} qdss_query_cti_rsp;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8 test_num;
  uint8 args[1];
} qdss_test_req;

typedef qdss_diag_pkt_rsp qdss_test_rsp;

#define QDSS_TRACER_DATA_TEST 1

typedef PACK(struct)
{
  uint8 count;
} qdss_tracer_data_test_args;

#define QDSS_FUNNEL_TEST 2

typedef PACK(struct)
{
  uint8 port_id;
  uint8 state;
} qdss_funnel_test_args;

#define QDSS_TRACER_TEST 3

typedef PACK(struct)
{
  uint8 type;
  uint8 subtype;
  uint16 option;
  uint32 mask;
  uint32 count1;
  uint32 count2;
} qdss_tracer_test_args;


/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint8 test_num;        /* 1 = start cfg id */
  uint8 start_cfg_id;    /* 0 = none, 1 = buffer, 2=default file 3=cfg1 file */
  uint16 option;         /* reserved */
} qdss_agent_test_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;   /**< Header */
  uint16 result;           /**< Test completion 0 = Yes, 1 = Aborted */
  uint16 status1;          /**< Primary result */
  uint16 status2;          /**< Secondary result */
} qdss_agent_test_rsp;


#define QDSS_DRAIN_TEST 4

typedef PACK(struct)
{
  uint8 type;
} qdss_drain_test_args;



/*-------------------------------------------------------------------------*/
typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint16   id;
  uint16   reserved; //keep the args 32 bit aligned
  uint8  args[1];
} qdss_generic_req;

typedef PACK(struct)
{
  qdss_diag_pkt_hdr hdr;
  uint16   id;
  uint16   result;
  uint8  args[1];
} qdss_generic_rsp;

typedef PACK(struct)
{
   uint8 reserved;
}qdss_generic_no_args;


typedef qdss_generic_req qdss_tpdm_req;
typedef qdss_generic_rsp qdss_tpdm_rsp;

#define QDSS_TPDM_SET                 1
#define QDSS_TPDM_DATASET_SET         2
#define QDSS_TPDM_GET_GPR             3
#define QDSS_TPDM_SET_GPR             4
#define QDSS_TPDM_DSB_SET_EDCMR       5
#define QDSS_TPDM_DSB_GET_PARAM       6
#define QDSS_TPDM_DSB_SET_PARAM       7
#define QDSS_TPDM_CMB_GET_PARAM       8
#define QDSS_TPDM_CMB_SET_PARAM       9

typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 state;
}qdss_tpdm_set_req;

typedef qdss_generic_no_args qdss_tpdm_set_rsp;

typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 enable_mask;
   uint8 state;
}qdss_tpdm_dataset_set_req;

typedef qdss_generic_no_args qdss_tpdm_dataset_set_rsp;

typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 reg_num;
}qdss_tpdm_get_gpr_req;

typedef PACK(struct)
{
   uint32 val;
}qdss_tpdm_get_gpr_rsp;

typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 reg_num;
   uint16 reserved;
   uint32 val;
}qdss_tpdm_set_gpr_req;

typedef qdss_generic_no_args qdss_tpdm_set_gpr_rsp;

typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 start;
   uint8 end;
   uint8 state;
}qdss_tpdm_dsb_set_edcmr_req;

typedef qdss_generic_no_args qdss_tpdm_dsb_set_edcmr_rsp;

typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 param_id;
}qdss_tpdm_dsb_get_param_req;

typedef PACK(struct)
{
   uint32 val;
}qdss_tpdm_dsb_get_param_rsp;

typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 param_id;
   uint16 reserved;
   uint32 val;
}qdss_tpdm_dsb_set_param_req;

typedef qdss_generic_no_args qdss_tpdm_dsb_set_param_rsp;


typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 param_id;
}qdss_tpdm_cmb_get_param_req;

typedef PACK(struct)
{
   uint32 val;
}qdss_tpdm_cmb_get_param_rsp;

typedef PACK(struct)
{
   uint8 tpdm_id;
   uint8 param_id;
   uint16 reserved;
   uint32 val;
}qdss_tpdm_cmb_set_param_req;

typedef qdss_generic_no_args qdss_tpdm_cmb_set_param_rsp;

typedef qdss_generic_req qdss_tpda_req;
typedef qdss_generic_rsp qdss_tpda_rsp;

#define QDSS_TPDA_SET_PARAM       1
#define QDSS_TPDA_GET_PARAM       2


typedef PACK(struct)
{
   uint8 tpda_id;
   uint8 param_id;
}qdss_tpda_get_param_req;

typedef PACK(struct)
{
   uint32 val;
}qdss_tpda_get_param_rsp;

typedef PACK(struct)
{
   uint8 tpda_id;
   uint8 param_id;
   uint16 reserved;
   uint32 val;
}qdss_tpda_set_param_req;

typedef qdss_generic_no_args qdss_tpda_set_param_rsp;

/*-------------------------------------------------------------------------*/

#endif //QDSS_DIAG_CMD_H
