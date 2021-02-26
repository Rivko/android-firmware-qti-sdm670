#ifndef SLIMBUSCONSTANTS_H
#define SLIMBUSCONSTANTS_H
/*
===========================================================================

FILE:         SlimBusConstants.h

DESCRIPTION:  This file defines constants related to the SLIMbus
              core driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/SlimBusConsts.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/05/16   MJS     Add support for multiple endpoints per port.
09/09/14   mq      Multi-dataline support.
06/17/14   MJS     Send master power up/down notification to satellites.
07/30/13   MJS     Support for user-define message API.
04/30/13   MJS     Add next active framer support.
04/22/13   MJS     Fix masking for USR_CHANNEL_DEF_1[PROTO].
12/11/11   MJS     Remove references to DAL.
05/24/11   MJS     Add pause clock support.
09/28/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010, 2011, 2013, 2014, 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/


/* Slimbus frame structure constants */
#define SB_MAX_CLK_GEAR               10
#define SB_CELLS_PER_SUPERFRAME       6144
#define SB_CELLS_PER_SUPERFRAME_DIV8  (SB_CELLS_PER_SUPERFRAME/8)
#define SB_SLOTS_PER_SUPERFRAME       (SB_CELLS_PER_SUPERFRAME/SB_CELLS_PER_SLOT)
#define SB_CELLS_PER_SLOT             4
#define SB_FRAME_SLOTS_PER_SUPERFRAME 16
#define SB_GUIDE_SLOTS_PER_SUPERFRAME 2
#define SB_MAX_INTERVAL_COEFF_3       (SB_SLOTS_PER_SUPERFRAME/3)
#define SB_MAX_INTERVAL_COEFF_1       SB_SLOTS_PER_SUPERFRAME

#define SB_DEFAULT_ROOT_FREQ_HZ       24576000
#define SB_DEFAULT_ROOT_FREQ_HZ_DIV8  (SB_DEFAULT_ROOT_FREQ_HZ/8)

#define SB_VE_MAP_RESERVED_ADDR     0xC00
#define SB_VE_MAX_SLICE_SIZE_BYTES  16

#define SB_MAX_MSG_PAYLOAD_SIZE_BYTES 27 

/* Logical address of the manager */
#define SB_MSG_LA_MANAGER  0xff

/* Slimbus message types */
typedef enum
{
  SB_MSG_MT_CORE                        = 0x0,
  SB_MSG_MT_DEST_REFERRED_CLASS         = 0x1,
  SB_MSG_MT_DEST_REFERRED_USER          = 0x2,
  SB_MSG_MT_SRC_REFERRED_CLASS          = 0x5,
  SB_MSG_MT_SRC_REFERRED_USER           = 0x6,
} SlimBusMsgMtType;

/* Slimbus message destination types */
typedef enum
{
  SB_MSG_DT_UNICAST                     = 0x0,
  SB_MSG_DT_BROADCAST                   = 0x1,
} SlimBusMsgDtType;

