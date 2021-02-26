/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                                INCLUDE FILES
===========================================================================*/
#if defined (FEATURE_QCRIL_QMI_CAT)

#include <string.h>
#include <framework/Log.h>
#include <framework/qcrili.h>
#include <telephony/ril.h>
#include "modules/android/qcril_android_event_context.h"
#include "modules/uim/qcril_gstk_srvc.h"
#include "modules/uim/GstkQmiCatRequestMsg.h"
#include "modules/uim/GstkQmiCatRequestSyncMsg.h"
#include "GstkQmiCatRequestCallback.h"
#include "qcril_gstk.h"
#include "qcril_scws.h"
extern "C"
{
  #include "util_timer.h"
}

#ifndef TAG
#define TAG "GSTK"
#endif /* TAG */

/*===========================================================================
                        INTERNAL DEFINITIONS AND TYPES
===========================================================================*/

#define QCRIL_GSTK_QMI_FREE_PTR(ptr)                                        \
  if (ptr != NULL)                                                          \
  {                                                                         \
    qcril_free(ptr);                                                        \
    ptr = NULL;                                                             \
  }                                                                         \

#define QCRIL_GSTK_QMI_MALLOC_AND_CPY_CMD(d_ptr,d_len,s_ptr,s_len)               \
  d_ptr = NULL;                                                                  \
  /* Destination buffer needs to hold string data hence *2 */                    \
  d_ptr = static_cast<uint8_t *>(qcril_malloc(2 * s_len + 1));                     \
  if(d_ptr != NULL){                                                             \
    memset(d_ptr,0x00,(2 * s_len + 1));                                          \
    qcril_gstk_qmi_bin_to_hexstring((uint8_t *)d_ptr, (uint8_t *)s_ptr, s_len);  \
    d_len = 2 * s_len + 1;                                                       \
  }                                                                              \

/* Hexadecimal digits, for binary <==> hexadecimal conversions */
#define QCRIL_GSTK_QMI_HEXDIGITS                "0123456789ABCDEF"

#define QCRIL_GSTK_QMI_ENVCMD_DATA_SIZE(n)      ((n) / 2)

#define QCRIL_GSTK_QMI_ENVCMD_COPY(pd, ps, n)                \
          qcril_gstk_qmi_hexstring_to_bin((uint8_t *)(pd), (uint8_t *)(ps), (n))

#define QCRIL_GSTK_QMI_ENVRSP_DATA_SIZE(n)      (2 * (n))

#define QCRIL_GSTK_QMI_ENVRSP_COPY(pd, ps, n)                \
          qcril_gstk_qmi_bin_to_hexstring((pd), (ps), (n))

#define QCRIL_GSTK_QMI_RIL_IS_READY_BMSK         0x00000001

/* Tags */
#define QCRIL_GSTK_QMI_PROACTIVE_SIM_COMMAND_TAG      0xD0
#define QCRIL_GSTK_QMI_IDLE_SCRN_AVAILABLE_EVT        0x05
#define QCRIL_GSTK_QMI_LANGUAGE_SELECTION_EVT         0x07
#define QCRIL_GSTK_QMI_BROWSER_TERMINATION_EVT        0x08
#define QCRIL_GSTK_QMI_HCI_CONNECTIVITY_EVT           0x13
#define QCRIL_GSTK_QMI_CONTACTLESS_STATE_REQ_EVT      0x16
#define QCRIL_GSTK_QMI_PROACTIVE_CMD_LEN_OFFSET       0x01
#define QCRIL_GSTK_QMI_COMMAND_DETAILS_TAG            0x01
#define QCRIL_GSTK_QMI_COMMAND_DETAILS_TLV_LENGTH     0x03

/* Maximum packet size for scws data available callback */
#define QCRIL_GSTK_QMI_SCWS_DATA_PKT_SIZE           500

/* SCWS OPEN CHANNEL retry defines */
#define QCRIL_GSTK_SCWS_OPEN_CH_RETRY_TIME          2
#define QCRIL_GSTK_SCWS_OPEN_CH_MAX_RETRIES         5

/* After ATEL restarts, QCRIL retrieves the cached proactive commands from modem
   (SETUP MENU, SETUP EVENT LIST, SETUP IDLE MODE TEXT )QCRIL modifies the
   Command number value to 0xFE, This is valid as per 102 223, but unlikely to
   be used. When QCRIL receives TERMINAL RESPONSE request from ATEL, it discards
   it if Command Command number value is 0xFE
 */
#define QCRIL_GSTK_CACHED_PCMD_CMD_NUM            0xFE

/* Bitmask for Set Event Report */
#define QMI_CAT_SET_EVENT_REPORT_DISPLAY_TEXT                     (0x00000001)
#define QMI_CAT_SET_EVENT_REPORT_GET_INKEY                        (0x00000002)
#define QMI_CAT_SET_EVENT_REPORT_GET_INPUT                        (0x00000004)
#define QMI_CAT_SET_EVENT_REPORT_SETUP_MENU                       (0x00000008)
#define QMI_CAT_SET_EVENT_REPORT_SELECT_ITEM                      (0x00000010)
#define QMI_CAT_SET_EVENT_REPORT_SEND_SMS                         (0x00000020)
#define QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_USER_ACTIVITY        (0x00000040)
#define QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_IDLE_SCREEN_NOTIFY   (0x00000080)
#define QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_LANGUAGE_SEL_NOTIFY  (0x00000100)
#define QMI_CAT_SET_EVENT_REPORT_SETUP_IDLE_MODE_TEXT             (0x00000200)
#define QMI_CAT_SET_EVENT_REPORT_LANGUAGE_NOTIFICATION            (0x00000400)
#define QMI_CAT_SET_EVENT_REPORT_REFRESH_ALPHA                    (0x00000800)
#define QMI_CAT_SET_EVENT_REPORT_END_PROACTIVE_SESSION            (0x00001000)
#define QMI_CAT_SET_EVENT_REPORT_PLAY_TONE                        (0x00002000)
#define QMI_CAT_SET_EVENT_REPORT_SETUP_CALL                       (0x00004000)
#define QMI_CAT_SET_EVENT_REPORT_SEND_DTMF                        (0x00008000)
#define QMI_CAT_SET_EVENT_REPORT_LAUNCH_BROWSER                   (0x00010000)
#define QMI_CAT_SET_EVENT_REPORT_SEND_SS                          (0x00020000)
#define QMI_CAT_SET_EVENT_REPORT_SEND_USSD                        (0x00040000)
#define QMI_CAT_SET_EVENT_REPORT_PROVIDE_LOCAL_INFO_LANG          (0x00080000)
#define QMI_CAT_SET_EVENT_REPORT_BIP                              (0x00100000)
#define QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_BROWSER_TERM         (0x00200000)
#define QMI_CAT_SET_EVENT_REPORT_PROVIDE_LOCAL_INFO_TIME          (0x00400000)
#define QMI_CAT_SET_EVENT_REPORT_SCWS                             (0x00800000)
#define QMI_CAT_SET_EVENT_REPORT_ACTIVATE                         (0x01000000)
#define QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_HCI_CONN             (0x02000000)
#define QMI_CAT_SET_EVENT_REPORT_CONTACTLESS_SUPPORT              (0x08000000)

/*===========================================================================
                               LOCAL VARIABLES
===========================================================================*/

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_GSTK_QMI_UNSOL_INFO_TYPE

   DESCRIPTION:
     Structure used to store info that is part of the unsol indication sent
     to Android Telephony
-------------------------------------------------------------------------------*/
typedef struct
{
  int                              ril_unsol_type;
  qcril_unsol_resp_params_type     ril_unsol_resp;
} qcril_gstk_qmi_unsol_info_type;

/* -----------------------------------------------------------------------------
   STRUCT:      QCRIL_GSTK_CMD_ID_QMI_REQ_ID_MAP_TYPE

   DESCRIPTION:
     Structure used to map QCRIL_GSTK_QMI_CMD_STK_XXX to cat_cached_command_id_enum
-------------------------------------------------------------------------------*/
typedef struct
{
  uint32                            gstk_cmd_id;
  qmi_cat_cached_command_id_type    qmi_req_cmd_id;
} qcril_gstk_cmd_id_qmi_req_id_map_type;

typedef struct
{
  uint32                              timer_id;
  uint8                               retry_count;
  struct timeval                      scws_open_ch_timer;
  qmi_cat_scws_open_ch_ind_msg       *scws_open_ch_msg_ptr;
} qcril_gstk_scws_open_ch_timer;

typedef struct
{
  uint32                    uim_ref_id;   /*!< UIM reference id                 */
  uint32                    command_type; /*!< Proactive command type           */
  uint32                    length;       /*!< Proactive command length         */
  uint8                     *data;        /*!< Proactive command ASCII hex data */
} qcril_gstk_raw_proactive_command_type;

typedef struct
{
  int                                    call_timeout;    /*!< call timeout (ms) for SETUP_CALL        */
  qcril_gstk_raw_proactive_command_type *cmd_ptr;         /*!< Pending proactive cmd                   */
  size_t                                 cmd_len;         /*!< Size of command data pointed by cmd_ptr */
  uint32                                 flags;           /*!< Flags of events having occured          */
} qcril_gstk_qmi_command_info_type;

static qcril_gstk_qmi_command_info_type    gstk_command_info;
static qcril_gstk_scws_open_ch_timer       gstk_scws_open_ch_info;
static bool                                scws_err_sent;

static void qcril_gstk_qmi_scws_open_channel
(
  qcril_scws_slot_enum_type         slot,
  qmi_cat_scws_open_ch_ind_msg    * open_ch_ind_ptr
);

/*===========================================================================
                             INTERNAL FUNCTIONS
===========================================================================*/
/*=========================================================================

  FUNCTION:  qcril_gstk_get_ril_token

===========================================================================*/
/*!
    @brief
    Get token.

    @return
    int
*/
/*=========================================================================*/
static int qcril_gstk_get_ril_token
(
  RIL_Token token
)
{
  qcril_event_context  * ctx   = (qcril_event_context  *)token;
  if (ctx != nullptr && ctx->t != nullptr)
  {
    RIL_Token t = ctx->t;
    return (*(int *)t);
  }
  return 0;
} /* qcril_gstk_get_ril_token */


/*=========================================================================

  FUNCTION:  qcril_gstk_send_qmi_sync_msg

===========================================================================*/
/*!
    @brief
    QMI CAT send qmi sync message.

    @return
    RIL_Errno
*/
/*=========================================================================*/
static RIL_Errno qcril_gstk_send_qmi_sync_msg
(
  qcril_gstk_request_type                          msg_id,
  std::shared_ptr<qcril_gstk_request_data_type>    dataPtr,
  std::shared_ptr<qmi_cat_sync_rsp_data_type>      rsp_data_ptr
)
{
  std::shared_ptr<GstkQmiCatRequestSyncMsg> req_msg_ptr    = nullptr;
  std::shared_ptr<int>                      respPtr        = nullptr;

  if (rsp_data_ptr == nullptr || rsp_data_ptr == nullptr)
  {
    return RIL_E_INTERNAL_ERR;
  }

  req_msg_ptr = std::make_shared<GstkQmiCatRequestSyncMsg>(msg_id,
                                                           dataPtr,
                                                           rsp_data_ptr);
  if (req_msg_ptr == nullptr)
  {
    return RIL_E_INTERNAL_ERR;
  }

  if (req_msg_ptr->dispatchSync(respPtr) == Message::Callback::Status::SUCCESS)
  {
    if (respPtr == nullptr)
    {
      return RIL_E_INTERNAL_ERR;
    }
  }
  else
  {
    return RIL_E_INTERNAL_ERR;
  }

  if (*respPtr != 0 || rsp_data_ptr->qmi_err_code != 0)
  {
    QCRIL_LOG_ERROR("qmi cat request failed sys_err: %d, qmi_err: %d",
                         *respPtr, rsp_data_ptr->qmi_err_code);
    return RIL_E_MODEM_ERR;
  }
  else
  {
    return RIL_E_SUCCESS;
  }
} /* qcril_gstk_send_qmi_sync_msg */


