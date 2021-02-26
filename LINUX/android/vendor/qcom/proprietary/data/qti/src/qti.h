
/******************************************************************************

                        QTI.H

******************************************************************************/

/******************************************************************************

  @file    qti.h
  @brief   Tethering Interface module

  DESCRIPTION
  Header file for tethering interface module.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        --------------------------------------------------------
05/02/14   pm         Added log message macros
01/22/14   sb         Added support for Fusion
12/9/13    sb         Add port mapper functionality. Modem interface file interaction
11/15/12   sb         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef __QTI_H
#define __QTI_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_addr.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdarg.h>
#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

#ifndef FEATURE_MDM_LE
 #include <cutils/properties.h>
#endif

#include "ds_util.h"
#include "qmi_client.h"
#include "qmi_qmux_if.h"
#include "qmi_port_defs.h"
#include "qmi_client_instance_defs.h"
#include "data_common_v01.h"
#include "qcmap_utils.h"

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

/*--------------------------------------------------------------------------
 Generic definitions
---------------------------------------------------------------------------*/
#define QTI_SUCCESS   0
#define QTI_FAILURE   (-1)
#define TRUE          1
#define FALSE         0

/*--------------------------------------------------------------------------
  RNDIS/ECM definitions
---------------------------------------------------------------------------*/
#define QTI_MAX_FILE_NAME_SIZE              50
#define QTI_MAX_COMMAND_STR_LEN             300
#define QTI_FILE_NODE_OPEN_MAX_RETRY        10
#define ONE_SEC                             1
#define RMNET_MAX_TRANSFER_SIZE             4096
#define MAX_NUM_OF_FD                       10
#define QTI_QMI_MSG_TIMEOUT_VALUE           90000
#define QTI_QMI_CLIENT_MAX_RETRY            10

#ifndef FEATURE_DATA_ADPL_ONLY
#define QTI_NL_MSG_MAX_LEN                  16384
#define RNDIS_INTERFACE                     "rndis0"
#define ECM_INTERFACE                       "ecm0"
#define ETH_INTERFACE                       "eth0"
#define IF_NAME_LEN                         16
#define QTI_INTERFACES                      3
#define QTI_QMI_SVC_INIT_TIMEOUT_MS         500
#define QTI_DEFAULT_INTERFACE_ID            (-99999)
#define QTI_QMI_MAX_TIMEOUT_MS              500 /*Timeout value in miliseconds */
#define QTI_MAX_MOBILEAP_RETRY              10
#define QTI_MAX_PH_DRIVER                   2

#define QMI_IP_STA_FILE "/etc/qmi_ip_sta"
#define QTI_PPP_LS_FILE "/etc/qti_ppp_ls_uds_file"

/*--------------------------------------------------------------------------
  RmNet definitions
---------------------------------------------------------------------------*/
#define FRMNET_CTRL_MODEM_OFFLINE           _IO(FRMNET_CTRL_IOCTL_MAGIC, 4)
#define FRMNET_CTRL_MODEM_ONLINE            _IO(FRMNET_CTRL_IOCTL_MAGIC, 5)
#define MHI_UCI_IOCTL_MAGIC                 'm'
#define MHI_UCI_EP_LOOKUP                   _IOR(MHI_UCI_IOCTL_MAGIC, 2, ep_info_type)
#define SET_DTR_HIGH                        1
#define SET_DTR_LOW                         0
#define MODEM_OUT_OF_SERVICE                0
#define MODEM_IN_SERVICE                    1
#define QTI_ENETRESET                       102

#define RMNET_SMD_DEV_FILE           "/dev/smdcntl8"
#define RMNET_USB_DEV_FILE           "/dev/rmnet_ctrl"
#define RMNET_USB_DEV_NODE           "/dev/rmnet_ctrl0"
#define RMNET_MHI_PH_DEV_FILE        "/dev/mhi_0304_00.01.00_pipe_14"
#define USB_TETHERED_SMD_CH          "DATA40_CNTL"

#define RMNET_MHI_DEV_FILE           "/dev/mhi_0304_00.01.00_pipe_16"

#define LOG_FILE                     "/data/LOGs.txt"

#define QTI_FOLDER                   "mkdir /data/qti"
#define HEX_DUMP_FILE_FLAG           "log"
#define HEX_DUMP_QXDM_FLAG           "qxdm"
#define HEX_DUMP_LOG_FILE            "/data/vendor/dataqti/log"
#define HEX_DUMP_QXDM_FILE           "/data/vendor/dataqti/qxdm"
#define HEX_DUMP_LOGS                "/data/vendor/dataqti/hex_dumps.txt"
#define DPL_USB_DEV_NODE             "/dev/dpl_ctrl0"
#endif //FEATURE_DATA_ADPL_ONLY
#define DPL_USB_DEV_FILE             "/dev/dpl_ctrl"
#define QTI_USB_RETRY                       5
#define FRMNET_CTRL_IOCTL_MAGIC             'r'
#define FRMNET_CTRL_EP_LOOKUP               _IOR(FRMNET_CTRL_IOCTL_MAGIC, 3, ep_info_type)
#define FRMNET_CTRL_GET_LINE_STATE          _IOR(FRMNET_CTRL_IOCTL_MAGIC, 2, int)