/* Slimbus core message codes */
typedef enum
{
  SB_MSG_MC_REPORT_PRESENT              = 0x1,
  SB_MSG_MC_ASSIGN_LOGICAL_ADDRESS      = 0x2,
  SB_MSG_MC_RESET_DEVICE                = 0x4,
  SB_MSG_MC_CHANGE_LOGICAL_ADDRESS      = 0x8,
  SB_MSG_MC_CHANGE_ARBITRATION_PRIORITY = 0x9,
  SB_MSG_MC_REQUEST_SELF_ANNOUNCEMENT   = 0xC,
  SB_MSG_MC_REPORT_ABSENT               = 0xF,

  SB_MSG_MC_CONNECT_SOURCE              = 0x10,
  SB_MSG_MC_CONNECT_SINK                = 0x11,
  SB_MSG_MC_DISCONNECT_PORT             = 0x14,
  SB_MSG_MC_CHANGE_CONTENT              = 0x18,

  SB_MSG_MC_REQUEST_INFORMATION         = 0x20,
  SB_MSG_MC_REQUEST_CLEAR_INFORMATION   = 0x21,
  SB_MSG_MC_REPLY_INFORMATION           = 0x24,
  SB_MSG_MC_CLEAR_INFORMATION           = 0x28,
  SB_MSG_MC_REPORT_INFORMATION          = 0x29,

  SB_MSG_MC_BEGIN_RECONFIGURATION       = 0x40,
  SB_MSG_MC_NEXT_ACTIVE_FRAMER          = 0x44,
  SB_MSG_MC_NEXT_SUBFRAME_MODE          = 0x45,
  SB_MSG_MC_NEXT_CLOCK_GEAR             = 0x46,
  SB_MSG_MC_NEXT_ROOT_FREQUENCY         = 0x47,
  SB_MSG_MC_NEXT_PAUSE_CLOCK            = 0x4A,
  SB_MSG_MC_NEXT_RESET_BUS              = 0x4B,
  SB_MSG_MC_NEXT_SHUTDOWN_BUS           = 0x4C,
  SB_MSG_MC_NEXT_DEFINE_CHANNEL         = 0x50,
  SB_MSG_MC_NEXT_DEFINE_CONTENT         = 0x51,
  SB_MSG_MC_NEXT_ACTIVATE_CHANNEL       = 0x54,
  SB_MSG_MC_NEXT_DEACTIVATE_CHANNEL     = 0x55,
  SB_MSG_MC_NEXT_REMOVE_CHANNEL         = 0x58,
  SB_MSG_MC_RECONFIGURE_NOW             = 0x5F,

  SB_MSG_MC_REQUEST_VALUE               = 0x60,
  SB_MSG_MC_REQUEST_CHANGE_VALUE        = 0x61,
  SB_MSG_MC_REPLY_VALUE                 = 0x64,
  SB_MSG_MC_CHANGE_VALUE                = 0x68,
} SlimBusMsgMcType;

/* Slimbus user-defined message codes */
typedef enum
{
  SB_MSG_MC_USR_QC_MASTER_CAPABILITY  = 0x00,
  SB_MSG_MC_USR_REPORT_QC_SATELLITE   = 0x01,
  
  SB_MSG_MC_USR_NOTIFY_MGR_SLEEP      = 0x05,
  SB_MSG_MC_USR_NOTIFY_MGR_AWAKE      = 0x06,

  SB_MSG_MC_USR_LA_ANNOUNCE           = 0x0C,
  SB_MSG_MC_USR_LA_QUERY              = 0x0D,
  SB_MSG_MC_USR_LA_REPLY              = 0x0E,

  SB_MSG_MC_USR_CHANNEL_DEF           = 0x20,
  SB_MSG_MC_USR_CHANNEL_DEF_ACTIVATE  = 0x21,
  SB_MSG_MC_USR_CHANNEL_CONTROL       = 0x23,
  SB_MSG_MC_USR_DO_RECONFIGURE_NOW    = 0x24,
  SB_MSG_MC_USR_GENERIC_ACK           = 0x25,
  SB_MSG_MC_USR_REQ_MSG_BANDWIDTH     = 0x28,

  SB_MSG_MC_USR_REQ_CONNECT_SOURCE    = 0x2C,
  SB_MSG_MC_USR_REQ_CONNECT_SINK      = 0x2D,
  SB_MSG_MC_USR_REQ_DISCONNECT_PORT   = 0x2E,
} SlimBusMsgMcUsrType;

/* Slimbus message lengths */
#define SB_MSG_MC_REPORT_PRESENT_LEN          10
#define SB_MSG_MC_ASSIGN_LOGICAL_ADDRESS_LEN  9
#define SB_MSG_MC_CONNECT_SOURCE_LEN          5
#define SB_MSG_MC_DISCONNECT_PORT_LEN         4
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_LEN     6
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_LEN     6
#define SB_MSG_MC_NEXT_ACTIVE_FRAMER_LEN      6
#define SB_MSG_MC_NEXT_SUBFRAME_MODE_LEN      3
#define SB_MSG_MC_NEXT_CLOCK_GEAR_LEN         3
#define SB_MSG_MC_NEXT_PAUSE_CLOCK_LEN        3
#define SB_MSG_MC_BEGIN_RECONFIGURATION_LEN   2
#define SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN   3
#define SB_MSG_MC_RECONFIGURE_NOW_LEN         2