/*=========================================================================

  FUNCTION:  qcril_gstk_send_qmi_async_msg

===========================================================================*/
/*!
    @brief
    QMI UIM send qmi async message.

    @return
    RIL_Errno
*/
/*=========================================================================*/

static RIL_Errno qcril_gstk_send_qmi_async_msg
(
  qcril_gstk_request_type                            request_type,
  std::shared_ptr<qcril_gstk_request_data_type>      req_ptr,
  std::shared_ptr<qcril_gstk_original_request_type>  orig_ptr
)
{
  std::shared_ptr<GstkQmiCatRequestMsg>     req_msg_ptr    = nullptr;
  GstkQmiCatRequestCallback                 gstkReqcb("QMI-CAT REQUEST");

  req_msg_ptr = std::make_shared<GstkQmiCatRequestMsg>(request_type,
                                                       req_ptr,
                                                       orig_ptr,
                                                       &gstkReqcb);
  if (req_msg_ptr == nullptr)
  {
    return RIL_E_NO_MEMORY;
  }

  req_msg_ptr->dispatch();

  return RIL_E_SUCCESS;
} /* qcril_gstk_send_qmi_async_msg */


/*=========================================================================

  FUNCTION:  qcril_gstk_get_num_slots

===========================================================================*/
/*!
    @brief
    Returns the number of slots supported by QCRIL.

    @return
    Number of slots.
*/
/*=========================================================================*/
static uint8_t qcril_gstk_get_num_slots
(
  void
)
{
  if (ril_to_uim_is_tsts_enabled())
  {
    return 3;
  }
  else if (ril_to_uim_is_dsds_enabled())
  {
    return 2;
  }

  return 1;
} /* qcril_gstk_get_num_slots */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_convert_instance_to_slot_index
 *   ======================================================================*/
/*!
 *   @brief
 *     Converts an instance id to the respective slot index.
 *
 *   @return
 *     Slot index
 *                                                                         */
/*=========================================================================*/
static uint8_t qcril_gstk_qmi_convert_instance_to_slot_index
(
  qcril_instance_id_e_type      instance_id
)
{
  uint8 slot_index = QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE;

  /* Convert instance id to slot index */
  switch (instance_id)
  {
    case QCRIL_DEFAULT_INSTANCE_ID:
      slot_index = 0;
      break;
    case QCRIL_SECOND_INSTANCE_ID:
      if (ril_to_uim_is_dsds_enabled() || ril_to_uim_is_tsts_enabled())
      {
        slot_index = 1;
      }
      break;
    case QCRIL_THIRD_INSTANCE_ID:
      if (ril_to_uim_is_tsts_enabled())
      {
        slot_index = 2;
      }
      break;
    default:
      QCRIL_LOG_ERROR("Invalid instance id for conversion: 0x%x \n", instance_id);
      break;
  }

  QCRIL_LOG_DEBUG( "Slot index found: 0x%x\n", slot_index );
  return slot_index;
} /* qcril_gstk_qmi_convert_instance_to_slot_index */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_convert_slot_index_to_slot_type
 *   ======================================================================*/
/*!
 *   @brief
 *     Converts a slot index to the respective slot id.
 *
 *   @return
 *     Nothing
 *                                                                         */
/*=========================================================================*/
static qmi_cat_slot_type qcril_gstk_qmi_convert_slot_index_to_slot_type
(
  uint8_t     slot_index
)
{
  qmi_cat_slot_type slot_type = QMI_CAT_SLOT1;

  /* Convert instance id to slot index */
  switch (slot_index)
  {
    case 0:
      slot_type = QMI_CAT_SLOT1;
      break;
    case 1:
      slot_type = QMI_CAT_SLOT2;
      break;
    case 2:
      slot_type = QMI_CAT_SLOT3;
      break;
    default:
      QCRIL_LOG_ERROR("Invalid slot index for conversion: 0x%x \n", slot_index);
      break;
  }

  QCRIL_LOG_DEBUG( "Slot id found: 0x%x\n", slot_type );
  return slot_type;
} /* qcril_gstk_qmi_convert_slot_index_to_slot_type */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_convert_scws_slot_id
 *   ======================================================================*/
/*!
 *   @brief
 *     Converts the slot id from scws enum type to cat enum type.
 *
 *   @return
 *     Converted cat enum type
 *                                                                         */
/*=========================================================================*/
static qmi_cat_slot_type qcril_gstk_qmi_convert_scws_slot_id
(
  qcril_scws_slot_enum_type       scws_slot_id_type
)
{
  qmi_cat_slot_type cat_slot_type = QMI_CAT_SLOT1;

  /* Convert scws type to cat type */
  switch (scws_slot_id_type)
  {
    case QCRIL_SCWS_SLOT_1:
      cat_slot_type = QMI_CAT_SLOT1;
      break;
    case QCRIL_SCWS_SLOT_2:
      cat_slot_type = QMI_CAT_SLOT2;
      break;
    case QCRIL_SCWS_SLOT_3:
      cat_slot_type = QMI_CAT_SLOT3;
      break;
    default:
      QCRIL_LOG_ERROR("Invalid input slot_id: %d \n", scws_slot_id_type);
      break;
  }
  return cat_slot_type;
} /* qcril_gstk_qmi_convert_scws_slot_id */


/*===========================================================================
  FUNCTION:  qcril_gstk_bin_to_hexstring
===========================================================================*/
/*!
  @brief
    Convert an array of binary bytes into a hex string.

  @param[out] dest_ptr  Pointer to destination area. Must not be NULL.
  @param[in]  src_ptr   Pointer to source area. Must not be NULL.
  @param[in]  length    Length (in bytes) of the data pointed to by src_ptr.
                        The length of the data output via dest_ptr will be
                        twice this.

  @return
    Nothing
 */