#define MHI_DATA_INTERFACE "rmnet_mhi0"
#define USB_DATA_INTERFACE "usb_rmnet0"
#define MHI_DPL_DATA_INTERFACE          "rmnet_mhi1"
#define USB_DPL_DATA_INTERFACE          "dpl_usb0"

#define DEV_FOLDER                   "/dev/"
#define MHI_ESOC_FILE_PATH           "/sys/bus/esoc/devices/esoc0/esoc_link_info"
#define MDM_FILE_PATH_MHI_START      "/sys/bus/mhi/devices"
#define MDM_FILE_PATH_MHI_END        "_QMI1/mhi_uci"
#define MAX_SOC_LINK_INFO_LEN        60
#define MHI_PATH_STRING_LEN          300
#define MHI_DIR_OPEN_MAX_RETRY       10

#define PROPERTY_MTU_SIZE                5
#define QTI_PROPERTY_VALUE_MAX           256
#define MAX_COMMAND_STR_LEN              200
#define QTI_PROPERTY_DPL_AGGR            "persist.data.dpl.aggr.enable"
#define QTI_PROPERTY_DPL_AGGR_SIZE       (5)
#define QTI_PROPERTY_DPL_AGGR_DEFAULT    QTI_MTU_INVALID
#define QTI_MTU_INVALID                  (0)
#define QTI_MTU_MAX                      (100000)
#define QTI_DPL_SETUP_RETRY_MAX          5
#define PROPERTY_DATA_MODE_SIZE          sizeof("tethered")
#define QTI_PROPERTY_DATA_MODE           "persist.data.mode"
#define QTI_QMI_MAX_TIMEOUT              10000
#define TETHERED_VAL                     "tethered"

#ifndef FEATURE_DATA_ADPL_ONLY
#define HEX_DUMPS_FOLDER      "/data/vendor/dataqti"

/*-----------------------------------------------------------------------------
  Service IDs
-----------------------------------------------------------------------------*/
#define WDA_SERVICE_ID                   0x1A
#define WDS_SERVICE_ID                   0x01
#define QMI_QOS_SERVICE_ID                      0x04
#define DFS_SERVICE_ID                          0x30

/*-----------------------------------------------------------------------------
  Message IDs
-----------------------------------------------------------------------------*/
#define WDA_SET_DATA_FORMAT_MESSAGE_ID   0x20
#define WDA_GET_DATA_FORMAT_MSG_ID              0x0021
#define WDA_SET_QMAP_SETTINGS_MSG_ID            0x002B
#define WDA_GET_QMAP_SETTINGS_MSG_ID            0x002C

#define WDS_BIND_MUX_DATA_PORT           0xA2
#define WDS_START_NETWORK_INTERFACE_MSG_ID      0x20
#define WDS_GET_LAST_THROUGHPUT_INFO_MSG_ID     0x00A4

#define QMI_QOS_BIND_DATA_PORT                  0x002B

#define DFS_BIND_CLIENT                         0x0021
#define DFS_GET_CLIENT_BINDING                  0x0022

/*----------------------------------------------------------------------------
  QMI TLVs
-----------------------------------------------------------------------------*/
#define QMI_WDA_SET_DATA_FORMAT_REQ_DATA_EP_ID_TYPE                   (0x17)
#define QMI_WDA_SET_DATA_FORMAT_RESP_UL_DATA_AGG_PROTOCOL             (0x12)
#define QMI_WDA_SET_DATA_FORMAT_RESP_DL_DATA_AGG_PROTOCOL             (0x13)
#define QMI_WDA_SET_DATA_FORMAT_RESP_DL_DATA_AGG_MAX_DATAGRAMS        (0x15)
#define QMI_WDA_SET_DATA_FORMAT_RESP_DL_DATA_AGG_MAX_SIZE             (0x16)
#define QMI_WDA_SET_DATA_FORMAT_RESP_UL_DATA_AGG_MAX_DATAGRAMS        (0x17)
#define QMI_WDA_SET_DATA_FORMAT_RESP_UL_DATA_AGG_MAX_SIZE             (0x18)
#define QMI_WDA_GET_DATA_FORMAT_REQ_DATA_EP_ID_TYPE                   (0x10)
#define QMI_WDA_SET_QMAP_SETTINGS_REQ_DATA_EP_ID_TYPE                 (0x11)
#define QMI_WDA_GET_QMAP_SETTINGS_REQ_DATA_EP_ID_TYPE                 (0x10)

#define QMI_WDS_BIND_MUX_DATA_PORT_REQ_DATA_EP_ID_TYPE                (0x10)
#define QMI_WDS_START_NETWORK_INTERFACE_RESP_DATA_EP_ID_TYPE          (0x12)
#define QMI_WDS_GET_LAST_THROUGHPUT_INFO_RESP_DATA_EP_ID_TYPE         (0x10)
#define QMI_QOS_BIND_DATA_PORT_REQ_DATA_EP_ID_TYPE                    (0x10)

#define DFS_BIND_CLIENT_REQ_DATA_EP_ID_TYPE                           (0x12)
#define DFS_GET_CLIENT_BINDING_RESP_DATA_EP_ID_TYPE                   (0x12)