/* Slimbus user-defined message lengths */
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_LEN   7
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_LEN  7

#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_MIN_LEN   7
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_MIN_LEN  7

#define SB_MSG_MC_USR_NOTIFY_MGR_SLEEP_LEN    4
#define SB_MSG_MC_USR_NOTIFY_MGR_AWAKE_LEN    5

#define SB_MSG_MC_USR_LA_QUERY_LEN            10
#define SB_MSG_MC_USR_DO_RECONFIGURE_NOW_LEN  5
#define SB_MSG_MC_USR_GENERIC_ACK_LEN         5
#define SB_MSG_MC_USR_LA_REPLY_LEN            11
#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_LEN   6
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_LEN  7
#define SB_MSG_MC_USR_REQ_DISCONNECT_PORT_LEN 6

/* Slimbus message fields */
#define SB_MSG_MC_GENERIC_0__LEN_SHFT 0
#define SB_MSG_MC_GENERIC_0__LEN_BMSK 0x1f
#define SB_MSG_MC_GENERIC_0__MT_SHFT  5
#define SB_MSG_MC_GENERIC_0__MT_BMSK  0xE0
#define SB_MSG_MC_GENERIC_0__MC_SHFT  8
#define SB_MSG_MC_GENERIC_0__MC_BMSK  0x7f00
#define SB_MSG_MC_GENERIC_0__DT_SHFT  15
#define SB_MSG_MC_GENERIC_0__DT_BMSK  0x8000
#define SB_MSG_MC_GENERIC_0__LA_SHFT  16
#define SB_MSG_MC_GENERIC_0__LA_BMSK  0xff0000

#define SB_MSG_MC_CHANGE_VALUE__CM_OFFSET 5

#define SB_MSG_MC_REQUEST_VALUE__TID_OFFSET 3

#define SB_MSG_MC_REPLY_VALUE_0__TID_SHFT  24
#define SB_MSG_MC_REPLY_VALUE_0__TID_BMSK  0xff000000
#define SB_MSG_MC_REPLY_VALUE__VS_OFFSET 4

#define SB_MSG_MC_ASSIGN_LOGICAL_ADDRESS_2__LA_SHFT 0
#define SB_MSG_MC_ASSIGN_LOGICAL_ADDRESS_2__LA_BMSK 0xff

#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_0__CN_SHFT  16
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_0__CN_BMSK  0xff0000
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_0__SD_LSB_SHFT  24
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_0__SD_LSB_BMSK  0xff000000
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_1__SD_MSB_SHFT  0
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_1__SD_MSB_BMSK  0xf
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_1__TP_SHFT  4
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_1__TP_BMSK  0xf0
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_1__SL_SHFT  8
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_1__SL_BMSK  0x1f00
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_1__DL_SHFT  13
#define SB_MSG_MC_NEXT_DEFINE_CHANNEL_1__DL_BMSK  0xe000

#define SB_MSG_MC_NEXT_DEFINE_CONTENT_0__CN_SHFT  16
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_0__CN_BMSK  0xff0000
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_0__PR_SHFT  24
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_0__PR_BMSK  0xff000000
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_1__DF_SHFT  0
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_1__DF_BMSK  0xf
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_1__SF_SHFT  4
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_1__SF_BMSK  0xf0
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_1__DL_SHFT  8
#define SB_MSG_MC_NEXT_DEFINE_CONTENT_1__DL_BMSK  0x1f00