/*=========================================================================*/
static void qcril_gstk_qmi_bin_to_hexstring
(
 uint8_t       *dest_ptr,
 const uint8_t *src_ptr,
 uint32_t       length
)
{
  QCRIL_LOG_INFO("qcril_gstk_bin_to_hexstring, length %d\n", (int) length);

  /* Sanity check */
  if ((dest_ptr == NULL) || (src_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  while (length--)
  {
    *dest_ptr++ = QCRIL_GSTK_QMI_HEXDIGITS[(*src_ptr >> 4) & 0x0F];
    *dest_ptr++ = QCRIL_GSTK_QMI_HEXDIGITS[(*src_ptr >> 0) & 0x0F];
    ++src_ptr;
  }
} /* qcril_gstk_qmi_bin_to_hexstring */


/*===========================================================================
  FUNCTION:  qcril_gstk_hexdigit_to_bin
===========================================================================*/
/*!
  @brief
    Convert a hex digit to a binary value.

  @param[in] src Source hex digit.

  @return
    The converted binary value in the range 0 to 15
 */
/*=========================================================================*/
static uint8_t qcril_gstk_hexdigit_to_bin
(
 uint8_t src
)
{
  QCRIL_LOG_INFO("qcril_gstk_hexdigit_to_bin digit = 0x%02X\n", src);

  switch (src)
  {
    case 0x00: case 0x01: case 0x02: case 0x03:
    case 0x04: case 0x05: case 0x06: case 0x07:
    case 0x08: case 0x09: case 0x0A: case 0x0B:
    case 0x0C: case 0x0D: case 0x0E: case 0x0F:
      return src;
      break;

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      return src - '0';
      break;

    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      return (src - 'A') + 10;
      break;

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      return (src - 'a') + 10;
      break;
  }
  return 0;
} /* qcril_gstk_hexdigit_to_bin */


/*===========================================================================
  FUNCTION:  qcril_gstk_qmi_hexstring_to_bin
===========================================================================*/
/*!
  @brief
    Convert a hex string to an array of binary bytes.

  @param[out] dest_ptr  Pointer to destination area. Must not be NULL.
  @param[in]  src_ptr   Pointer to source area. Must not be NULL.
  @param[in]  length    Length (in bytes) of the data pointed to by src_ptr.
                        The length of the data output via dest_ptr will be
                        half this.

  @return
    Nothing
 */
/*=========================================================================*/
static void qcril_gstk_qmi_hexstring_to_bin
(
 uint8_t       *dest_ptr,
 const uint8_t *src_ptr,
 uint32_t       length
)
{
  QCRIL_LOG_INFO("qcril_gstk_hexstring_to_bin, length %d\n", (int) length);

  /* Sanity check */
  if ((dest_ptr == NULL) || (src_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  for (length /= 2; length; --length)
  {
    uint8_t temp;

    temp  = qcril_gstk_hexdigit_to_bin(*src_ptr++) << 4;
    temp |= qcril_gstk_hexdigit_to_bin(*src_ptr++) << 0;
    *dest_ptr++ = temp;
  }
} /* qcril_gstk_qmi_hexstring_to_bin */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_send_unsol_resp
 *   ======================================================================*/
/*!
 *   @brief
 *     Utility function to send the unsol response to Android Telephony based
 *     on the passed payload.
 *
 *   @return
 *     None
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_send_unsol_resp
(
  qcril_instance_id_e_type         instance_id,
  int                              ril_unsol_type,
  void                           * resp_pkt_ptr,
  size_t                           resp_pkt_len
)
{
  qcril_unsol_resp_params_type     ril_unsol_resp;

  /* Updated header & send the unsol response. Note: it is OK to send
     a NULL packet with 0 length for certain UNSOL types */
  qcril_default_unsol_resp_params(instance_id, ril_unsol_type, &ril_unsol_resp);
  ril_unsol_resp.resp_pkt = resp_pkt_ptr;
  ril_unsol_resp.resp_len = resp_pkt_len;

  QCRIL_LOG_INFO("Sending unsol_resp, resp_len: 0x%zx", ril_unsol_resp.resp_len);
  qcril_send_unsol_response(&ril_unsol_resp);
} /* qcril_gstk_qmi_send_unsol_resp */


/*=========================================================================

  FUNCTION:  qcril_gstk_qmi_filter_setup_events

===========================================================================*/
/*!
    @brief
    Removed events not supported in Android UI from the set up event list
    command

    @return
    pointer to new apdu with un-supported events removed.
*/
/*=========================================================================*/
static uint8_t * qcril_gstk_qmi_filter_setup_events
(
  uint8_t   *apdu_ptr,
  uint16_t  *apdu_len_ptr
)
{
  uint16_t    offset                =  0;
  uint16_t    new_offset            =  0;
  uint16_t    num_of_event_offset   =  0;
  uint8_t     len_of_length_field   =  0;
  uint8_t    *new_apdu_ptr          =  NULL;
  uint16_t    total_length          =  0;
  uint8_t     tag                   =  0;
  uint16_t    tag_len               =  0;
  uint8_t     i                     =  0;
  uint8_t     delete_cnt            =  0;
  boolean   evt_list_tag          =  FALSE;


  QCRIL_LOG_INFO( "Entering %s\n", __FUNCTION__);
  if(apdu_ptr == NULL || apdu_len_ptr == NULL || *apdu_len_ptr <= 2)
  {
    QCRIL_LOG_ERROR( "%s\n", "Invalid input APDU Param");
    return new_apdu_ptr;
  }

  /* Make check for proactive command APDU tag the first byte */
  if(apdu_ptr[offset] != QCRIL_GSTK_QMI_PROACTIVE_SIM_COMMAND_TAG)
  {
    QCRIL_LOG_ERROR( "apdu_ptr[%d] != QCRIL_GSTK_QMI_PROACTIVE_SIM_COMMAND_TAG\n", offset);
    return new_apdu_ptr;
  }

  offset++;  /* Points to length field */
  new_offset++;
  /*
  ** BER-TLV LENGTH
  ** This length field can either be 0x00-0x7F or
  ** 0x81 if the 2nd byte is used
  */
  if(apdu_ptr[offset] <= 0x7F)
  {
    len_of_length_field = 1;
  }
  else if(apdu_ptr[offset] == 0x81)
  {
    len_of_length_field = 2;
  }
  else
  {
    QCRIL_LOG_ERROR( "Incorrect length value apdu[%d]=0x%x", offset,apdu_ptr[offset]);
    return new_apdu_ptr;
  }
  /* offset
  ** 0        Proactive SIM Command Tag
  ** 1-2      total length
  ** +1       Command Details tag  0x81
  ** +1       Command details len = 3
  ** +1       Command number
  ** +1       Command Type
  ** +1       Command Qualifier
  */
  if(len_of_length_field == 1)
  {
    total_length = apdu_ptr[offset];
  }
  else
  {
    total_length = apdu_ptr[offset+1];
  }

  if(total_length != (*apdu_len_ptr - 1 - len_of_length_field))
  {
    QCRIL_LOG_ERROR( "Malformed APDU apdu length %d apdu[%d]=0x%x",
                     *apdu_len_ptr,offset,apdu_ptr[offset]);
    return new_apdu_ptr;
  }

  new_apdu_ptr = (uint8_t*)qcril_malloc(*apdu_len_ptr);
  if(new_apdu_ptr == NULL)
  {
    QCRIL_LOG_ERROR( "%s \n", "qcril_malloc error new_apdu_ptr\n" );
    return new_apdu_ptr;
  }
  memset(new_apdu_ptr, 0x00,*apdu_len_ptr);

  /* copy Proactive SIM Command Tag & len_of_length_field*/
  memcpy((void*)new_apdu_ptr, (void*)apdu_ptr, (1+ len_of_length_field));
  offset += len_of_length_field;
  new_offset += len_of_length_field;

  while(offset < *apdu_len_ptr)
  {
    /* get the tag len and look for next tag*/
    tag = apdu_ptr[offset];
    new_apdu_ptr[new_offset++] = apdu_ptr[offset++];

    /* check if apdu tag contains a tag len*/
    if(offset >= *apdu_len_ptr)
    {
      QCRIL_LOG_ERROR( "Malformed APDU tag: apdu length %d, offset %d\n",
                   *apdu_len_ptr, offset);
      QCRIL_GSTK_QMI_FREE_PTR(new_apdu_ptr);
      return NULL;
    }

    tag_len = apdu_ptr[offset];
    new_apdu_ptr[new_offset++] = apdu_ptr[offset++];

    /* check for valid tag_len*/
    if(tag_len + offset > *apdu_len_ptr ||
       tag_len + offset > QMI_CAT_RAW_PROACTIVE_CMD_MAX_LENGTH)
    {
      QCRIL_LOG_ERROR( "Incorrect APDU: apdu length %d, offset %d, tag_len %d\n",
                       *apdu_len_ptr, offset, tag_len);
      QCRIL_GSTK_QMI_FREE_PTR(new_apdu_ptr);
      return NULL;
    }

    if(tag == 0x99 || tag == 0x19)
    {
      /* check for evt_list_tag in case tag is seen more than once */
      if (evt_list_tag)
      {
        QCRIL_LOG_ERROR( "duplicat evt_list_tag: evt_list_tag %d\n",
                         evt_list_tag);
        QCRIL_GSTK_QMI_FREE_PTR(new_apdu_ptr);
        return NULL;
      }

      evt_list_tag = TRUE;
      num_of_event_offset = new_offset - 1;

      for(i=0; i < tag_len ; i++)
      {
        /* filter out the non-UI events from set up event list APDU */
        switch(apdu_ptr[offset])
        {
          case QCRIL_GSTK_QMI_IDLE_SCRN_AVAILABLE_EVT:
          case QCRIL_GSTK_QMI_LANGUAGE_SELECTION_EVT:
          case QCRIL_GSTK_QMI_HCI_CONNECTIVITY_EVT:
#ifdef FEATURE_QCRIL_GSTK_CONTACTLESS
          case QCRIL_GSTK_QMI_CONTACTLESS_STATE_REQ_EVT:
#endif /* FEATURE_QCRIL_GSTK_CONTACTLESS */
            QCRIL_LOG_INFO("Setup Event 0x%x added \n", apdu_ptr[offset]);
            new_apdu_ptr[new_offset++] = apdu_ptr[offset++];
            break;
          default:
            QCRIL_LOG_INFO("Setup event 0x%x stripped \n", apdu_ptr[offset]);
            offset++;
            delete_cnt++;
            break;
        }
      }
      if(delete_cnt <= new_apdu_ptr[num_of_event_offset])
      {
        /* set correct value of number of events*/
        new_apdu_ptr[num_of_event_offset] -= delete_cnt;
      }
      else
      {
        QCRIL_LOG_ERROR( "Incorrect APDU: delete_cnt (0x%x) > new_apdu_ptr[%d](0x%x)\n",
                          delete_cnt, num_of_event_offset,
                          new_apdu_ptr[num_of_event_offset]);
        QCRIL_GSTK_QMI_FREE_PTR(new_apdu_ptr);
        return NULL;
      }
    }
    else
    {
      /* copy tag field*/
      memcpy((void*)&new_apdu_ptr[new_offset], (void*)&apdu_ptr[offset], tag_len);
      offset += tag_len;
      new_offset += tag_len;
    }
  }

  /* set correct value for length of proactive command */
  if(len_of_length_field == 1)
  {
    new_apdu_ptr[QCRIL_GSTK_QMI_PROACTIVE_CMD_LEN_OFFSET] -= delete_cnt;
  }
  else if(len_of_length_field == 2)
  {
    new_apdu_ptr[QCRIL_GSTK_QMI_PROACTIVE_CMD_LEN_OFFSET+1] -= delete_cnt;
  }

  /* set correct value for overall apdu len*/
  *apdu_len_ptr -= delete_cnt;
  QCRIL_LOG_INFO("apdu_len_ptr 0x%x after filtering\n", *apdu_len_ptr);

  return new_apdu_ptr;
} /* qcril_gstk_qmi_filter_setup_events */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_process_event_report_ind
 *   ======================================================================*/
/*!
 *   @brief
 *     Processes the event report indication by performing the necessary data
 *     parsing, packaging the data and sends the UNSOL response to Android
 *     Telephony. It also sends event confirmation to the modem for certain
 *     cases where there is no Android UI support.
 *
 *   @return
 *     Boolean that specifies success or failure
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_process_raw_proactive_cmd
(
  void
)
{
  qcril_instance_id_e_type  instance_id                    =
                                         qmi_ril_get_process_instance_id();
  int                       ril_unsol_type                 = 0;
  int                       ril_additional_unsol_type      = 0;
  void                    * ril_additional_unsol_data_ptr  = NULL;
  size_t                    ril_additional_unsol_data_size = 0;
  boolean                   send_evt_conf                  = FALSE;

  QCRIL_LOG_INFO("qcril_gstk_qmi_process_raw_proactive_cmd\n");

  if (gstk_command_info.cmd_ptr != NULL &&
      gstk_command_info.cmd_len != 0)
  {
    QCRIL_LOG_INFO("Proactive command(0x%x)\n",
       gstk_command_info.cmd_ptr->command_type);
    switch (gstk_command_info.cmd_ptr->command_type)
    {
      /* Commands which are processed fully within Android */
      case QCRIL_GSTK_QMI_CMD_STK_SET_UP_EVENT_LIST:
      case QCRIL_GSTK_QMI_CMD_STK_LAUNCH_BROWSER:
      case QCRIL_GSTK_QMI_CMD_STK_PLAY_TONE:
      case QCRIL_GSTK_QMI_CMD_STK_DISPLAY_TEXT:
      case QCRIL_GSTK_QMI_CMD_STK_GET_INKEY:
      case QCRIL_GSTK_QMI_CMD_STK_GET_INPUT:
      case QCRIL_GSTK_QMI_CMD_STK_SELECT_ITEM:
      case QCRIL_GSTK_QMI_CMD_STK_SET_UP_MENU:
      case QCRIL_GSTK_QMI_CMD_STK_SET_UP_IDLE_MODE_TEXT:
      case QCRIL_GSTK_QMI_CMD_STK_PROVIDE_LOCAL_INFO:
      case QCRIL_GSTK_QMI_CMD_STK_LANG_NOTIFICATION:
      case QCRIL_GSTK_QMI_CMD_STK_ACTIVATE:
        ril_unsol_type = RIL_UNSOL_STK_PROACTIVE_COMMAND;
        break;

      /* Commands for which Android handles the UI indication, */
      /* and the modem does the real work in parallel          */
      case QCRIL_GSTK_QMI_CMD_STK_SEND_SS:
      case QCRIL_GSTK_QMI_CMD_STK_USSD:
      case QCRIL_GSTK_QMI_CMD_STK_SEND_SHORT_MESSAGE:
      case QCRIL_GSTK_QMI_CMD_STK_SEND_DTMF:
      case QCRIL_GSTK_QMI_CMD_STK_CLOSE_CHANNEL:
      case QCRIL_GSTK_QMI_CMD_STK_RECEIVE_DATA:
      case QCRIL_GSTK_QMI_CMD_STK_SEND_DATA:
      case QCRIL_GSTK_QMI_CMD_STK_REFRESH:
        send_evt_conf = TRUE;
        ril_unsol_type = RIL_UNSOL_STK_EVENT_NOTIFY;
        break;

      /* Commands for which Android handles the UI confirmation, */
      /* and the modem does the real work if the user accepts    */
      case QCRIL_GSTK_QMI_CMD_STK_OPEN_CHANNEL:
        ril_unsol_type = RIL_UNSOL_STK_EVENT_NOTIFY;
        /* additional_unsol_type is set to STK_CALL_SETUP till a new unsol type
           is avail for BIP*/
        ril_additional_unsol_type = RIL_UNSOL_STK_CALL_SETUP;
        ril_additional_unsol_data_ptr = &gstk_command_info.call_timeout;
        ril_additional_unsol_data_size = sizeof(gstk_command_info.call_timeout);
        break;

      /* Commands for which Android handles the UI confirmation, */
      /* and the modem does the real work if the user accepts    */
      case QCRIL_GSTK_QMI_CMD_STK_SET_UP_CALL:
        ril_unsol_type = RIL_UNSOL_STK_EVENT_NOTIFY;
        ril_additional_unsol_type = RIL_UNSOL_STK_CALL_SETUP;
        ril_additional_unsol_data_ptr = &gstk_command_info.call_timeout;
        ril_additional_unsol_data_size = sizeof(gstk_command_info.call_timeout);
        break;

      /* Pseudo command to indicate the end of the proactive command session */
      case QCRIL_GSTK_QMI_CMD_STK_END_OF_PROACTIVE_SES:
        ril_unsol_type = RIL_UNSOL_STK_SESSION_END;
        break;

      /* Commands which are handled entirely by the modem */
      /* - we shouldn't see any of these                  */
      case QCRIL_GSTK_QMI_CMD_STK_MORE_TIME:
      case QCRIL_GSTK_QMI_CMD_STK_POLL_INTERVAL:
      case QCRIL_GSTK_QMI_CMD_STK_POLLING_OFF:
      case QCRIL_GSTK_QMI_CMD_STK_TIMER_MANAGEMENT:
        break;

      /* Valid command types that are not supported (yet) */
      case QCRIL_GSTK_QMI_CMD_STK_GET_CHANNEL_STATUS:
      case QCRIL_GSTK_QMI_CMD_STK_PERFORM_CARD_APDU:
      case QCRIL_GSTK_QMI_CMD_STK_POWER_ON_CARD:
      case QCRIL_GSTK_QMI_CMD_STK_POWER_OFF_CARD:
      case QCRIL_GSTK_QMI_CMD_STK_GET_READER_STATUS:
      case QCRIL_GSTK_QMI_CMD_STK_RUN_AT_COMMAND:
        break;

      /* Don't recognise the command type at all */
      default:
        QCRIL_LOG_ERROR("Command type not recognised (0x%x)\n",
                        gstk_command_info.cmd_ptr->command_type);
        break;
    }
  }
  else
  {
    QCRIL_LOG_DEBUG("No pending proactive command");
  }

  /* Send one or more notifications if required */
  if (ril_unsol_type)
  {
    QCRIL_LOG_INFO("Pcmd data %s, length :%d \n",
      gstk_command_info.cmd_ptr->data, gstk_command_info.cmd_ptr->length);

    qcril_gstk_qmi_send_unsol_resp(instance_id,
                                   ril_unsol_type,
                                   gstk_command_info.cmd_ptr->data,
                                   (size_t)gstk_command_info.cmd_ptr->length);

    /* If the command requires an additional notification then send it now */
    if (ril_additional_unsol_type)
    {
      QCRIL_LOG_INFO("%s", "Additional RIL_ADDITONAL_UNSOL_TYPE will be sent\n");
      qcril_gstk_qmi_send_unsol_resp(instance_id,
                                     ril_additional_unsol_type,
                                     ril_additional_unsol_data_ptr,
                                     ril_additional_unsol_data_size);
    }
  }

  if (send_evt_conf)
  {
    auto    rsp_data_ptr = std::shared_ptr<qmi_cat_sync_rsp_data_type>(new qmi_cat_sync_rsp_data_type{});
    auto    msg_ptr      = std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});
    uint8_t slot         = qcril_gstk_qmi_convert_instance_to_slot_index(instance_id);

    if (msg_ptr ==nullptr || rsp_data_ptr == nullptr)
    {
      return;
    }
    msg_ptr->evt_cnf_data.slot     = qcril_gstk_qmi_convert_slot_index_to_slot_type(slot);
    msg_ptr->evt_cnf_data.evt_data = QMI_CAT_DISPLAY;
    (void) qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUEST_SEND_EVENT_CONFIRMATION,
                                        msg_ptr,
                                        rsp_data_ptr);
  }
} /* qcril_gstk_qmi_process_raw_proactive_cmd */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_process_event_report_ind
 *   ======================================================================*/