#define QTI_QMUX_SVC_TYPE_QMI_CTL (0x00)
#define QTI_QMUX_IF_TYPE_QMUX     (0x01)

/*! Minimum size of a QMI_CTL message: header data, plus one message with no
    TLVs.*/
#define QTI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES \
  (sizeof(qti_qmux_msg_s) - \
   sizeof(qti_qmux_sdu_s) + \
   sizeof(qti_qmux_qmi_ctl_sdu_s))

/*! Minimum size of a regular QMUX message (non-QMI_CTL): header data, plus one
    message with no TLVs */
#define QTI_QMUX_NON_QMI_CTL_MIN_MSG_LEN_BYTES (sizeof(qti_qmux_msg_s))

/*! Minimum size of a generic QMUX message.*/
#define QTI_QMUX_MIN_MSG_LEN_BYTES QTI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES

/*! Size of the QMUX header plus I/F type preamble byte.*/
#define QTI_QMUX_HDR_LEN_BYTES \
  (sizeof(qti_qmux_if_type_t) + \
   sizeof(qti_qmux_hdr_s))

#define QTI_QMUX_SDU_MESSAGE_HDR_LEN_BYTES \
  (sizeof(qti_qmux_if_type_t) + \
   sizeof(qti_qmux_hdr_s) + \
   sizeof(qti_qmux_sdu_hdr_s)+ \
   sizeof(qti_qmux_qmi_msg_hdr_s))
#endif //FEATURE_DATA_ADPL_ONLY

#define DEFAULT_MTU_MRU_VALUE           2000

/*Address lengths*/
#define INET_ADDRSTRLEN        16
#define INET6_ADDRSTRLEN       46


/*===========================================================================
                              VARIABLE DECLARARTIONS
===========================================================================*/
/*--------------------------------------------------------------------------
  GENERIC DECLARATIONS
---------------------------------------------------------------------------*/
boolean       file_log_mode;

#ifndef FEATURE_DATA_ADPL_ONLY
/*--------------------------------------------------------------------------
  FILE LOGGING DECALRATIONS
---------------------------------------------------------------------------*/
  FILE          *fp_logs;
  int           qtilogfd;

/*--------------------------------------------------------------------------
  HEX DUMPS LOGGING DECALRATIONS
---------------------------------------------------------------------------*/
  boolean       hex_dumps_qxdm_mode;
  boolean       hex_dumps_file_mode;
#endif //FEATURE_DATA_ADPL_ONLY
/*--------------------------------------------------------------------------
  DPM CLIENT DECALRATIONS
---------------------------------------------------------------------------*/
  qmi_client_type                      dpm_notifier;
#ifndef FEATURE_DATA_ADPL_ONLY
/*--------------------------------------------------------------------------
  RNDIS/ECM DECALRATIONS
---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
   The different states through which QTI transitions during a connect
   and disconnect of USB cable
---------------------------------------------------------------------------*/
typedef enum
{
  QTI_INIT = 0,
  QTI_LINK_UP_WAIT,
  QTI_LINK_UP,
  QTI_LINK_DOWN_WAIT,
  QTI_LINK_DOWN
} qti_nl_state_e;

/*--------------------------------------------------------------------------
   Interfaces names
---------------------------------------------------------------------------*/
typedef enum
{
  RNDIS_IF = 1,
  ECM_IF = 2,
  ETH_IF = 4
} qti_interface_e;

/*--------------------------------------------------------------------------
   Interfaces name and interface index
---------------------------------------------------------------------------*/
typedef struct
{
 char dev_name[IF_NAME_LEN];
 int if_index;
 int prev_if_index;
 boolean enabled;
} qti_ifi_dev_name_t;

/*--------------------------------------------------------------------------
   QTI configuration variable which maintains the information needed with
   respect to a QTI call
---------------------------------------------------------------------------*/
typedef struct
{
 qti_nl_state_e          state;
 qti_ifi_dev_name_t      if_dev[QTI_INTERFACES];
 qmi_client_type         qti_qcmap_msgr_handle;
 uint32_t                qti_mobile_ap_handle;
 int                     qcmap_link_ref_cnt;
} qti_conf_t;

#endif //FEATURE_DATA_ADPL_ONLY
/*--------------------------------------------------------------------------
   Events that need to propagated to QCMAP from QTI
---------------------------------------------------------------------------*/
typedef enum
{
  QTI_QCMAP_INIT_EVENT = 1,
  QTI_LINK_UP_EVENT,
  QTI_LINK_DOWN_EVENT,
  QTI_RMNET_DPM_MODEM_NOT_IN_SERVICE_EVENT,
  QTI_DPM_INIT_EVENT
} qti_event_e;

/*--------------------------------------------------------------------------
  RMNET DECALRATIONS
---------------------------------------------------------------------------*/

typedef enum {
  DATA_EP_TYPE_RESERVED   = 0x00,
  /*Reserved */
  DATA_EP_TYPE_HSIC       = 0x01,
  /*HSIC*/
  DATA_EP_TYPE_HSUSB      = 0x02,
  /*HSUSB */
  DATA_EP_TYPE_PCIE       = 0x03,
  /* PCIE */
  DATA_EP_TYPE_EMBEDDED   = 0x04
  /* Embedded */
} peripheral_ep_type_enum;