#define SB_MSG_MC_NEXT_ACTIVE_FRAMER_0__LAIF_SHFT  16
#define SB_MSG_MC_NEXT_ACTIVE_FRAMER_0__LAIF_BMSK  0xff0000
#define SB_MSG_MC_NEXT_ACTIVE_FRAMER_0__NCO_SHFT  24
#define SB_MSG_MC_NEXT_ACTIVE_FRAMER_0__NCO_BMSK  0xff000000
#define SB_MSG_MC_NEXT_ACTIVE_FRAMER_1__NCI_SHFT  4
#define SB_MSG_MC_NEXT_ACTIVE_FRAMER_1__NCI_BMSK  0xfff0

#define SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_0__CN_SHFT  16
#define SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_0__CN_BMSK  0xff0000

#define SB_MSG_MC_CONNECT_SOURCE_0__PN_SHFT  24
#define SB_MSG_MC_CONNECT_SOURCE_0__PN_BMSK  0x1f000000
#define SB_MSG_MC_CONNECT_SOURCE_0__EP_SHFT  29
#define SB_MSG_MC_CONNECT_SOURCE_0__EP_BMSK  0xe0000000
#define SB_MSG_MC_CONNECT_SOURCE_1__CN_SHFT  0
#define SB_MSG_MC_CONNECT_SOURCE_1__CN_BMSK  0xff

#define SB_MSG_MC_DISCONNECT_PORT_0__PN_SHFT  24
#define SB_MSG_MC_DISCONNECT_PORT_0__PN_BMSK  0xff000000


/* Slimbus user-defined message fields */
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_0__MAGIC_LSB_SHFT  24
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_0__MAGIC_LSB_BMSK  0xff000000
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_1__MAGIC_MSB_SHFT  0
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_1__MAGIC_MSB_BMSK  0xff
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_1__PROTO_VER_SHFT  8
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_1__PROTO_VER_BMSK  0xff00
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_1__SAT_PROTO_SHFT  16
#define SB_MSG_MC_USR_REPORT_QC_SATELLITE_1__SAT_PROTO_BMSK  0xff0000

#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_0__MAGIC_LSB_SHFT  24
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_0__MAGIC_LSB_BMSK  0xff000000
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_1__MAGIC_MSB_SHFT  0
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_1__MAGIC_MSB_BMSK  0xff
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_1__PROTO_VER_SHFT  8
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_1__PROTO_VER_BMSK  0xff00
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_1__SAT_PROTO_SHFT  16
#define SB_MSG_MC_USR_QC_MASTER_CAPABILITY_1__SAT_PROTO_BMSK  0xff0000

#define SB_MSG_MC_USR_NOTIFY_MGR_SLEEP__TID_SHFT  24
#define SB_MSG_MC_USR_NOTIFY_MGR_SLEEP__TID_BMSK  0xff000000

#define SB_MSG_MC_USR_NOTIFY_MGR_AWAKE_0__TID_SHFT   24
#define SB_MSG_MC_USR_NOTIFY_MGR_AWAKE_0__TID_BMSK   0xff000000
#define SB_MSG_MC_USR_NOTIFY_MGR_AWAKE_1__CODE_SHFT  0
#define SB_MSG_MC_USR_NOTIFY_MGR_AWAKE_1__CODE_BMSK  0xff