/*!
 *   @brief
 *     Processes the event report indication by performing the necessary data
 *     parsing, packaging the data and sends the UNSOL response to Android
 *     Telephony. It also sends event confirmation to the modem for certain
 *     cases where there is no Android UI support.
 *
 *   @return
 *     Boolean that specifies success or failure
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_process_event_report_ind
(
  qmi_cat_evt_rpt_ind_msg   * evt_rpt_ptr
)
{
  QCRIL_LOG_INFO("%s", __FUNCTION__);

  if (evt_rpt_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid data");
    return;
  }

  if (gstk_command_info.cmd_ptr != nullptr)
  {
    QCRIL_GSTK_QMI_FREE_PTR(gstk_command_info.cmd_ptr->data);
    QCRIL_GSTK_QMI_FREE_PTR(gstk_command_info.cmd_ptr);
  }
  gstk_command_info.cmd_ptr = 
    static_cast<qcril_gstk_raw_proactive_command_type *>(qcril_malloc(sizeof(qcril_gstk_raw_proactive_command_type)));
  if (gstk_command_info.cmd_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Failed to allocate memory.");
    return;
  }

  memset(gstk_command_info.cmd_ptr, 0x00, sizeof(qcril_gstk_raw_proactive_command_type));
  if (evt_rpt_ptr->is_end_session == FALSE)
  {
    uint16_t   apdu_length  = evt_rpt_ptr->cmd_len;
    uint8_t  * temp_buf_ptr = evt_rpt_ptr->cmd_data;

    if (evt_rpt_ptr->cmd_type == QCRIL_GSTK_QMI_CMD_STK_SET_UP_EVENT_LIST)
    {
      temp_buf_ptr = qcril_gstk_qmi_filter_setup_events(
                                evt_rpt_ptr->cmd_data,
                                &apdu_length);
    }
    if(temp_buf_ptr == nullptr)
    {
      QCRIL_LOG_ERROR("%s", "Malformed APDU.");
      return;
    }

    QCRIL_GSTK_QMI_MALLOC_AND_CPY_CMD(gstk_command_info.cmd_ptr->data,
                                      gstk_command_info.cmd_ptr->length,
                                      temp_buf_ptr,
                                      apdu_length);
    if (gstk_command_info.cmd_ptr->data == nullptr ||
        gstk_command_info.cmd_ptr->length == 0)
    {
      QCRIL_LOG_ERROR("Failed to save the proactive command.");
      return;
    }
  }
  gstk_command_info.cmd_len =
    sizeof(qcril_gstk_raw_proactive_command_type) + gstk_command_info.cmd_ptr->length;
  gstk_command_info.cmd_ptr->command_type = evt_rpt_ptr->cmd_type;
  gstk_command_info.cmd_ptr->uim_ref_id   = evt_rpt_ptr->unique_ref_id;

  QCRIL_LOG_INFO("Command info flag: 0x%x", gstk_command_info.flags);
  if (gstk_command_info.flags & QCRIL_GSTK_QMI_RIL_IS_READY_BMSK)
  {
    qcril_gstk_qmi_process_raw_proactive_cmd();
  }
  else
  {
    QCRIL_LOG_DEBUG("Android FW not ready");
  }
} /* qcril_gstk_qmi_process_event_report_ind */


/*===========================================================================
  FUNCTION: qcril_gstk_scws_open_ch_timer_cb
===========================================================================*/
/*!
    @brief
    Timer callback function responsible to retry SCWS OPEN Channel request.
    @return
    None
*/
/*=========================================================================*/
static void qcril_gstk_scws_open_ch_timer_cb
(
  void *param_ptr
)
{
  auto ind_ptr =
    std::shared_ptr<qcril_gstk_qmi_ind_params_type>(new qcril_gstk_qmi_ind_params_type{});
  auto ind_msg_ptr =
    std::make_shared<GstkQmiCatIndicationMsg>(QMI_CAT_SCWS_OPEN_CHANNEL_IND, ind_ptr);

  (void) param_ptr;

  if((gstk_scws_open_ch_info.timer_id != 0 ) &&
     (gstk_scws_open_ch_info.scws_open_ch_msg_ptr != nullptr) &&
     ind_ptr != nullptr)
  {
    QCRIL_LOG_INFO("SCWS OPEN Channel timer callback");

    ind_ptr->slot = qmi_ril_get_sim_slot();
    gstk_scws_open_ch_info.timer_id = 0;
    memcpy(&ind_ptr->ind_data.scws_open_ch,
           gstk_scws_open_ch_info.scws_open_ch_msg_ptr,
           sizeof(qmi_cat_scws_open_ch_ind_msg));
  }
  if (ind_msg_ptr != nullptr)
  {
    ind_msg_ptr->broadcast();
  }
} /* qcril_uim_resend_ara_m_cmd_timer_cb */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_scws_open_channel
 *   ======================================================================*/
/*!
 *   @brief
 *     Sends open channel command to SCWS agent and sends the result back to
 *     the modem.
 *
 *   @return
 *     None
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_scws_open_channel
(
  qcril_scws_slot_enum_type         slot,
  qmi_cat_scws_open_ch_ind_msg    * open_ch_ind_ptr
)
{
  auto                       rsp_data_ptr =
    std::shared_ptr<qmi_cat_sync_rsp_data_type>(new qmi_cat_sync_rsp_data_type{});
  auto                       msg_ptr      =
    std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});
  qcril_scws_slot_enum_type  slot_id      = QCRIL_SCWS_SLOT_1;
  int                        open_ch_err  = -1;

  /* Sanity check */
  if (open_ch_ind_ptr == nullptr || msg_ptr == nullptr || rsp_data_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL open_channel_ind_ptr");
    return;
  }

  /* Convert slot_id and validate */
  if (qmi_ril_get_sim_slot() != slot)
  {
    QCRIL_LOG_ERROR("Invalid input slot_id: %d", slot);
    return;
  }

  /* Init open channel params */
  msg_ptr->ch_data.ch_id            = open_ch_ind_ptr->ch_id;
  msg_ptr->ch_data.channel_status   = QMI_CAT_SCWS_CHANNEL_CLOSED_STATE;
  msg_ptr->ch_data.slot             = qcril_gstk_qmi_convert_scws_slot_id(slot);

  QCRIL_LOG_INFO( "%s, <to Agent> for channel_id: 0x%X, cat_slot_type: %d, port: %d \n",
                  __FUNCTION__, open_ch_ind_ptr->ch_id, slot, open_ch_ind_ptr->port);

  open_ch_err = qcril_scws_open_channel(open_ch_ind_ptr->port,
                                        open_ch_ind_ptr->ch_id,
                                        slot_id);
  if (open_ch_err == EADDRINUSE)
  {
    if (gstk_scws_open_ch_info.timer_id != 0)
    {
      /* Timer is already running do nothing and wait for timeout */
      return;
    }

    /* Retry scws open channel */
    if (gstk_scws_open_ch_info.retry_count < QCRIL_GSTK_SCWS_OPEN_CH_MAX_RETRIES)
    {
      if (gstk_scws_open_ch_info.scws_open_ch_msg_ptr != NULL)
      {
        QCRIL_GSTK_QMI_FREE_PTR(gstk_scws_open_ch_info.scws_open_ch_msg_ptr);
      }

      gstk_scws_open_ch_info.scws_open_ch_msg_ptr =
              (qmi_cat_scws_open_ch_ind_msg *)qcril_malloc(sizeof(qmi_cat_scws_open_ch_ind_msg));

      if (gstk_scws_open_ch_info.scws_open_ch_msg_ptr != NULL)
      {
        memcpy(gstk_scws_open_ch_info.scws_open_ch_msg_ptr,
               open_ch_ind_ptr,
               sizeof(qmi_cat_scws_open_ch_ind_msg));

        gstk_scws_open_ch_info.retry_count++;
        gstk_scws_open_ch_info.scws_open_ch_timer.tv_sec  = QCRIL_GSTK_SCWS_OPEN_CH_RETRY_TIME;

        if(qcril_setup_timed_callback( qmi_ril_get_process_instance_id(),
                                       QCRIL_DEFAULT_MODEM_ID,
                                       qcril_gstk_scws_open_ch_timer_cb,
                                       &gstk_scws_open_ch_info.scws_open_ch_timer,
                                       &gstk_scws_open_ch_info.timer_id) == 0)
        {
          return;
        }
      }
    }

    /* No more retries reset the SCWS open channel timer */
    if (gstk_scws_open_ch_info.scws_open_ch_msg_ptr != NULL)
    {
      QCRIL_GSTK_QMI_FREE_PTR(gstk_scws_open_ch_info.scws_open_ch_msg_ptr);
    }
    memset(&gstk_scws_open_ch_info, 0, sizeof(gstk_scws_open_ch_info));
  }
  else if (open_ch_err == 0)
  {
    /* On success, update channel_state parameter to 'listen' */
    msg_ptr->ch_data.channel_status = QMI_CAT_SCWS_CHANNEL_LISTEN_STATE;
  }

  (void) qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUEST_SCWS_OPEN_CHANNEL,
                                      msg_ptr,
                                      rsp_data_ptr);
} /* qcril_gstk_qmi_scws_open_channel */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_scws_close_channel
 *   ======================================================================*/