typedef enum {
  PH_DRIVER_TYPE_USB   = 0x00,
  PH_DRIVER_TYPE_MHI   = 0x01
}peripheral_driver_type_enum;

typedef struct {
  peripheral_ep_type_enum  ep_type;
  /* Peripheral end point type. Values:
       @ENUM()
       All other values are reserved and will be ignored by service or clients.
  */
  uint32_t peripheral_iface_id;
  /* Peripheral interface number.*/
} peripheral_ep_info_type;


typedef struct {
  uint32_t consumer_pipe_num;
  /* Consumer pipe number for the hardware accelerated port*/

  uint32_t producer_pipe_num;
  /* Producer pipe number for the hardware accelerated port*/
} ipa_ep_pair_type;

typedef struct
{
  peripheral_ep_info_type ph_ep_info;

  ipa_ep_pair_type ipa_ep_pair;

} ep_info_type;

typedef struct
{
  int                          ph_iface_fd;
  char                         ph_iface_device_file[QTI_MAX_FILE_NAME_SIZE];
  char                         ph_data_iface_name[QTI_MAX_FILE_NAME_SIZE];
  boolean                      ph_enabled;
}ph_iface_info;


/*-------------------------------------------------------------------------
  RmNet config parameters
--------------------------------------------------------------------------*/
typedef struct{
#ifndef FEATURE_DATA_ADPL_ONLY
  ph_iface_info            ph_iface[QTI_MAX_PH_DRIVER];
  peripheral_ep_info_type  ph_iface_ep;
  char                     modem_iface_device_file[QTI_MAX_FILE_NAME_SIZE];
  int                      modem_iface_fd;
  char                     modem_data_iface_name[QTI_MAX_FILE_NAME_SIZE];
  peripheral_ep_info_type  modem_iface_ep;
  boolean                  dtr_enabled;
  qmi_client_type          qti_dfs_handle;
  ep_info_type             ep_info;
  qcmap_sk_fd_set_info_t   *fd_set;
#endif //FEATURE_DATA_ADPL_ONLY
  qmi_qmux_if_hndl_t       qti_qmux_qmi_handle;
  qmi_connection_id_type   qmux_conn_id;
  qmi_client_type          qti_dpm_handle;
  ds_target_t              target;
}qti_rmnet_param;

/*---------------------------------------------------------------------------
  QMUX message packet structure
----------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
  Control channel message preamble byte: Interface Type, which indicates the
  protocol used for the message
---------------------------------------------------------------------------*/
#ifndef FEATURE_DATA_ADPL_ONLY
typedef uint8_t qti_qmux_if_type_t;

/*--------------------------------------------------------------------------
  QMUX Header, follows the preamble
---------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  uint16_t length;     /*!< Length of the QMUX message including the QMUX header,but not the I/F Type */
  uint8_t  ctl_flags;  /*!< QMUX Control Flags indicating the sender */
  uint8_t  svc_type;   /*!< QMI service type of the SDU */
  uint8_t  client_id;  /*!< Client ID pertaining to this message */
} qti_qmux_hdr_s;

/*--------------------------------------------------------------------------
  QMUX Service Data Unit Transaction Header for a non-QMI_CTL message
---------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  uint8_t  svc_ctl_flags;  /*!< QMUX SDU Control Flags indicating message type */
  uint16_t txn_id;         /*!< Transaction ID (unique among control points) */
} qti_qmux_sdu_hdr_s;

/*--------------------------------------------------------------------------
  QMI_CTL Message Header (replaces QMUX SDU Transaction Header for QMI_CTL
    messages)
---------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  uint8_t svc_ctl_flags;  /*!< QMI_CTL Control Flags indicating message type */
  uint8_t txn_id;         /*!< Transaction ID (unique for every message) */
} qti_qmux_qmi_ctl_msg_hdr_s;

/*-------------------------------------------------------------------------
  Message ID and message length
--------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  uint16_t msg_id;     /*!< QMI Service-specific message ID */
  uint16_t msg_length; /*!< Length of TLV data to follow */
} qti_qmux_qmi_msg_hdr_s;

/*-------------------------------------------------------------------------
  SDU structure specifically for QMI_CTL messages
--------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  qti_qmux_qmi_ctl_msg_hdr_s hdr;
  qti_qmux_qmi_msg_hdr_s     msg;
} qti_qmux_qmi_ctl_sdu_s;

/*-------------------------------------------------------------------------
  SDU structure for non-QMI_CTL messages
--------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  qti_qmux_sdu_hdr_s     hdr;
  qti_qmux_qmi_msg_hdr_s msg;
} qti_qmux_sdu_s;

/*-------------------------------------------------------------------------
  QMUX Message including everything for a single QMI message in the
  transaction except for the TLV data
  @note Because of the union for QMI_CTL/regular SDUs, the size of this struct
  is not accurate to the size of the QMI_CTL header data. Use the constant
  QTI_QMUX_QMI_CTL_MIN_MSG_LEN_BYTES to get an offset to the start of TLV data
  for a QMI_CTL message
--------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) {
  qti_qmux_if_type_t if_type;
  qti_qmux_hdr_s     qmux_hdr;
  union __attribute__((packed)) {
    qti_qmux_qmi_ctl_sdu_s qmi_ctl;
    qti_qmux_sdu_s         qmux;
  } sdu;

  /*note Followed by msg_length bytes of TLV data, and potentially additional
    msg_id/msg_length/tlv_data sets of data */
} qti_qmux_msg_s;