#define SB_MSG_MC_USR_CHANNEL_DEF_0__CLIENT_NUM_SHFT  24
#define SB_MSG_MC_USR_CHANNEL_DEF_0__CLIENT_NUM_BMSK  0x1f000000
#define SB_MSG_MC_USR_CHANNEL_DEF_0__DF_SHFT          29
#define SB_MSG_MC_USR_CHANNEL_DEF_0__DF_BMSK          0xe0000000
#define SB_MSG_MC_USR_CHANNEL_DEF_1__DL_SHFT          0
#define SB_MSG_MC_USR_CHANNEL_DEF_1__DL_BMSK          0x1f
#define SB_MSG_MC_USR_CHANNEL_DEF_1__COEFF_SHFT       5
#define SB_MSG_MC_USR_CHANNEL_DEF_1__COEFF_BMSK       0x20
#define SB_MSG_MC_USR_CHANNEL_DEF_1__SF_SHFT          6
#define SB_MSG_MC_USR_CHANNEL_DEF_1__SF_BMSK          0xc0
#define SB_MSG_MC_USR_CHANNEL_DEF_1__PROTO_SHFT       8
#define SB_MSG_MC_USR_CHANNEL_DEF_1__PROTO_BMSK       0xf00
#define SB_MSG_MC_USR_CHANNEL_DEF_1__EXP_SHFT         12
#define SB_MSG_MC_USR_CHANNEL_DEF_1__EXP_BMSK         0xf000
#define SB_MSG_MC_USR_CHANNEL_DEF_1__PR_SHFT          16
#define SB_MSG_MC_USR_CHANNEL_DEF_1__PR_BMSK          0xff0000
#define SB_MSG_MC_USR_CHANNEL_DEF_1__TID_SHFT         24
#define SB_MSG_MC_USR_CHANNEL_DEF_1__TID_BMSK         0xff000000
#define SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET          8

#define SB_MSG_MC_USR_CHANNEL_CONTROL_0__CLIENT_NUM_SHFT  24
#define SB_MSG_MC_USR_CHANNEL_CONTROL_0__CLIENT_NUM_BMSK  0x1f000000
#define SB_MSG_MC_USR_CHANNEL_CONTROL_0__CTRL_SHFT        30
#define SB_MSG_MC_USR_CHANNEL_CONTROL_0__CTRL_BMSK        0xc0000000
#define SB_MSG_MC_USR_CHANNEL_CONTROL_1__TID_SHFT         0
#define SB_MSG_MC_USR_CHANNEL_CONTROL_1__TID_BMSK         0xff
#define SB_MSG_MC_USR_CHANNEL_CONTROL__CN_OFFSET          5

#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_0__CLIENT_NUM_SHFT     24
#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_0__CLIENT_NUM_BMSK     0x1f000000
#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_0__MSG_SLOTS_LSB_SHFT  29
#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_0__MSG_SLOTS_LSB_BMSK  0xe0000000
#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_1__MSG_SLOTS_MSB_SHFT  0
#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_1__MSG_SLOTS_MSB_BMSK  0xff
#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_1__TID_SHFT            8
#define SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_1__TID_BMSK            0xff00

#define SB_MSG_MC_USR_DO_RECONFIGURE_NOW_0__TID_SHFT        24
#define SB_MSG_MC_USR_DO_RECONFIGURE_NOW_0__TID_BMSK        0xff000000
#define SB_MSG_MC_USR_DO_RECONFIGURE_NOW_1__CLIENT_NUM_SHFT 0
#define SB_MSG_MC_USR_DO_RECONFIGURE_NOW_1__CLIENT_NUM_BMSK 0x1f

#define SB_MSG_MC_USR_LA_QUERY_0__TID_SHFT  24
#define SB_MSG_MC_USR_LA_QUERY_0__TID_BMSK  0xff000000
#define SB_MSG_MC_USR_LA_QUERY_1__EA_SHFT   0

#define SB_MSG_MC_USR_LA_REPLY_0__TID_SHFT  24
#define SB_MSG_MC_USR_LA_REPLY_0__TID_BMSK  0xff000000

#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_0__DEST_LA_SHFT   24
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_0__DEST_LA_BMSK   0xff000000
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_1__PORT_NUM_SHFT  0
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_1__PORT_NUM_BMSK  0x1f
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_1__EP_NUM_SHFT    5
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_1__EP_NUM_BMSK    0xe0
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_1__CHAN_NUM_SHFT  8
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_1__CHAN_NUM_BMSK  0xff00
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_1__TID_SHFT       16
#define SB_MSG_MC_USR_REQ_CONNECT_SOURCE_1__TID_BMSK       0xff0000