/*!
 *   @brief
 *     Sends close channel command to SCWS agent and sends the result back to
 *     the modem.
 *
 *   @return
 *     None
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_scws_close_channel
(
  qcril_scws_slot_enum_type          slot_id,
  qmi_cat_scws_close_ch_ind_msg    * close_ch_ind_ptr
)
{
  boolean                    close_server = FALSE;
  auto                       rsp_data_ptr =
    std::shared_ptr<qmi_cat_sync_rsp_data_type>(new qmi_cat_sync_rsp_data_type{});
  auto                       msg_ptr      =
    std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});

  /* Sanity check */
  if (close_ch_ind_ptr == nullptr || msg_ptr == nullptr || rsp_data_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL close_ch_ind_ptr");
    return;
  }

  if (close_ch_ind_ptr->state == QMI_CAT_SCWS_CHANNEL_CLOSED_STATE)
  {
    close_server = TRUE;
  }

  QCRIL_LOG_INFO( "%s, <to Agent> for channel_id: 0x%X, cat_slot_type: %d, close_server: %s \n",
                  __FUNCTION__, close_ch_ind_ptr->ch_id,
                  slot_id, !close_server ? "FALSE" : "TRUE" );

  /* Init close channel params */
  msg_ptr->ch_data.ch_id            = close_ch_ind_ptr->ch_id;
  msg_ptr->ch_data.channel_status   = close_ch_ind_ptr->state;
  msg_ptr->ch_data.slot             = qcril_gstk_qmi_convert_scws_slot_id(slot_id);

  if (!qcril_scws_close_channel(close_ch_ind_ptr->ch_id,
                                slot_id,
                                close_server))
  {
    /* On error, update channel_state parameter to 'closed' */
    msg_ptr->ch_data.channel_status = QMI_CAT_SCWS_CHANNEL_CLOSED_STATE;
  }

  /* Send the result to the card */
  QCRIL_LOG_INFO( "%s, <to card> for channel_id: 0x%X, channel_state: 0x%X \n",
                  __FUNCTION__, msg_ptr->ch_data.ch_id,
                  msg_ptr->ch_data.channel_status);

  (void) qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUEST_SCWS_CLOSE_CHANNEL,
                                      msg_ptr,
                                      rsp_data_ptr);
} /* qcril_gstk_qmi_scws_close_channel */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_scws_send_data
 *   ======================================================================*/
/*!
 *   @brief
 *     Sends the data received from the modem to the SCWS agent and the result
 *     of the operation abck to modem. It first concatenates all the packets
 *     sent from modem if applicable and then sends the buffer the agent.
 *
 *   @return
 *     None
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_scws_send_data
(
  qcril_scws_slot_enum_type          slot,
  qmi_cat_scws_send_data_ind_msg   * send_data_ind_ptr
)
{
  auto                       rsp_data_ptr =
    std::shared_ptr<qmi_cat_sync_rsp_data_type>(new qmi_cat_sync_rsp_data_type{});
  auto                       msg_ptr      =
    std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});

  /* Sanity check */
  if (send_data_ind_ptr == nullptr || rsp_data_ptr == nullptr || msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input: NULL send_data_ind_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "%s, channel_id: 0x%X, cat_slot_type: %d, total_packets: %d, current_packet: %d, data_len: 0x%X \n",
                  __FUNCTION__, send_data_ind_ptr->ch_id, slot,
                  send_data_ind_ptr->total_packets, send_data_ind_ptr->current_packet,
                  send_data_ind_ptr->data_len );

  /* Init send data params */

  if ((send_data_ind_ptr->total_packets == 0) ||
      (send_data_ind_ptr->current_packet == 0) ||
      (send_data_ind_ptr->data_len == 0) ||
      (send_data_ind_ptr->data_len > QMI_CAT_SCWS_DATA_MAX_LENGTH) ||
      (send_data_ind_ptr->current_packet > send_data_ind_ptr->total_packets))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input for qmi_cat_scws_send_data! \n");
    return;
  }

  /* Ignore subsequent packets on error */
  if (scws_err_sent)
  {
    if (send_data_ind_ptr->current_packet ==
        send_data_ind_ptr->total_packets)
    {
      scws_err_sent = FALSE;
    }
    QCRIL_LOG_DEBUG( "Already acknowledged send_data with error, ignoring packet: %d\n",
                     send_data_ind_ptr->current_packet );
    return;
  }

  msg_ptr->send_data.ch_id    = send_data_ind_ptr->ch_id;
  msg_ptr->send_data.result   = TRUE;
  msg_ptr->send_data.slot     = qcril_gstk_qmi_convert_scws_slot_id(slot);

  /* Send the packet to the Agent */
  QCRIL_LOG_INFO( "%s, <to Agent> for channel_id: 0x%X, data_len: 0x%X \n", __FUNCTION__,
                  send_data_ind_ptr->ch_id,
                  send_data_ind_ptr->data_len );

  if (!qcril_scws_send_data(send_data_ind_ptr->ch_id,
                            slot,
                            send_data_ind_ptr->data,
                            send_data_ind_ptr->data_len))
  {
    /* On error, update result parameter to 'failure' */
    msg_ptr->send_data.result = FALSE;
  }

  /* For the last packet or an error condition, send response to modem */
  if ((send_data_ind_ptr->total_packets ==
        send_data_ind_ptr->current_packet) ||
      (msg_ptr->send_data.result == FALSE))
  {
    QCRIL_LOG_INFO( "%s, <to Card> for channel_id: 0x%X, result: %s \n",
                    __FUNCTION__, msg_ptr->send_data.ch_id ,
                    (msg_ptr->send_data.result == TRUE) ? "Success" : "Failure" );

    (void) qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUEST_SCWS_SEND_DATA,
                                        msg_ptr,
                                        rsp_data_ptr);

    /* If there was an error for a intermediate packet, we still get the remaining packets
       in that set, so keep track of it */
    if ((send_data_ind_ptr->total_packets !=
          send_data_ind_ptr->current_packet) &&
        (msg_ptr->send_data.result == FALSE))
    {
      scws_err_sent = TRUE;
    }
  }
} /* qcril_gstk_qmi_scws_send_data */


/*=========================================================================
  FUNCTION:  qcril_gstk_qmi_get_recovery_proactive_cache
===========================================================================*/
/*!
 *   @brief
 *     Requests recovery proactive commands from QMI CAT and performs the
 *     necessary data parsing, packaging the data and sends the UNSOL event
 *     to Android Telephony.
 *
 *   @return
 *     None
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_get_recovery_proactive_cache
(
  qcril_instance_id_e_type  instance_id
)
{
  uint8_t    slot_index      = QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE;
  uint8_t    i               = 0;


  /* Map STK_CMD to CAT_CACHED_COMMAND_ID */
  qcril_gstk_cmd_id_qmi_req_id_map_type recovery_proactive_commands[] =
  {
    {QCRIL_GSTK_QMI_CMD_STK_SET_UP_MENU,           QMI_CAT_CACHED_COMMAND_ID_SETUP_MENU},
    {QCRIL_GSTK_QMI_CMD_STK_SET_UP_EVENT_LIST,     QMI_CAT_CACHED_COMMAND_ID_SETUP_EVENT_LIST},
    {QCRIL_GSTK_QMI_CMD_STK_SET_UP_IDLE_MODE_TEXT, QMI_CAT_CACHED_COMMAND_ID_SETUP_IDLE_TEXT}
  };

  QCRIL_LOG_INFO("Enter: instance_id=0x%x", instance_id);

  slot_index = qcril_gstk_qmi_convert_instance_to_slot_index(instance_id);

  if ((instance_id >= QCRIL_MAX_INSTANCE_ID)               ||
      (slot_index  == QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE) ||
      (slot_index  >= qcril_gstk_get_num_slots()))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, slot_index: 0x%x",
                     instance_id, slot_index);
    QCRIL_ASSERT(0);
    return;
  }

  for (i = 0;
       i < sizeof(recovery_proactive_commands)/sizeof(recovery_proactive_commands[0]);
       i++)
  {
    qcril_gstk_qmi_unsol_info_type                   ril_unsol_resp = {};
    std::shared_ptr<qcril_gstk_request_data_type>    msg_ptr       = nullptr;
    std::shared_ptr<qmi_cat_sync_rsp_data_type>      rsp_data_ptr  = nullptr;
    uint8_t                                          two_byte_len  = 0;

    /* Allocate message request */
    msg_ptr = std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});
    if (msg_ptr == nullptr)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory!");
      return;
    }

    msg_ptr->cached_cmd.slot    = qcril_gstk_qmi_convert_slot_index_to_slot_type(slot_index);
    msg_ptr->cached_cmd.cmd_id  = recovery_proactive_commands[i].qmi_req_cmd_id;

    /* Get command from QMI only if it is not in QCRIL cache */
    if (gstk_command_info.cmd_ptr &&
        recovery_proactive_commands[i].gstk_cmd_id == gstk_command_info.cmd_ptr->command_type)
    {
      continue;
    }

    rsp_data_ptr = std::shared_ptr<qmi_cat_sync_rsp_data_type>(new qmi_cat_sync_rsp_data_type{});

    QCRIL_LOG_INFO("Requesting qmi_req_cmd_id=0x%x", recovery_proactive_commands[i].qmi_req_cmd_id);

    if (qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUEST_GET_CACHED_CMD,
                                     msg_ptr,
                                     rsp_data_ptr) != RIL_E_SUCCESS)
    {
      if(rsp_data_ptr)
      {
        QCRIL_LOG_ERROR("Error GET_CACHED_PROACTIVE_CMD_REQ, qmi_err_code: 0x%x,", rsp_data_ptr->qmi_err_code);
      }
      QCRIL_LOG_ERROR("Error GET_CACHED_PROACTIVE_CMD_REQ");
      continue;
    }

    if (rsp_data_ptr == nullptr)
    {
      QCRIL_LOG_ERROR("Response is null");
      continue;
    }

    /* Prepare qcril_gstk_qmi_unsol_info_type */
    ril_unsol_resp.ril_unsol_type = RIL_UNSOL_STK_PROACTIVE_COMMAND;

    if (rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len < QCRIL_GSTK_QMI_COMMAND_MIN_SIZE ||
        rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len  > QMI_CAT_RAW_PROACTIVE_CMD_MAX_LENGTH)
    {
      QCRIL_LOG_ERROR("Length out of range: 0x%x, discarding TLV",
                       rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len);
      continue;
    }

    /* Length of the proactive command two bytes, increment command number bit position */
    if (rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[1] == 0x81)
    {
      two_byte_len++;
    }

    /* Command details can be 0x01 or 0x81, when comparing ignore the last bit */
    if (rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[0] == QCRIL_GSTK_QMI_PROACTIVE_SIM_COMMAND_TAG &&
        (rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[2 + two_byte_len] & 0x7F) == QCRIL_GSTK_QMI_COMMAND_DETAILS_TAG &&
        rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[3 + two_byte_len] == QCRIL_GSTK_QMI_COMMAND_DETAILS_TLV_LENGTH &&
        (rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[5 + two_byte_len] == QCRIL_GSTK_QMI_CMD_STK_SET_UP_MENU ||
         rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[5 + two_byte_len] == QCRIL_GSTK_QMI_CMD_STK_SET_UP_EVENT_LIST ||
         rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[5 + two_byte_len] == QCRIL_GSTK_QMI_CMD_STK_SET_UP_IDLE_MODE_TEXT))
    {
      /* Set the command number as 0xFE for cached proactive command, so that TR can be ignored */
      rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[4 + two_byte_len] = QCRIL_GSTK_CACHED_PCMD_CMD_NUM;

      /* Allocate & copy response packet's data & length */
      QCRIL_GSTK_QMI_MALLOC_AND_CPY_CMD(ril_unsol_resp.ril_unsol_resp.resp_pkt,
                                        ril_unsol_resp.ril_unsol_resp.resp_len,
                                        rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data,
                                        rsp_data_ptr->rsp_data.cached_rsp_data.cmd_len);
    }
    else
    {
      QCRIL_LOG_ERROR("Invalid proactive command : %d", rsp_data_ptr->rsp_data.cached_rsp_data.cmd_data[5 + two_byte_len]);
      continue;
    }

    QCRIL_LOG_INFO("Received response: resp_pkt=%p, resp_len=%zx",
                   ril_unsol_resp.ril_unsol_resp.resp_pkt, ril_unsol_resp.ril_unsol_resp.resp_len);

    /* Send qcril_unsol_resp_params_type to STK */
    if (ril_unsol_resp.ril_unsol_resp.resp_pkt &&
        ril_unsol_resp.ril_unsol_resp.resp_len)
    {
      QCRIL_LOG_INFO("Sending Cached proactive command for 0x%x",
                     msg_ptr->cached_cmd.cmd_id);

      qcril_gstk_qmi_send_unsol_resp(instance_id,
                                     ril_unsol_resp.ril_unsol_type,
                                     ril_unsol_resp.ril_unsol_resp.resp_pkt,
                                     ril_unsol_resp.ril_unsol_resp.resp_len);

      /* Free buffer allocated for the UNSOL response */
      QCRIL_GSTK_QMI_FREE_PTR(ril_unsol_resp.ril_unsol_resp.resp_pkt);
    }
  }
}/* qcril_gstk_qmi_get_recovery_proactive_cache */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_scws_data_available_callback
 *   ======================================================================*/