// TODO: check if the QMUX structures are already defined in a header file.
//       Include the above structures from that header file if possible.

/*-------------------------------------------------------------------------
  Temporary buffer structure used to hold QMUX packet
--------------------------------------------------------------------------*/
typedef struct {
  uint32_t  size;  /*!< Size of data buffer in bytes */
  void     *data;  /*!< Pointer to location of buffer */
} qti_qmux_buf_s;
#endif //FEATURE_DATA_ADPL_ONLY


/*-------------------------------------------------------------------------
  Structure to hold parameters for DPL accelerated path enablement.
--------------------------------------------------------------------------*/
typedef struct {
  int             dpl_iface_fd;
  char            dpl_iface_device_file[QTI_MAX_FILE_NAME_SIZE];
  ep_info_type    ep_info;
  boolean         dtr_enabled;
  qmi_client_type qti_dpm_handle;
  ds_target_t     target;
} qti_dpl_param;


/*============================================================
Log Message Macros
=============================================================*/
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR

/*============================================================
Log Message Print Api's
=============================================================*/

void PRINT_msg(const char *funcName, uint32_t lineNum, const char*fmt, ...);

#undef PRINT_MSG
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        if ( file_log_mode )                                         \
        {                                                            \
          PRINT_msg( __FUNCTION__, __LINE__, fmtString, x, y, z);\
        }\
        else\
        {\
          MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                         __FUNCTION__, x, y, z);\
        }\

#define PRINT_MSG_6( level, fmtString, a, b, c, d, e, f)              \
        if ( file_log_mode )\
        {\
          PRINT_msg( __FUNCTION__, __LINE__, fmtString, a, b, c, d, e, f);   \
        }\
        else\
        {\
          MSG_SPRINTF_7( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,            \
                         __FUNCTION__, a, b, c, d, e, f);\
        }\

#undef LOG_MSG_INFO1
#undef LOG_MSG_INFO2
#undef LOG_MSG_INFO3
#undef LOG_MSG_ERROR
#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO1_6( fmtString, a, b, c, d, e, f)                 \
{                                                                     \
  PRINT_MSG_6 ( LOG_MSG_INFO1_LEVEL, fmtString, a, b, c, d, e, f);    \
}
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_ERROR_6( fmtString, a, b, c, d, e, f)                 \
{                                                                     \
  PRINT_MSG_6( LOG_MSG_ERROR_LEVEL, fmtString, a, b, c, d, e, f);     \
}

FILE *fp;