#define SB_MSG_MC_USR_REQ_DISCONNECT_PORT_0__DEST_LA_SHFT   24
#define SB_MSG_MC_USR_REQ_DISCONNECT_PORT_0__DEST_LA_BMSK   0xff000000
#define SB_MSG_MC_USR_REQ_DISCONNECT_PORT_1__PORT_NUM_SHFT  0
#define SB_MSG_MC_USR_REQ_DISCONNECT_PORT_1__PORT_NUM_BMSK  0xff
#define SB_MSG_MC_USR_REQ_DISCONNECT_PORT_1__TID_SHFT       8
#define SB_MSG_MC_USR_REQ_DISCONNECT_PORT_1__TID_BMSK       0xff00

#define SB_MSG_MC_USR_GENERIC_ACK_0__TID_SHFT         24
#define SB_MSG_MC_USR_GENERIC_ACK_0__TID_BMSK         0xff000000
#define SB_MSG_MC_USR_GENERIC_ACK_1__CLIENT_NUM_SHFT  0
#define SB_MSG_MC_USR_GENERIC_ACK_1__CLIENT_NUM_BMSK  0x1f
#define SB_MSG_MC_USR_GENERIC_ACK_1__RESP_SHFT        5
#define SB_MSG_MC_USR_GENERIC_ACK_1__RESP_BMSK        0x20

/* Other bitfields */

/* Element Code bitfield */
#define SB_BF_EC__SS_SHFT         0
#define SB_BF_EC__SS_BMSK         0x7
#define SB_BF_EC__BYTE_MODE_SHFT  3
#define SB_BF_EC__BYTE_MODE_BMSK  0x8
#define SB_BF_EC__ADDR_SHFT       4
#define SB_BF_EC__ADDR_BMSK       0xfff0

/* Possible values for Element Code byte mode */
#define SB_EC_BYTE_ACCESS      1
#define SB_EC_ELEMENTAL_ACCESS 0

/* Possible values for NEXT_PAUSE_CLOCK() payload */
#define SB_NPC_FAST_RECOVERY            0
#define SB_NPC_CONSTANT_PHASE_RECOVERY  1
#define SB_NPC_UNSPECIFIED_DELAY        2

/* RxMarker bitfield */
#define SB_BF_RXMARKER__LA_SHFT  0
#define SB_BF_RXMARKER__LA_BMSK  0xff
#define SB_BF_RXMARKER__TID_SHFT  8
#define SB_BF_RXMARKER__TID_BMSK  0xff00
#define SB_BF_RXMARKER__BUF_SIZE_SHFT  16
#define SB_BF_RXMARKER__BUF_SIZE_BMSK  0xff0000

/* Msg field get-set macros */
#define SB_MSG_GET(word,msg,field) \
  (((word)&SB_MSG_MC_##msg##__##field##_BMSK)>>SB_MSG_MC_##msg##__##field##_SHFT)
#define SB_MSG_SET(value,msg,field) \
  (((value)<<SB_MSG_MC_##msg##__##field##_SHFT)&SB_MSG_MC_##msg##__##field##_BMSK)

/* Misc bitfield get-set macros */
#define SB_BF_GET(word,bf,field) \
  (((word)&SB_BF_##bf##__##field##_BMSK)>>SB_BF_##bf##__##field##_SHFT)
#define SB_BF_SET(value,bf,field) \
  (((value)<<SB_BF_##bf##__##field##_SHFT)&SB_BF_##bf##__##field##_BMSK)

/* Slimbus protocol versions */
#define SB_QC_MASTER_PROTOCOL_V1 (1<<0)

/* Slimbus master-satellite magic word */
#define SB_QC_MASTER_PROTOCOL_MAGIC     0xC5D9UL
#define SB_QC_MASTER_PROTOCOL_MAGIC_LSB 0xD9UL
#define SB_QC_MASTER_PROTOCOL_MAGIC_MSB 0xC5UL

/* Slimbus master-satellite message versioning */
#define SB_QC_MASTER_CAPABILITY_VER 1
#define SB_QC_REPORT_SATELLITE_VER 1

#endif /* SLIMBUSCONSTANTS_H */