/*!
 *   @brief
 *     Callback that is executed by the SCWS agent when it needs to send any
 *     data to the modem.
 *
 *   @return
 *     None
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_scws_data_available_callback
(
  uint32                      bip_id,
  qcril_scws_slot_enum_type   slot_id,
  uint16                      data_len,
  uint8                     * data_ptr,
  uint16                      remaining_data_len
)
{
  auto orig_ptr      = std::shared_ptr<qcril_gstk_original_request_type>(new qcril_gstk_original_request_type{});
  auto msg_ptr       = std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});

  QCRIL_LOG_INFO( "%s, for bip_id: 0x%X, scws_slot_id: %d, data_len: 0x%X, remaining_data_len: 0x%X \n",
                 __FUNCTION__, bip_id, slot_id, data_len, remaining_data_len);

  if (orig_ptr == nullptr || msg_ptr == nullptr || data_len > QMI_CAT_SCWS_DATA_MAX_LENGTH)
  {
    QCRIL_LOG_ERROR("No memory ");
    return;
  }

  /* Init data available params */
  msg_ptr->data_avail.data_len           = data_len;
  msg_ptr->data_avail.remaining_data_len = remaining_data_len;
  msg_ptr->data_avail.ch_id              = bip_id;
  msg_ptr->data_avail.slot               = qcril_gstk_qmi_convert_scws_slot_id(slot_id);

  memcpy(msg_ptr->data_avail.data, data_ptr, data_len);

  orig_ptr->req_ptr      = nullptr;
  orig_ptr->data.bip_id  = bip_id;
  orig_ptr->qmi_req_id   = QCRIL_GSTK_REQUEST_SCWS_DATA_AVAILABLE;

  (void) qcril_gstk_send_qmi_async_msg(QCRIL_GSTK_REQUEST_SCWS_DATA_AVAILABLE,
                                       msg_ptr,
                                       orig_ptr);
} /* qcril_gstk_qmi_scws_data_available_callback */


/*===========================================================================
 *   FUNCTION:  qcril_gstk_qmi_scws_channel_status_callback
 *   ======================================================================*/
/*!
 *   @brief
 *     Callback that is executed by the SCWS agent when it needs to send any
 *     channel status information to the modem.
 *
 *   @return
 *                                                                         */
/*=========================================================================*/
static void qcril_gstk_qmi_scws_channel_status_callback
(
  uint32                              bip_id,
  qcril_scws_slot_enum_type           slot_id,
  qcril_scws_socket_state_enum_type   socket_state
)
{
  auto rsp_data_ptr = std::shared_ptr<qmi_cat_sync_rsp_data_type>(new qmi_cat_sync_rsp_data_type{});
  auto msg_ptr      = std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});

  QCRIL_LOG_INFO( "%s, for bip_id: 0x%X, scws_slot_id: %d, socket_state: 0x%X \n",
                  __FUNCTION__, bip_id, slot_id, socket_state);

  if (rsp_data_ptr == nullptr ||
      msg_ptr     == nullptr)
  {
    return;
  }

  /* Init channel status params */
  msg_ptr->ch_data.slot  = qcril_gstk_qmi_convert_scws_slot_id(slot_id);
  msg_ptr->ch_data.ch_id = bip_id;
  msg_ptr->ch_data.channel_status = static_cast<qmi_cat_scws_channel_state_type>(socket_state);

  (void) qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUEST_SCWS_CHANNEL_STATUS,
                                      msg_ptr,
                                      rsp_data_ptr);
} /* qcril_gstk_qmi_scws_channel_status_callback */


/*=========================================================================

  FUNCTION:  qcril_gstk_qmi_process_qmi_data_available_response

===========================================================================*/
static void qcril_gstk_qmi_process_qmi_data_available_response
(
  qmi_cat_rsp_data_type    * rsp_params_ptr
)
{
  if (rsp_params_ptr == NULL || rsp_params_ptr->orig_ptr == nullptr)
  {
    return;
  }

  if ((rsp_params_ptr->transp_err != QMI_NO_ERR) ||
      (rsp_params_ptr->qmi_err    != QMI_NO_ERR))
  {
    /* Notify the Agent only about the error */
    QCRIL_LOG_ERROR("%s", "Error for a previous data_available command, notifying agent");

    /* Notify the Agent about the error */
    qcril_scws_data_available_error(rsp_params_ptr->orig_ptr->data.bip_id,
                                    static_cast<qcril_scws_slot_enum_type>(qmi_ril_get_sim_slot()));
  }
} /* qcril_gstk_qmi_process_qmi_data_available_response */


/*=========================================================================

  FUNCTION:  qcril_gstk_qmi_process_qmi_envelope_response

===========================================================================*/
static void qcril_gstk_qmi_process_qmi_envelope_response
(
  qmi_cat_rsp_data_type    * rsp_params_ptr
)
{
  std::shared_ptr<gstk_ril_resp_payload>   rsp_ptr      = nullptr;
  RIL_Errno                                ril_err      = RIL_E_INTERNAL_ERR;
  std::shared_ptr<GstkRilRequestMessage>   req_ptr      = nullptr;
  void                                   * rsp_data_ptr = nullptr;
  uint32_t                                 rsp_len      = 0;
  uint8_t                                * env_str_ptr  = nullptr;
  RIL_SIM_IO_Response                      sim_io_rsp{};
  

  if (rsp_params_ptr == nullptr ||
      rsp_params_ptr->orig_ptr == nullptr ||
      rsp_params_ptr->orig_ptr->req_ptr == nullptr)
  {
    return;
  }

  req_ptr = rsp_params_ptr->orig_ptr->req_ptr;
  if(req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("req_ptr is NULL");
    return;
  }

  if ((rsp_params_ptr->transp_err == QMI_NO_ERR) &&
      (rsp_params_ptr->qmi_err    == QMI_NO_ERR))
  {
    ril_err = RIL_E_SUCCESS;

    QCRIL_GSTK_QMI_MALLOC_AND_CPY_CMD(env_str_ptr,
                                      rsp_len,
                                      rsp_params_ptr->rsp_data.env_rsp_data.env_resp_data,
                                      rsp_params_ptr->rsp_data.env_rsp_data.env_resp_data_len);

    if (req_ptr->get_req_id() == RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS)
    {
      sim_io_rsp.sw1 = rsp_params_ptr->rsp_data.env_rsp_data.sw1;
      sim_io_rsp.sw2 = rsp_params_ptr->rsp_data.env_rsp_data.sw2;
      sim_io_rsp.simResponse = (char *)env_str_ptr;
      rsp_data_ptr = static_cast<void *>(&sim_io_rsp);
      rsp_len = sizeof(RIL_SIM_IO_Response);
    }
    else
    {
      rsp_data_ptr = static_cast<void *>(env_str_ptr);
    }
  }

  rsp_ptr = std::make_shared<gstk_ril_resp_payload>(ril_err,
                                                    static_cast<void *>(rsp_data_ptr),
                                                    static_cast<size_t>(rsp_len));

  req_ptr->sendResponse(rsp_params_ptr->orig_ptr->req_ptr,
                        Message::Callback::Status::SUCCESS,
                        rsp_ptr);

  if (env_str_ptr != nullptr)
  {
    QCRIL_GSTK_QMI_FREE_PTR(env_str_ptr);
  }
} /* qcril_gstk_qmi_process_qmi_envelope_response */


/*=========================================================================

  FUNCTION:  qcril_gstk_qmi_process_qmi_envelope_response

===========================================================================*/
static void qcril_gstk_qmi_process_qmi_tr_response
(
  qmi_cat_rsp_data_type    * rsp_params_ptr
)
{
  std::shared_ptr<gstk_ril_resp_payload> rsp_ptr = nullptr;
  RIL_Errno                              ril_err = RIL_E_INTERNAL_ERR;

  if (rsp_params_ptr == nullptr ||
      rsp_params_ptr->orig_ptr == nullptr ||
      rsp_params_ptr->orig_ptr->req_ptr == nullptr)
  {
    return;
  }

  if ((rsp_params_ptr->transp_err == QMI_NO_ERR) &&
      (rsp_params_ptr->qmi_err    == QMI_NO_ERR))
  {
    ril_err = RIL_E_SUCCESS;
  }

  rsp_ptr = std::make_shared<gstk_ril_resp_payload>(ril_err, nullptr, 0);

  rsp_params_ptr->orig_ptr->req_ptr->sendResponse(rsp_params_ptr->orig_ptr->req_ptr,
                                                  Message::Callback::Status::SUCCESS,
                                                  rsp_ptr);
} /* qcril_gstk_qmi_process_qmi_tr_response */