#define HEXDUMP_LOG_MSG(std,...)  if( hex_dumps_file_mode )       \
                                  { \
                                    fp = fopen(HEX_DUMP_LOGS, "a+"); \
                                    if(fp != NULL) \
                                    { \
                                      fprintf(fp, std, ##__VA_ARGS__); \
                                      fclose(fp); \
                                    } \
                                  }

#define HEXDUMP_QXDM_MSG(std, x, y, z) if( hex_dumps_qxdm_mode )       \
                                       { \
                                         LOG_MSG_INFO1(std, x, y, z); \
                                       }

/*===========================================================================
                       FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION PRINT_BUFFER()

DESCRIPTION

  This function
  - prints the QMI packet.
  - used for debugging.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/
void print_buffer
(
  char *buf,
  int size
);

#ifndef FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_NETLINK_INIT()

DESCRIPTION

  This function initializes QTI netlink part:
  - obtains the interface index and name mapping

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/
int qti_netlink_init(qti_conf_t * qti_conf);


/*===========================================================================

FUNCTION QTI_QCMAP_INIT()

DESCRIPTION

  This function initializes QTI:
  - initializes a QCMAP MSGR client

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/

int qti_qcmap_init(qti_conf_t * qti_conf);

/*===========================================================================
FUNCTION QTI_NL_RECV_MSG()

DESCRIPTION

  Function to receive incoming messages over the NETLINK routing socket.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None
==========================================================================*/
int qti_nl_recv_msg(int fd);

/*===========================================================================
FUNCTION QTI_NL_RECV_MSG()

DESCRIPTION

  Function to receive incoming messages over the NETLINK routing socket.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None
==========================================================================*/
int qti_nl_recv_msg(int fd);

/*===========================================================================

FUNCTION QTI_QCMAP_CMD_EXEC()

DESCRIPTION

  This function performs the execution of commands present in command queue.
  It mainly is involved in sending required QCMAP messages to QCMAP daemon to
  perform QCMAP specific operations

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int qti_qcmap_cmd_exec(qti_event_e event, qti_interface_e interface);



/*===========================================================================

FUNCTION QTI_RMNET_PH_INIT()

DESCRIPTION

  This function
  - sets up QTI to start listening for QMI packets coming on USB interface.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_ph_init
(
  qti_rmnet_param  * rmnet_state,
  qcmap_sk_fd_set_info_t * fd_set,
  qcmap_sock_thrd_fd_read_f read_f
);

/*===========================================================================

FUNCTION QTI_RMNET_PH_RECV_MSG()

DESCRIPTION

  This function
  - receives QMI message from USB interface.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_ph_recv_msg
(
   int rmnet_ph_fd
);

/*===========================================================================

FUNCTION QTI_RMNET_PH_SEND_MSG()

DESCRIPTION
  Sends QMI messages over USB interface

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/


void qti_rmnet_ph_send_msg
(
   void      *data,
   int       len
);


/*===========================================================================

FUNCTION QTI_RMNET_PH_GET_EP_INFO()

DESCRIPTION
  Queries the USB and IPA end point information from USB.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_ph_get_ep_info();

#endif //FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_DPM_INIT()

DESCRIPTION
  Initializes a Data Port Mapper QMI client for QTI

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_dpm_init
(
   qti_rmnet_param       * rmnet_state,
   qti_dpl_param         * dpl_state
);


#ifndef FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_DPM_PORT_OPEN()

DESCRIPTION
  Opens the SMD port using DPM QMI message

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_dpm_port_open();

/*===========================================================================

FUNCTION QTI_RMNET_DPM_PORT_CLOSE()

DESCRIPTION
  Closes the SMD port using the DPM QMI message

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_dpm_port_close();


/*===========================================================================

FUNCTION QTI_RMNET_MODEM_SEND_MSG()

DESCRIPTION
  Sends QMI message over SMD to modem

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

void qti_rmnet_modem_send_msg
(
   void      *data,
   int       len
);

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_SEND_MSG_THRU_QMUXD()

DESCRIPTION

  This function
  - send QMI message to modem

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

void qti_rmnet_modem_send_msg_thru_qmuxd
(
   void      *data,
   uint32_t     len
);

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_RECV_MSG()

DESCRIPTION
  Receives QMI message over SMD from modem

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_modem_recv_msg
(
   int rmnet_modem_fd
);

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_SET_DTR()

DESCRIPTION
  Sets DTR high/low on the SMD channel

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_modem_set_dtr
(
   uint8_t set
);
#endif //FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_RECV_MSG_THRU_QMUXD()

DESCRIPTION
  Receives QMI message over SMD from modem

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

void qti_rmnet_modem_recv_msg_thru_qmuxd
(
  qmi_connection_id_type conn_id,
  qmi_service_id_type    service_id,
  qmi_client_id_type     client_id,
  unsigned char          control_flags,
  unsigned char         *rx_msg,
  int                    rx_msg_len
);

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_INIT()

DESCRIPTION

  This function
  - initializes RMNET SMD state parameters.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_modem_init
(
  qti_rmnet_param         * rmnet_state,
  qti_dpl_param           * dpl_state
);

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_INIT_THRU_QMUXD()

DESCRIPTION

  This function
  - initializes RMNET SMD state parameters.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_modem_init_thru_qmuxd
(
  qti_rmnet_param  * rmnet_state,
  qcmap_sock_thrd_fd_read_f read_f,
  qti_dpl_param          * dpl_state
);
#ifndef FEATURE_DATA_ADPL_ONLY

/*=============================================================================
  FUNCTION QTI_QCMAP_EXIT()

  DESCRIPTION

  This function releases QCMAP client

  DEPENDENCIES
  None.

  RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

  SIDE EFFECTS
  None
==============================================================================*/
 int qti_qcmap_exit();
#endif //FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_DATA_INIT_BRIDGE()

DESCRIPTION
  - initializes the RmNet data driver

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_data_init_bridge();


/*===========================================================================

FUNCTION QTI_RMNET_DATA_TEARDOWN_BRIDGE()

DESCRIPTION
  resets RmNet data driver

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_data_teardown_bridge();
#ifndef FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_PH_SET_MRU()

DESCRIPTION
  - sets the MRU on the peripheral interface

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/
int qti_rmnet_ph_set_mru
(
  uint32_t mru,
  char* data_iface
);
#endif //FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_PH_SET_MTU()

DESCRIPTION
  - sets the MTU on the peripheral interface

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None


==========================================================================*/
int qti_rmnet_ph_set_mtu
(
  uint32_t mtu,
  char* data_iface
);

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_GET_EP_ID()

DESCRIPTION
  - obtains PCIe EP ID

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/

int qti_rmnet_modem_get_ep_id
(
  uint32_t *ep_id,
  char* data_iface
);

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_SET_MRU()

DESCRIPTION
  - sets MRU on modem interface

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/
int qti_rmnet_modem_set_mru
(
  uint32_t mru,
  char* data_iface
);

#ifndef FEATURE_DATA_ADPL_ONLY
/*===========================================================================

FUNCTION QTI_RMNET_MODEM_SET_MTU()

DESCRIPTION
- sets MTU on modem interface

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None


==========================================================================*/
int qti_rmnet_modem_set_mtu
(
  uint32_t mtu,
  char* data_iface
);

/*===========================================================================

FUNCTION QTI_RMNET_PROCESS_QMI_TX_TO_MODEM()

DESCRIPTION

  This function
  - processes QMI messages from peripheral interface to modem.


DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_process_qmi_tx_to_modem
(
  void            *data,
  uint32_t        qmux_msg_len
);

/*===========================================================================

FUNCTION QTI_RMNET_PROCESS_QMI_RX_FROM_MODEM()

DESCRIPTION

  This function
  - receives all messages from modem and passes onto USB interface.
  - processes set data format response message.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/


int qti_rmnet_process_qmi_rx_from_modem
(
  void            *qmux_msg,
  uint32_t        qmux_msg_len
);

/*===========================================================================

FUNCTION QTI_NL_QUERY_IF()

DESCRIPTION

  This function
  - brings up RNDIS and ECM interfaces to listen to netlink events
    coming up on those interfaces
  - sends RTM_GETLINK to kernel to check if RNDIS/ECM interfaces
    are running

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

==========================================================================*/

int qti_nl_query_if();

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_CHANGE_SLEEP_STATE()

DESCRIPTION
  - sets/resets sleep state on MHI interface

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/
int qti_rmnet_modem_change_sleep_state
(
  boolean sleep_state
);
#endif //FEATURE_DATA_ADPL_ONLY


/*===========================================================================

FUNCTION QTI_RMNET_DPL_INIT()

DESCRIPTION

  This function
  - opens the ph device file for DPL logging
  - adds the ph fd to wait on select call

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_dpl_ph_init
(
  qti_dpl_param           * dpl_state,
  qcmap_sk_fd_set_info_t    * fd_set,
  qcmap_sock_thrd_fd_read_f read_f
);

/*===========================================================================

FUNCTION QTI_DPL_PROCESS_PH_RESET()

DESCRIPTION
  - processes peripheral reset for DPL logging
  - upon USB cable plug-in : DPM open
  - upon USB cable plug-out : DPM close.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_dpl_process_ph_reset();

/*===========================================================================

FUNCTION QTI_DPL_PH_RECV_MSG()

DESCRIPTION

  This function
  - receives data from USB interface.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_dpl_ph_recv_msg
(
   int dpl_ph_fd
);

/*===========================================================================

FUNCTION QTI_DPL_DPM_PORT_OPEN()

DESCRIPTION
  Opens the USB tethered DPL port using DPM port open request

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int qti_dpl_dpm_port_open();

#ifndef FEATURE_DATA_ADPL_ONLY
/*===========================================================================

FUNCTION QTI_RMNET_DFS_INIT()

DESCRIPTION
  Initializes a DFS QMI client

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_dfs_init();

/*===========================================================================

FUNCTION QTI_RMNET_DFS_RELEASE()

DESCRIPTION
  Releases the DFS QMI client

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_dfs_release();
#endif //FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_DPM_RELEASE()

DESCRIPTION
  Releases a DPM QMI client

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_dpm_release();

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_NOT_IN_SERVICE()

DESCRIPTION
  Function invoked during SSR functionality, when modem is not in service.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void qti_rmnet_modem_not_in_service();

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_IN_SERVICE()

DESCRIPTION
  Function is used during SSR functionality, when modem is back in service.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

==========================================================================*/
void qti_rmnet_modem_in_service();

/*===========================================================================

FUNCTION DPM_ERROR_CB()

DESCRIPTION
  This function is invoked by dpm, to notify QTI when modem is not in
  service.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

=========================================================================*/
void dpm_erro_cb
(
  __attribute__((unused)) qmi_client_type user_handle,
  __attribute__((unused)) qmi_client_error_type error,
  __attribute__((unused)) void *err_cb_data
);

/*=========================================================================

FUNCTION QTI_DPL_DPM_PORT_CLOSE()

DESCRIPTION
  Closes the USB tethered SMD port using DPM port close request

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_dpl_dpm_port_close();


/*===========================================================================

FUNCTION DPM_SERVICE_AVAILABLE_CB()

DESCRIPTION
  This function is invoked by dpm, to notify QTI when service is available.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

=========================================================================*/
void dpm_service_available_cb
(
   __attribute__((unused)) qmi_client_type user_handle,
   __attribute__((unused)) qmi_idl_service_object_type service_obj,
   __attribute__((unused)) qmi_client_notify_event_type service_event,
   __attribute__((unused)) void *notify_cb_data
);


/*===========================================================================

FUNCTION DPM_CLIENT_INIT()

DESCRIPTION
  This function is invoked to initialize DPM Client

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

=========================================================================*/
int dpm_client_init();


/*===========================================================================

FUNCTION QTI_DPL_PH_GET_EP_INFO()

DESCRIPTION
  Gets the ph, IPA EP information from ph

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_dpl_ph_get_ep_info();
#ifndef FEATURE_DATA_ADPL_ONLY

/*===========================================================

FUNCTION QTI_RMNET_PROCESS_WDA_MESSAGE_TO_MODEM()

DESCRIPTION
  This function does EP translation for all the WDA messages to modem.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

============================================================*/
int qti_rmnet_process_wda_message_to_modem
(
  void        *msg,
  uint32_t    qmux_msg_len,
  int         message_id);

/*===========================================================

FUNCTION QTI_RMNET_PROCESS_WDS_MESSAGE_TO_MODEM()

DESCRIPTION
  This function does EP translation for all the WDS messages to modem

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

============================================================*/
int qti_rmnet_process_wds_message_to_modem
(
  void        *msg,
  uint32_t    qmux_msg_len,
  int         message_id
);


/*===========================================================

FUNCTION QTI_RMNET_PROCESS_DFS_MESSAGE_TO_MODEM()

DESCRIPTION
  This function does EP translation for all the DFS messages to modem.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

============================================================*/
int qti_rmnet_process_dfs_message_to_modem
(
  void        *msg,
  uint32_t    qmux_msg_len,
  int         message_id);


/*===========================================================

FUNCTION QTI_RMNET_PROCESS_WDS_MESSAGE_FROM_MODEM()

DESCRIPTION
  This function does EP translation for all the WDS messages from modem

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

============================================================*/
int qti_rmnet_process_wds_message_from_modem
(
  void        *msg,
  uint32_t    qmux_msg_len,
  int         message_id
);

/*===========================================================

FUNCTION QTI_RMNET_PROCESS_WDS_MESSAGE_FROM_MODEM()

DESCRIPTION
  This function does EP translation for all the DFS messages from modem

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

============================================================*/
int qti_rmnet_process_dfs_message_from_modem
(
  void        *msg,
  uint32_t    qmux_msg_len,
  int         message_id
);


/*===========================================================

FUNCTION QTI_RMNET_QMI_MESSAGE_DECODE()

DESCRIPTION
  This function decodes the QMI message and fills the message structure with the decoded
  values.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

===========================================================*/
int qti_rmnet_qmi_message_decode
(
  qmi_idl_service_object_type p_service,
  qmi_idl_type_of_message_type req_resp_ind,
  uint16_t message_id,
  void *msg,
  uint32_t qmux_msg_len,
  void *p_dst,
  uint32_t dst_len
);

/*===========================================================

FUNCTION QTI_RMNET_QMI_MESSAGE_ENCODE()

DESCRIPTION
  This function encodes the contents of the QMI message structure to a QMI message
  values.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS
  None

============================================================*/
int qti_rmnet_qmi_message_encode
(
  qmi_idl_service_object_type p_service,
  qmi_idl_type_of_message_type req_resp_ind,
  uint16_t message_id,
  void *p_src,
  uint32_t src_len,
  void *msg,
  uint32_t qmux_msg_len
);

/*===========================================================

FUNCTION QTI_RMNET_EP_INFO_TRANSLATION_REQ()

DESCRIPTION
  This function replaces the peripheral device end point ID and type with the modem iface EP ID
  and type values.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None

============================================================*/
void  qti_rmnet_ep_info_translation_req
(
  uint32_t *ep_id_ptr,
  data_ep_type_enum_v01 *ep_type_ptr
);


/*===========================================================

FUNCTION QTI_RMNET_EP_INFO_TRANSLATION_RESP()

DESCRIPTION
  This function replaces the modem iface end point ID and type with the peripheral device EP ID
  and type values.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None

============================================================*/
void  qti_rmnet_ep_info_translation_resp
(
  uint32_t *ep_id_ptr,
  data_ep_type_enum_v01 *ep_type_ptr
);
#endif //FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_FILE_OPEN()

DESCRIPTION

  This function
  - opens the device file which is used for interfacing with peripheral

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/


int qti_file_open
(
   char * rmnet_dev_file,
   int  * fd
);
#ifndef FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_CLEAR_FD()

DESCRIPTION

  This function
  - Removes the fd to the list of FD on which select call listens.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/

int qti_clear_fd
(
   qcmap_sk_fd_set_info_t *fd_set,
   int                     fd
);

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_RESET()

DESCRIPTION

  This function does the following on modem fd
  - Clears the fd from the fd set
  - Close the fd
  - Reset the fd

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/


int qti_rmnet_modem_reset();

/*===========================================================================

FUNCTION QTI_RMNET_QMI_INIT()

DESCRIPTION

  This function
  - initializes RmNet state for QMI message processing

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_qmi_init
(
   qti_rmnet_param       * rmnet_state
);

/*===========================================================================

FUNCTION QTI_RMNET_PH_SET_MODEM_STATE()

DESCRIPTION
  - sets the modem state on USB driver

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

==========================================================================*/

int qti_rmnet_ph_set_modem_state(int modem_state);
#endif //FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_QMI_WDA_SET_DATA_FORMAT()

DESCRIPTION
  Initializes a WDA QMI client and set data format

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

int qti_dpl_qmi_wda_set_data_format
(
   int aggr_size,
   int mhi_ep_type,
   int mhi_id
);

/*===========================================================================

FUNCTION QTI_IS_IFACE_UP()

DESCRIPTION
- checks if the interface is up

DEPENDENCIES
  None.

RETURN VALUE
  TRUE on success
  FALSE on failure

SIDE EFFECTS
  None

==========================================================================*/
int qti_is_iface_up
(
  char* data_iface
);

#endif