/*===========================================================================
  FUNCTION:  qcril_gstk_init_state
===========================================================================*/
/*!
  @brief
    Init GSTK.

  @return
    Nothing

  @msc
  @endmsc
*/
/*=========================================================================*/
int qcril_gstk_init_state
(
  void
)
{
  RIL_Errno ril_err      = RIL_E_SUCCESS;
  auto      dataPtr      = std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});
  auto      rsp_data_ptr = std::shared_ptr<qmi_cat_sync_rsp_data_type>(new qmi_cat_sync_rsp_data_type{});

  gstk_command_info.call_timeout = (2 * 60 * 1000);

  if (dataPtr == nullptr || rsp_data_ptr == nullptr)
  {
    return -1;
  }

  ril_err = qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUSET_CHECK_CONFIGURATION,
                                         nullptr,
                                         rsp_data_ptr);

  if (ril_err == RIL_E_SUCCESS)
  {
    qcril_gstk_request_data_type * evt_rpt_ptr = dataPtr.get();

    qcril_scws_initalize(qcril_gstk_qmi_scws_data_available_callback,
                         qcril_gstk_qmi_scws_channel_status_callback);

    memset(rsp_data_ptr.get(), 0x00, sizeof(qmi_cat_sync_rsp_data_type));
    evt_rpt_ptr->evt_rpt.slot = qmi_ril_get_sim_slot();
    evt_rpt_ptr->evt_rpt.dec_evt_mask = QMI_CAT_SET_EVENT_REPORT_SCWS;
    evt_rpt_ptr->evt_rpt.evt_mask =
      QMI_CAT_SET_EVENT_REPORT_DISPLAY_TEXT                    |
      QMI_CAT_SET_EVENT_REPORT_GET_INKEY                       |
      QMI_CAT_SET_EVENT_REPORT_GET_INPUT                       |
      QMI_CAT_SET_EVENT_REPORT_SETUP_MENU                      |
      QMI_CAT_SET_EVENT_REPORT_SELECT_ITEM                     |
      QMI_CAT_SET_EVENT_REPORT_SEND_SMS                        |
      QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_USER_ACTIVITY       |
      QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_IDLE_SCREEN_NOTIFY  |
      QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_LANGUAGE_SEL_NOTIFY |
      QMI_CAT_SET_EVENT_REPORT_SETUP_IDLE_MODE_TEXT            |
      QMI_CAT_SET_EVENT_REPORT_LANGUAGE_NOTIFICATION           |
      QMI_CAT_SET_EVENT_REPORT_REFRESH_ALPHA                   |
      QMI_CAT_SET_EVENT_REPORT_END_PROACTIVE_SESSION           |
      QMI_CAT_SET_EVENT_REPORT_PLAY_TONE                       |
      QMI_CAT_SET_EVENT_REPORT_SETUP_CALL                      |
      QMI_CAT_SET_EVENT_REPORT_SEND_DTMF                       |
      QMI_CAT_SET_EVENT_REPORT_LAUNCH_BROWSER                  |
      QMI_CAT_SET_EVENT_REPORT_SEND_SS                         |
      QMI_CAT_SET_EVENT_REPORT_SEND_USSD                       |
      QMI_CAT_SET_EVENT_REPORT_PROVIDE_LOCAL_INFO_LANG         |
      QMI_CAT_SET_EVENT_REPORT_PROVIDE_LOCAL_INFO_TIME         |
      QMI_CAT_SET_EVENT_REPORT_BIP                             |
      QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_BROWSER_TERM        |
      QMI_CAT_SET_EVENT_REPORT_ACTIVATE                        |
#ifdef FEATURE_QCRIL_GSTK_CONTACTLESS
      QMI_CAT_SET_EVENT_REPORT_CONTACTLESS_SUPPORT             |
#endif /* FEATURE_QCRIL_GSTK_CONTACTLESS */
      QMI_CAT_SET_EVENT_REPORT_SETUP_EVENT_HCI_CONN;

    ril_err = qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUSET_SET_EVENT_REPORT,
                                           dataPtr,
                                           rsp_data_ptr);

    if (ril_err != RIL_E_SUCCESS)
    {
      return -1;
    }
  }
  else
  {
    return -1;
  }
  return 0;
} /* qcril_gstk_init_state */


/*===========================================================================
  FUNCTION:  qcril_gstk_reset_state
===========================================================================*/
/*!
  @brief
    Reset GSTK.

  @return
    Nothing

  @msc
  @endmsc
*/
/*=========================================================================*/
void qcril_gstk_reset_state
(
  boolean is_refresh
)
{
  QCRIL_LOG_INFO("%s", __FUNCTION__);

  if (gstk_command_info.cmd_ptr != nullptr)
  {
    QCRIL_GSTK_QMI_FREE_PTR(gstk_command_info.cmd_ptr->data);
    QCRIL_GSTK_QMI_FREE_PTR(gstk_command_info.cmd_ptr);
  }
  gstk_command_info.cmd_ptr = nullptr;
  gstk_command_info.cmd_len = 0;
  /* Clear the flags if not refresh */
  if (!is_refresh)
  {
    gstk_command_info.flags = 0;
  }

  qcril_scws_deinitalize();
  if (gstk_scws_open_ch_info.timer_id != 0)
  {
    QCRIL_LOG_INFO("%s", "De-activating open_ch timer");
    qcril_cancel_timed_callback((void *)(uintptr_t) gstk_scws_open_ch_info.timer_id);
    gstk_scws_open_ch_info.timer_id = 0;
  }
  if (gstk_scws_open_ch_info.scws_open_ch_msg_ptr != nullptr)
  {
    QCRIL_GSTK_QMI_FREE_PTR(gstk_scws_open_ch_info.scws_open_ch_msg_ptr);
  }
  memset(&gstk_scws_open_ch_info, 0x00, sizeof(gstk_scws_open_ch_info));
  scws_err_sent = false;
} /* qcril_gstk_reset_state */


/*===========================================================================

FUNCTION:  qcril_gstk_qmi_request_stk_send_envelope_command
===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND.

  @param[in]  req_ptr  Android RIL request message

  @return
    RIL_Errno

  @msc
  @endmsc

  @see
*/
/*=========================================================================*/
RIL_Errno qcril_gstk_qmi_request_stk_send_envelope_command
(
  std::shared_ptr<GstkRilRequestMessage> req_ptr
)
{
  qcril_instance_id_e_type                            instance_id   = QCRIL_DEFAULT_INSTANCE_ID;
  uint8_t                                             slot_index    = QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE;
  std::shared_ptr<qcril_gstk_original_request_type>   orig_ptr      = nullptr;
  std::shared_ptr<qcril_gstk_request_data_type>       msg_ptr       = nullptr;
  char                                              * env_ptr       = nullptr;
  uint32_t                                            env_len       = 0;

  /* Sanity checks */
  if (req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    return RIL_E_INVALID_ARGUMENTS;
  }

  env_ptr = (char *)req_ptr->get_params(&env_len);
  if ((env_ptr == nullptr) || (env_len == 0))
  {
    QCRIL_LOG_ERROR("Invalid input, NULL data pointer or env_ptr: %p, len: 0x%x",
                     env_ptr, env_len);
    return RIL_E_INVALID_ARGUMENTS;
  }

  instance_id = req_ptr->get_instance_id();
  slot_index = qcril_gstk_qmi_convert_instance_to_slot_index(instance_id);;

  if ((instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (slot_index  >= qcril_gstk_get_num_slots()))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, slot_index: 0x%x",
                     instance_id, slot_index);
    return RIL_E_INVALID_ARGUMENTS;
  }

  QCRIL_LOG_INFO("%s", "qcril_gstk_qmi_request_stk_send_envelope_command\n");

  /* Allocate message request */
  msg_ptr = std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});
  if (msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for qcril_gstk_request_data_type!");
    return RIL_E_NO_MEMORY;
  }

  msg_ptr->env_data.slot_id = qcril_gstk_qmi_convert_slot_index_to_slot_type(slot_index);
  msg_ptr->env_data.token = qcril_gstk_get_ril_token(req_ptr->get_token());
  msg_ptr->env_data.envelope_data_len = QCRIL_GSTK_QMI_ENVCMD_DATA_SIZE(env_len);
  if (msg_ptr->env_data.envelope_data_len > QMI_CAT_ENVELOPE_DATA_MAX_LENGTH)
  {
    QCRIL_LOG_ERROR("Length of Envelope too long: 0x%x\n",
                     msg_ptr->env_data.envelope_data_len);
    return RIL_E_INVALID_ARGUMENTS;
  }
  /* Copy over the command */
  QCRIL_GSTK_QMI_ENVCMD_COPY(msg_ptr->env_data.envelope_data,
                             env_ptr,
                             env_len);

  /* Allocate original request */
  orig_ptr = std::shared_ptr<qcril_gstk_original_request_type>(new qcril_gstk_original_request_type{});
  if (orig_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_type!");
    return RIL_E_NO_MEMORY;
  }

  orig_ptr->req_ptr      = req_ptr;
  orig_ptr->qmi_req_id   = QCRIL_GSTK_REQUEST_SEND_ENVELOPE_COMMAND;

  return qcril_gstk_send_qmi_async_msg(QCRIL_GSTK_REQUEST_SEND_ENVELOPE_COMMAND,
                                       msg_ptr,
                                       orig_ptr);
} /* qcril_gstk_qmi_request_stk_send_envelope_command */


/*===========================================================================
  FUNCTION:  qcril_gstk_qmi_request_stk_send_terminal_response
===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE

    For proactive commands for which the Terminal Response originates from
    the Android Toolkit application, this function arranges for that
    response to be sent to the card.

  @param[in]  req_ptr  Android RIL request message

  @return
    RIL_Errno
*/
/*=========================================================================*/
RIL_Errno qcril_gstk_qmi_request_stk_send_terminal_response
(
  std::shared_ptr<GstkRilRequestMessage> req_ptr
)
{
  qcril_instance_id_e_type                            instance_id   = QCRIL_DEFAULT_INSTANCE_ID;
  uint8_t                                             slot_index    = QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE;
  std::shared_ptr<qcril_gstk_original_request_type>   orig_ptr      = nullptr;
  std::shared_ptr<qcril_gstk_request_data_type>       msg_ptr       = nullptr;
  std::shared_ptr<gstk_ril_resp_payload>              rsp_ptr       = nullptr;
  char                                              * tr_ptr        = nullptr;
  uint32_t                                            tr_len        = 0;

  /* Sanity checks */
  if (req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    return RIL_E_INVALID_ARGUMENTS;
  }

  tr_ptr = (char *)req_ptr->get_params(&tr_len);
  if ((tr_ptr == nullptr) || (tr_len == 0))
  {
    QCRIL_LOG_ERROR("Invalid input, NULL data pointer or tr_ptr: %p, tr_len: 0x%x",
                     tr_ptr, tr_len);
    return RIL_E_INVALID_ARGUMENTS;
  }

  instance_id = req_ptr->get_instance_id();
  slot_index = qcril_gstk_qmi_convert_instance_to_slot_index(instance_id);;

  if ((instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (slot_index  >= qcril_gstk_get_num_slots()))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, slot_index: 0x%x",
                     instance_id, slot_index);
    return RIL_E_INVALID_ARGUMENTS;
  }

  QCRIL_LOG_INFO("%s", "qcril_gstk_qmi_request_stk_send_terminal_response\n");

  /* Allocate message request */
  msg_ptr = std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});
  if (msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for qcril_gstk_request_data_type!");
    return RIL_E_NO_MEMORY;
  }

  msg_ptr->tr_data.slot_id = qcril_gstk_qmi_convert_slot_index_to_slot_type(slot_index);
  msg_ptr->tr_data.token = qcril_gstk_get_ril_token(req_ptr->get_token());
  msg_ptr->tr_data.terminal_response_len = QCRIL_GSTK_QMI_ENVCMD_DATA_SIZE(tr_len);
  if (msg_ptr->tr_data.terminal_response_len > QMI_CAT_TERMINAL_RESPONSE_MAX_LENGTH)
  {
    QCRIL_LOG_ERROR("Length of Terminal response too long: 0x%x\n",
                     msg_ptr->tr_data.terminal_response_len);
    return RIL_E_INVALID_ARGUMENTS;
  }
  /* Copy over the command */
  QCRIL_GSTK_QMI_ENVCMD_COPY(msg_ptr->tr_data.terminal_response,
                             tr_ptr,
                             tr_len);

  /* Allocate original request */
  orig_ptr = std::shared_ptr<qcril_gstk_original_request_type>(new qcril_gstk_original_request_type{});
  if (orig_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_type!");
    return RIL_E_NO_MEMORY;
  }

  orig_ptr->req_ptr      = req_ptr;
  orig_ptr->qmi_req_id   = QCRIL_GSTK_REQUEST_SEND_TERMINAL_RESPONSE;

  /* Check the TR is for cached proactive command or not
     check command number of TR command details byte 3 for 0xFE and any of the proactive commands
     SETUP MENU, SETUP EVENT LIST, SETUP IDLE MODE TEXT */
  if ((msg_ptr->tr_data.terminal_response_len > QCRIL_GSTK_QMI_COMMAND_DETAILS_TLV_LENGTH) &&
      ((msg_ptr->tr_data.terminal_response[0] & 0x7F) == QCRIL_GSTK_QMI_COMMAND_DETAILS_TAG) &&
      (msg_ptr->tr_data.terminal_response[1] == QCRIL_GSTK_QMI_COMMAND_DETAILS_TLV_LENGTH) &&
      (msg_ptr->tr_data.terminal_response[2] == QCRIL_GSTK_CACHED_PCMD_CMD_NUM) &&
      ((msg_ptr->tr_data.terminal_response[3] == QCRIL_GSTK_QMI_CMD_STK_SET_UP_IDLE_MODE_TEXT) ||
       (msg_ptr->tr_data.terminal_response[3] == QCRIL_GSTK_QMI_CMD_STK_SET_UP_EVENT_LIST) ||
       (msg_ptr->tr_data.terminal_response[3] == QCRIL_GSTK_QMI_CMD_STK_SET_UP_MENU)))
  {
    QCRIL_LOG_INFO("TR for cached proactive command Ignored and sending success response back");
    rsp_ptr = std::make_shared<gstk_ril_resp_payload>(RIL_E_SUCCESS, nullptr, 0);
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, rsp_ptr);
    return RIL_E_SUCCESS;
  }

  if(gstk_command_info.cmd_ptr != nullptr)
  {
    msg_ptr->tr_data.uim_ref_id = gstk_command_info.cmd_ptr->uim_ref_id;

    return qcril_gstk_send_qmi_async_msg(QCRIL_GSTK_REQUEST_SEND_TERMINAL_RESPONSE,
                                         msg_ptr,
                                         orig_ptr);
  }
  else
  {
    return RIL_E_INVALID_ARGUMENTS;
  }
} /* qcril_gstk_qmi_request_stk_send_terminal_response */


/*===========================================================================
  FUNCTION:  qcril_gstk_qmi_request_stk_handle_call_setup_requested_from_sim
===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM.

    This request arrives from the SIM Toolkit application after the user
    has accepted or rejected a call originating from a SETUP_CALL command,
    and the request data indicates which.

  @param[in]  req_ptr  Android RIL request message

  @return
    RIL_Errno
*/
/*=========================================================================*/
RIL_Errno qcril_gstk_qmi_request_stk_handle_call_setup_requested_from_sim
(
  std::shared_ptr<GstkRilRequestMessage> req_ptr
)
{
  qcril_instance_id_e_type                            instance_id   = QCRIL_DEFAULT_INSTANCE_ID;
  uint8_t                                             slot_index    = QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE;
  std::shared_ptr<qcril_gstk_request_data_type>       msg_ptr       = nullptr;
  std::shared_ptr<gstk_ril_resp_payload>              rsp_ptr       = nullptr;
//  std::shared_ptr<qmi_cat_sync_rsp_data_type>       * rsp_data_ptr  = nullptr;
  int                                               * usr_cnf_ptr   = nullptr;
  uint32_t                                            usr_cnf_len   = 0;

  /* Sanity checks */
  if (req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    return RIL_E_INVALID_ARGUMENTS;
  }

  usr_cnf_ptr = (int *)req_ptr->get_params(&usr_cnf_len);
  if ((usr_cnf_ptr == nullptr) || (usr_cnf_len == 0))
  {
    QCRIL_LOG_ERROR("Invalid input, NULL data pointer or tr_ptr: %p, tr_len: 0x%x",
                     usr_cnf_ptr, usr_cnf_len);
    return RIL_E_INVALID_ARGUMENTS;
  }

  instance_id = req_ptr->get_instance_id();
  slot_index = qcril_gstk_qmi_convert_instance_to_slot_index(instance_id);;

  if ((instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (slot_index  >= qcril_gstk_get_num_slots()))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, slot_index: 0x%x",
                     instance_id, slot_index);
    return RIL_E_INVALID_ARGUMENTS;
  }

  QCRIL_LOG_INFO("%s", "qcril_gstk_qmi_request_stk_send_envelope_command\n");

  /* Allocate message request */
  msg_ptr = std::shared_ptr<qcril_gstk_request_data_type>(new qcril_gstk_request_data_type{});
  if (msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for qcril_gstk_qmi_request_stk_handle_call_setup_requested_from_sim!");
    return RIL_E_NO_MEMORY;
  }

  QCRIL_LOG_INFO("User %s call\n",(*usr_cnf_ptr ? "ACCEPTED" : "REJECTED"));

  msg_ptr->evt_cnf_data.slot     = qcril_gstk_qmi_convert_slot_index_to_slot_type(slot_index);
  msg_ptr->evt_cnf_data.evt_data = QMI_CAT_CONFIRM;
  if (*usr_cnf_ptr > 0)
  {
    msg_ptr->evt_cnf_data.cnf_data = true;
  }
  else
  {
    msg_ptr->evt_cnf_data.cnf_data = false;
  }

  if (gstk_command_info.cmd_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid cache gstk_command_info cmd_ptr");
    return RIL_E_INVALID_ARGUMENTS;
  }

  /* Use QMI handle from the command cache */
  if((gstk_command_info.cmd_ptr->command_type == QCRIL_GSTK_QMI_CMD_STK_SET_UP_CALL)||
     (gstk_command_info.cmd_ptr->command_type == QCRIL_GSTK_QMI_CMD_STK_OPEN_CHANNEL))
  {
    RIL_Errno ril_err = RIL_E_SUCCESS;
    auto rsp_data_ptr = std::shared_ptr<qmi_cat_sync_rsp_data_type>(new qmi_cat_sync_rsp_data_type{});

    if (rsp_data_ptr == nullptr)
    {
      return RIL_E_NO_MEMORY;
    }

    ril_err = qcril_gstk_send_qmi_sync_msg(QCRIL_GSTK_REQUEST_SEND_EVENT_CONFIRMATION,
                                           msg_ptr,
                                           rsp_data_ptr);

    rsp_ptr = std::make_shared<gstk_ril_resp_payload>(ril_err, nullptr, 0);
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, rsp_ptr);

    return RIL_E_SUCCESS;
  }

  QCRIL_LOG_ERROR("Request received for wrong proactive command : 0x%x",
      gstk_command_info.cmd_ptr->command_type);
  return RIL_E_INVALID_ARGUMENTS;
} /* qcril_gstk_qmi_request_stk_handle_call_setup_requested_from_sim */


/*===========================================================================
  FUNCTION:  qcril_gstk_qmi_request_stk_service_is_running
===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING

  @param[in]  req_ptr  Android RIL request message

  @return
    RIL_Errno
*/
/*=========================================================================*/
RIL_Errno qcril_gstk_qmi_request_stk_service_is_running
(
  std::shared_ptr<GstkRilRequestMessage> req_ptr,
  bool                                   m_ready
)
{
  qcril_instance_id_e_type                 instance_id                    = QCRIL_DEFAULT_INSTANCE_ID;
  uint8_t                                  slot_index                     = QCRIL_GSTK_INVALID_SLOT_INDEX_VALUE;
  std::shared_ptr<gstk_ril_resp_payload>   rsp_ptr                        = nullptr;

  /* Sanity checks */
  if (req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    return RIL_E_INVALID_ARGUMENTS;
  }

  instance_id = req_ptr->get_instance_id();
  slot_index = qcril_gstk_qmi_convert_instance_to_slot_index(instance_id);;

  if ((instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (slot_index  >= qcril_gstk_get_num_slots()))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, slot_index: 0x%x",
                     instance_id, slot_index);
    return RIL_E_INVALID_ARGUMENTS;
  }

  QCRIL_LOG_INFO("%s", "qcril_gstk_qmi_request_stk_service_is_running\n");

  rsp_ptr = std::make_shared<gstk_ril_resp_payload>(RIL_E_SUCCESS, nullptr, 0);
  req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, rsp_ptr);

  gstk_command_info.flags |= QCRIL_GSTK_QMI_RIL_IS_READY_BMSK;

  if (m_ready)
  {
    qcril_gstk_qmi_process_raw_proactive_cmd();

    /* Get supported proactive commands from QMI CAT for STK recovery */
    qcril_gstk_qmi_get_recovery_proactive_cache(instance_id);
  }

  return RIL_E_SUCCESS;  
} /* qcril_gstk_qmi_request_stk_service_is_running */

/*===========================================================================

FUNCTION:  qcril_gstk_process_qmi_cat_ind
===========================================================================*/
/*!
  @brief
    Handles QMI CAT indications.

  @param[in]  ind_id  Qmi Cat indication id
  @param[in]  ind_ptr Qmi cat indication message

  @return
    None

  @msc
  @endmsc

  @see
*/
/*=========================================================================*/
void qcril_gstk_process_qmi_cat_ind
(
  qmi_cat_indication_id_type       ind_id,
  qcril_gstk_qmi_ind_params_type  *ind_ptr
)
{
  if (ind_ptr == nullptr)
  {
    return;
  }

  QCRIL_LOG_INFO("Indication slot: %d, ind_id: %d", ind_ptr->slot, ind_id);
  if (ind_ptr->slot != qmi_ril_get_process_instance_id())
  {
    QCRIL_LOG_ERROR("Invalid slot :%d", qmi_ril_get_process_instance_id());
    return;
  }

  switch(ind_id)
  {
    case QMI_CAT_EVENT_REPORT_IND:
      qcril_gstk_qmi_process_event_report_ind(&ind_ptr->ind_data.evt_rpt);
      break;
    case QMI_CAT_SCWS_OPEN_CHANNEL_IND:
      qcril_gstk_qmi_scws_open_channel(
         static_cast<qcril_scws_slot_enum_type>(ind_ptr->slot),
         &ind_ptr->ind_data.scws_open_ch);
      break;
    case QMI_CAT_SCWS_CLOSE_CHANNEL_IND:
      qcril_gstk_qmi_scws_close_channel(
         static_cast<qcril_scws_slot_enum_type>(ind_ptr->slot),
         &ind_ptr->ind_data.scws_close_ch);
      break;
    case QMI_CAT_SCWS_SEND_DATA_IND:
      qcril_gstk_qmi_scws_send_data(
         static_cast<qcril_scws_slot_enum_type>(ind_ptr->slot),
         &ind_ptr->ind_data.scws_send_data);
      break;
    default:
      break;
  }
} /* qcril_gstk_process_qmi_cat_ind */

/*===========================================================================

FUNCTION:  qcril_gstk_process_qmi_cat_cb
===========================================================================*/
/*!
  @brief
    Handles QMI CAT async callback.

  @param[in]  cb_ptr Qmi cat callback message

  @return
    None

  @msc
  @endmsc

  @see
*/
/*=========================================================================*/
void qcril_gstk_process_qmi_cat_cb
(
  qmi_cat_rsp_data_type  *cb_ptr
)
{
  if (cb_ptr == nullptr)
  {
    return;
  }

  switch (cb_ptr->rsp_id)
  {
    case QMI_CAT_SRVC_SCWS_DATA_AVAILABLE_RSP_MSG:
      qcril_gstk_qmi_process_qmi_data_available_response(cb_ptr);
      break;
    case QMI_CAT_SRVC_SEND_ENVELOPE_COMMAND_RSP_MSG:
      qcril_gstk_qmi_process_qmi_envelope_response(cb_ptr);
      break;
    case QMI_CAT_SRVC_SEND_TERMINAL_RESPONSE_RSP_MSG:
      qcril_gstk_qmi_process_qmi_tr_response(cb_ptr);
      break;
    default:
      QCRIL_LOG_ERROR("Invalid Msg ID: 0x%x", cb_ptr->rsp_id);
      break;
  }
} /* qcril_gstk_process_qmi_cat_cb */

#endif /* (FEATURE_QCRIL_QMI_CAT) */
