/*===========================================================================

  Copyright (c) 2010-2015, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/sandbox/users/micheleb/ril/qcril_uim_file.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/18   ra      Support for case-2 Extended APDU
11/25/15   ar      Extending MCC list which has 3 digit MNCs
10/06/15   gm      Clear select response cache entry upon channel close
08/11/15   lxu     Fake 6F 00 response for not supported reselect AID
04/13/15   at      Added digit check during parsing of GW IMSI
01/19/15   hh      memset read_params before checking data length
01/16/15   at      Reselect via send APDU for backward compatibility
12/08/14   at      Select response support for RIL_REQUEST_SIM_OPEN_CHANNEL
12/01/14   hh      Support for get MCC and MNC
08/13/14   tkl     Fixed session mapping for ISIM Auth
07/24/14   tkl     Use RIL_SIM_IO_Response instead of RIL_SimAuthenticationResponse
06/18/14   at      Support for SelectNext using reselect QMI command
06/11/14   at      Support for open logical channel API
06/17/14   tl      Added logic to better determine FCI value from AID
06/10/14   tl      Removed array structures for slot specific parameters
05/14/14   yt      Support for STATUS command as part of SIM_IO request
05/02/14   tkl     Added support for RIL_REQUEST_SIM_AUTHENTICATION
04/24/14   yt      Return 6D 00 for invalid instruction in APDU
04/18/14   tl      Add check for mastercard AID
01/21/14   at      Added support for getSelectResponse()
01/17/14   at      Changed the feature check in qcril_uim_request_send_apdu
01/28/14   at      Do not terminate app when closing logical channel
01/17/14   at      Updated function definition for verify pin2 cb
12/10/13   at      Updated feature checks with new ones for APDU APIs
11/19/13   at      Changed the feature checks for streaming APDU APIs
10/08/13   vdc     Return FALSE when pointer is NULL while composing APDU data
08/29/13   yt      Allow P3 value to be more than 255 for READ/WRITE requests
08/12/13   at      Added support for Long APDU indication
05/02/13   at      Update P3 sanity check for qcril_uim_request_send_apdu
04/17/13   yt      Fix critical KW errors
01/29/13   yt      Support for third SIM card slot
01/14/13   yt      Fix critical KW errors
12/19/12   at      Move to newer qcril_event_queue API
12/05/12   at      Fix for status byte in get file attributes response
11/27/12   at      Move an older QMI_UIM API to newer QCCI based one
10/25/12   at      Explicit check for P1, P2 & P3 in RIL_SIM_IO_v6 request
10/08/12   at      Support for ISIM Authentication API
08/21/12   at      Fix for sending the correct slot in open logical channel
07/26/12   at      Request FCP template while opening channel for ICC cards
06/29/12   at      Error code mapping for open logical channel response
05/10/12   at      Support for fetching IMSI_M from RUIM App
04/10/12   at      Updated session type and request id in PIN2 cmd callback
03/22/12   at      Replacing malloc()/free() with qcril_malloc()/qcril_free()
03/20/12   tl      Transition to QCCI
02/15/12   at      Support for fetching IMSI_M in RIL_REQUEST_GET_IMSI
11/30/11   at      Send CM_UPDATE_FDN_STATUS based on correct RILD instance;
                   Made get fdn status asynchrounous
08/19/11   yt      Fixed Klocwork errors
07/08/11   at      Converting AID to binary in logical_channel request
05/18/11   yt      Update QCRIL_CM with FDN status at power up
03/30/11   at      Support for logical channel & send apdu commands
03/22/11   at      Support for ril.h v6
03/02/11   at      Fixed response parameter if PIN2 verification failed
01/18/11   at      Removed slot id parameter from all requests
11/12/10   at      Added support for UIM queue implementation
11/08/10   at      Fix for sending proper error codes for PIN2 verification
11/02/10   at      Sending prov session in SIM_IO PIN2 check for 2G cards
10/06/10   at      Support for handling instance_id passed in requests
09/09/10   at      Changed the way sessions are fetched, added handling for
                   opening & closing non-prov session on demand
08/25/10   at      Added featurization for async SIM_IO calls and check for
                   sw1 in case of read record, write binary & write record
08/03/10   at      APIs using aidPtr to be handled properly
07/13/10   at      Added support for set and get FDN status
05/13/10   at      Fixed compile errors & clean up for merging with mainline
04/13/10   mib     Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#if defined (FEATURE_QCRIL_UIM_QMI)

#include "ril.h"
#include "IxErrno.h"
#include "qcrili.h"
#include "qcril_arb.h"
#include "qcril_log.h"
#include "qcril_reqlist.h"
#include "qcril_uim.h"
#include "qcril_uim_util.h"
#include "qcril_uim_file.h"
#include "qcril_uim_queue.h"
#include "qcril_uim_qcci.h"
#include "util_timer.h"
#include <string.h>


/*===========================================================================

                           DEFINES

===========================================================================*/
/* SIM IO commands */
#define SIM_CMD_READ_BINARY                         176
#define SIM_CMD_READ_RECORD                         178
#define SIM_CMD_GET_RESPONSE                        192
#define SIM_CMD_UPDATE_BINARY                       214
#define SIM_CMD_RETRIEVE_DATA                       203
#define SIM_CMD_SET_DATA                            219
#define SIM_CMD_UPDATE_RECORD                       220
#define SIM_CMD_STATUS                              242

#define QCRIL_UIM_IMSI_PATH_SIZE                    4
#define QCRIL_UIM_FILEID_EF_IMSI                    0x6F07
#define QCRIL_UIM_FILEID_EF_IMSI_M                  0x6F22
#define QCRIL_UIM_FILEID_EF_AD                      0x6FAD
#define QCRIL_UIM_IMSI_M_RAW_SIZE                   10
#define QCRIL_UIM_IMSI_M_PARSED_SIZE                16

#define QCRIL_UIM_APDU_MIN_P3                       0
#define QCRIL_UIM_APDU_MIN_SIZE                     4
#define QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3             5
#define QCRIL_UIM_LONG_APDU_MIN_SIZE_PLUS_P3        7
#define QCRIL_UIM_APDU_MAX_SHORT_APDU_SIZE          256
#define QCRIL_UIM_APDU_MAX_LONG_APDU_SIZE           65536
#define QCRIL_UIM_INS_BYTE_GET_RESPONSE             0xC0
#define QCRIL_UIM_CHANNEL_ID_MAX_VAL                19
#define QCRIL_UIM_INVALID_INS_BYTE_MASK             0x60
#define QCRIL_UIM_SW1_INS_CODE_NOT_SUPPORTED        0x6D
#define QCRIL_UIM_SW2_NORMAL_END                    0x00
#define QCRIL_UIM_INS_BYTE_SELECT                   0xA4
#define QCRIL_UIM_P1_VALUE_SELECT_BY_DF_NAME        0x04
#define QCRIL_UIM_P2_MASK_SELECT_NEXT               0x02

/* Get file attributes response defines */
/* Default value: not invalidated, readable and updatable when invalidated */
#define QCRIL_UIM_GET_RESPONSE_MIN_SIZE             14
#define QCRIL_UIM_GET_RESPONSE_FILE_STATUS          0X05
#define QCRIL_UIM_TAG_FCP_TEMPLATE                  0x62
#define QCRIL_UIM_TAG_LIFE_CYCLE_STATUS             0x8A

#define QCRIL_UIM_AUTH_GSM_CONTEXT                  0x80
#define QCRIL_UIM_AUTH_3G_CONTEXT                   0x81
#define QCRIL_UIM_AUTH_VGCS_VBS_SECURITY_CONTEXT    0x82

#define QCRIL_UIM_AUTH_IMS_AKA                      0x81
#define QCRIL_UIM_AUTH_HTTP_DIGEST_SECURITY_CONTEXT 0x82

#define QCRIL_UIM_SW1_WRONG_PARAMS                  0x6A
#define QCRIL_UIM_SW2_BAD_PARAMS_P1_P2              0x86

#define QCRIL_UIM_SW1_TECH_PROBLEM                  0X6F
#define QCRIL_UIM_SW2_TECH_PROBLEM                  0x00

#define QCRIL_UIM_ARA_M_APP_OPEN_WAIT_TIME          3

#define QCRIL_UIM_INVALID_OPEN_CH_P2                -1


/*===========================================================================
  structures
===========================================================================*/
/*===========================================================================
   STRUCT:      QCRIL_UIM_CARD_ARA_M_DELAY_TIMER_TYPE

   DESCRIPTION:
     Stores the info needed during the ARA-M app open delay
===========================================================================*/
struct
{
  uint32                        timer_id;
  struct timeval                ara_m_delay_timer;
  qcril_instance_id_e_type      instance_id;
  const char                    aid_data[QMI_UIM_MAX_AID_LEN + 1];
  int                           p2;
  RIL_Token                     token;
  int                           request_id;
  qmi_uim_slot_type             slot;
} qcril_uim_ara_m_delay_timer =
       {0, {QCRIL_UIM_ARA_M_APP_OPEN_WAIT_TIME, 0}, 0, {0}, 0, NULL, 0, 0};


/*===========================================================================
                               FUNCTION DECLARATIONS
===========================================================================*/
static void qcril_uim_resend_ara_m_cmd_timer_cb
(
  void *param_ptr
);


/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_clone_read_transparent_request

===========================================================================*/
/*!
    @brief
    Allocates the memory and clones a read transparent request.
    This function is used when PIN2 needs to be verified before
    executing the read transparent. The function allocates a single
    block of memory to execute the complete copy.

    @return
    Buffer with cloned request
*/
/*=========================================================================*/
static qcril_uim_pin2_original_request_type* qcril_uim_clone_read_transparent_request
(
  qcril_instance_id_e_type                     instance_id,
  qcril_modem_id_e_type                        modem_id,
  RIL_Token                                    token,
  const qmi_uim_read_transparent_params_type * read_params_ptr
)
{
  uint32                                 tot_size  = 0;
  uint16                                 req_size  = 0;
  uint16                                 aid_size  = 0;
  uint16                                 path_size = 0;
  qcril_uim_pin2_original_request_type * ret_ptr   = NULL;

  if(read_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL read_params_ptr");
    QCRIL_ASSERT(0);
    return NULL;
  }

  /* Calculate the size of each block, aligning to word */
  req_size = qcril_uim_align_size(sizeof(qcril_uim_pin2_original_request_type));
  if (read_params_ptr->session_info.aid.data_ptr != NULL)
  {
    aid_size = qcril_uim_align_size(read_params_ptr->session_info.aid.data_len);
  }
  if (read_params_ptr->file_id.path.data_ptr != NULL)
  {
    path_size = qcril_uim_align_size(read_params_ptr->file_id.path.data_len);
  }

  if ((req_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (aid_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (path_size == QCRIL_UIM_UINT16_MAXVAL))
  {
    QCRIL_LOG_ERROR("%s", "Error in qcril_uim_align_size");
    return NULL;
  }

  tot_size = req_size + aid_size + path_size;

  /* Allocate memory */
  ret_ptr = (qcril_uim_pin2_original_request_type*)qcril_malloc(tot_size);

  if (ret_ptr == NULL)
  {
    return NULL;
  }

  memset(ret_ptr, 0, tot_size);

  /* Generic fields */
  ret_ptr->size         = tot_size;
  ret_ptr->cmd          = QCRIL_UIM_ORIG_SIM_IO_READ_BINARY;
  ret_ptr->token        = token;
  ret_ptr->instance_id  = instance_id;
  ret_ptr->modem_id     = modem_id;

  /* Session */
  ret_ptr->data.read_transparent.session_info.session_type = read_params_ptr->session_info.session_type;
  ret_ptr->data.read_transparent.session_info.aid.data_len = read_params_ptr->session_info.aid.data_len;
  if (read_params_ptr->session_info.aid.data_len > 0 &&
      read_params_ptr->session_info.aid.data_ptr != NULL)
  {
    ret_ptr->data.read_transparent.session_info.aid.data_ptr = (uint8*)ret_ptr + req_size;
    memcpy(ret_ptr->data.read_transparent.session_info.aid.data_ptr,
           read_params_ptr->session_info.aid.data_ptr,
           read_params_ptr->session_info.aid.data_len);
  }

  /* Path */
  ret_ptr->data.read_transparent.file_id.file_id = read_params_ptr->file_id.file_id;
  ret_ptr->data.read_transparent.file_id.path.data_len = read_params_ptr->file_id.path.data_len;
  if (read_params_ptr->file_id.path.data_len > 0 &&
      read_params_ptr->file_id.path.data_ptr != NULL)
  {
    ret_ptr->data.read_transparent.file_id.path.data_ptr = (uint8*)ret_ptr + req_size + aid_size;
    memcpy(ret_ptr->data.read_transparent.file_id.path.data_ptr,
           read_params_ptr->file_id.path.data_ptr,
           read_params_ptr->file_id.path.data_len);
  }

  /* Other read transparent parameters */
  ret_ptr->data.read_transparent.offset = read_params_ptr->offset;
  ret_ptr->data.read_transparent.length = read_params_ptr->length;

  return ret_ptr;
} /* qcril_uim_clone_read_transparent_request */


/*=========================================================================

  FUNCTION:  qcril_uim_clone_read_record_request

===========================================================================*/
/*!
    @brief
    Allocates the memory and clones a read record request.
    This function is used when PIN2 needs to be verified before
    executing the read record. The function allocates a single
    block of memory to execute the complete copy.

    @return
    Buffer with cloned request
*/
/*=========================================================================*/
static qcril_uim_pin2_original_request_type* qcril_uim_clone_read_record_request
(
  qcril_instance_id_e_type                instance_id,
  qcril_modem_id_e_type                   modem_id,
  RIL_Token                               token,
  const qmi_uim_read_record_params_type * read_params_ptr
)
{
  uint32                                 tot_size  = 0;
  uint16                                 req_size  = 0;
  uint16                                 aid_size  = 0;
  uint16                                 path_size = 0;
  qcril_uim_pin2_original_request_type * ret_ptr   = NULL;

  if(read_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL read_params_ptr");
    QCRIL_ASSERT(0);
    return NULL;
  }

  /* Calculate the size of each block, aligning to word */
  req_size = qcril_uim_align_size(sizeof(qcril_uim_pin2_original_request_type));
  if (read_params_ptr->session_info.aid.data_ptr != NULL)
  {
    aid_size = qcril_uim_align_size(read_params_ptr->session_info.aid.data_len);
  }
  if (read_params_ptr->file_id.path.data_ptr != NULL)
  {
    path_size = qcril_uim_align_size(read_params_ptr->file_id.path.data_len);
  }

  if ((req_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (aid_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (path_size == QCRIL_UIM_UINT16_MAXVAL))
  {
    QCRIL_LOG_ERROR("%s", "Error in qcril_uim_align_size");
    return NULL;
  }

  tot_size = req_size + aid_size + path_size;

  /* Allocate memory */
  ret_ptr = (qcril_uim_pin2_original_request_type*)qcril_malloc(tot_size);

  if (ret_ptr == NULL)
  {
    return NULL;
  }

  memset(ret_ptr, 0, tot_size);

  /* Generic fields */
  ret_ptr->size         = tot_size;
  ret_ptr->cmd          = QCRIL_UIM_ORIG_SIM_IO_READ_RECORD;
  ret_ptr->token        = token;
  ret_ptr->instance_id  = instance_id;
  ret_ptr->modem_id     = modem_id;

  /* Session */
  ret_ptr->data.read_record.session_info.session_type = read_params_ptr->session_info.session_type;
  ret_ptr->data.read_record.session_info.aid.data_len = read_params_ptr->session_info.aid.data_len;
  if (read_params_ptr->session_info.aid.data_len > 0 &&
      read_params_ptr->session_info.aid.data_ptr != NULL)
  {
    ret_ptr->data.read_record.session_info.aid.data_ptr = (uint8*)ret_ptr + req_size;
    memcpy(ret_ptr->data.read_record.session_info.aid.data_ptr,
           read_params_ptr->session_info.aid.data_ptr,
           read_params_ptr->session_info.aid.data_len);
  }

  /* Path */
  ret_ptr->data.read_record.file_id.file_id = read_params_ptr->file_id.file_id;
  ret_ptr->data.read_record.file_id.path.data_len = read_params_ptr->file_id.path.data_len;
  if (read_params_ptr->file_id.path.data_len > 0 &&
      read_params_ptr->file_id.path.data_ptr != NULL)
  {
    ret_ptr->data.read_record.file_id.path.data_ptr = (uint8*)ret_ptr + req_size + aid_size;
    memcpy(ret_ptr->data.read_record.file_id.path.data_ptr,
           read_params_ptr->file_id.path.data_ptr,
           read_params_ptr->file_id.path.data_len);
  }

  /* Other read transparent parameters */
  ret_ptr->data.read_record.record = read_params_ptr->record;
  ret_ptr->data.read_record.length = read_params_ptr->length;

  return ret_ptr;
} /* qcril_uim_clone_read_record_request */


/*=========================================================================

  FUNCTION:  qcril_uim_clone_write_transparent_request

===========================================================================*/
/*!
    @brief
    Allocates the memory and clones a write transparent request.
    This function is used when PIN2 needs to be verified before
    executing the write transparent. The function allocates a single
    block of memory to execute the complete copy.

    @return
    Buffer with cloned request
*/
/*=========================================================================*/
static qcril_uim_pin2_original_request_type* qcril_uim_clone_write_transparent_request
(
  qcril_instance_id_e_type                      instance_id,
  qcril_modem_id_e_type                         modem_id,
  RIL_Token                                     token,
  const qmi_uim_write_transparent_params_type * write_params_ptr
)
{
  uint32                                 tot_size  = 0;
  uint16                                 req_size  = 0;
  uint16                                 aid_size  = 0;
  uint16                                 path_size = 0;
  uint16                                 data_size = 0;
  qcril_uim_pin2_original_request_type * ret_ptr   = NULL;

  if(write_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL write_params_ptr");
    QCRIL_ASSERT(0);
    return NULL;
  }

  /* Calculate the size of each block, aligning to word */
  req_size = qcril_uim_align_size(sizeof(qcril_uim_pin2_original_request_type));
  if (write_params_ptr->session_info.aid.data_ptr != NULL)
  {
    aid_size = qcril_uim_align_size(write_params_ptr->session_info.aid.data_len);
  }
  if (write_params_ptr->file_id.path.data_ptr != NULL)
  {
    path_size = qcril_uim_align_size(write_params_ptr->file_id.path.data_len);
  }
  if (write_params_ptr->data.data_ptr != NULL)
  {
    data_size = qcril_uim_align_size(write_params_ptr->data.data_len);
  }

  if ((req_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (aid_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (path_size == QCRIL_UIM_UINT16_MAXVAL) ||
      (data_size == QCRIL_UIM_UINT16_MAXVAL))
  {
    QCRIL_LOG_ERROR("%s", "Error in qcril_uim_align_size");
    return NULL;
  }

  tot_size = req_size + aid_size + path_size + data_size;

  /* Allocate memory */
  ret_ptr = (qcril_uim_pin2_original_request_type*)qcril_malloc(tot_size);

  if (ret_ptr == NULL)
  {
    return NULL;
  }

  memset(ret_ptr, 0, tot_size);

  /* Generic fields */
  ret_ptr->size         = tot_size;
  ret_ptr->cmd          = QCRIL_UIM_ORIG_SIM_IO_UPDATE_BINARY;
  ret_ptr->token        = token;
  ret_ptr->instance_id  = instance_id;
  ret_ptr->modem_id     = modem_id;

  /* Session */
  ret_ptr->data.write_transparent.session_info.session_type = write_params_ptr->session_info.session_type;
  ret_ptr->data.write_transparent.session_info.aid.data_len = write_params_ptr->session_info.aid.data_len;
  if (write_params_ptr->session_info.aid.data_len > 0 &&
      write_params_ptr->session_info.aid.data_ptr != NULL)
  {
    ret_ptr->data.write_transparent.session_info.aid.data_ptr = (uint8*)ret_ptr + req_size;
    memcpy(ret_ptr->data.write_transparent.session_info.aid.data_ptr,
           write_params_ptr->session_info.aid.data_ptr,
           write_params_ptr->session_info.aid.data_len);
  }

  /* Path */
  ret_ptr->data.write_transparent.file_id.file_id = write_params_ptr->file_id.file_id;
  ret_ptr->data.write_transparent.file_id.path.data_len = write_params_ptr->file_id.path.data_len;
  if (write_params_ptr->file_id.path.data_len > 0 &&
      write_params_ptr->file_id.path.data_ptr != NULL)
  {
    ret_ptr->data.write_transparent.file_id.path.data_ptr = (uint8*)ret_ptr + req_size + aid_size;
    memcpy(ret_ptr->data.write_transparent.file_id.path.data_ptr,
           write_params_ptr->file_id.path.data_ptr,
           write_params_ptr->file_id.path.data_len);
  }

  /* Data */
  ret_ptr->data.write_record.data.data_len = write_params_ptr->data.data_len;
  if (write_params_ptr->data.data_len > 0 &&
      write_params_ptr->data.data_ptr != NULL)
  {
    ret_ptr->data.write_transparent.data.data_ptr = (uint8*)ret_ptr + req_size + aid_size + path_size;
    memcpy(ret_ptr->data.write_transparent.data.data_ptr,
           write_params_ptr->data.data_ptr,
           write_params_ptr->data.data_len);
  }

  /* Other write transparent parameters */
  ret_ptr->data.write_transparent.offset = write_params_ptr->offset;

  return ret_ptr;
} /* qcril_uim_clone_write_transparent_request */


/*=========================================================================

  FUNCTION:  qcril_uim_clone_write_record_request

===========================================================================*/
/*!
    @brief
    Allocates the memory and clones a write record request.
    This function is used when PIN2 needs to be verified before
    executing the write record. The function allocates a single
    block of memory to execute the complete copy.

    @return
    Buffer with cloned request
*/
/*=========================================================================*/
static qcril_uim_pin2_original_request_type* qcril_uim_clone_write_record_request
(
  qcril_instance_id_e_type                 instance_id,
  qcril_modem_id_e_type                    modem_id,
  RIL_Token                                token,
  const qmi_uim_write_record_params_type * write_params_ptr
)
{
  uint32                                 tot_size  = 0;
  uint16                                 req_size  = 0;
  uint16                                 aid_size  = 0;
  uint16                                 path_size = 0;
  uint16                                 data_size = 0;
  qcril_uim_pin2_original_request_type * ret_ptr   = NULL;

  if(write_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL write_params_ptr");
    QCRIL_ASSERT(0);
    return NULL;
  }

  /* Calculate the size of each block, aligning to word */
  req_size = qcril_uim_align_size(sizeof(qcril_uim_pin2_original_request_type));
  if (write_params_ptr->session_info.aid.data_ptr != NULL)
  {
    aid_size = qcril_uim_align_size(write_params_ptr->session_info.aid.data_len);
  }
  if (write_params_ptr->file_id.path.data_ptr != NULL)
  {
    path_size = qcril_uim_align_size(write_params_ptr->file_id.path.data_len);
  }
  if (write_params_ptr->data.data_ptr != NULL)
  {
    data_size = qcril_uim_align_size(write_params_ptr->data.data_len);
  }

  if ((req_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (aid_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (path_size == QCRIL_UIM_UINT16_MAXVAL) ||
      (data_size == QCRIL_UIM_UINT16_MAXVAL))
  {
    QCRIL_LOG_ERROR("%s", "Error in qcril_uim_align_size");
    return NULL;
  }

  tot_size = req_size + aid_size + path_size + data_size;

  /* Allocate memory */
  ret_ptr = (qcril_uim_pin2_original_request_type*)qcril_malloc(tot_size);

  if (ret_ptr == NULL)
  {
    return NULL;
  }

  memset(ret_ptr, 0, tot_size);

  /* Generic fields */
  ret_ptr->size         = tot_size;
  ret_ptr->cmd          = QCRIL_UIM_ORIG_SIM_IO_UPDATE_RECORD;
  ret_ptr->token        = token;
  ret_ptr->instance_id  = instance_id;
  ret_ptr->modem_id     = modem_id;

  /* Session */
  ret_ptr->data.write_record.session_info.session_type = write_params_ptr->session_info.session_type;
  ret_ptr->data.write_record.session_info.aid.data_len = write_params_ptr->session_info.aid.data_len;
  if (write_params_ptr->session_info.aid.data_len > 0 &&
      write_params_ptr->session_info.aid.data_ptr != NULL)
  {
    ret_ptr->data.write_record.session_info.aid.data_ptr = (uint8*)ret_ptr + req_size;
    memcpy(ret_ptr->data.write_record.session_info.aid.data_ptr,
           write_params_ptr->session_info.aid.data_ptr,
           write_params_ptr->session_info.aid.data_len);
  }

  /* Path */
  ret_ptr->data.write_record.file_id.file_id = write_params_ptr->file_id.file_id;
  ret_ptr->data.write_record.file_id.path.data_len = write_params_ptr->file_id.path.data_len;
  if (write_params_ptr->file_id.path.data_len > 0 &&
      write_params_ptr->file_id.path.data_ptr != NULL)
  {
    ret_ptr->data.write_record.file_id.path.data_ptr = (uint8*)ret_ptr + req_size + aid_size;
    memcpy(ret_ptr->data.write_record.file_id.path.data_ptr,
           write_params_ptr->file_id.path.data_ptr,
           write_params_ptr->file_id.path.data_len);
  }

  /* Data */
  ret_ptr->data.write_record.data.data_len = write_params_ptr->data.data_len;
  if (write_params_ptr->data.data_len > 0 &&
      write_params_ptr->data.data_ptr != NULL)
  {
    ret_ptr->data.write_record.data.data_ptr = (uint8*)ret_ptr + req_size + aid_size + path_size;
    memcpy(ret_ptr->data.write_record.data.data_ptr,
           write_params_ptr->data.data_ptr,
           write_params_ptr->data.data_len);
  }

  /* Other write record parameters */
  ret_ptr->data.write_record.record = write_params_ptr->record;

  return ret_ptr;
} /* qcril_uim_clone_write_record_request */


/*=========================================================================

  FUNCTION:  qcril_uim_clone_set_fdn_status_request

===========================================================================*/
/*!
    @brief
    Allocates the memory and clones a fdn status request.
    This function is used when PIN2 needs to be verified before
    executing the write record. The function allocates a single
    block of memory to execute the complete copy.

    @return
    Buffer with cloned request
*/
/*=========================================================================*/
static qcril_uim_pin2_original_request_type* qcril_uim_clone_set_fdn_status_request
(
  qcril_instance_id_e_type                       instance_id,
  qcril_modem_id_e_type                          modem_id,
  RIL_Token                                      token,
  const qmi_uim_set_service_status_params_type * service_status_params_ptr
)
{
  uint32                                 tot_size  = 0;
  uint16                                 req_size  = 0;
  uint16                                 aid_size  = 0;
  qcril_uim_pin2_original_request_type * ret_ptr   = NULL;

  if(service_status_params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL service_status_params_ptr");
    QCRIL_ASSERT(0);
    return NULL;
  }

  /* Calculate the size of each block, aligning to word */
  req_size = qcril_uim_align_size(sizeof(qcril_uim_pin2_original_request_type));
  if (service_status_params_ptr->session_info.aid.data_ptr != NULL)
  {
    aid_size = qcril_uim_align_size(service_status_params_ptr->session_info.aid.data_len);
  }

  if ((req_size  == QCRIL_UIM_UINT16_MAXVAL) ||
      (aid_size  == QCRIL_UIM_UINT16_MAXVAL))
  {
    QCRIL_LOG_ERROR("%s", "Error in qcril_uim_align_size");
    return NULL;
  }

  tot_size = req_size + aid_size;

  /* Allocate memory */
  ret_ptr = (qcril_uim_pin2_original_request_type*)qcril_malloc(tot_size);

  if (ret_ptr == NULL)
  {
    return NULL;
  }

  memset(ret_ptr, 0, tot_size);

  /* Generic fields */
  ret_ptr->size         = tot_size;
  ret_ptr->cmd          = QCRIL_UIM_ORIG_SET_SERVICE_STATUS_FDN;
  ret_ptr->token        = token;
  ret_ptr->instance_id  = instance_id;
  ret_ptr->modem_id     = modem_id;

  /* Session */
  ret_ptr->data.set_service_status.session_info.session_type = service_status_params_ptr->session_info.session_type;
  ret_ptr->data.set_service_status.session_info.aid.data_len = service_status_params_ptr->session_info.aid.data_len;
  if (service_status_params_ptr->session_info.aid.data_len > 0 &&
      service_status_params_ptr->session_info.aid.data_ptr != NULL)
  {
    ret_ptr->data.set_service_status.session_info.aid.data_ptr = (uint8*)ret_ptr + req_size;
    memcpy(ret_ptr->data.set_service_status.session_info.aid.data_ptr,
           service_status_params_ptr->session_info.aid.data_ptr,
           service_status_params_ptr->session_info.aid.data_len);
  }

  /* Service status */
  ret_ptr->data.set_service_status.fdn_status = service_status_params_ptr->fdn_status;

  return ret_ptr;
} /* qcril_uim_clone_set_fdn_status_request */


/*=========================================================================

  FUNCTION:  qmi_uim_internal_pin2_callback

===========================================================================*/
/*!
    @brief
    Callback for the PIN2 verification before executing another
    function that accesses the files on the card (read or write).
    This function posts an event, so it can be processed in the
    QCRIL context.

    @return
    None
*/
/*=========================================================================*/
static void qmi_uim_internal_pin2_callback
(
  qmi_uim_rsp_data_type        * rsp_data,
  void                         * user_data
)
{
  qcril_uim_pin2_original_request_type * original_request_ptr = NULL;
  IxErrnoType                            result               = E_FAILURE;

  if(rsp_data == NULL || user_data == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve the original request */
  original_request_ptr = (qcril_uim_pin2_original_request_type*)user_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }
  if(original_request_ptr->size == 0)
  {
    QCRIL_LOG_ERROR("%s", "original_request_ptr->size is 0");
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
    QCRIL_ASSERT(0);
    return;
  }

  /* Verify that the response is for a PIN verification. This should
     always happen as we use this callback only for this purpose */
  if(rsp_data->rsp_id != QMI_UIM_SRVC_VERIFY_PIN_RSP_MSG)
  {
    QCRIL_LOG_ERROR("invalid rsp_id 0x%x", rsp_data->rsp_id);
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
    QCRIL_ASSERT(0);
    return;
  }

  /* Store the PIN2 result in the same structure with the original
     request and send it to QCRIL context to be processed */
  original_request_ptr->pin2_result      = rsp_data->qmi_err_code;
  original_request_ptr->pin2_num_retries = rsp_data->rsp_data.verify_pin_rsp.num_retries;

  QCRIL_LOG_INFO( "%s qcril_event_queue\n", __FUNCTION__);
  result = qcril_event_queue( original_request_ptr->instance_id,
                              original_request_ptr->modem_id,
                              QCRIL_DATA_NOT_ON_STACK,
                              QCRIL_EVT_INTERNAL_UIM_VERIFY_PIN_COMMAND_CALLBACK,
                              (void *)original_request_ptr,
                              original_request_ptr->size,
                              original_request_ptr->token);
  if (result != E_SUCCESS)
  {
    QCRIL_LOG_ERROR( " qcril_event_queue failed, result: 0x%x\n", result);
    /* Free allocated memory in case event queueing fails */
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }
} /* qmi_uim_internal_pin2_callback */


/*=========================================================================

  FUNCTION:  qcril_uim_get_file_status_byte

===========================================================================*/
/*!
    @brief
    This function parses the raw response data for get file attibutes to
    check for the Life cycle status integer. This byte is mapped to the
    file status byte coding defined in 51.011, Section 9.2.1

    @return
    Mapped File status byte
*/
/*=========================================================================*/
static uint8 qcril_uim_get_file_status_byte
(
  uint16                         data_len,
  const uint8                  * data_ptr
)
{
  uint16 data_index = 0;
  uint8  file_status_byte = QCRIL_UIM_GET_RESPONSE_FILE_STATUS;

  if ((data_ptr == NULL) || (data_len == 0))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, returning default value 0x05");
    return file_status_byte;
  }

  /* Parse through the raw response to find the TLV */
  if (data_ptr[data_index] != QCRIL_UIM_TAG_FCP_TEMPLATE)
  {
    QCRIL_LOG_ERROR("Invalid FCP template tag: 0x%X", data_ptr[data_index]);
    return file_status_byte;
  }

  /* Byte 2 & 3 has the total length of FCP Template
     We need to udpate the data_index appropriately  */
  if (data_len >= 2 && data_ptr[1] <= 0x7F)
  {
    if (data_len < (data_ptr[1] + 2))
    {
      return file_status_byte;
    }
    data_len    = data_ptr[1] + 2;
    data_index += 2;
  }
  else if (data_len >= 3 && data_ptr[1] == 0x81)
  {
    if (data_len < (data_ptr[2] + 3))
    {
      return file_status_byte;
    }
    data_len   = data_ptr[2] + 3;
    data_index += 3;
  }
  else
  {
    return file_status_byte;
  }

  /* Parse through the raw response to find the TLV */
  while ((data_index+1) < data_len)
  {
    uint8 tag_len   = 0;
    uint8 read_byte = 0;

    switch (data_ptr[data_index])
    {
      case QCRIL_UIM_TAG_LIFE_CYCLE_STATUS:
        /* Sanity check the length */
        tag_len = data_ptr[data_index+1];
        if (((data_index + 2 + tag_len) > data_len) ||
             (tag_len != 1))
        {
          /* Incorrect data length, cannot parse further */
          QCRIL_LOG_ERROR("Incorrect tag length, cannot parse further: 0x%X \n",
                          tag_len);
          return file_status_byte;
        }
        read_byte = data_ptr[data_index+2];
        QCRIL_LOG_INFO( "Life cycle status integer byte: 0x%X", read_byte);
        /* Check for bit b1 and these condition:
           1. b3 should be set for operational state
           2. b4 - b8 should be 0s */
        if ((read_byte <= 0x07) && (read_byte >= 0x04))
        {
          if (read_byte & 0x01)
          {
            /* Not Invalidated, readable/updatable when invalidated */
            file_status_byte = 0x05;
          }
          else
          {
            /* Invalidated, not readable/updatable when invalidated */
            file_status_byte = 0x00;
          }
        }
        /* Nothing else to do, return */
        return file_status_byte;

      default:
        /* Tag that we are not interested in, move to next one if avaliable */
        tag_len = data_ptr[data_index+1];
        data_index += tag_len + 2;
        break;
    }
  }

  return file_status_byte;
} /* qcril_uim_get_file_status_byte */


#if defined(RIL_REQUEST_SIM_APDU) || defined(RIL_REQUEST_SIM_TRANSMIT_CHANNEL) || \
    defined(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC) || defined(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
/*=========================================================================

  FUNCTION:  qcril_uim_compose_apdu_data

===========================================================================*/
/*!
    @brief
    Function to compose raw APDU command. Composed data pointer and length
    are updated based on the request.

    @return
    TRUE if successful, FALSE otherwise.
*/
/*=========================================================================*/
static boolean qcril_uim_compose_apdu_data
(
  qmi_uim_data_type       * apdu_data_ptr,
  int                       cla,
  int                       ins,
  int                       p1,
  int                       p2,
  int                       p3,
  const char              * data_ptr
)
{
  qmi_uim_data_type     binary_apdu_data;
  uint16                total_apdu_len       = 0;

  if ((apdu_data_ptr == NULL) ||
      (apdu_data_ptr->data_ptr == NULL) ||
      (apdu_data_ptr->data_len == 0))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return FALSE;
  }

  memset(apdu_data_ptr->data_ptr, 0, apdu_data_ptr->data_len);
  memset(&binary_apdu_data, 0, sizeof(qmi_uim_data_type));

  total_apdu_len = apdu_data_ptr->data_len;

  /* Update mandatory parameters - CLA, INS, P1 & P2 */
  if (total_apdu_len >= QCRIL_UIM_APDU_MIN_SIZE)
  {
    apdu_data_ptr->data_ptr[0] = (uint8)(cla & 0xFF);
    apdu_data_ptr->data_ptr[1] = (uint8)(ins & 0xFF);
    apdu_data_ptr->data_ptr[2] = (uint8)(p1 & 0xFF);
    apdu_data_ptr->data_ptr[3] = (uint8)(p2 & 0xFF);
    apdu_data_ptr->data_len = QCRIL_UIM_APDU_MIN_SIZE;
  }

  /* Update P3 parameter if valid */
  if (p3 > QCRIL_UIM_APDU_MIN_P3 &&
      p3 < QCRIL_UIM_APDU_MAX_SHORT_APDU_SIZE)
  {
    apdu_data_ptr->data_ptr[4] = (uint8)(p3 & 0xFF);
    apdu_data_ptr->data_len = QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3;
  }
  else if (p3 >= QCRIL_UIM_APDU_MAX_SHORT_APDU_SIZE)
  {
    apdu_data_ptr->data_ptr[4] = (uint8)(0x00);
    apdu_data_ptr->data_ptr[5] = (uint8)((p3 >> 8) & 0xFF);
    apdu_data_ptr->data_ptr[6] = (uint8)(p3 & 0xFF);
    apdu_data_ptr->data_len = QCRIL_UIM_LONG_APDU_MIN_SIZE_PLUS_P3;
  }

  /* Update data parameter if valid */
  if((p3 >= QCRIL_UIM_APDU_MAX_SHORT_APDU_SIZE) &&
      (total_apdu_len == QCRIL_UIM_LONG_APDU_MIN_SIZE_PLUS_P3))
  {
    QCRIL_LOG_INFO("%s", "Case - 2 Extended APDU p3: 0x%x",
                 p3);
  }
  else if (total_apdu_len > QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3)
  {
    if ((data_ptr == NULL) || (strlen(data_ptr) == 0))
    {
      QCRIL_LOG_ERROR("%s", "Mismatch in total_apdu_len & input APDU data!");
      return FALSE;
    }

    binary_apdu_data.data_ptr = qcril_uim_alloc_hexstring_to_bin(data_ptr,
                                                                 &binary_apdu_data.data_len);
    if (binary_apdu_data.data_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "Unable to convert input APDU data!");
      return FALSE;
    }

    /* Update data parameter if valid */
    if (binary_apdu_data.data_len <= (total_apdu_len - QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3))
    {
      if (total_apdu_len >= QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3 &&
          total_apdu_len <= QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3 + QCRIL_UIM_APDU_MAX_SHORT_APDU_SIZE)
      {
        memcpy(&apdu_data_ptr->data_ptr[5], binary_apdu_data.data_ptr, binary_apdu_data.data_len);
        apdu_data_ptr->data_len = QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3 + binary_apdu_data.data_len;
      }
      else if (total_apdu_len > QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3 + QCRIL_UIM_APDU_MAX_SHORT_APDU_SIZE)
      {
        memcpy(&apdu_data_ptr->data_ptr[7], binary_apdu_data.data_ptr, binary_apdu_data.data_len);
        apdu_data_ptr->data_len = QCRIL_UIM_LONG_APDU_MIN_SIZE_PLUS_P3 + binary_apdu_data.data_len;
      }
    }

    /* Free temp buffer */
    qcril_free(binary_apdu_data.data_ptr);
    binary_apdu_data.data_ptr = NULL;
  }

  return TRUE;
} /* qcril_uim_compose_apdu_data */


/*=========================================================================

  FUNCTION:  qcril_uim_remove_select_response_info_entry

===========================================================================*/
/*!
    @brief
    Function that clears the select response data corresponding to the client
    id.

    @return
    void
*/
/*=========================================================================*/
static void qcril_uim_remove_select_response_info_entry
(
  int channel_id
)
{
  uint8  i                 = 0;

  if ((channel_id == 0) ||
      (channel_id > QCRIL_UIM_CHANNEL_ID_MAX_VAL))
  {
    QCRIL_LOG_ERROR("Invalid channel_id: 0x%x", channel_id);
    return;
  }

  QCRIL_LOG_INFO("%s", "Clearing select response info entry channel_id: 0x%x",
                 channel_id);

  for (i = 0; i < QCRIL_UIM_MAX_SELECT_RESP_COUNT; i++)
  {
    if (qcril_uim.select_response_info[i].in_use == TRUE &&
        qcril_uim.select_response_info[i].channel_id == channel_id)
    {
      if (qcril_uim.select_response_info[i].select_resp_ptr != NULL)
      {
        qcril_free(qcril_uim.select_response_info[i].select_resp_ptr);
        qcril_uim.select_response_info[i].select_resp_ptr = NULL;
      }

      memset(&qcril_uim.select_response_info[i],
             0x00,
             sizeof(qcril_uim_select_response_info_type));
    }
  }
} /* qcril_uim_remove_select_response_info_entry */


/*=========================================================================

  FUNCTION:  qcril_uim_store_select_response_info

===========================================================================*/
/*!
    @brief
    Function that temporarily caches the select response data if available in
    a global variable. It is cleaned upon the next APDU request by the client
    in any circumstance.

    @return
    RIL_E_SUCCESS if successful, RIL_E_GENERIC_FAILURE otherwise.
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_store_select_response_info
(
  const qmi_uim_logical_channel_rsp_type   * logical_ch_rsp_ptr
)
{
  uint8  i                 = 0;
  uint8  select_resp_index = QCRIL_UIM_MAX_SELECT_RESP_COUNT;

  if ((logical_ch_rsp_ptr->channel_id == 0) ||
      (logical_ch_rsp_ptr->channel_id > QCRIL_UIM_CHANNEL_ID_MAX_VAL))
  {
    QCRIL_LOG_ERROR("Invalid channel_id: 0x%x", logical_ch_rsp_ptr->channel_id);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  for (i = 0; i < QCRIL_UIM_MAX_SELECT_RESP_COUNT; i++)
  {
    if (qcril_uim.select_response_info[i].in_use == FALSE)
    {
      select_resp_index = i;
      break;
    }
  }

  if (select_resp_index == QCRIL_UIM_MAX_SELECT_RESP_COUNT)
  {
    QCRIL_LOG_ERROR("%s", "Couldnt get select resp array index !");
    return QCRIL_UIM_RIL_E_NO_RESOURCES;
  }

  /* Allocate & save the data */
  QCRIL_LOG_INFO("Storing logical_channel_rsp for select_resp_index 0x%x, select_resp_len: 0x%x",
                 select_resp_index, logical_ch_rsp_ptr->select_response.data_len);

  memset(&qcril_uim.select_response_info[select_resp_index],
         0,
         sizeof(qcril_uim_select_response_info_type));

  /* In some cases, we may have no actual select response,
     so store only the channel & SW info */
  if ((logical_ch_rsp_ptr->select_response.data_len > 0) &&
      (logical_ch_rsp_ptr->select_response.data_ptr != NULL))
  {
    qcril_uim.select_response_info[select_resp_index].select_resp_ptr =
      qcril_malloc(logical_ch_rsp_ptr->select_response.data_len);
    if (qcril_uim.select_response_info[select_resp_index].select_resp_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "Couldnt allocate select resp array index !");
      return QCRIL_UIM_RIL_E_NO_MEMORY;
    }
    qcril_uim.select_response_info[select_resp_index].select_resp_len =
      logical_ch_rsp_ptr->select_response.data_len;
    memcpy(qcril_uim.select_response_info[select_resp_index].select_resp_ptr,
           logical_ch_rsp_ptr->select_response.data_ptr,
           logical_ch_rsp_ptr->select_response.data_len);
  }

  qcril_uim.select_response_info[select_resp_index].in_use     = TRUE;
  qcril_uim.select_response_info[select_resp_index].sw1        = logical_ch_rsp_ptr->sw1;
  qcril_uim.select_response_info[select_resp_index].sw2        = logical_ch_rsp_ptr->sw2;
  qcril_uim.select_response_info[select_resp_index].channel_id = logical_ch_rsp_ptr->channel_id;

  return RIL_E_SUCCESS;
} /* qcril_uim_store_select_response_info */


/*=========================================================================

  FUNCTION:  qcril_uim_update_get_response_apdu

===========================================================================*/
/*!
    @brief
    Function to compose a get response APDU command based on what is cached
    in the global select response info data.

    @return
    RIL_E_SUCCESS if successful, RIL_E_GENERIC_FAILURE otherwise.
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_update_get_response_apdu
(
  uint8                    select_resp_index,
  RIL_SIM_IO_Response    * ril_response_ptr
)
{
  if (ril_response_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  if (select_resp_index >= QCRIL_UIM_MAX_SELECT_RESP_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid input, select_resp_index: 0x%x", select_resp_index);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  QCRIL_LOG_INFO("Updating get_response from select_resp_index 0x%x, select_resp_len: 0x%x",
                 select_resp_index,
                 qcril_uim.select_response_info[select_resp_index].select_resp_len);

  /* Fill RIL_SIM_IO_Response with necesary info */
  ril_response_ptr->sw1 = qcril_uim.select_response_info[select_resp_index].sw1;
  ril_response_ptr->sw2 = qcril_uim.select_response_info[select_resp_index].sw2;

  if ((qcril_uim.select_response_info[select_resp_index].select_resp_ptr != NULL) &&
      (qcril_uim.select_response_info[select_resp_index].select_resp_len != 0))
  {
    ril_response_ptr->simResponse = qcril_uim_alloc_bin_to_hexstring(
      qcril_uim.select_response_info[select_resp_index].select_resp_ptr,
      qcril_uim.select_response_info[select_resp_index].select_resp_len);
  }

  return RIL_E_SUCCESS;
} /* qcril_uim_update_get_response_apdu */
#endif /* RIL_REQUEST_SIM_APDU || RIL_REQUEST_SIM_TRANSMIT_CHANNEL ||
          RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC || RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL */


/*=========================================================================

  FUNCTION:  qcril_uim_read_binary

===========================================================================*/
/*!
    @brief
    Function to read transparent

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_read_binary
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 token,
  const RIL_SIM_IO_v6*      request_io_ptr
)
{
  qcril_modem_id_e_type                 modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                                   res;
  RIL_Errno                             err      = RIL_E_SUCCESS;
  qmi_uim_read_transparent_params_type  read_params;
  uint8                                 aid[QCRIL_UIM_MAX_AID_SIZE];
  uint8                                 path[QCRIL_UIM_MAX_PATH_SIZE];
  uint16                                first_level_df_path = 0;
  qcril_uim_original_request_type     * callback_request_ptr = NULL;
  uint8                                 slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  qcril_uim_callback_params_type        callback_params;

  if(request_io_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&read_params, 0, sizeof(qmi_uim_read_transparent_params_type));

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  /* Sanity check */
  if ((request_io_ptr->p1 < 0) || (request_io_ptr->p1 > 0xFF) ||
      (request_io_ptr->p2 < 0) || (request_io_ptr->p2 > 0xFF) ||
      (request_io_ptr->p3 < 0))
  {
    QCRIL_LOG_ERROR( "Unsupported case, P1: 0x%X, P2: 0x%X, P3: 0x%X \n",
                     request_io_ptr->p1, request_io_ptr->p2, request_io_ptr->p3);
    qcril_uim_response(instance_id, token, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS,
                       NULL, 0, TRUE, NULL);
    return;
  }

  /* File id */
  err = qcril_uim_extract_file_id(request_io_ptr,
                                  &read_params.file_id,
                                  path,
                                  sizeof(path));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_file_id");
    return;
  }

  /* Extract the first level DF */
  if ((read_params.file_id.path.data_len >= 4) && (read_params.file_id.path.data_ptr))
  {
    /* Interested only in 3rd & 4th bytes */
    first_level_df_path = (*(read_params.file_id.path.data_ptr + 2) << 8) |
                          (*(read_params.file_id.path.data_ptr + 3));
  }

  /* Session information */
  err = qcril_uim_extract_session_type(slot,
                                       request_io_ptr->aidPtr,
                                       first_level_df_path,
                                       &read_params.session_info,
                                       aid,
                                       sizeof(aid));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_session_type");
    return;
  }

  QCRIL_LOG_INFO( "Session type found: %d", read_params.session_info.session_type);

  /* Keep track of non-prov session, will be removed in response handling */
  if ((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1) ||
      (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2) ||
      (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3))
  {
    err = qcril_uim_add_non_provisioning_session(&read_params.session_info,
                                                 token);
    if (err != RIL_E_SUCCESS)
    {
      QCRIL_LOG_ERROR("%s", "Error in adding non prov session!");
    }
  }

  /* Read parameters: length and offset */
  read_params.offset = (request_io_ptr->p1) << 8 | (request_io_ptr->p2);
  read_params.length = request_io_ptr->p3;

  if (request_io_ptr->pin2 != NULL)
  {
    qmi_uim_verify_pin_params_type         verify_pin_params;
    qcril_uim_pin2_original_request_type * original_request_ptr = NULL;

    memset(&verify_pin_params, 0, sizeof(qmi_uim_verify_pin_params_type));

    /* Session information: same as for read, except card sessions */
    verify_pin_params.session_info = read_params.session_info;

    /* We try to fake a GSM/CDMA first level DF path only for PIN2 verification */
    if ((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      err = qcril_uim_extract_session_type(slot,
                                           request_io_ptr->aidPtr,
                                           QCRIL_UIM_FILEID_DF_CDMA,
                                           &verify_pin_params.session_info,
                                           NULL,
                                           0);
      if ((err != RIL_E_SUCCESS) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
      {
        err = qcril_uim_extract_session_type(slot,
                                             request_io_ptr->aidPtr,
                                             QCRIL_UIM_FILEID_DF_GSM,
                                             &verify_pin_params.session_info,
                                             NULL,
                                             0);
        if ((err != RIL_E_SUCCESS) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
        {
          err = RIL_E_REQUEST_NOT_SUPPORTED;
          goto send_read_binary_error;
        }
      }
      QCRIL_LOG_INFO( "Provisioning session type found: %d",
                      verify_pin_params.session_info.session_type);
      /* Use this session type until the modem resolves card session issue */
      read_params.session_info = verify_pin_params.session_info;
    }

    /* PIN id: PIN2 */
    verify_pin_params.pin_id = QMI_UIM_PIN_ID_PIN2;

    /* PIN value */
    verify_pin_params.pin_data.data_len = (uint16)(strlen(request_io_ptr->pin2));
    verify_pin_params.pin_data.data_ptr = (uint8*)request_io_ptr->pin2;

    /* Allocate original request */
    original_request_ptr = qcril_uim_clone_read_transparent_request(instance_id,
                                                                    modem_id,
                                                                    token,
                                                                    &read_params);

    if (original_request_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for clone_read_transparent_request!");
      err = QCRIL_UIM_RIL_E_NO_MEMORY;
      goto send_read_binary_error;
    }

    QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "verify pin" );
    res = qcril_qmi_uim_verify_pin(qcril_uim.qmi_handle,
                                   &verify_pin_params,
                                   qmi_uim_internal_pin2_callback,
                                   (void*)original_request_ptr,
                                   NULL);

    if (res < 0)
    {
      qcril_free(original_request_ptr);
      QCRIL_LOG_ERROR("%s", "error in qmi_uim_verify_pin!");
      err = qcril_uim_convert_err_value(res);
      goto send_read_binary_error;
    }

    /* original_request_ptr is freed when the PIN2 callback is received */
    return;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  callback_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         modem_id,
                                                         token,
                                                         request_io_ptr->command,
                                                         read_params.session_info.session_type);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_read_binary_error;
  }

  /* No pin2 was provided: proceed with read transparent */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "read transparent" );

#ifndef QMI_RIL_UTF
  /* If read request is for EF-ICCID and session type is card session, send
     response from cached ICCID file */
  if (((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
       (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
       (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3)) &&
      (read_params.file_id.file_id == QCRIL_UIM_EF_ICCID))
  {
    memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

    callback_params.qmi_rsp_data.qmi_err_code = RIL_E_GENERIC_FAILURE;

    if (qcril_uim.card_status.card[slot].card_state == QMI_UIM_CARD_STATE_PRESENT &&
        qcril_uim.card_iccid_info.iccid_len > 0 &&
        read_params.length + read_params.offset <=
          qcril_uim.card_iccid_info.iccid_len)
    {
      unsigned short read_length = 0;

      if(read_params.length == 0)
      {
        read_length = qcril_uim.card_iccid_info.iccid_len - read_params.offset;
      }
      else
      {
        read_length = read_params.length;
      }
      callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr =
        qcril_malloc(read_length);

      if(callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr != NULL)
      {
        memcpy(callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr,
               &qcril_uim.card_iccid_info.iccid_data[read_params.offset],
               read_length);
        callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_len =
          read_length;
        callback_params.qmi_rsp_data.qmi_err_code = RIL_E_SUCCESS;
      }
    }

    if(callback_params.qmi_rsp_data.qmi_err_code == RIL_E_SUCCESS)
    {
      callback_params.orig_req_data = callback_request_ptr;
      qcril_uim_read_binary_resp(&callback_params);

      /* Client needs to free the memory for raw data */
      if(callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr)
      {
        qcril_free(callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr);
      }
      return;
    }
  }
#endif /* QMI_RIL_UTF */

#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_READ_TRANSPARENT,
                                   qcril_uim.qmi_handle,
                                   &read_params,
                                   qmi_uim_callback,
                                   (void*)callback_request_ptr) >= 0)
  {
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  if (qcril_qmi_uim_read_transparent(qcril_uim.qmi_handle,
                                     &read_params,
                                     NULL,
                                     (void*)callback_request_ptr,
                                     &callback_params.qmi_rsp_data) >= 0)
  {
    callback_params.orig_req_data = callback_request_ptr;
    qcril_uim_read_binary_resp(&callback_params);

    /* Client needs to free the memory for raw data */
    if(callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr)
    {
      qcril_free(callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr);
    }
    return;
  }
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_read_binary_error:
  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);
  qcril_uim_response(instance_id, token, err,
                     NULL, 0, TRUE, "error in qcril_qmi_uim_read_transparent");
  /* Clean up any original request if allocated */
  if (callback_request_ptr)
  {
    qcril_free(callback_request_ptr);
    callback_request_ptr = NULL;
  }

} /* qcril_uim_read_binary */


/*=========================================================================

  FUNCTION:  qcril_uim_read_record

===========================================================================*/
/*!
    @brief
    Function to read record

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_read_record
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 token,
  const RIL_SIM_IO_v6*      request_io_ptr
)
{
  qcril_modem_id_e_type             modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                               res;
  RIL_Errno                         err      = RIL_E_SUCCESS;
  qmi_uim_read_record_params_type   read_params;
  uint8                             aid[QCRIL_UIM_MAX_AID_SIZE];
  uint8                             path[QCRIL_UIM_MAX_PATH_SIZE];
  uint16                            first_level_df_path = 0;
  qcril_uim_original_request_type * callback_request_ptr = NULL;
  uint8                             slot =  QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
#ifndef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  qcril_uim_callback_params_type    callback_params;
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */

  if(request_io_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&read_params, 0, sizeof(qmi_uim_read_record_params_type));

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  /* Sanity check */
  if ((request_io_ptr->p1 < 0) || (request_io_ptr->p1 > 0xFF) ||
      (request_io_ptr->p2 < 0) || (request_io_ptr->p2 > 0xFF) ||
      (request_io_ptr->p3 < 0) || (request_io_ptr->p3 > 0xFF))
  {
    QCRIL_LOG_ERROR( "Unsupported case, P1: 0x%X, P2: 0x%X, P3: 0x%X \n",
                     request_io_ptr->p1, request_io_ptr->p2, request_io_ptr->p3);
    qcril_uim_response(instance_id, token, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS,
                       NULL, 0, TRUE, NULL);
    return;
  }

  /* File id */
  err = qcril_uim_extract_file_id(request_io_ptr,
                                  &read_params.file_id,
                                  path,
                                  sizeof(path));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_file_id");
    return;
  }

  /* Extract the first level DF */
  if ((read_params.file_id.path.data_len >= 4) && (read_params.file_id.path.data_ptr))
  {
    /* Interested only in 3rd & 4th bytes */
    first_level_df_path = (*(read_params.file_id.path.data_ptr + 2) << 8) |
                          (*(read_params.file_id.path.data_ptr + 3));
  }

  /* Session information */
  err = qcril_uim_extract_session_type(slot,
                                       request_io_ptr->aidPtr,
                                       first_level_df_path,
                                       &read_params.session_info,
                                       aid,
                                       sizeof(aid));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_session_type");
    return;
  }

  QCRIL_LOG_INFO( "Session type found: %d", read_params.session_info.session_type);

  /* Keep track of non-prov session, will be removed in response handling */
  if ((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1) ||
      (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2) ||
      (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3))
  {
    err = qcril_uim_add_non_provisioning_session(&read_params.session_info,
                                                 token);
    if (err != RIL_E_SUCCESS)
    {
      QCRIL_LOG_ERROR("%s", "Error in adding non prov session!");
    }
  }

  /* Read parameters: length and record */
  read_params.record = request_io_ptr->p1;
  read_params.length = request_io_ptr->p3;

  /* p2 can take values 2, 3, 4, per 3GPP TS 51.011, however QCRIL
   * does not support next record (2) or previous record (3) reads */
  if (request_io_ptr->p2 != 4)
  {
    QCRIL_LOG_ERROR( "%s: unsupported case P2 = %d\n", __FUNCTION__, request_io_ptr->p2);
    qcril_uim_response(instance_id, token, RIL_E_REQUEST_NOT_SUPPORTED,
                       NULL, 0, TRUE, NULL);
    return;
  }

  if (request_io_ptr->pin2 != NULL)
  {
    qmi_uim_verify_pin_params_type         verify_pin_params;
    qcril_uim_pin2_original_request_type * original_request_ptr = NULL;

    memset(&verify_pin_params, 0, sizeof(qmi_uim_verify_pin_params_type));

    /* Session information: same as for read, except card sessions */
    verify_pin_params.session_info = read_params.session_info;

    /* We try to fake a GSM/CDMA first level DF path only for PIN2 verification */
    if ((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      err = qcril_uim_extract_session_type(slot,
                                           request_io_ptr->aidPtr,
                                           QCRIL_UIM_FILEID_DF_CDMA,
                                           &verify_pin_params.session_info,
                                           NULL,
                                           0);
      if ((err != RIL_E_SUCCESS) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
      {
        err = qcril_uim_extract_session_type(slot,
                                             request_io_ptr->aidPtr,
                                             QCRIL_UIM_FILEID_DF_GSM,
                                             &verify_pin_params.session_info,
                                             NULL,
                                             0);
        if ((err != RIL_E_SUCCESS) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
        {
          err = RIL_E_REQUEST_NOT_SUPPORTED;
          goto send_read_record_error;
        }
      }
      QCRIL_LOG_INFO( "Provisioning session type found: %d",
                      verify_pin_params.session_info.session_type);
      /* Use this session type until the modem resolves card session issue */
      read_params.session_info = verify_pin_params.session_info;
    }

    /* PIN id: PIN2 */
    verify_pin_params.pin_id = QMI_UIM_PIN_ID_PIN2;

    /* PIN value */
    verify_pin_params.pin_data.data_len = (uint16)(strlen(request_io_ptr->pin2));
    verify_pin_params.pin_data.data_ptr = (uint8*)request_io_ptr->pin2;

    /* Allocate original request */
    original_request_ptr = qcril_uim_clone_read_record_request(instance_id,
                                                               modem_id,
                                                               token,
                                                               &read_params);

    if (original_request_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for clone_read_record_request!");
      err = QCRIL_UIM_RIL_E_NO_MEMORY;
      goto send_read_record_error;
    }

    QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "verify pin" );
    res = qcril_qmi_uim_verify_pin(qcril_uim.qmi_handle,
                                   &verify_pin_params,
                                   qmi_uim_internal_pin2_callback,
                                   (void*)original_request_ptr,
                                   NULL);

    if (res < 0)
    {
      qcril_free(original_request_ptr);
      QCRIL_LOG_ERROR("%s", "error in qmi_uim_verify_pin!");
      err = qcril_uim_convert_err_value(res);
      goto send_read_record_error;
    }

    /* original_request_ptr is freed when the PIN2 callback is received */
    return;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  callback_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         modem_id,
                                                         token,
                                                         request_io_ptr->command,
                                                         read_params.session_info.session_type);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_read_record_error;
  }

  /* No pin2 was provided: proceed with read transparent */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "read record" );
#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_READ_RECORD,
                                   qcril_uim.qmi_handle,
                                   &read_params,
                                   qmi_uim_callback,
                                   (void*)callback_request_ptr) >= 0)
  {
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  if (qcril_qmi_uim_read_record(qcril_uim.qmi_handle,
                                &read_params,
                                NULL,
                                (void*)callback_request_ptr,
                                &callback_params.qmi_rsp_data) >= 0)
  {
    callback_params.orig_req_data = callback_request_ptr;
    qcril_uim_read_record_resp(&callback_params);

    /* Client needs to free the memory for raw data */
    if(callback_params.qmi_rsp_data.rsp_data.read_record_rsp.content.data_ptr)
    {
      qcril_free(callback_params.qmi_rsp_data.rsp_data.read_record_rsp.content.data_ptr);
    }
    return;
  }
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_read_record_error:
  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);
  qcril_uim_response(instance_id, token, err,
                     NULL, 0, TRUE, "error in qcril_qmi_uim_read_record");
  /* Clean up any original request if allocated */
  if (callback_request_ptr)
  {
    qcril_free(callback_request_ptr);
    callback_request_ptr = NULL;
  }

} /* qcril_uim_read_record */


/*=========================================================================

  FUNCTION:  qcril_uim_update_binary

===========================================================================*/
/*!
    @brief
    Function to write transparent

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_update_binary
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 token,
  const RIL_SIM_IO_v6*      request_io_ptr
)
{
  qcril_modem_id_e_type                  modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                                    res;
  RIL_Errno                              err;
  qmi_uim_write_transparent_params_type  write_params;
  uint8                                  aid[QCRIL_UIM_MAX_AID_SIZE];
  uint8                                  path[QCRIL_UIM_MAX_PATH_SIZE];
  uint16                                 first_level_df_path = 0;
  qcril_uim_original_request_type      * callback_request_ptr = NULL;
  uint8                                  slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
#ifndef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  qcril_uim_callback_params_type         callback_params;
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */

  if(request_io_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&write_params, 0, sizeof(qmi_uim_write_transparent_params_type));

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  /* Sanity check */
  if ((request_io_ptr->p1 < 0) || (request_io_ptr->p1 > 0xFF) ||
      (request_io_ptr->p2 < 0) || (request_io_ptr->p2 > 0xFF) ||
      (request_io_ptr->p3 < 0))
  {
    QCRIL_LOG_ERROR( "Unsupported case, P1: 0x%X, P2: 0x%X, P3: 0x%X \n",
                     request_io_ptr->p1, request_io_ptr->p2, request_io_ptr->p3);
    qcril_uim_response(instance_id, token, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS,
                       NULL, 0, TRUE, NULL);
    return;
  }

  /* File id */
  err = qcril_uim_extract_file_id(request_io_ptr,
                                  &write_params.file_id,
                                  path,
                                  sizeof(path));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_file_id");
    return;
  }

  /* Extract the first level DF */
  if ((write_params.file_id.path.data_len >= 4) && (write_params.file_id.path.data_ptr))
  {
    /* Interested only in 3rd & 4th bytes */
    first_level_df_path = (*(write_params.file_id.path.data_ptr + 2) << 8) |
                          (*(write_params.file_id.path.data_ptr + 3));
  }

  /* Session information */
  err = qcril_uim_extract_session_type(slot,
                                       request_io_ptr->aidPtr,
                                       first_level_df_path,
                                       &write_params.session_info,
                                       aid,
                                       sizeof(aid));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_session_type");
    return;
  }

  QCRIL_LOG_INFO( "Session type found: %d", write_params.session_info.session_type);

  /* Keep track of non-prov session, will be removed in response handling */
  if ((write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1) ||
      (write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2) ||
      (write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3))
  {
    err = qcril_uim_add_non_provisioning_session(&write_params.session_info,
                                                 token);
    if (err != RIL_E_SUCCESS)
    {
      QCRIL_LOG_ERROR("%s", "Error in adding non prov session!");
    }
  }

  /* Data */
  write_params.data.data_ptr = qcril_uim_alloc_hexstring_to_bin(request_io_ptr->data,
                                                                &write_params.data.data_len);
  if (write_params.data.data_ptr == NULL)
  {
    qcril_uim_response(instance_id, token, QCRIL_UIM_RIL_E_NO_MEMORY, NULL, 0, TRUE,
                       "data_ptr is NULL");
    return;
  }
  if (write_params.data.data_len != request_io_ptr->p3)
  {
    qcril_free(write_params.data.data_ptr);
    qcril_uim_response(instance_id, token, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0, TRUE,
                       "data_len mismatch");
    return;
  }

  /* Write parameters: length and offset */
  write_params.offset = (request_io_ptr->p1) << 8 | (request_io_ptr->p2);

  if (request_io_ptr->pin2 != NULL)
  {
    qmi_uim_verify_pin_params_type         verify_pin_params;
    qcril_uim_pin2_original_request_type * original_request_ptr = NULL;

    memset(&verify_pin_params, 0, sizeof(qmi_uim_verify_pin_params_type));

    /* Session information: same as for write, except card sessions */
    verify_pin_params.session_info = write_params.session_info;

    /* We try to fake a GSM/CDMA first level DF path only for PIN2 verification */
    if ((write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      err = qcril_uim_extract_session_type(slot,
                                           request_io_ptr->aidPtr,
                                           QCRIL_UIM_FILEID_DF_CDMA,
                                           &verify_pin_params.session_info,
                                           NULL,
                                           0);
      if ((err != RIL_E_SUCCESS) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
      {
        err = qcril_uim_extract_session_type(slot,
                                             request_io_ptr->aidPtr,
                                             QCRIL_UIM_FILEID_DF_GSM,
                                             &verify_pin_params.session_info,
                                             NULL,
                                             0);
        if ((err != RIL_E_SUCCESS) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
        {
          err = RIL_E_REQUEST_NOT_SUPPORTED;
          qcril_free(write_params.data.data_ptr);
          goto send_update_binary_error;
        }
      }
      QCRIL_LOG_INFO( "Provisioning session type found: %d",
                      verify_pin_params.session_info.session_type);
      /* Use this session type until the modem resolves card session issue */
      write_params.session_info = verify_pin_params.session_info;
    }

    /* PIN id: PIN2 */
    verify_pin_params.pin_id = QMI_UIM_PIN_ID_PIN2;

    /* PIN value */
    verify_pin_params.pin_data.data_len = (uint16)(strlen(request_io_ptr->pin2));
    verify_pin_params.pin_data.data_ptr = (uint8*)request_io_ptr->pin2;

    /* Allocate original request */
    original_request_ptr = qcril_uim_clone_write_transparent_request(instance_id,
                                                                     modem_id,
                                                                     token,
                                                                     &write_params);

    if (original_request_ptr == NULL)
    {
      qcril_free(write_params.data.data_ptr);
      err = QCRIL_UIM_RIL_E_NO_MEMORY;
      goto send_update_binary_error;
    }

    QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "verify pin" );
    res = qcril_qmi_uim_verify_pin(qcril_uim.qmi_handle,
                                   &verify_pin_params,
                                   qmi_uim_internal_pin2_callback,
                                   (void*)original_request_ptr,
                                   NULL);

    qcril_free(write_params.data.data_ptr);

    if (res < 0)
    {
      qcril_free(original_request_ptr);
      err = qcril_uim_convert_err_value(res);
      goto send_update_binary_error;
    }

    /* original_request_ptr is freed when the PIN2 callback is received */
    return;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  callback_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         modem_id,
                                                         token,
                                                         request_io_ptr->command,
                                                         write_params.session_info.session_type);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    qcril_free(write_params.data.data_ptr);
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_update_binary_error;
  }

  /* No pin2 was provided: proceed with write transparent */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "write transparent" );
#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_WRITE_TRANSPARENT,
                                     qcril_uim.qmi_handle,
                                     &write_params,
                                     qmi_uim_callback,
                                     (void*)callback_request_ptr);

  qcril_free(write_params.data.data_ptr);
  write_params.data.data_ptr = NULL;

  if (res >= 0)
  {
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  res = qcril_qmi_uim_write_transparent(qcril_uim.qmi_handle,
                                        &write_params,
                                        NULL,
                                        (void*)callback_request_ptr,
                                        &callback_params.qmi_rsp_data);

  qcril_free(write_params.data.data_ptr);
  write_params.data.data_ptr = NULL;

  if (res >= 0)
  {
    callback_params.orig_req_data = callback_request_ptr;
    qcril_uim_update_binary_resp(&callback_params);
    return;
  }
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_update_binary_error:
  /* On error, clean up & remove non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);
  qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                     "error in qcril_qmi_uim_write_transparent");
  /* Clean up any original request if allocated */
  if (callback_request_ptr)
  {
    qcril_free(callback_request_ptr);
    callback_request_ptr = NULL;
  }

} /* qcril_uim_update_binary */


/*=========================================================================

  FUNCTION:  qcril_uim_update_record

===========================================================================*/
/*!
    @brief
    Function to write record

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_update_record
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 token,
  const RIL_SIM_IO_v6*      request_io_ptr
)
{
  qcril_modem_id_e_type             modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                               res;
  RIL_Errno                         err;
  qmi_uim_write_record_params_type  write_params;
  uint8                             aid[QCRIL_UIM_MAX_AID_SIZE];
  uint8                             path[QCRIL_UIM_MAX_PATH_SIZE];
  uint16                            first_level_df_path = 0;
  qcril_uim_original_request_type * callback_request_ptr = NULL;
  uint8                             slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
#ifndef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  qcril_uim_callback_params_type    callback_params;
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */

  if(request_io_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&write_params, 0, sizeof(qmi_uim_write_record_params_type));

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  /* Sanity check */
  if ((request_io_ptr->p1 < 0) || (request_io_ptr->p1 > 0xFF) ||
      (request_io_ptr->p2 < 0) || (request_io_ptr->p2 > 0xFF) ||
      (request_io_ptr->p3 < 0) || (request_io_ptr->p3 > 0xFF))
  {
    QCRIL_LOG_ERROR( "Unsupported case, P1: 0x%X, P2: 0x%X, P3: 0x%X \n",
                     request_io_ptr->p1, request_io_ptr->p2, request_io_ptr->p3);
    qcril_uim_response(instance_id, token, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS,
                       NULL, 0, TRUE, NULL);
    return;
  }

  /* File id */
  err = qcril_uim_extract_file_id(request_io_ptr,
                                  &write_params.file_id,
                                  path,
                                  sizeof(path));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_file_id");
    return;
  }

  /* Extract the first level DF */
  if ((write_params.file_id.path.data_len >= 4) && (write_params.file_id.path.data_ptr))
  {
    /* Interested only in 3rd & 4th bytes */
    first_level_df_path = (*(write_params.file_id.path.data_ptr + 2) << 8) |
                          (*(write_params.file_id.path.data_ptr + 3));
  }

  /* Session information */
  err = qcril_uim_extract_session_type(slot,
                                       request_io_ptr->aidPtr,
                                       first_level_df_path,
                                       &write_params.session_info,
                                       aid,
                                       sizeof(aid));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_session_type");
    return;
  }

  QCRIL_LOG_INFO( "Session type found: %d", write_params.session_info.session_type);

  /* Keep track of non-prov session, will be removed in response handling */
  if ((write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1) ||
      (write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2) ||
      (write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3))
  {
    err = qcril_uim_add_non_provisioning_session(&write_params.session_info,
                                                 token);
    if (err != RIL_E_SUCCESS)
    {
      QCRIL_LOG_ERROR("%s", "Error in adding non prov session!");
    }
  }

  /* Data */
  write_params.data.data_ptr = qcril_uim_alloc_hexstring_to_bin(request_io_ptr->data,
                                                                &write_params.data.data_len);
  if (write_params.data.data_ptr == NULL)
  {
    qcril_uim_response(instance_id, token, QCRIL_UIM_RIL_E_NO_MEMORY, NULL, 0, TRUE,
                       "data_ptr is NULL");
    return;
  }
  if (write_params.data.data_len != request_io_ptr->p3)
  {
    qcril_free(write_params.data.data_ptr);
    qcril_uim_response(instance_id, token, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0, TRUE,
                       "data_len mismatch");
    return;
  }

  /* Write parameters: length and offset */
  write_params.record = request_io_ptr->p1;

  /* p2 can take values 2, 3, 4, per 3GPP TS 51.011, however QCRIL
   * does not support next record (2) updates */
  if ((request_io_ptr->p2 != 4) && (request_io_ptr->p2 != 3))
  {
    qcril_free(write_params.data.data_ptr);
    QCRIL_LOG_ERROR( "%s: unsupported case P2 = %d\n", __FUNCTION__, request_io_ptr->p2);
    qcril_uim_response(instance_id, token,
                       RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0, TRUE, NULL);
    return;
  }

  if (request_io_ptr->pin2 != NULL)
  {
    qmi_uim_verify_pin_params_type         verify_pin_params;
    qcril_uim_pin2_original_request_type * original_request_ptr = NULL;

    memset(&verify_pin_params, 0, sizeof(qmi_uim_verify_pin_params_type));

    /* Session information: same as for write, except card sessions */
    verify_pin_params.session_info = write_params.session_info;

    /* We try to fake a GSM/CDMA first level DF path */
    if ((write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (write_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      err = qcril_uim_extract_session_type(slot,
                                           request_io_ptr->aidPtr,
                                           QCRIL_UIM_FILEID_DF_CDMA,
                                           &verify_pin_params.session_info,
                                           NULL,
                                           0);
      if ((err != RIL_E_SUCCESS) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
          (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
      {
        err = qcril_uim_extract_session_type(slot,
                                             request_io_ptr->aidPtr,
                                             QCRIL_UIM_FILEID_DF_GSM,
                                             &verify_pin_params.session_info,
                                             NULL,
                                             0);
        if ((err != RIL_E_SUCCESS) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
            (verify_pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
        {
          err = RIL_E_REQUEST_NOT_SUPPORTED;
          qcril_free(write_params.data.data_ptr);
          goto send_update_record_error;
        }
      }
      QCRIL_LOG_INFO( "Provisioning session type found: %d",
                      verify_pin_params.session_info.session_type);
      /* Use this session type until the modem resolves card session issue */
      write_params.session_info = verify_pin_params.session_info;
    }

    /* PIN id: PIN2 */
    verify_pin_params.pin_id = QMI_UIM_PIN_ID_PIN2;

    /* PIN value */
    verify_pin_params.pin_data.data_len = (uint16)(strlen(request_io_ptr->pin2));
    verify_pin_params.pin_data.data_ptr = (uint8*)request_io_ptr->pin2;

    /* Allocate original request */
    original_request_ptr = qcril_uim_clone_write_record_request(instance_id,
                                                                modem_id,
                                                                token,
                                                                &write_params);

    if (original_request_ptr == NULL)
    {
      qcril_free(write_params.data.data_ptr);
      err = QCRIL_UIM_RIL_E_NO_MEMORY;
      goto send_update_record_error;
    }

    QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "verify pin" );
    res = qcril_qmi_uim_verify_pin(qcril_uim.qmi_handle,
                                   &verify_pin_params,
                                   qmi_uim_internal_pin2_callback,
                                   (void*)original_request_ptr,
                                   NULL);

    qcril_free(write_params.data.data_ptr);
    write_params.data.data_ptr = NULL;

    if (res < 0)
    {
      qcril_free(original_request_ptr);
      err = qcril_uim_convert_err_value(res);
      goto send_update_record_error;
    }

    /* original_request_ptr is freed when the PIN2 callback is received */
    return;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  callback_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         modem_id,
                                                         token,
                                                         request_io_ptr->command,
                                                         write_params.session_info.session_type);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    qcril_free(write_params.data.data_ptr);
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_update_record_error;
  }

  /* No pin2 was provided: proceed with write record */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "write record" );
#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_WRITE_RECORD,
                                     qcril_uim.qmi_handle,
                                     &write_params,
                                     qmi_uim_callback,
                                     (void*)callback_request_ptr);

  qcril_free(write_params.data.data_ptr);
  write_params.data.data_ptr = NULL;

  if (res >= 0)
  {
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  res = qcril_qmi_uim_write_record(qcril_uim.qmi_handle,
                                   &write_params,
                                   NULL,
                                   (void*)callback_request_ptr,
                                   &callback_params.qmi_rsp_data);

  qcril_free(write_params.data.data_ptr);
  write_params.data.data_ptr = NULL;

  if (res >= 0)
  {
    callback_params.orig_req_data = callback_request_ptr;
    qcril_uim_update_record_resp(&callback_params);
    return;
  }

#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_update_record_error:
  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);
  qcril_uim_response(instance_id, token, err,
                     NULL, 0, TRUE, "error in qcril_qmi_uim_write_record");
  /* Clean up any original request if allocated */
  if (callback_request_ptr)
  {
    qcril_free(callback_request_ptr);
    callback_request_ptr = NULL;
  }

} /* qcril_uim_update_record */


/*=========================================================================

  FUNCTION:  qcril_uim_get_response

===========================================================================*/
/*!
    @brief
    Function to get the file attributes

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_get_response
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 token,
  const RIL_SIM_IO_v6*      request_io_ptr
)
{
  qcril_modem_id_e_type                    modem_id = QCRIL_MAX_MODEM_ID - 1;
  RIL_Errno                                err      = RIL_E_SUCCESS;
  qmi_uim_get_file_attributes_params_type  get_attr_params;
  uint8                                    aid[QCRIL_UIM_MAX_AID_SIZE];
  uint8                                    path[QCRIL_UIM_MAX_PATH_SIZE];
  uint16                                   first_level_df_path = 0;
  qcril_uim_original_request_type        * callback_request_ptr = NULL;
  uint8                                    slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
#ifndef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  qcril_uim_callback_params_type           callback_params;
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */

  if(request_io_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "%s Enter \n", __FUNCTION__);

  memset(&get_attr_params, 0, sizeof(qmi_uim_get_file_attributes_params_type));

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  /* File id */
  err = qcril_uim_extract_file_id(request_io_ptr,
                                  &get_attr_params.file_id,
                                  path,
                                  sizeof(path));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token,
                       err, NULL, 0, TRUE, "error in qcril_uim_extract_file_id");
    return;
  }

  /* Extract the first level DF */
  if ((get_attr_params.file_id.path.data_len >= 4) && (get_attr_params.file_id.path.data_ptr))
  {
    /* Interested only in 3rd & 4th bytes */
    first_level_df_path = (*(get_attr_params.file_id.path.data_ptr + 2) << 8) |
                          (*(get_attr_params.file_id.path.data_ptr + 3));
  }

  /* Session information */
  err = qcril_uim_extract_session_type(slot,
                                       request_io_ptr->aidPtr,
                                       first_level_df_path,
                                       &get_attr_params.session_info,
                                       aid,
                                       sizeof(aid));
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_session_type");
    return;
  }

  QCRIL_LOG_INFO( "Session type found: %d", get_attr_params.session_info.session_type);

  /* Keep track of non-prov session, will be removed in response handling */
  if ((get_attr_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1) ||
      (get_attr_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2) ||
      (get_attr_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3))
  {
    err = qcril_uim_add_non_provisioning_session(&get_attr_params.session_info,
                                                 token);
    if (err != RIL_E_SUCCESS)
    {
      QCRIL_LOG_ERROR("%s", "Error in adding non prov session!");
    }
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  callback_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         modem_id,
                                                         token,
                                                         request_io_ptr->command,
                                                         get_attr_params.session_info.session_type);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_get_response_error;
  }

  /* In case of get_file_attributes, the FW cannot pass
     an optional PIN2 */

  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "get file attributes" );
#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_GET_RESPONSE,
                                   qcril_uim.qmi_handle,
                                   &get_attr_params,
                                   qmi_uim_callback,
                                   (void*)callback_request_ptr) >= 0)
  {
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  if (qcril_qmi_uim_get_file_attributes(qcril_uim.qmi_handle,
                                        &get_attr_params,
                                        NULL,
                                        (void*)callback_request_ptr,
                                        &callback_params.qmi_rsp_data) >= 0)
  {
    callback_params.orig_req_data = callback_request_ptr;
    qcril_uim_get_response_resp(&callback_params);

    /* Client needs to free the memory for raw data */
    if(callback_params.qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_ptr)
    {
      qcril_free(callback_params.qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_ptr);
    }
    return;
  }
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_get_response_error:
  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);
  qcril_uim_response(instance_id, token, err,
                     NULL, 0, TRUE, "error in qcril_qmi_uim_get_file_attributes");
  /* Clean up any original request if allocated */
  if (callback_request_ptr)
  {
    qcril_free(callback_request_ptr);
    callback_request_ptr = NULL;
  }

} /* qcril_uim_get_response */


/*===========================================================================

  FUNCTION:  qcril_uim_request_get_fdn_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_INTERNAL_MMGSDI_GET_FDN_STATUS request from QCRIL.
    This is due to handling of RIL_REQUEST_QUERY_FACILITY_LOCK with facility
    string "FD" from the framework.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_fdn_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                     modem_id  = QCRIL_MAX_MODEM_ID - 1;
  uint8                                  ** in_ptr    = NULL;
  RIL_Errno                                 err       = RIL_E_SUCCESS;
  uint8                                     slot      = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  uint16                                    aid_size  = 0;
  uint16                                    first_level_df_path = 0;
  qmi_uim_get_service_status_params_type    service_status_params;
  qcril_uim_original_request_type         * callback_request_ptr = NULL;
#ifndef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  qcril_uim_callback_params_type            callback_params;
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&service_status_params, 0, sizeof(qmi_uim_get_service_status_params_type));

  /* Parse input pin info */
  in_ptr = (uint8 **)(params_ptr->data);

  /* Sanity checks
     in_ptr[0]: facility string code
     in_ptr[1]: password
     in_ptr[2]: service class bit (unused)
     in_ptr[3]: AID value */
  if(in_ptr == NULL || in_ptr[0] == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INTERNAL_ERR, NULL, 0,
                       TRUE, "NULL pointer in facilty string" );
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_get_fdn_status(%s, %s, %s)\n",
                  in_ptr[0],
                  (in_ptr[1] != NULL) ? (const char *)in_ptr[1] : "NULL",
                  (in_ptr[3] != NULL) ? (const char *)in_ptr[3] : "NULL" );

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Check facility string */
  if (memcmp(in_ptr[0], "FD", 2) != 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0,
                       TRUE, "unsupported facilty string" );
    return;
  }

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  /* Update the file path based on passed aid pointer */
  aid_size = (in_ptr[3] == NULL) ? 0 : strlen((const char *)in_ptr[3]);
  first_level_df_path = (aid_size == 0) ?
                         QCRIL_UIM_FILEID_DF_CDMA : QCRIL_UIM_FILEID_ADF_USIM_CSIM;

  /* Extract session type, we need prov session for getting FDN service status */
  err = qcril_uim_extract_session_type(slot,
                                       (const char *)in_ptr[3],
                                       first_level_df_path,
                                       &service_status_params.session_info,
                                       NULL,
                                       0);
  if ((err != RIL_E_SUCCESS) ||
      (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
      (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
      (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
  {
    if (first_level_df_path == QCRIL_UIM_FILEID_DF_CDMA)
    {
      err = qcril_uim_extract_session_type(slot,
                                           (const char *)in_ptr[3],
                                           QCRIL_UIM_FILEID_DF_GSM,
                                           &service_status_params.session_info,
                                           NULL,
                                           0);
    }
    if ((err != RIL_E_SUCCESS) ||
        (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0,
                         TRUE, "error extracting session info");
      return;
    }
  }

  QCRIL_LOG_INFO( "Session type found: %d", service_status_params.session_info.session_type);

  /* Capabilities mask paramter */
  service_status_params.mask = QMI_UIM_CAPS_MASK_SERVICE_FDN;

  /* Allocate original request, it is freed in qmi_uim_callback */
  callback_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id,
                                                         service_status_params.session_info.session_type);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_get_fdn_error;
  }

  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "get service status" );
#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_GET_FDN,
                                   qcril_uim.qmi_handle,
                                   &service_status_params,
                                   qmi_uim_callback,
                                   (void*)callback_request_ptr) >= 0)
  {
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  if (qcril_qmi_uim_get_service_status(qcril_uim.qmi_handle,
                                       &service_status_params,
                                       NULL,
                                       (void*)callback_request_ptr,
                                       &callback_params.qmi_rsp_data) >= 0)
  {
    callback_params.orig_req_data = callback_request_ptr;
    qcril_uim_get_fdn_status_resp(&callback_params);
    return;
  }
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_get_fdn_error:
  qcril_uim_response(params_ptr->instance_id, params_ptr->t, err,
                     NULL, 0, TRUE, "error in qcril_qmi_uim_get_service_status");
  /* Clean up any original request if allocated */
  if (callback_request_ptr)
  {
    qcril_free(callback_request_ptr);
    callback_request_ptr = NULL;
  }

} /* qcril_uim_request_get_fdn_status */


/*===========================================================================

  FUNCTION:  qcril_uim_request_set_fdn_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_INTERNAL_MMGSDI_SET_FDN_STATUS request from QCRIL.
    This is due to handling of RIL_REQUEST_SET_FACILITY_LOCK with facility
    string "FD" from the framework.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_set_fdn_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                     modem_id  = QCRIL_MAX_MODEM_ID - 1;
  uint8                                  ** in_ptr    = NULL;
  int                                       res       = 0;
  RIL_Errno                                 err       = RIL_E_SUCCESS;
  uint8                                     slot      = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  uint16                                    aid_size  = 0;
  uint16                                    first_level_df_path = 0;
  qmi_uim_set_service_status_params_type    service_status_params;
  qcril_uim_original_request_type         * callback_request_ptr = NULL;
#ifndef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  qcril_uim_callback_params_type            callback_params;
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&service_status_params, 0, sizeof(qmi_uim_set_service_status_params_type));

  /* Parse input pin info */
  in_ptr = (uint8 **)(params_ptr->data);

  /* Sanity checks
     in_ptr[0]: facility string code
     in_ptr[1]: lock/unlock
     in_ptr[2]: password
     in_ptr[3]: service class bit (unused)
     in_ptr[4]: AID value */
  if(in_ptr == NULL || in_ptr[0] == NULL || in_ptr[1] == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INTERNAL_ERR, NULL, 0,
                       TRUE, "NULL pointer in facilty string" );
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_set_fdn_status(%s, %s, %s, %s)\n",
                  in_ptr[0],
                  in_ptr[1],
                  (in_ptr[2] != NULL) ? (const char *)in_ptr[2] : "NULL",
                  (in_ptr[4] != NULL) ? (const char *)in_ptr[4] : "NULL");

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Check facility string */
  if (memcmp(in_ptr[0], "FD", 2) != 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0,
                       TRUE, "unsupported facilty string" );
    return;
  }

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  /* Update the file path based on passed aid_ptr */
  aid_size = (in_ptr[4] == NULL) ? 0 : strlen((const char *)in_ptr[4]);
  first_level_df_path = (aid_size == 0) ?
                         QCRIL_UIM_FILEID_DF_CDMA : QCRIL_UIM_FILEID_ADF_USIM_CSIM;

  /* Extract session type, we need prov session for setting FDN status */
  err = qcril_uim_extract_session_type(slot,
                                       (const char *)in_ptr[4],
                                       first_level_df_path,
                                       &service_status_params.session_info,
                                       NULL,
                                       0);
  if ((err != RIL_E_SUCCESS) ||
      (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
      (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
      (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
  {
    if (first_level_df_path == QCRIL_UIM_FILEID_DF_CDMA)
    {
      err = qcril_uim_extract_session_type(slot,
                                           (const char *)in_ptr[4],
                                           QCRIL_UIM_FILEID_DF_GSM,
                                           &service_status_params.session_info,
                                           NULL,
                                           0);
    }
    if ((err != RIL_E_SUCCESS) ||
        (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (service_status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0,
                         TRUE, "error extracting session info");
      return;
    }
  }

  QCRIL_LOG_INFO( "Session type found: %d", service_status_params.session_info.session_type);

  /* service status parameter */
  if (*in_ptr[1] == '0')
  {
    service_status_params.fdn_status = QMI_UIM_SERVICE_DISABLE;
  }
  else if (*in_ptr[1] == '1')
  {
    service_status_params.fdn_status = QMI_UIM_SERVICE_ENABLE;
  }
  else
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                       TRUE, "invalid input paramter data[1]");
    return;
  }

  /* If PIN2 was provided, verify PIN2 */
  if (in_ptr[2] != NULL)
  {
    qmi_uim_verify_pin_params_type         verify_pin_params;
    qcril_uim_pin2_original_request_type * original_request_ptr = NULL;

    memset(&verify_pin_params, 0, sizeof(qmi_uim_verify_pin_params_type));

    /* Session information */
    verify_pin_params.session_info = service_status_params.session_info;

    /* PIN id: PIN2 */
    verify_pin_params.pin_id = QMI_UIM_PIN_ID_PIN2;

    /* PIN value */
    verify_pin_params.pin_data.data_len = (uint16)(strlen((const char*)in_ptr[2]));
    verify_pin_params.pin_data.data_ptr = (uint8*)in_ptr[2];

    /* Allocate original request */
    original_request_ptr = qcril_uim_clone_set_fdn_status_request(params_ptr->instance_id,
                                                                  modem_id,
                                                                  params_ptr->t,
                                                                  &service_status_params);

    if (original_request_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "error allocating memory for clone_set_fdn_status_request!");
      err = QCRIL_UIM_RIL_E_NO_MEMORY;
      goto send_set_fdn_error;
    }

    QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "verify pin" );
    res = qcril_qmi_uim_verify_pin(qcril_uim.qmi_handle,
                                   &verify_pin_params,
                                   qmi_uim_internal_pin2_callback,
                                   (void*)original_request_ptr,
                                   NULL);

    if (res < 0)
    {
      qcril_free(original_request_ptr);
      QCRIL_LOG_ERROR("%s", "error in qmi_uim_verify_pin!");
      err = qcril_uim_convert_err_value(res);
      goto send_set_fdn_error;
    }

    /* original_request_ptr is freed when the PIN2 callback is received */
    return;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  callback_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id,
                                                         service_status_params.session_info.session_type);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_set_fdn_error;
  }

  callback_request_ptr->data.fdn_status = service_status_params.fdn_status;

  /* No pin2 provided, so proceed with set service status */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "set service status" );
#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SET_FDN,
                                   qcril_uim.qmi_handle,
                                   &service_status_params,
                                   qmi_uim_callback,
                                   (void*)callback_request_ptr) >= 0)
  {
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  if (qcril_qmi_uim_set_service_status(qcril_uim.qmi_handle,
                                       &service_status_params,
                                       NULL,
                                       (void*)callback_request_ptr,
                                       &callback_params.qmi_rsp_data) >= 0)
  {
    callback_params.orig_req_data = callback_request_ptr;
    qcril_uim_set_fdn_status_resp(&callback_params);
    return;
  }
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_set_fdn_error:
  qcril_uim_response(params_ptr->instance_id, params_ptr->t, err,
                     NULL, 0, TRUE, "error in qcril_qmi_uim_set_service_status");
  /* Clean up any original request if allocated */
  if (callback_request_ptr)
  {
    qcril_free(callback_request_ptr);
    callback_request_ptr = NULL;
  }

} /* qcril_uim_request_set_fdn_status */


/*=========================================================================

  FUNCTION:  qcril_uim_get_status

===========================================================================*/
/*!
    @brief
    Function to send card status

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_get_status
(
  qcril_instance_id_e_type  instance_id,
  RIL_Token                 token,
  const RIL_SIM_IO_v6*      request_io_ptr
)
{
  qcril_modem_id_e_type             modem_id = QCRIL_MAX_MODEM_ID - 1;
  RIL_Errno                         err      = RIL_E_SUCCESS;
  qmi_uim_status_cmd_params_type    status_params;
  uint8                             aid[QCRIL_UIM_MAX_AID_SIZE];
  qcril_uim_original_request_type * callback_request_ptr = NULL;
  uint8                             slot =  QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
#ifndef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  qcril_uim_callback_params_type    callback_params;
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */

  if(request_io_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&status_params, 0, sizeof(qmi_uim_status_cmd_params_type));

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  /* Extract command mode and response type */
  switch (request_io_ptr->p1)
  {
    case 0:
      status_params.mode = QMI_UIM_STATUS_CMD_MODE_NO_INDICATION;
      break;
    case 1:
      status_params.mode = QMI_UIM_STATUS_CMD_MODE_APP_INITIALIZED;
      break;
    case 2:
      status_params.mode = QMI_UIM_STATUS_CMD_MODE_WILL_TERMINATE_APP;
      break;
    default:
      QCRIL_LOG_ERROR("Unsupported case, P1: 0x%X\n",request_io_ptr->p1);
      qcril_uim_response(instance_id, token, RIL_E_REQUEST_NOT_SUPPORTED,
                         NULL, 0, TRUE, NULL);
      return;
  }

  switch (request_io_ptr->p2)
  {
    case 0:
      status_params.resp_type = QMI_UIM_STATUS_CMD_FCP_RSP;
      break;
    case 1:
      status_params.resp_type = QMI_UIM_STATUS_CMD_AID_RSP;
      break;
    case 12:
      status_params.resp_type = QMI_UIM_STATUS_CMD_NO_DATA_RSP;
      break;
    default:
      QCRIL_LOG_ERROR("Unsupported case, P2: 0x%X\n",request_io_ptr->p2);
      qcril_uim_response(instance_id, token, RIL_E_REQUEST_NOT_SUPPORTED,
                         NULL, 0, TRUE, NULL);
      return;
  }

  /* Extract session information */
  if (request_io_ptr->aidPtr == NULL)
  {
    /* In case of NULL AID, use a GW session if it is active,
       else use a 1x session */
    err = qcril_uim_extract_session_type(slot,
                                         NULL,
                                         QCRIL_UIM_FILEID_DF_GSM,
                                         &status_params.session_info,
                                         NULL,
                                         0);
    if ((err != RIL_E_SUCCESS) ||
        (status_params.session_info.session_type != QMI_UIM_SESSION_TYPE_PRI_GW_PROV &&
         status_params.session_info.session_type != QMI_UIM_SESSION_TYPE_SEC_GW_PROV &&
         status_params.session_info.session_type != QMI_UIM_SESSION_TYPE_TER_GW_PROV))
    {
      err = qcril_uim_extract_session_type(slot,
                                           NULL,
                                           QCRIL_UIM_FILEID_DF_CDMA,
                                           &status_params.session_info,
                                           NULL,
                                           0);
    }
  }
  else
  {
    err = qcril_uim_extract_session_type(slot,
                                         request_io_ptr->aidPtr,
                                         QCRIL_UIM_FILEID_ADF_USIM_CSIM,
                                         &status_params.session_info,
                                         aid,
                                         sizeof(aid));
  }
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(instance_id, token, err, NULL, 0, TRUE,
                       "error in qcril_uim_extract_session_type");
    return;
  }

  QCRIL_LOG_INFO( "Session type found: %d", status_params.session_info.session_type);

  /* Keep track of non-prov session, will be removed in response handling */
  if ((status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1) ||
      (status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2) ||
      (status_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3))
  {
    err = qcril_uim_add_non_provisioning_session(&status_params.session_info,
                                                 token);
    if (err != RIL_E_SUCCESS)
    {
      QCRIL_LOG_ERROR("%s", "Error in adding non prov session!");
    }
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  callback_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         modem_id,
                                                         token,
                                                         request_io_ptr->command,
                                                         status_params.session_info.session_type);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_status_error;
  }

  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "status" );
#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SEND_STATUS,
                                   qcril_uim.qmi_handle,
                                   &status_params,
                                   qmi_uim_callback,
                                   (void*)callback_request_ptr) >= 0)
  {
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  if (qcril_qmi_uim_send_status(qcril_uim.qmi_handle,
                                &status_params,
                                NULL,
                                (void*)callback_request_ptr,
                                &callback_params.qmi_rsp_data) >= 0)
  {
    callback_params.orig_req_data = callback_request_ptr;
    qcril_uim_send_status_resp(&callback_params);

    /* Client needs to free the memory for raw data */
    if(callback_params.qmi_rsp_data.rsp_data.send_status_rsp.status_response.data_ptr)
    {
      qcril_free(callback_params.qmi_rsp_data.rsp_data.send_status_rsp.status_response.data_ptr);
    }
    return;
  }
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_status_error:
  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);
  qcril_uim_response(instance_id, token, err,
                     NULL, 0, TRUE, "error in qcril_uim_get_status");
  /* Clean up any original request if allocated */
  if (callback_request_ptr)
  {
    qcril_free(callback_request_ptr);
    callback_request_ptr = NULL;
  }
} /* qcril_uim_get_status */


/*=========================================================================

  FUNCTION:  qcril_uim_read_binary_resp

===========================================================================*/
/*!
    @brief
    Process the response for read binary.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_read_binary_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  RIL_SIM_IO_Response               ril_response;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retreive original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_read_binary_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.sw1;
  ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.sw2;

  ril_response.simResponse = qcril_uim_alloc_bin_to_hexstring(
                                params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr,
                                params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_len);

  QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=%d sw2=%d data=%s\n",
                    ril_response.sw1, ril_response.sw2,
                    ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);

  /* Workaround until the modem build is ready */
  if (ril_response.sw1 == 0)
  {
    if (params_ptr->qmi_rsp_data.qmi_err_code == RIL_E_SUCCESS)
    {
      ril_response.sw1 = 0x90;
      ril_response.sw2 = 0x00;
    }
    else
    {
      ril_response.sw1 = 0x94;
      ril_response.sw2 = 0x04;
    }
  }

  /* Generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     &ril_response,
                     sizeof(RIL_SIM_IO_Response),
                     TRUE,
                     NULL);

  if (ril_response.simResponse)
  {
    qcril_free(ril_response.simResponse);
    ril_response.simResponse = NULL;
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_read_binary_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_read_record_resp

===========================================================================*/
/*!
    @brief
    Process the response for read record.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_read_record_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  RIL_SIM_IO_Response               ril_response;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_read_record_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.read_record_rsp.sw1;
  ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.read_record_rsp.sw2;

  ril_response.simResponse = qcril_uim_alloc_bin_to_hexstring(
                                params_ptr->qmi_rsp_data.rsp_data.read_record_rsp.content.data_ptr,
                                params_ptr->qmi_rsp_data.rsp_data.read_record_rsp.content.data_len);

  QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=0x%X sw2=0x%X data=%s\n",
                    ril_response.sw1, ril_response.sw2,
                    ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);

  /* Workaround until the modem build is ready */
  if (ril_response.sw1 == 0)
  {
    if (params_ptr->qmi_rsp_data.qmi_err_code == RIL_E_SUCCESS)
    {
      ril_response.sw1 = 0x90;
      ril_response.sw2 = 0x00;
    }
    else
    {
      ril_response.sw1 = 0x94;
      ril_response.sw2 = 0x04;
    }
  }

  /* Generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     &ril_response,
                     sizeof(RIL_SIM_IO_Response),
                     TRUE,
                     NULL);

  if (ril_response.simResponse)
  {
    qcril_free(ril_response.simResponse);
    ril_response.simResponse = NULL;
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_read_record_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_update_binary_resp

===========================================================================*/
/*!
    @brief
    Process the response for write transparent.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_update_binary_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  RIL_SIM_IO_Response               ril_response;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_update_binary_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.write_transparent_rsp.sw1;
  ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.write_transparent_rsp.sw2;

  ril_response.simResponse = NULL;

  QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=%d sw2=%d data=%s\n",
                    ril_response.sw1, ril_response.sw2,
                    ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);

  /* Workaround until the modem build is ready */
  if (ril_response.sw1 == 0)
  {
    if (params_ptr->qmi_rsp_data.qmi_err_code == RIL_E_SUCCESS)
    {
      ril_response.sw1 = 0x90;
      ril_response.sw2 = 0x00;
    }
    else
    {
      ril_response.sw1 = 0x94;
      ril_response.sw2 = 0x04;
    }
  }

  /* Generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     &ril_response,
                     sizeof(RIL_SIM_IO_Response),
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_update_binary_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_update_record_resp

===========================================================================*/
/*!
    @brief
    Process the response for write record.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_update_record_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  RIL_SIM_IO_Response               ril_response;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_update_record_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.write_record_rsp.sw1;
  ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.write_record_rsp.sw2;

  ril_response.simResponse = NULL;

  QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=%d sw2=%d data=%s\n",
                    ril_response.sw1, ril_response.sw2,
                    ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);

  /* Workaround until the modem build is ready */
  if (ril_response.sw1 == 0)
  {
    if (params_ptr->qmi_rsp_data.qmi_err_code == RIL_E_SUCCESS)
    {
      ril_response.sw1 = 0x90;
      ril_response.sw2 = 0x00;
    }
    else
    {
      ril_response.sw1 = 0x94;
      ril_response.sw2 = 0x04;
    }
  }

  /* Generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     &ril_response,
                     sizeof(RIL_SIM_IO_Response),
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_update_record_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_get_response_resp

===========================================================================*/
/*!
    @brief
    Process the response for get file attributes.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_response_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  RIL_SIM_IO_Response               ril_response;
  uint8                             icc_attributes[QCRIL_UIM_GET_RESPONSE_MIN_SIZE+1];
  qcril_uim_original_request_type * original_request_ptr = NULL;
  uint8                             slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;
  slot    = qcril_uim_instance_id_to_slot(original_request_ptr->instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot %d", slot);
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_DEBUG( "qcril_uim_get_response_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.sw1;
  ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.sw2;

  ril_response.simResponse = NULL;

  /* Compose file attributes in ICC format */
  if (params_ptr->qmi_rsp_data.qmi_err_code == 0)
  {
    memset(icc_attributes, 0x00, sizeof(icc_attributes));

    /* If it is a 2G card, just use the raw data if available */
    if ((qcril_uim_find_app_in_slot(slot, QMI_UIM_APP_SIM) ||
         qcril_uim_find_app_in_slot(slot, QMI_UIM_APP_RUIM)) &&
        (params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_ptr != NULL) &&
        (params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_len >= QCRIL_UIM_GET_RESPONSE_MIN_SIZE))
    {
      /* Copy necessary data after sanity check */
      uint16 data_len = (params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_len <= sizeof(icc_attributes)) ?
                          params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_len : sizeof(icc_attributes);
      memcpy(icc_attributes,
             params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_ptr,
             data_len);
    }
    else
    {
      /* File size */
      icc_attributes[2] = (uint8)(params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.file_size >> 8);
      icc_attributes[3] = (uint8)(params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.file_size & 0xFF);
      /* File id */
      icc_attributes[4] = (uint8)(params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.file_id >> 8);
      icc_attributes[5] = (uint8)(params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.file_id & 0xFF);
      /* File type: EF */
      icc_attributes[6] = 0x04;
      switch (params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.file_type)
      {
        case QMI_UIM_FILE_TYPE_LINEAR_FIXED:
          /* Linear fixed */
          icc_attributes[13] = 0x01;
          /* Record size */
          icc_attributes[14] =
            (uint8)(params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.record_size);
          break;
        case QMI_UIM_FILE_TYPE_CYCLIC:
          /* Cyclic */
          icc_attributes[13] = 0x03;
          /* Increase allowed */
          icc_attributes[7] = 1;
          /* Record size */
          icc_attributes[14] =
            (uint8)(params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.record_size);
          break;
        case QMI_UIM_FILE_TYPE_TRANSPARENT:
          /* Transparent */
          icc_attributes[13] = 0x00;
          break;
        default:
          /* non EF! */
          break;
      }
      /* Security attributes - need to be implemented */
      /* File status byte */
      icc_attributes[11] = qcril_uim_get_file_status_byte(
                             params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_len,
                             params_ptr->qmi_rsp_data.rsp_data.get_file_attributes_rsp.raw_value.data_ptr);

      /* Hardcoded to 2 because we only simulate byte 13 and 14 */
      icc_attributes[12] = 2;
    }

    ril_response.simResponse = qcril_uim_alloc_bin_to_hexstring(
                                  icc_attributes,
                                  sizeof(icc_attributes));
  }

  QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=%d sw2=%d data=%s\n",
                    ril_response.sw1, ril_response.sw2,
                    ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);

  /* Workaround until the modem build is ready */
  if (ril_response.sw1 == 0)
  {
    if (params_ptr->qmi_rsp_data.qmi_err_code == RIL_E_SUCCESS)
    {
      ril_response.sw1 = 0x90;
      ril_response.sw2 = 0x00;
    }
    else
    {
      ril_response.sw1 = 0x94;
      ril_response.sw2 = 0x04;
    }
  }

  /* Generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     &ril_response,
                     sizeof(RIL_SIM_IO_Response),
                     TRUE,
                     NULL);

  if (ril_response.simResponse)
  {
    qcril_free(ril_response.simResponse);
    ril_response.simResponse = NULL;
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_get_response_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_parse_gw_imsi

===========================================================================*/
/*!
    @brief
    Parses the data present in EF IMSI for SIM/USIM and packages the
    response in a null terminated ASCII string format.

    The EF structure is defined in 31.102 section 4.2.2:

    <TABLE>
      <TR>
        <TH> Byte(s) </TH>
        <TH> Description </TH>
      </TR>
      <TR>
        <TH> 1 </TH>
        <TD> IMSI length (bytes) </TD>
      </TR>
      <TR>
        <TH> 2 </TH>
        <TD> Bits 0-3: Unused  <BR> Bits 4-7: Digit 1   </TD>
      </TR>
      <TR>
        <TH> 3 to 9  </TH>
        <TD> Bits 0-3: Digit n <BR> Bits 4-7: Digit n+1 </TD>
      </TR>
    </TABLE>

    @return
    Pointer to the memory that contains the IMSI in ASCII string format
*/
/*=========================================================================*/
static char * qcril_uim_parse_gw_imsi
(
  const unsigned char * raw_imsi_ptr,
  unsigned short        raw_imsi_len,
  int                 * parsed_imsi_len_ptr
)
{
  int             src             = 0;
  int             dst             = 0;
  char          * parsed_imsi_ptr = NULL;

  /* Sanity check on input parameters */
  if ((raw_imsi_ptr == NULL) || (parsed_imsi_len_ptr == NULL))
  {
    QCRIL_LOG_ERROR( "Invalid input: raw_imsi_ptr 0x%x, parsed_imsi_len_ptr 0x%x\n",
                     raw_imsi_ptr, parsed_imsi_len_ptr);
    return NULL;
  }

  /* Check for the length of IMSI bytes in the first byte */
  *parsed_imsi_len_ptr = *raw_imsi_ptr;
  if (*parsed_imsi_len_ptr >= raw_imsi_len)
  {
    QCRIL_LOG_ERROR( "Invalid data length %d\n", *parsed_imsi_len_ptr);
    *parsed_imsi_len_ptr = 0;
    return NULL;
  }

  /* Allocate required amount of memory for IMSI in ASCII string format,
     note that it is freed by the caller */
  parsed_imsi_ptr = qcril_malloc((2 * (*parsed_imsi_len_ptr)));
  if (parsed_imsi_ptr == NULL)
  {
    QCRIL_LOG_ERROR( "%s: memory allocation failed\n", __FUNCTION__);
    *parsed_imsi_len_ptr = 0;
    return NULL;
  }

  /* Compose IMSI */
  memset(parsed_imsi_ptr, 0, (2 * (*parsed_imsi_len_ptr)));
  for (src = 1, dst = 0;
      (src <= (*parsed_imsi_len_ptr)) && (dst < ((*parsed_imsi_len_ptr) * 2));
       src++)
  {
    uint8 imsi_nibble = 0;

    QCRIL_LOG_VERBOSE( "IMSI[%d] src=%4x, dst=", src, raw_imsi_ptr[src]);

    /* Only process lower part of byte for second and subsequent bytes */
    if (src > 1)
    {
      imsi_nibble = raw_imsi_ptr[src] & 0x0F;
      if (imsi_nibble < 0x0a)
      {
        parsed_imsi_ptr[dst] = qcril_uim_bin_to_hexchar(imsi_nibble);
        QCRIL_LOG_VERBOSE( "%c", parsed_imsi_ptr[dst]);
        dst++;
      }
    }
    /* Process upper part of byte for all bytes */
    imsi_nibble = raw_imsi_ptr[src] >> 4;
    if (imsi_nibble < 0x0a)
    {
      parsed_imsi_ptr[dst] = qcril_uim_bin_to_hexchar(imsi_nibble);
      QCRIL_LOG_VERBOSE( "%c\n", parsed_imsi_ptr[dst]);
      dst++;
    }
  }

  /* Update parsed IMSI length in bytes - number of ASCII bytes copied plus NULL termination */
  *parsed_imsi_len_ptr = dst + 1;

  return parsed_imsi_ptr;
} /* qcril_uim_parse_gw_imsi */


/*=========================================================================

  FUNCTION:  qcril_uim_parse_1x_imsi

===========================================================================*/
/*!
    @brief
    Parses the data present in EF IMSI_M for RUIM/CSIM and packages the
    response in a null terminated ASCII string format.

    The EF structure is defined in C.S0065-0 section 5.2.2:

    <TABLE>
      <TR>
        <TH> Byte(s) </TH>
        <TH> Description </TH>
      </TR>
      <TR>
        <TH> 1 </TH>
        <TD> IMSI_M_CLASS (1 byte) </TD>
      </TR>
      <TR>
        <TH> 2 to 3 </TH>
        <TD> IMSI_M_S2 from IMSI_M_S (2 bytes) </TD>
      </TR>
      <TR>
        <TH> 4 to 6 </TH>
        <TD> IMSI_M_S1 from IMSI_M_S (3 bytes)  </TD>
      </TR>
      <TR>
        <TH> 7 </TH>
        <TD> IMSI_M_11_12 (1 byte)  </TD>
      </TR>
      <TR>
        <TH> 8 </TH>
        <TD> IMSI_M_PROGRAMMED/IMSI_M_ADDR_NUM (1 byte)  </TD>
      </TR>
      <TR>
        <TH> 9 to 10  </TH>
        <TD> MCC_M (2 bytes)  </TD>
      </TR>
    </TABLE>

    @return
    Pointer to the memory that contains the IMSI in ASCII string format
*/
/*=========================================================================*/
static char * qcril_uim_parse_1x_imsi
(
  const unsigned char * raw_imsi_ptr,
  unsigned short        raw_imsi_len,
  int                 * parsed_imsi_len_ptr
)
{
  uint8     i             = 0;
  uint16    mcc           = 0;
  uint8     mnc           = 0;
  uint32    min1          = 0;
  uint16    min2          = 0;
  uint16    second_three  = 0;
  uint8     thousands     = 0xFF;
  uint16    last_three    = 0;
  uint8     min_to_num[]  = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
  uint8     bcd_to_num[]  = { 0xFF, '1', '2', '3', '4', '5', '6', '7', '8',
                              '9', '0', 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  char          * parsed_imsi_ptr = NULL;

  /* Sanity check on input parameters */
  if ((raw_imsi_ptr == NULL) || (parsed_imsi_len_ptr == NULL))
  {
    QCRIL_LOG_ERROR( "Invalid input: raw_imsi_ptr 0x%x, parsed_imsi_len_ptr 0x%x\n",
                     raw_imsi_ptr, parsed_imsi_len_ptr);
    return NULL;
  }

  /* Check for the minumim length of IMSI_M expected */
  if (raw_imsi_len < QCRIL_UIM_IMSI_M_RAW_SIZE)
  {
    QCRIL_LOG_ERROR( "Invalid data length %d\n", raw_imsi_len);
    return NULL;
  }

  /* Sanity check for IMSI_M_PROGRAMMED indicator */
  if ((raw_imsi_ptr[7] & 0x80) == 0)
  {
    QCRIL_LOG_ERROR("%s", "IMSI_M has not been programmed\n");
    return NULL;
  }

  /* Update parsed length - null terminated ASCII string length */
  *parsed_imsi_len_ptr = QCRIL_UIM_IMSI_M_PARSED_SIZE;

  /* Allocate the number of bytes */
  parsed_imsi_ptr = qcril_malloc(*parsed_imsi_len_ptr);
  if (parsed_imsi_ptr == NULL)
  {
    QCRIL_LOG_ERROR( "%s", "Memory allocation failed for parsed_imsi_ptr! \n");
    *parsed_imsi_len_ptr = 0;
    return NULL;
  }

  memset(parsed_imsi_ptr, 0, *parsed_imsi_len_ptr);

  /* Parse certain parameters */
  mcc           = (((raw_imsi_ptr[9] & 0x03) << 8) + raw_imsi_ptr[8]);
  mnc           = raw_imsi_ptr[6] & 0x7F;
  min1          = ((raw_imsi_ptr[5] <<16) +(raw_imsi_ptr[4] <<8) + raw_imsi_ptr[3]);
  min2          = ((raw_imsi_ptr[2] <<8)  + raw_imsi_ptr[1]);
  /* left 10 bits */
  second_three  = (min1 & 0x00FFC000) >> 14;
  /* middle 4 bits */
  thousands     = (min1 & 0x00003C00) >> 10;
  /* right 10 bits */
  last_three    = (min1 & 0x000003FF);
  thousands     = bcd_to_num[thousands];

  QCRIL_LOG_DEBUG("mcc %d, mnc %d, min2 %d, thousands %d, second_three %d, last_three %d\n",
                  mcc, mnc, min2, thousands, second_three, last_three);

  if ((mcc > 999) || (mnc > 99) || (min2 > 999) || (thousands == 0xFF) ||
      (second_three > 999) || (last_three > 999))
  {
    QCRIL_LOG_ERROR("%s", "Invalid data while parsing IMSI_M \n");
    qcril_free(parsed_imsi_ptr);
    *parsed_imsi_len_ptr = 0;
    return NULL;
  }

  /* Construct ASCII IMSI_M, format:
     <3_digit_MCC><2_digit_11_12_digits><LS_10_digits_IMSI> */
  /* Update MCC - 3 digits */
  parsed_imsi_ptr[i++] = min_to_num[mcc/100];
  mcc %= 100;
  parsed_imsi_ptr[i++] = min_to_num[mcc/10];
  parsed_imsi_ptr[i++] = min_to_num[mcc%10];
  /* Update MNC - 2 digits */
  parsed_imsi_ptr[i++] = min_to_num[mnc/10];
  parsed_imsi_ptr[i++] = min_to_num[mnc%10];
  /* Update the first 3 digits of IMSI */
  parsed_imsi_ptr[i++] = min_to_num[min2/100];
  min2 %= 100;
  parsed_imsi_ptr[i++] = min_to_num[min2/10];
  parsed_imsi_ptr[i++] = min_to_num[min2%10];
  /* Update the last 7 digits of IMSI */
  parsed_imsi_ptr[i++] = min_to_num[second_three/100];
  second_three %= 100;
  parsed_imsi_ptr[i++] = min_to_num[second_three/10];
  parsed_imsi_ptr[i++] = min_to_num[second_three%10];
  parsed_imsi_ptr[i++] = thousands;
  parsed_imsi_ptr[i++] = min_to_num[last_three/100];
  last_three %= 100;
  parsed_imsi_ptr[i++] = min_to_num[last_three/10];
  parsed_imsi_ptr[i++] = min_to_num[last_three%10];

  QCRIL_LOG_VERBOSE( "Parsed IMSI_M value:[%s]", parsed_imsi_ptr);

  return parsed_imsi_ptr;
} /* qcril_uim_parse_1x_imsi */


/*=========================================================================

  FUNCTION:  qcril_uim_get_imsi_resp

===========================================================================*/
/*!
    @brief
    Processes the response for RIL_REQUEST_GET_IMSI. IMSI/IMSI_M is stored
    as a transparent file on the SIM/USIM/CSIM. This information is contained
    within the data field returned in callback from QMI. The EF structure
    and coding is different between SIM/USIM and RUIM/CSIM and are defined
    in the respective Specifications.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_imsi_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token       = 0;
  RIL_Errno                         ril_err     = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  char                            * imsi_ptr    = NULL;
  int                               imsi_length = 0;
  qcril_uim_original_request_type * original_request_ptr = NULL;
  int                               is_gwl      = FALSE;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = (RIL_Token)original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_get_imsi_resp: token=%d, qmi_err_code=%d, session_type=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code,
                    original_request_ptr->session_type);

  /* Based on the type of request, fetch appropriate IMSI */
  if (ril_err == RIL_E_SUCCESS)
  {
    if((original_request_ptr->session_type == QMI_UIM_SESSION_TYPE_PRI_GW_PROV) ||
       (original_request_ptr->session_type == QMI_UIM_SESSION_TYPE_SEC_GW_PROV) ||
       (original_request_ptr->session_type == QMI_UIM_SESSION_TYPE_TER_GW_PROV))
    {
      imsi_ptr = qcril_uim_parse_gw_imsi(
                  params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr,
                  params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_len,
                  &imsi_length);
      is_gwl = TRUE;
    }
    else if((original_request_ptr->session_type == QMI_UIM_SESSION_TYPE_PRI_1X_PROV) ||
            (original_request_ptr->session_type == QMI_UIM_SESSION_TYPE_SEC_1X_PROV) ||
            (original_request_ptr->session_type == QMI_UIM_SESSION_TYPE_TER_1X_PROV))
    {
      imsi_ptr = qcril_uim_parse_1x_imsi(
                  params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr,
                  params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_len,
                  &imsi_length);
      is_gwl = FALSE;
    }
  }

  qcril_common_update_current_imsi( imsi_ptr, is_gwl );

  if (imsi_ptr == NULL)
  {
    imsi_length = 0;
    ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* No need to call qcril_uim_remove_non_provisioning_session since this
     response happens only for a prov session, continue to generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     imsi_ptr,
                     imsi_length,
                     TRUE,
                     NULL);

  if (imsi_ptr)
  {
    qcril_free(imsi_ptr);
    imsi_ptr = NULL;
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_get_imsi_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_get_fdn_status_resp

===========================================================================*/
/*!
    @brief
    Process the response for get FDN status.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_fdn_status_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  int                               ret_value            = 0;
  boolean                           cm_indication        = FALSE;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_get_fdn_status_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  /* Map the response */
  if (params_ptr->qmi_rsp_data.qmi_err_code == 0)
  {
    switch (params_ptr->qmi_rsp_data.rsp_data.get_service_status_rsp.fdn_status)
    {
      case 0:
        ret_value = 2;
        QCRIL_LOG_INFO( "%s", "FDN service not available\n" );
        break;
      case 1:
        ret_value = 0;
        QCRIL_LOG_INFO( "%s", "FDN is available but disabled\n" );
        break;
      case 2:
        ret_value     = 1;
        cm_indication = TRUE;
        QCRIL_LOG_INFO( "%s", "FDN is available and enabled\n" );
        break;
      default:
        ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
        QCRIL_LOG_ERROR( "FDN status unknown 0x%x\n",
                         params_ptr->qmi_rsp_data.rsp_data.get_service_status_rsp.fdn_status );
        break;
    }
  }

  /* No need to call qcril_uim_remove_non_provisioning_session since this
     response happens only for a prov session, continue to generate response */
  if (original_request_ptr->request_id == QCRIL_EVT_INTERNAL_MMGSDI_GET_FDN_STATUS)
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       &ret_value,
                       sizeof(int),
                       TRUE,
                       NULL);
  }
  else if ((original_request_ptr->request_id == QCRIL_EVT_CM_UPDATE_FDN_STATUS) &&
           (ril_err == RIL_E_SUCCESS))
  {
    /* Send updated FDN status to QCRIL_CM based on the request id */
    if (qcril_process_event(original_request_ptr->instance_id,
                            original_request_ptr->modem_id,
                            QCRIL_EVT_CM_UPDATE_FDN_STATUS,
                            (void *) &cm_indication,
                            sizeof(boolean),
                            token) != E_SUCCESS )
    {
      QCRIL_LOG_ERROR("%s","Internal QCRIL CM Event processing Failed for FDN status update!");
    }
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_get_fdn_status_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_set_fdn_status_resp

===========================================================================*/
/*!
    @brief
    Process the response for set FDN status.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_set_fdn_status_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  qcril_uim_original_request_type * original_request_ptr = NULL;
  boolean                           cm_indication = FALSE;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  if(ril_err == RIL_E_SUCCESS)
  {
    qcril_instance_id_e_type      instance_id = original_request_ptr->instance_id;

    /* Find the correct RILD instance */
#ifdef FEATURE_QCRIL_UIM_QMI_RPC_QCRIL
#ifdef FEATURE_QCRIL_DSDS
    if (qcril_arb_lookup_instance_id_from_session_type(original_request_ptr->session_type,
                                                       &instance_id) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR( "%s", "qcril_instance_id lookup failed!\n");
    }
#else
    QCRIL_LOG_INFO( "%s\n", "Using original request instance_id");
#endif
#else
    instance_id = qmi_ril_get_process_instance_id();
#endif

    QCRIL_LOG_DEBUG("instance_id: %d", instance_id);

    if(original_request_ptr->data.fdn_status == QMI_UIM_SERVICE_ENABLE)
    {
      cm_indication = TRUE;
    }

    /* Send updated FDN status to CM*/
    if ( qcril_process_event(instance_id,
                             original_request_ptr->modem_id,
                             QCRIL_EVT_CM_UPDATE_FDN_STATUS,
                             (void *) &cm_indication,
                             sizeof(boolean),
                             token ) != E_SUCCESS )
    {
      QCRIL_LOG_ERROR("%s","Internal QCRIL CM Event processing Failed for FDN status update!");
    }
  }

  QCRIL_LOG_DEBUG( "qcril_uim_set_fdn_status_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  /* No need to call qcril_uim_remove_non_provisioning_session since this
     response happens only for a prov session, continue to generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     NULL,
                     0,
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_set_fdn_status_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_logical_channel_resp

===========================================================================*/
/*!
    @brief
    Process the response for logical channel command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_logical_channel_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_logical_channel_resp: token=%d qmi_err_code=%d request_id=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code,
                    original_request_ptr->request_id );

  /* No need to call qcril_uim_remove_non_provisioning_session since we do not try
     to open one in the request, continue to generate response */

  /* For open channel request, we send channel id & select response, for
        close channel request, we send NULL in response */
  if (original_request_ptr->request_id == RIL_REQUEST_SIM_OPEN_CHANNEL)
  {
    if (params_ptr->qmi_rsp_data.qmi_err_code == 0)
    {
      uint16   open_ch_rsp_len = 0;
      int    * open_ch_rsp_ptr = NULL;

      QCRIL_LOG_DEBUG("qcril_uim_logical_channel_resp: channel_id=%d, SW1=0x%x, SW2=0x%x",
                      params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.channel_id,
                      params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw1,
                      params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw2);

      /* Note that we are supporting both types of clients by this response:
         1. Clients who expects only channel id in the response. So, we have to store select
            response data if available so we can send it in the next GET RESPONSE APDU request by client
         2. Client who expects both channel id & select response as an int array -
            as per the L release support in ATEL */
      (void)qcril_uim_store_select_response_info(
              &params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp);

      /* Create the data in the form of an int array:
         0th index is the channel id,
         1 to 'len-2' indexes are the select response bytes, one byte per integer array index,
         'len-1' index is SW1, 'len' index is SW2 (last 2 bytes) */
      if ((params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_len >  0) &&
          (params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr != NULL))
      {
        open_ch_rsp_len = sizeof(int) *
          params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_len;
      }

      /* Allocate the total response bytes. Note that even if there are no resp bytes,
         we still have the channel id, SW1/SW2 bytes to return, hence the 3 bytes */
      open_ch_rsp_len += sizeof(int) * 3;
      open_ch_rsp_ptr = (int*)qcril_malloc(open_ch_rsp_len);
      if (open_ch_rsp_ptr == NULL)
      {
        open_ch_rsp_len = 0;
        ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
        QCRIL_LOG_ERROR( "Error allocating open_ch_rsp_ptr for 0x%x bytes", open_ch_rsp_len);
      }
      else
      {
        uint16 int_idx  = 0;
        uint16 byte_idx = 0;

        /* Update the response with necessary info: Channel id, Select resp bytes & SW1/SW2 bytes */
        open_ch_rsp_ptr[int_idx++] = params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.channel_id;
        if ((params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_len >  0) &&
            (params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr != NULL))
        {
          while ((byte_idx <  params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_len) &&
                 (int_idx  <= params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_len))
          {
            open_ch_rsp_ptr[int_idx++] =
              params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.select_response.data_ptr[byte_idx++];
          }
        }
        open_ch_rsp_ptr[int_idx++] = params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw1;
        open_ch_rsp_ptr[int_idx]   = params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw2;
      }

      /* If select response data wasnt present or not allocated, pass only channel id */
      qcril_uim_response(original_request_ptr->instance_id,
                         token,
                         ril_err,
                         open_ch_rsp_ptr,
                         open_ch_rsp_len,
                         TRUE,
                         NULL);
      if (open_ch_rsp_ptr)
      {
        qcril_free(open_ch_rsp_ptr);
        open_ch_rsp_ptr = NULL;
      }
    }
    else
    {
      /* In the cases that open channel fails due to incorrect P1/P2 (this generally
         means that the FCI value was incorrect), retry opening the channel but with
         an alternative FCI value. */
      if (params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw1 == QCRIL_UIM_SW1_WRONG_PARAMS &&
          params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw2 == QCRIL_UIM_SW2_BAD_PARAMS_P1_P2 &&
          original_request_ptr->data.channel_info.fci_value == QCRIL_UIM_FCI_VALUE_FCI_FALLBACK_FCP)
      {
        qmi_uim_open_logical_channel_params_type  open_logical_ch_params;

        QCRIL_LOG_ERROR( "%s\n", "Entering fallback to retry SELECT with alternative FCI value");

        memset(&open_logical_ch_params, 0, sizeof(qmi_uim_open_logical_channel_params_type));

        open_logical_ch_params.aid_present  = QMI_UIM_TRUE;
        open_logical_ch_params.aid.data_ptr = original_request_ptr->data.channel_info.aid_buffer;
        open_logical_ch_params.aid.data_len = original_request_ptr->data.channel_info.aid_size;
        open_logical_ch_params.slot         = original_request_ptr->data.channel_info.slot;
        open_logical_ch_params.file_control_information.is_valid  = QMI_UIM_TRUE;
        open_logical_ch_params.file_control_information.fci_value = QMI_UIM_FCI_VALUE_FCP;

        /* Update new original request FCI value */
        original_request_ptr->data.channel_info.fci_value =
          (qcril_uim_fci_value_type) open_logical_ch_params.file_control_information.fci_value;

        /* Proceed with open logical channel request */
        if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_OPEN_LOGICAL_CHANNEL,
                                         qcril_uim.qmi_handle,
                                         &open_logical_ch_params,
                                         qmi_uim_callback,
                                         (void*)original_request_ptr) >= 0)
        {
          return;
        }
      }

      /* On error, map to appropriate ril error code */
      switch (params_ptr->qmi_rsp_data.qmi_err_code)
      {
        case QMI_ERR_INSUFFICIENT_RESOURCES_V01:
          ril_err = RIL_E_MISSING_RESOURCE;
          break;

        case QMI_ERR_INCOMPATIBLE_STATE_V01:
        case QMI_ERR_SIM_FILE_NOT_FOUND_V01:
          ril_err = RIL_E_NO_SUCH_ELEMENT;
          break;

        default:
          ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
          break;
      }

      /* Validate the status words before filling */
      if(params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw1 == 0x00 &&
         params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw2 == 0x00)
      {
        /* Send NULL in response */
        qcril_uim_response(original_request_ptr->instance_id,
                           token,
                           ril_err,
                           NULL,
                           0,
                           TRUE,
                           NULL);
      }
      else
      {
        /* 1st byte contains channel id, 2nd & 3rd byte contains status words */
        int open_ch_rsp[3] = {0};

        open_ch_rsp[1] = params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw1;
        open_ch_rsp[2] = params_ptr->qmi_rsp_data.rsp_data.logical_channel_rsp.sw2;

        /* Send error status words in response */
        qcril_uim_response(original_request_ptr->instance_id,
                           token,
                           ril_err,
                           open_ch_rsp,
                           sizeof(int) * 3,
                           TRUE,
                           NULL);
      }
    }
  }
  else if (original_request_ptr->request_id == RIL_REQUEST_SIM_CLOSE_CHANNEL)
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       NULL,
                       0,
                       TRUE,
                       NULL);
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_logical_channel_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_reselect_resp

===========================================================================*/
/*!
    @brief
    Process the response for reselect. Note that this request comes via the
    send apdu API, hence the response uses that data type.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_reselect_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  unsigned short                    select_resp_len = 0;
  RIL_SIM_IO_Response               ril_response;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_reselect_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  /* If the error code was 'QMI_SERVICE_ERR_INVALID_QMI_CMD' it means that the reselect QMI API
     is not supported by modem, send reselect cmd via QM_UIM_SEND_APDU to be b/w compatible.
     For any other error code, send the error back to the client */
  if (params_ptr->qmi_rsp_data.qmi_err_code == QMI_ERR_INVALID_QMI_CMD_V01)
  {
    QCRIL_LOG_ERROR( "%s\n", "Fallback to reselect via QMI_UIM_SEND_APDU");

    /* Proceed with send APDU request */
    QCRIL_LOG_QMI( original_request_ptr->modem_id, "qmi_uim_service", "Send APDU" );
    if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SEND_APDU,
                                     qcril_uim.qmi_handle,
                                     &original_request_ptr->data.send_apdu,
                                     qmi_uim_callback,
                                     (void*)original_request_ptr) >= 0)
    {
      /* Upon success, free APDU data pointer here but re-use original_request_ptr */
      if (original_request_ptr->data.send_apdu.apdu.data_ptr)
      {
        qcril_free(original_request_ptr->data.send_apdu.apdu.data_ptr);
        original_request_ptr->data.send_apdu.apdu.data_ptr = NULL;
      }
      return;
    }
  }

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  if (params_ptr->qmi_rsp_data.qmi_err_code == QMI_ERR_NOT_SUPPORTED_V01)
  {
    /* Fake 6F 00 response for not supported reselect AID */
    ril_response.sw1 = QCRIL_UIM_SW1_TECH_PROBLEM;
    ril_response.sw2 = QCRIL_UIM_SW2_TECH_PROBLEM;
  }
  else
  {
    /* SW1 & SW2 are the last 2 bytes of the APDU response */
    ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.reselect_rsp.sw1;
    ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.reselect_rsp.sw2;
    if (params_ptr->qmi_rsp_data.rsp_data.reselect_rsp.select_response.data_len > 0)
    {
      select_resp_len = params_ptr->qmi_rsp_data.rsp_data.reselect_rsp.select_response.data_len;
      ril_response.simResponse = qcril_uim_alloc_bin_to_hexstring(
                                    params_ptr->qmi_rsp_data.rsp_data.reselect_rsp.select_response.data_ptr,
                                    select_resp_len);
    }
    ril_err = RIL_E_SUCCESS;
  }

  QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=0x%X sw2=0x%X data=%s\n",
                    ril_response.sw1, ril_response.sw2,
                    ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

  /* No need to call qcril_uim_remove_non_provisioning_session since we do not try
     to open one in the request, continue to generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     &ril_response,
                     sizeof(RIL_SIM_IO_Response),
                     TRUE,
                     NULL);

  if (ril_response.simResponse)
  {
    qcril_free(ril_response.simResponse);
    ril_response.simResponse = NULL;
  }

  /* Free APDU data pointer if needed */
  if (original_request_ptr->data.send_apdu.apdu.data_ptr)
  {
    qcril_free(original_request_ptr->data.send_apdu.apdu.data_ptr);
    original_request_ptr->data.send_apdu.apdu.data_ptr = NULL;
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_reselect_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_send_apdu_resp

===========================================================================*/
/*!
    @brief
    Process the response for send apdu command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_send_apdu_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  unsigned short                    apdu_len = 0;
  RIL_SIM_IO_Response               ril_response;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_send_apdu_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  /* Special case for long APDUs - we send the response of the long APDU
     stream after we get all the chunks in the corresponding SEND_APDU_INDs */
  if (params_ptr->qmi_rsp_data.qmi_err_code == QMI_ERR_INSUFFICIENT_RESOURCES_V01)
  {
    QCRIL_LOG_DEBUG( "qcril_uim_send_apdu_resp: total_len=0x%x bytes, token=0x%x",
                     params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.total_len,
                     params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.token );

    /* Store only if the Long APDU response TLV is valid. We also need to check
       and handle cases where the INDs might have come earlier than this response.
       Note that original_request_ptr will be freed when we get SEND_APDU_INDs */
    if (qcril_uim.long_apdu_info.in_use == TRUE)
    {
      /* If Indication already came, we need to check incoming info */
      if ((qcril_uim.long_apdu_info.token          ==
             params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.token) &&
          (qcril_uim.long_apdu_info.total_apdu_len ==
             params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.total_len))
      {
        /* If Indication already came & incoming info matches,
           nothing else to do */
        return;
      }

      /* Error condition - mismatch in data, send error if there was any previous
         request & store the current response's token */
      QCRIL_LOG_ERROR("Mismatch with global data, token: 0x%x, total_apdu_len: 0x%x",
                      qcril_uim.long_apdu_info.token,
                      qcril_uim.long_apdu_info.total_apdu_len);
      /* Send error for any pending response & proceed to store current resp info */
      qcril_uim_cleanup_long_apdu_info();
    }

    /* Store response info. We return after successfully storing since
       we expect subsequent INDs */
    if (params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.total_len > 0)
    {
      QCRIL_LOG_INFO("Storing long_apdu_info");
      qcril_uim.long_apdu_info.in_use = TRUE;
      qcril_uim.long_apdu_info.token = params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.token;
      qcril_uim.long_apdu_info.total_apdu_len = params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.total_len;
      qcril_uim.long_apdu_info.original_request_ptr = original_request_ptr;
      return;
    }
  }

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  if (ril_err == RIL_E_SUCCESS)
  {
    /* SW1 & SW2 are the last 2 bytes of the APDU response */
    if (params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_len >= 2)
    {
      apdu_len = params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_len;
      ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_ptr[apdu_len-2];
      ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_ptr[apdu_len-1];

      ril_response.simResponse = qcril_uim_alloc_bin_to_hexstring(
                                    params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_ptr,
                                    apdu_len - 2);
    }
    else
    {
      ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
      QCRIL_LOG_ERROR( "Invalid apdu_response.data_len: 0x%x\n",
                       params_ptr->qmi_rsp_data.rsp_data.send_apdu_rsp.apdu_response.data_len );
    }
  }

  QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=0x%X sw2=0x%X data=%s\n",
                    ril_response.sw1, ril_response.sw2,
                    ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

  /* No need to call qcril_uim_remove_non_provisioning_session since we do not try
     to open one in the request, continue to generate response */
  if(original_request_ptr->request_id == QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ)
  {
    RIL_SlotSendApduResp  slot_apdu_rsp;
    uint32                resp_len  = 0;

    memset(&slot_apdu_rsp, 0, sizeof(slot_apdu_rsp));
    slot_apdu_rsp.slot_id = original_request_ptr->data.slot_id;
    slot_apdu_rsp.ril_err  = ril_err;
    resp_len = sizeof(slot_apdu_rsp.slot_id) + sizeof(slot_apdu_rsp.ril_err);

    slot_apdu_rsp.sw1 = ril_response.sw1;
    slot_apdu_rsp.sw2 = ril_response.sw2;
    resp_len += sizeof(slot_apdu_rsp.sw1) + sizeof(slot_apdu_rsp.sw2);

    if(ril_response.simResponse != NULL)
    {
      memcpy(slot_apdu_rsp.simResponse, ril_response.simResponse, strlen(ril_response.simResponse));
      resp_len += strlen(ril_response.simResponse);
    }

    /* Payload will have actual response */
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       RIL_E_SUCCESS,
                       &slot_apdu_rsp,
                       resp_len,
                       TRUE,
                       NULL);
  }
  else
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       &ril_response,
                       sizeof(RIL_SIM_IO_Response),
                       TRUE,
                       NULL);
  }

  if (ril_response.simResponse)
  {
    qcril_free(ril_response.simResponse);
    ril_response.simResponse = NULL;
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_send_apdu_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_set_apdu_behavior_resp

===========================================================================*/
/*!
    @brief
    Processes the response for set APDU behavior command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_set_apdu_behavior_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  /* Sanity checks */
  if (params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_set_apdu_behavior_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  /* Send the response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     NULL,
                     0,
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_set_apdu_behavior_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_process_send_apdu_ind

===========================================================================*/
/*!
    @brief
    Function for processing send APDU indication. Based on the data received
    in the send APDU response, this routine is responsible for concatenating
    all the chunks of the APDU indication & preparing & sending one long APDU
    to the client.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_send_apdu_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
)
{
  uint16                            remaining_len = 0;
  uint16                            stored_len    = 0;
  qmi_uim_send_apdu_ind_type      * apdu_ind_ptr  = NULL;

  QCRIL_NOTUSED(ret_ptr);
  if(ind_param_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  apdu_ind_ptr = (qmi_uim_send_apdu_ind_type*)&ind_param_ptr->ind_data.send_apdu_ind;

  QCRIL_LOG_INFO("Send APDU Indication - token: 0x%x, total_len: 0x%x, offset: 0x%x, data_len: 0x%x",
                 apdu_ind_ptr->token,
                 apdu_ind_ptr->total_len,
                 apdu_ind_ptr->offset,
                 apdu_ind_ptr->apdu.data_len);

  if (qcril_uim.long_apdu_info.in_use == TRUE)
  {
    /* If Response already came, we need to check incoming info */
    if ((qcril_uim.long_apdu_info.token          != apdu_ind_ptr->token) ||
        (qcril_uim.long_apdu_info.total_apdu_len != apdu_ind_ptr->total_len))
    {
      /* Error condition - mismatch in data, discrd the response */
      QCRIL_LOG_ERROR("Mismatch with global data, token: 0x%x, total_apdu_len: 0x%x",
                    qcril_uim.long_apdu_info.token,
                    qcril_uim.long_apdu_info.total_apdu_len);
      qcril_uim_cleanup_long_apdu_info();
      return;
    }
  }
  else
  {
    /* Response hasn't come yet, we can still store IND info */
    QCRIL_LOG_INFO("long_apdu_info.in_use is FALSE, storing info");
    qcril_uim.long_apdu_info.in_use         = TRUE;
    qcril_uim.long_apdu_info.token          = apdu_ind_ptr->token;
    qcril_uim.long_apdu_info.total_apdu_len = apdu_ind_ptr->total_len;
  }

  /* If this is the first chunk, allocate the buffer. This buffer will
     only be freed at the end of the receiving all the INDs */
  if (qcril_uim.long_apdu_info.apdu_ptr == NULL)
  {
    qcril_uim.long_apdu_info.rx_len = 0;
    qcril_uim.long_apdu_info.apdu_ptr = qcril_malloc(apdu_ind_ptr->total_len);
    if (qcril_uim.long_apdu_info.apdu_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "Couldnt allocate apdu_ptr pointer !");
      qcril_uim.long_apdu_info.in_use = FALSE;
      return;
    }
  }

  /* Find out the remaining APDU buffer length */
  stored_len    = qcril_uim.long_apdu_info.rx_len;
  remaining_len = qcril_uim.long_apdu_info.total_apdu_len - stored_len;

  /* If this chunk cannot fit in our global buffer, discard the IND */
  if ((apdu_ind_ptr->apdu.data_len > remaining_len) ||
      (apdu_ind_ptr->offset  >= qcril_uim.long_apdu_info.total_apdu_len) ||
      ((apdu_ind_ptr->offset + apdu_ind_ptr->apdu.data_len) > qcril_uim.long_apdu_info.total_apdu_len))
  {
    QCRIL_LOG_ERROR("Mismatch with global data, total_apdu_len: 0x%x stored_len: 0x%x, remaining_len: 0x%x",
                    qcril_uim.long_apdu_info.total_apdu_len,
                    stored_len,
                    remaining_len);
    qcril_uim.long_apdu_info.in_use = FALSE;
    return;
  }

  /* Save the data & update the data length */
  memcpy(qcril_uim.long_apdu_info.apdu_ptr + apdu_ind_ptr->offset,
         apdu_ind_ptr->apdu.data_ptr,
         apdu_ind_ptr->apdu.data_len);
  qcril_uim.long_apdu_info.rx_len += apdu_ind_ptr->apdu.data_len;

  /* If it is the last one, send the response back & clean up global buffer */
  if (qcril_uim.long_apdu_info.total_apdu_len == qcril_uim.long_apdu_info.rx_len)
  {
    if (qcril_uim.long_apdu_info.original_request_ptr != NULL)
    {
      RIL_Token           token = qcril_uim.long_apdu_info.original_request_ptr->token;
      unsigned short      apdu_len = qcril_uim.long_apdu_info.rx_len;
      RIL_SIM_IO_Response ril_response;

      QCRIL_LOG_DEBUG("qcril_uim_process_send_apdu_ind: token=0x%x apdu_len=0x%x",
                       qcril_log_get_token_id(token),
                       apdu_len);

      memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));
      ril_response.sw1 = qcril_uim.long_apdu_info.apdu_ptr[apdu_len-2];
      ril_response.sw2 = qcril_uim.long_apdu_info.apdu_ptr[apdu_len-1];
      ril_response.simResponse = qcril_uim_alloc_bin_to_hexstring(
                                   qcril_uim.long_apdu_info.apdu_ptr,
                                   apdu_len - 2);

      QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=0x%X sw2=0x%X data=%s",
                       ril_response.sw1, ril_response.sw2,
                       ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

      /* Generate long APDU response */
      if(qcril_uim.long_apdu_info.original_request_ptr->request_id == QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ)
      {
        RIL_SlotSendApduResp  slot_apdu_rsp;
        uint32                resp_len;

        memset(&slot_apdu_rsp, 0, sizeof(slot_apdu_rsp));
        slot_apdu_rsp.slot_id = qcril_uim.long_apdu_info.original_request_ptr->data.slot_id;
        slot_apdu_rsp.ril_err  = RIL_E_SUCCESS;
        resp_len = sizeof(slot_apdu_rsp.slot_id) + sizeof(slot_apdu_rsp.ril_err);

        slot_apdu_rsp.sw1 = ril_response.sw1;
        slot_apdu_rsp.sw2 = ril_response.sw2;
        resp_len += sizeof(slot_apdu_rsp.sw1) +  sizeof(slot_apdu_rsp.sw2);

        if (ril_response.simResponse != NULL)
        {
          memcpy(slot_apdu_rsp.simResponse, ril_response.simResponse, strlen(ril_response.simResponse));
          resp_len += strlen(ril_response.simResponse);
        }

        qcril_uim_response(qcril_uim.long_apdu_info.original_request_ptr->instance_id,
                           token,
                           RIL_E_SUCCESS,
                           &slot_apdu_rsp,
                           resp_len,
                           TRUE,
                           NULL);
      }
      else
      {
        qcril_uim_response(qcril_uim.long_apdu_info.original_request_ptr->instance_id,
                           token,
                           RIL_E_SUCCESS,
                           &ril_response,
                           sizeof(RIL_SIM_IO_Response),
                           TRUE,
                           NULL);
      }

      if (ril_response.simResponse)
      {
        qcril_free(ril_response.simResponse);
        ril_response.simResponse = NULL;
      }
      /* Free memory allocated originally in the request and set the pointer to
         NULL so that error response is not sent again in the cleanup routine */
      qcril_free(qcril_uim.long_apdu_info.original_request_ptr);
      qcril_uim.long_apdu_info.original_request_ptr = NULL;
    }
    /* Also clean up the global APDU data */
    qcril_uim_cleanup_long_apdu_info();
  }
} /* qcril_uim_process_send_apdu_ind */


/*===========================================================================

  FUNCTION:  qcril_uim_cleanup_long_apdu_info

===========================================================================*/
/*!
    @brief
    Frees if any memory is allocated in global APDU info structure. It also
    sends an error response in case the original request pointer is still
    pending.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_cleanup_long_apdu_info
(
  void
)
{
  /* Free any internal buffers we might have */
  if (qcril_uim.long_apdu_info.apdu_ptr != NULL)
  {
    qcril_free(qcril_uim.long_apdu_info.apdu_ptr);
    qcril_uim.long_apdu_info.apdu_ptr = NULL;
  }
  /* Send error response if request is still pending */
  if (qcril_uim.long_apdu_info.original_request_ptr != NULL)
  {
    RIL_SIM_IO_Response ril_response;

    memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

    QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=0x%X sw2=0x%X data=%s",
                       ril_response.sw1, ril_response.sw2,
                       ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

    /* Generate response */
    if(qcril_uim.long_apdu_info.original_request_ptr->request_id == QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ)
    {
      RIL_SlotSendApduResp  slot_apdu_rsp;
      uint32                resp_len  = 0;

      memset(&slot_apdu_rsp, 0, sizeof(slot_apdu_rsp));
      slot_apdu_rsp.slot_id = qcril_uim.long_apdu_info.original_request_ptr->data.slot_id;
      slot_apdu_rsp.ril_err  = QCRIL_UIM_RIL_E_INTERNAL_ERR;
      resp_len = sizeof(slot_apdu_rsp.slot_id) + sizeof(slot_apdu_rsp.ril_err);

      /* Payload will have actual respone */
      qcril_uim_response(qcril_uim.long_apdu_info.original_request_ptr->instance_id,
                         qcril_uim.long_apdu_info.original_request_ptr->token,
                         RIL_E_SUCCESS,
                         &slot_apdu_rsp,
                         resp_len,
                         TRUE,
                         NULL);
    }
    else
    {
      qcril_uim_response(qcril_uim.long_apdu_info.original_request_ptr->instance_id,
                         qcril_uim.long_apdu_info.original_request_ptr->token,
                         QCRIL_UIM_RIL_E_INTERNAL_ERR,
                         &ril_response,
                         sizeof(RIL_SIM_IO_Response),
                         TRUE,
                         NULL);
    }

    qcril_free(qcril_uim.long_apdu_info.original_request_ptr);
    qcril_uim.long_apdu_info.original_request_ptr = NULL;
  }

  memset(&qcril_uim.long_apdu_info,
         0,
         sizeof(qcril_uim_long_apdu_info_type));
} /* qcril_uim_cleanup_long_apdu_info */


/*===========================================================================

  FUNCTION:  qcril_uim_cleanup_select_response_info

===========================================================================*/
/*!
    @brief
    Cleans the global select response info structure & frees any memory
    allocated for the raw response data.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_cleanup_select_response_info
(
  void
)
{
  uint8  select_resp_index = 0;

  /* Loop thorough all the entries for the particular slot &
     free any cached info we might have */
  for (select_resp_index = 0; select_resp_index < QCRIL_UIM_MAX_SELECT_RESP_COUNT; select_resp_index++)
  {
    if (qcril_uim.select_response_info[select_resp_index].select_resp_ptr != NULL)
    {
      qcril_free(qcril_uim.select_response_info[select_resp_index].select_resp_ptr);
      qcril_uim.select_response_info[select_resp_index].select_resp_ptr = NULL;
    }
    memset(&qcril_uim.select_response_info[select_resp_index],
           0,
           sizeof(qcril_uim_select_response_info_type));
  }
} /* qcril_uim_cleanup_select_response_info */


/*=========================================================================

  FUNCTION:  qcril_uim_sim_authenticate_resp

===========================================================================*/
/*!
    @brief
    Process the response for SIM authenticate command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sim_authenticate_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token         = 0;
  RIL_Errno                         ril_err       = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  char                            * auth_resp_ptr = NULL;
  RIL_SIM_IO_Response               ril_response;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = (RIL_Token)original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_sim_authenticate_resp: token=%d, qmi_err_code=%d, request_id=0x%x\n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code,
                    original_request_ptr->request_id);

  if (ril_err == RIL_E_SUCCESS)
  {
    if (original_request_ptr->request_id == RIL_REQUEST_ISIM_AUTHENTICATION)
    {
      auth_resp_ptr = qcril_uim_alloc_bin_to_base64string(
                        params_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.auth_response.data_ptr,
                        params_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.auth_response.data_len);
      ril_err = (auth_resp_ptr == NULL) ? QCRIL_UIM_RIL_E_NO_MEMORY : RIL_E_SUCCESS;
    }
    else
    {
      ril_response.simResponse = qcril_uim_alloc_bin_to_base64string(
         params_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.auth_response.data_ptr,
         params_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.auth_response.data_len);
      ril_err = (ril_response.simResponse == NULL) ? QCRIL_UIM_RIL_E_NO_MEMORY : RIL_E_SUCCESS;
      ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.sw1;
      ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.sw2;
    }
  }
  else
  {
    /* Propagate SW1 SW2 even in case of authentication error
       ISIM_AUTH structure doesn't have sw1 sw2 */
    if (original_request_ptr->request_id != RIL_REQUEST_ISIM_AUTHENTICATION)
    {
      ril_response.simResponse = NULL;
      ril_response.sw1 = params_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.sw1;
      ril_response.sw2 = params_ptr->qmi_rsp_data.rsp_data.authenticate_rsp.sw2;
    }
  }

  /* Generate response */
  if (original_request_ptr->request_id == RIL_REQUEST_ISIM_AUTHENTICATION)
  {
    /* No need to call qcril_uim_remove_non_provisioning_session since this
       response happens only for a prov session, continue to generate response */
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       auth_resp_ptr,
                       (auth_resp_ptr == NULL) ? 0 : strlen((const char *)auth_resp_ptr),
                       TRUE,
                       NULL);

    if (auth_resp_ptr)
    {
      qcril_free(auth_resp_ptr);
      auth_resp_ptr = NULL;
    }
  }
  else
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       &ril_response,
                       sizeof(ril_response),
                       TRUE,
                       NULL);
    if (ril_response.simResponse)
    {
      qcril_free(ril_response.simResponse);
      ril_response.simResponse = NULL;
    }
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_sim_authenticate_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_send_status_resp

===========================================================================*/
/*!
    @brief
    Process the response for STATUS command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_send_status_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  RIL_SIM_IO_Response               ril_response;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_send_status_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));

  if (params_ptr->qmi_rsp_data.rsp_data.send_status_rsp.status_response.data_len > 0)
  {
    ril_response.simResponse = qcril_uim_alloc_bin_to_hexstring(
                                  params_ptr->qmi_rsp_data.rsp_data.send_status_rsp.status_response.data_ptr,
                                  params_ptr->qmi_rsp_data.rsp_data.send_status_rsp.status_response.data_len);
  }

  if (ril_err == RIL_E_SUCCESS)
  {
    ril_response.sw1 = 0x90;
    ril_response.sw1 = 0x00;
  }

  QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=0x%X sw2=0x%X data=%s",
                   ril_response.sw1, ril_response.sw2,
                   ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

  /* Remove the non-prov session based on the last token */
  qcril_uim_remove_non_provisioning_session(token);

  /* Generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     &ril_response,
                     sizeof(RIL_SIM_IO_Response),
                     TRUE,
                     NULL);

  if (ril_response.simResponse)
  {
    qcril_free(ril_response.simResponse);
    ril_response.simResponse = NULL;
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_send_status_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_file_get_mcc_mnc_get_path_and_session_info

===========================================================================*/
/*!
    @brief
    Helper function to get Path and Session info based on AID

    @return
    RIL_Errno
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_file_get_mcc_mnc_get_path_and_session_info
(
   const char                 *ril_aid_ptr,
   qmi_uim_app_type            app_type,
   qmi_uim_data_type          *file_path_ptr,
   qmi_uim_session_info_type  *session_info_ptr
)
{
  RIL_Errno  ret_err             = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  uint16     first_level_df_path = 0;
  uint32     slot                = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  uint8      file_path[3][4]     = {
                                    {0x3F, 0x00, 0x7F, 0x20}, /* DF GSM        */
                                    {0x3F, 0x00, 0x7F, 0x25}, /* DF CDMA       */
                                    {0x3F, 0x00, 0x7F, 0xFF}  /* ADF USIM/CSIM */
                                   };

  /* Sanity check */
  if (NULL == file_path_ptr || NULL == session_info_ptr)
  {
    QCRIL_LOG_ERROR("NULL pointer, file_path_ptr=0x%x, session_info_ptr=0x%x",
                    file_path_ptr, session_info_ptr);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* Get slot info */
  slot = qmi_ril_get_sim_slot();
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot value 0x%x", slot);
    QCRIL_ASSERT(0);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* Update the file path based on passed aid pointer or app_type if aid is NULL or empty */
  QCRIL_UIM_FREE_IF_NOT_NULL(file_path_ptr->data_ptr);
  if ((ril_aid_ptr && strlen(ril_aid_ptr) > 0) ||
      (app_type == QMI_UIM_APP_CSIM) ||
      (app_type == QMI_UIM_APP_USIM))
  {
    QCRIL_UIM_DUPLICATE(file_path_ptr->data_ptr,
                        file_path[2],
                        QCRIL_UIM_IMSI_PATH_SIZE);
    first_level_df_path = QCRIL_UIM_FILEID_ADF_USIM_CSIM;
  }
  else if (qcril_uim_find_app_in_slot(slot, app_type))
  {
    if (app_type == QMI_UIM_APP_SIM)
    {
      /* If SIM App is present we always get EF-AD or EF-IMSI from DF GSM */
      QCRIL_UIM_DUPLICATE(file_path_ptr->data_ptr,
                          file_path[0],
                          QCRIL_UIM_IMSI_PATH_SIZE);
      first_level_df_path = QCRIL_UIM_FILEID_DF_GSM;
    }
    else if(app_type == QMI_UIM_APP_RUIM)
    {
      /* If SIM App is present we always get EF-AD or EF-IMSI from DF GSM */
      QCRIL_UIM_DUPLICATE(file_path_ptr->data_ptr,
                          file_path[1],
                          QCRIL_UIM_IMSI_PATH_SIZE);
      first_level_df_path = QCRIL_UIM_FILEID_DF_CDMA;
    }
  }
  else
  {
    QCRIL_LOG_ERROR("Not supported App\n");
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  file_path_ptr->data_len = file_path_ptr->data_ptr ? QCRIL_UIM_IMSI_PATH_SIZE : 0;
  if (file_path_ptr->data_ptr)
  {
    /* Extract session information */
    ret_err = qcril_uim_extract_session_type(slot,
                                             ril_aid_ptr,
                                             first_level_df_path,
                                             session_info_ptr,
                                             NULL,
                                             0);
    QCRIL_LOG_INFO("extract_session_type status=0x%x, session_type=0x%x\n",
                   ret_err, session_info_ptr->session_type);

    /* Free allocated memory in case of error */
    if (ret_err != RIL_E_SUCCESS)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(file_path_ptr->data_ptr);
    }
  }
  else
  {
    ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
  }

  return ret_err;
}/* qcril_uim_file_get_mcc_mnc_get_path_and_session_info */


/*=========================================================================

  FUNCTION:  qcril_uim_request_get_mcc_mnc

===========================================================================*/
/*!
    @brief
    Process the request to get MCC and MNC

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_mcc_mnc
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  RIL_Errno                             ret_err              = RIL_E_SUCCESS;
  qcril_uim_original_request_type      *original_request_ptr = NULL;
  qmi_uim_read_transparent_params_type  read_params;
  qcril_mcc_mnc_info_type               mcc_mnc_info;
  RIL_UIM_card_app_type                *app_type_ptr;
  qmi_uim_app_type                      app_type;

  /* Sanity check */
  if(NULL == params_ptr || NULL == ret_ptr)
  {
    QCRIL_LOG_ERROR("Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  if ((params_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (params_ptr->modem_id    >= QCRIL_MAX_MODEM_ID))
  {
    QCRIL_LOG_ERROR("instance_id: 0x%x, modem_id: 0x%x\n",
                    params_ptr->instance_id,
                    params_ptr->modem_id);
    QCRIL_ASSERT(0);
    return;
  }

  memset(&read_params, 0, sizeof(qmi_uim_read_transparent_params_type));

  if (NULL == params_ptr->data || params_ptr->datalen == 0)
  {
    QCRIL_LOG_ERROR("DATA is NULL or datalen is 0");
    ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
    goto send_resp;
  }

  app_type_ptr = (RIL_UIM_card_app_type *)params_ptr->data;

  QCRIL_LOG_INFO("AID Strlen = 0x%x\n",
                 app_type_ptr->aid ? strlen(app_type_ptr->aid) : 0);

  if (app_type_ptr->aid != NULL && strlen(app_type_ptr->aid) > QMI_UIM_MAX_AID_LEN)
  {
    ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
    goto send_resp;
  }

  switch (app_type_ptr->app_type)
  {
    case RIL_APPTYPE_SIM:
      app_type = QMI_UIM_APP_SIM;
      break;
    case RIL_APPTYPE_USIM:
      app_type = QMI_UIM_APP_USIM;
      break;
    case RIL_APPTYPE_RUIM:
      app_type = QMI_UIM_APP_RUIM;
      break;
    case RIL_APPTYPE_CSIM:
      app_type = QMI_UIM_APP_CSIM;
      break;
    default:
      app_type = QMI_UIM_APP_UNKNOWN;
      break;
  }

  /* Get file path and session type info */
  ret_err = qcril_uim_file_get_mcc_mnc_get_path_and_session_info(
                                        app_type_ptr->aid,
                                        app_type,
                                        &read_params.file_id.path,
                                        &read_params.session_info);
  if (ret_err != RIL_E_SUCCESS)
  {
    goto send_resp;
  }

  /* Check session type & update file_id */
  if((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_PRI_GW_PROV) ||
     (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_SEC_GW_PROV) ||
     (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_TER_GW_PROV))
  {
    read_params.file_id.file_id = QCRIL_UIM_FILEID_EF_AD;
  }
  else if ((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_PRI_1X_PROV) ||
           (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_SEC_1X_PROV) ||
           (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_TER_1X_PROV))
  {
    read_params.file_id.file_id = QCRIL_UIM_FILEID_EF_IMSI_M;
  }
  else
  {
    QCRIL_LOG_ERROR("Not proper session type 0x%x for UIM_GET_MCC_MNC",
                    read_params.session_info.session_type);
    ret_err = RIL_E_REQUEST_NOT_SUPPORTED;
    goto send_resp;
  }

  /* Allocate original request */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         params_ptr->modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id,
                                                         read_params.session_info.session_type);
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("error allocating memory for callback_request_ptr!");
    ret_err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_resp;
  }

  /* Save AID, File ID in qcril_uim_original_request_type.data */
  if (app_type_ptr->aid != NULL)
  {
    memcpy(original_request_ptr->data.mcc_mnc_req.aid_buffer,
           app_type_ptr->aid,
           strlen(app_type_ptr->aid));
  }

  original_request_ptr->data.mcc_mnc_req.file_id = read_params.file_id.file_id;

  original_request_ptr->data.mcc_mnc_req.app_type = app_type;

  /* Free data received in the request */
  QCRIL_UIM_FREE_IF_NOT_NULL(app_type_ptr->aid);
  QCRIL_UIM_FREE_IF_NOT_NULL(app_type_ptr);

  /* Proceed with read transparent  */
  QCRIL_LOG_QMI(params_ptr->modem_id, "qmi_uim_service", "read transparent EF-AD or IMSI-M");
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_READ_TRANSPARENT,
                                   qcril_uim.qmi_handle,
                                   &read_params,
                                   qmi_uim_callback,
                                   (void*)original_request_ptr) >= 0)
  {
    /* Free allocated memory for file path */
    QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);
    return;
  }
  QCRIL_LOG_ERROR("Error queueing READ_TRANSPARENT for EF AD");
  ret_err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_resp:
  QCRIL_LOG_DEBUG( "qcril_uim_get_mcc_mnc: ret_err=0x%x\n", ret_err);
  memset(&mcc_mnc_info, 0x00, sizeof(mcc_mnc_info));
  mcc_mnc_info.err_code = ret_err;
  (void)qcril_event_queue(params_ptr->instance_id,
                          params_ptr->modem_id,
                          QCRIL_DATA_ON_STACK,
                          QCRIL_EVT_UIM_MCC_MNC_INFO,
                          (void *) &mcc_mnc_info,
                          sizeof(mcc_mnc_info),
                          (RIL_Token) QCRIL_TOKEN_ID_INTERNAL);

  /* Free allocated memory for file path */
  QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);

  /* Clean up any original request if allocated */
  QCRIL_UIM_FREE_IF_NOT_NULL(original_request_ptr);
}/* qcril_uim_request_get_mcc_mnc */


/*=========================================================================

  FUNCTION:  qcril_uim_get_mcc_mnc_resp_ad

===========================================================================*/
/*!
    @brief
    Process the response for reading EF-AD

    @return
    RIL_Errno
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_get_mcc_mnc_resp_ad
(
   const qmi_uim_data_type          *rsp_data_ptr,
   qcril_uim_original_request_type  *original_request_ptr
)
{
  qmi_uim_read_transparent_params_type  read_params;
  RIL_Errno                             ret_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  int                                   res     = 0;

  /* Basic sanity check, should not happen, check anyway */
  if (NULL == original_request_ptr)
  {
    QCRIL_LOG_ERROR("NULL original_request_ptr");
    return ret_err;
  }

  /* Read EF IMSI */
  memset(&read_params, 0x00, sizeof(read_params));
  ret_err = qcril_uim_file_get_mcc_mnc_get_path_and_session_info(
                      original_request_ptr->data.mcc_mnc_req.aid_buffer,
                      original_request_ptr->data.mcc_mnc_req.app_type,
                      &read_params.file_id.path,
                      &read_params.session_info);

  if (RIL_E_SUCCESS == ret_err)
  {
    /* Check session type & update file_id */
    if((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_PRI_GW_PROV) ||
       (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_SEC_GW_PROV) ||
       (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_TER_GW_PROV))
    {
      read_params.file_id.file_id = QCRIL_UIM_FILEID_EF_IMSI;
      original_request_ptr->data.mcc_mnc_req.file_id        = QCRIL_UIM_FILEID_EF_IMSI;
      original_request_ptr->data.mcc_mnc_req.num_mnc_digits = 0;

      /* Check length of EF AD */
      if (rsp_data_ptr)
      {
        QCRIL_LOG_DEBUG("data_ptr: 0x%x, data_len: 0x%x\n",
                        rsp_data_ptr->data_ptr,
                        rsp_data_ptr->data_len);
        if (rsp_data_ptr->data_len > 3  &&
            rsp_data_ptr->data_ptr      &&
            (2 == rsp_data_ptr->data_ptr[3] || 3 == rsp_data_ptr->data_ptr[3]))
        {
          original_request_ptr->data.mcc_mnc_req.num_mnc_digits = rsp_data_ptr->data_ptr[3];
        }
      }

      /* Proceed with read transparent  */
      QCRIL_LOG_QMI(original_request_ptr->modem_id, "qmi_uim_service", "read transparent EF-IMSI" );
      res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_READ_TRANSPARENT,
                                       qcril_uim.qmi_handle,
                                       &read_params,
                                       qmi_uim_callback,
                                       (void*)original_request_ptr);
      if (res < 0)
      {
        QCRIL_LOG_ERROR("Error queueing READ_TRANSPARENT for IMSI");
        ret_err = QCRIL_UIM_RIL_E_MODEM_ERR;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Not proper session type 0x%x for UIM_GET_MCC_MNC",
                      read_params.session_info.session_type);
      ret_err = RIL_E_REQUEST_NOT_SUPPORTED;
    }
  }

  /* Free allocated memory for file path */
  QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);

  return ret_err;
}/* qcril_uim_get_mcc_mnc_resp_ad */


/*=========================================================================

  FUNCTION:  qcril_uim_get_mcc_mnc_resp_imsi

===========================================================================*/
/*!
    @brief
    Process the response for reading EF-IMSI

    @return
    RIL_Errno
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_get_mcc_mnc_resp_imsi
(
   const qmi_uim_data_type          *rsp_data_ptr,
   qcril_uim_original_request_type  *original_request_ptr,
   qcril_mcc_mnc_info_type          *mcc_mnc_info_ptr
)
{
  uint16      mnc_3digit_mcc_list[] = {302, 310, 311, 312, 313, 314,
                                       315, 316, 334, 348, 405, 344,
                                       365, 722, 342, 350, 346, 732,
                                       366, 750, 352, 358, 354, 708,
                                       338, 356, 360, 374, 376, 1,
                                       2, 3, 4, 5, 6, 7, 8, 9, 10,
                                       11, 12};
  uint16      imsi_mcc              = 0;
  uint8       i                     = 0;
  uint8       imsi_num_mnc_digits   = 2;

  /* Basic sanity check, should not happen, check anyway */
  if (NULL == rsp_data_ptr ||
      NULL == original_request_ptr ||
      NULL == mcc_mnc_info_ptr)
  {
    QCRIL_LOG_ERROR("NULL pointer, original_request_ptr=0x%x, rsp_data_ptr=0x%x, mcc_mnc_info_ptr=0x%x",
                    original_request_ptr, rsp_data_ptr, mcc_mnc_info_ptr);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* Make sure MCC and MNC are NULL terminated */
  memset(mcc_mnc_info_ptr->mcc, 0x00, sizeof(mcc_mnc_info_ptr->mcc));
  memset(mcc_mnc_info_ptr->mnc, 0x00, sizeof(mcc_mnc_info_ptr->mnc));

  if (original_request_ptr->data.mcc_mnc_req.file_id == QCRIL_UIM_FILEID_EF_IMSI)
  {
    QCRIL_LOG_DEBUG("rsp_data_ptr->data_ptr=0x%x, rsp_data_ptr->data_len=0x%x\n",
                    rsp_data_ptr->data_ptr, rsp_data_ptr->data_len);
    if (NULL == rsp_data_ptr->data_ptr ||
        rsp_data_ptr->data_len    != 9 ||
        rsp_data_ptr->data_ptr[0] < 4)
    {
      return QCRIL_UIM_RIL_E_INTERNAL_ERR;
    }

    /* Extract MCC */
    imsi_mcc  = (uint16)((rsp_data_ptr->data_ptr[1] >> 4) & 0x0F) * 100;
    imsi_mcc += (uint16)((rsp_data_ptr->data_ptr[2]) & 0x0F) * 10;
    imsi_mcc += (uint16)((rsp_data_ptr->data_ptr[2] >> 4) & 0x0F);

    mcc_mnc_info_ptr->mcc[0] = qcril_uim_bin_to_hexchar((rsp_data_ptr->data_ptr[1] >> 4) & 0x0F);
    mcc_mnc_info_ptr->mcc[1] = qcril_uim_bin_to_hexchar(rsp_data_ptr->data_ptr[2] & 0x0F);
    mcc_mnc_info_ptr->mcc[2] = qcril_uim_bin_to_hexchar((rsp_data_ptr->data_ptr[2] >> 4) & 0x0F);

    /* The number of digits of MNC depends on the 4th byte of EF-AD.
       hardcoded table is used in case EF-AD has only 3 bytes.*/
    if (original_request_ptr->data.mcc_mnc_req.num_mnc_digits < 2 ||
        original_request_ptr->data.mcc_mnc_req.num_mnc_digits > 3)
    {
      for (i = 0; i < sizeof(mnc_3digit_mcc_list) / sizeof(mnc_3digit_mcc_list[0]); i++)
      {
        if (imsi_mcc == mnc_3digit_mcc_list[i])
        {
          imsi_num_mnc_digits = 3;
          break;
        }
      }
    }
    else
    {
      imsi_num_mnc_digits = original_request_ptr->data.mcc_mnc_req.num_mnc_digits;
    }

    /* Extract MNC */
    mcc_mnc_info_ptr->mnc[0] = qcril_uim_bin_to_hexchar(rsp_data_ptr->data_ptr[3] & 0x0F);
    mcc_mnc_info_ptr->mnc[1] = qcril_uim_bin_to_hexchar((rsp_data_ptr->data_ptr[3] >> 4) & 0x0F);
    if (imsi_num_mnc_digits == 3)
    {
      mcc_mnc_info_ptr->mnc[2] = qcril_uim_bin_to_hexchar(rsp_data_ptr->data_ptr[4] & 0x0F);
    }
  }
  else if (original_request_ptr->data.mcc_mnc_req.file_id == QCRIL_UIM_FILEID_EF_IMSI_M)
  {
    uint16          mcc_in_bcd        = 0;
    uint8           mnc_in_bcd        = 0;
    const char      imsi_m_to_char[]  =
                      { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

    if (NULL == rsp_data_ptr->data_ptr ||
        rsp_data_ptr->data_len != QCRIL_UIM_IMSI_M_RAW_SIZE)
    {
      return QCRIL_UIM_RIL_E_INTERNAL_ERR;
    }

    mcc_in_bcd = (uint16)(((rsp_data_ptr->data_ptr[9] & 0x03) << 8) +
                          rsp_data_ptr->data_ptr[8]);
    mnc_in_bcd = (rsp_data_ptr->data_ptr[6] & 0x7F);

    if (mcc_in_bcd > 999 || mnc_in_bcd > 99)
    {
      return QCRIL_UIM_RIL_E_INTERNAL_ERR;
    }

    /* MCC */
    mcc_mnc_info_ptr->mcc[0] = imsi_m_to_char[mcc_in_bcd / 100];
    mcc_in_bcd %= 100;
    mcc_mnc_info_ptr->mcc[1] = imsi_m_to_char[mcc_in_bcd / 10];
    mcc_mnc_info_ptr->mcc[2] = imsi_m_to_char[mcc_in_bcd % 10];

    /* MNC */
    mcc_mnc_info_ptr->mnc[0] = imsi_m_to_char[mnc_in_bcd / 10];
    mcc_mnc_info_ptr->mnc[1] = imsi_m_to_char[mnc_in_bcd % 10];
  }
  else
  {
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  QCRIL_LOG_DEBUG("mcc[%s], mnc[%s]\n",
                  mcc_mnc_info_ptr->mcc,
                  mcc_mnc_info_ptr->mnc);

  return RIL_E_SUCCESS;
}/* qcril_uim_get_mcc_mnc_resp_imsi */


/*=========================================================================

  FUNCTION:  qcril_uim_get_mcc_mnc_resp

===========================================================================*/
/*!
    @brief
    Processes the response for QCRIL_EVT_INTERNAL_UIM_GET_MCC_MNC

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_mcc_mnc_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token                = 0;
  RIL_Errno                         ret_err              = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  qcril_uim_original_request_type  *original_request_ptr = NULL;
  const qmi_uim_data_type          *rsp_data_ptr         = NULL;
  qcril_mcc_mnc_info_type           mcc_mnc_info;

  if(NULL == params_ptr)
  {
    QCRIL_LOG_ERROR("NULL params_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(NULL == original_request_ptr)
  {
    QCRIL_LOG_ERROR("NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&mcc_mnc_info, 0x00, sizeof(mcc_mnc_info));
  ret_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = (RIL_Token)original_request_ptr->token;
  QCRIL_LOG_DEBUG("qcril_uim_get_mcc_mnc_resp: token=%d, qmi_err_code=%d, session_type=%d, file_id=0x%x\n",
                  qcril_log_get_token_id(token),
                  params_ptr->qmi_rsp_data.qmi_err_code,
                  original_request_ptr->session_type,
                  original_request_ptr->data.mcc_mnc_req.file_id);

  /* Even if reading EF AD fails, still read EF IMSI */
  if (QCRIL_UIM_FILEID_EF_AD == original_request_ptr->data.mcc_mnc_req.file_id)
  {
    if (RIL_E_SUCCESS == ret_err)
    {
      rsp_data_ptr = &(params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content);
    }
    else
    {
      rsp_data_ptr = NULL;
    }

    ret_err = qcril_uim_get_mcc_mnc_resp_ad(rsp_data_ptr, original_request_ptr);
    if (RIL_E_SUCCESS == ret_err)
    {
      /* In the successful case, original request is used in READ_TRANSPARENT
         request for reading IMSI, no need to free */
      return;
    }
  }/* EF-AD */
  else if (RIL_E_SUCCESS               == ret_err  &&
           (QCRIL_UIM_FILEID_EF_IMSI   == original_request_ptr->data.mcc_mnc_req.file_id ||
            QCRIL_UIM_FILEID_EF_IMSI_M == original_request_ptr->data.mcc_mnc_req.file_id))
  {
    rsp_data_ptr = &(params_ptr->qmi_rsp_data.rsp_data.read_transparent_rsp.content);
    ret_err = qcril_uim_get_mcc_mnc_resp_imsi(rsp_data_ptr,
                                              original_request_ptr,
                                              &mcc_mnc_info);
  }/* EF-IMSI */
  else
  {
    ret_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  QCRIL_LOG_DEBUG("ret_err=0x%x\n", ret_err);

  /* Generate QCRIL_EVT_UIM_MCC_MNC_INFO */
  mcc_mnc_info.err_code = ret_err;
  (void)qcril_event_queue(params_ptr->orig_req_data->instance_id,
                          params_ptr->orig_req_data->modem_id,
                          QCRIL_DATA_ON_STACK,
                          QCRIL_EVT_UIM_MCC_MNC_INFO,
                          (void *)&mcc_mnc_info,
                          sizeof(mcc_mnc_info),
                          (RIL_Token)QCRIL_TOKEN_ID_INTERNAL);

  /* Free memory allocated originally in the request */
  QCRIL_UIM_FREE_IF_NOT_NULL(original_request_ptr);
}/* qcril_uim_get_mcc_mnc_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_get_terminal_capability_resp

===========================================================================*/
/*!
    @brief
    Processes the response for get terminal capability command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_terminal_capability_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  qcril_uim_original_request_type * original_request_ptr = NULL;
  RIL_TerminalCapabilityLPATLV      resp;
  uint8_t                           i                    = 0;

  memset(&resp, 0x00, sizeof(resp));

  /* Sanity checks */
  if (params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  for (i = 0; i < params_ptr->qmi_rsp_data.rsp_data.terminal_capability_rsp.num_tlvs && i < QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX; i++)
  {
    if (params_ptr->qmi_rsp_data.rsp_data.terminal_capability_rsp.tlvs[i].tag == QMI_UIM_TERMINAL_CAPABILITY_LPA_TLV_TAG &&
        params_ptr->qmi_rsp_data.rsp_data.terminal_capability_rsp.tlvs[i].value.data_len >= QMI_UIM_TERMINAL_CAPABILITY_LPA_TLV_LEN &&
        params_ptr->qmi_rsp_data.rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr)
    {
      /* LPA TLV should be one byte long per the spec. So, we try copying
         only first byte of value over. That means that if due to some
         wrong reason the value_len of this TLV, is reported as more than
         one byte by QMIUIM, we simply copy the first byte over, without
         flagging this as an error to QCRIL user. The reason is that QMIUIM
         is always expected to ensure LPA TLV's value is always one byte
         long */
      resp.value = params_ptr->qmi_rsp_data.rsp_data.terminal_capability_rsp.tlvs[i].value.data_ptr[0];
      resp.tlv_present = QMI_UIM_TRUE;
    }
  }

  QCRIL_LOG_DEBUG( "qcril_uim_get_terminal_capability_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  /* Send the response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     &resp,
                     sizeof(RIL_TerminalCapabilityLPATLV),
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  QCRIL_UIM_FREE_IF_NOT_NULL(original_request_ptr);
} /* qcril_uim_get_terminal_capability_resp */


/*===========================================================================*/
/*!
    @brief
    Processes the response for set terminal capability command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_set_terminal_capability_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  /* Sanity checks */
  if (params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_set_terminal_capability_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  /* Send the response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     NULL,
                     0,
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  QCRIL_UIM_FREE_IF_NOT_NULL(original_request_ptr);
} /* qcril_uim_set_terminal_capability_resp */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_request_sim_io

===========================================================================*/
/*!
    @brief
    Handler for RIL_REQUEST_SIM_IO.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_sim_io
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  const RIL_SIM_IO_v6* request_io_ptr = NULL;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  request_io_ptr = (const RIL_SIM_IO_v6*)params_ptr->data;
  if(request_io_ptr == NULL)
  {
    qcril_uim_response(params_ptr->instance_id,
                       params_ptr->t,
                       QCRIL_UIM_RIL_E_INTERNAL_ERR,
                       NULL,
                       0,
                       TRUE,
                       "NULL request_io_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_sim_io(aid: %s)\n",
                  request_io_ptr->aidPtr != NULL ? request_io_ptr->aidPtr : "NULL");

  QCRIL_LOG_INFO( "qcril_uim_request_sim_io(%d, 0x%X, %s, %d, %d, %d, %s, %s)\n",
                  request_io_ptr->command,
                  request_io_ptr->fileid,
                  request_io_ptr->path != NULL ? request_io_ptr->path : "NULL",
                  request_io_ptr->p1,
                  request_io_ptr->p2,
                  request_io_ptr->p3,
                  request_io_ptr->data != NULL ? request_io_ptr->data : "NULL",
                  request_io_ptr->pin2 != NULL ? request_io_ptr->pin2 : "NULL");

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  switch (request_io_ptr->command)
  {
     case SIM_CMD_READ_BINARY:
      qcril_uim_read_binary(params_ptr->instance_id, params_ptr->t, request_io_ptr);
      break;

    case SIM_CMD_READ_RECORD:
      qcril_uim_read_record(params_ptr->instance_id, params_ptr->t, request_io_ptr);
      break;

    case SIM_CMD_GET_RESPONSE:
      qcril_uim_get_response(params_ptr->instance_id, params_ptr->t, request_io_ptr);
      break;

    case SIM_CMD_UPDATE_BINARY:
      qcril_uim_update_binary(params_ptr->instance_id, params_ptr->t, request_io_ptr);
      break;

    case SIM_CMD_RETRIEVE_DATA:
      /* Not implemented */
      QCRIL_LOG_ERROR( "%s", "NOTIMPL: qcril_uim_request_sim_io SIM_CMD_RETRIEVE_DATA\n");
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0, TRUE, NULL);
      break;

    case SIM_CMD_SET_DATA:
      /* Not implemented */
      QCRIL_LOG_ERROR( "%s", "NOTIMPL: qcril_uim_request_sim_io SIM_CMD_SET_DATA\n");
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0, TRUE, NULL);
      break;

    case SIM_CMD_UPDATE_RECORD:
      qcril_uim_update_record(params_ptr->instance_id, params_ptr->t, request_io_ptr);
      break;

    case SIM_CMD_STATUS:
      qcril_uim_get_status(params_ptr->instance_id, params_ptr->t, request_io_ptr);
      break;

    default:
      QCRIL_LOG_ERROR( "%s", "ILLEGAL: qcril_uim_request_sim_io unknown cmd\n");
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0, TRUE, NULL);
      break;
  }
} /* qcril_uim_request_sim_io() */


/*=========================================================================

  FUNCTION:  qcril_uim_get_imsi_path_for_aid

===========================================================================*/
/*!
    @brief
    Gets Imsi path and session type for given aid.

    @return
    RIL_Errno
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_get_imsi_path_for_aid
(
  qmi_uim_read_transparent_params_type * read_params_ptr,
  const char                           * aid_ptr,
  uint8                                  slot
)
{
  uint8                                 path[3][4] =
                                        {
                                          {0x3F, 0x00, 0x7F, 0x20}, /* DF GSM        */
                                          {0x3F, 0x00, 0x7F, 0x25}, /* DF CDMA       */
                                          {0x3F, 0x00, 0x7F, 0xFF}  /* ADF USIM/CSIM */
                                        };
  uint16                                first_level_df_path = 0;
  RIL_Errno                             err                 = RIL_E_SUCCESS;

  if (read_params_ptr == NULL)
  {
    return QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
  }

  read_params_ptr->file_id.path.data_ptr = (unsigned char*)qcril_malloc(sizeof(uint8) * 4);

  if (read_params_ptr->file_id.path.data_ptr == NULL)
  {
    return QCRIL_UIM_RIL_E_NO_MEMORY;
  }

  /* Update the file path based on passed aid pointer */
  if ((aid_ptr == NULL) || (strlen(aid_ptr) == 0))
  {
    /* If SIM App is present we always fetch IMSI from DF GSM */
    if (qcril_uim_find_app_in_slot(slot, QMI_UIM_APP_SIM))
    {
      memcpy(read_params_ptr->file_id.path.data_ptr, path[0], 4);
      first_level_df_path = QCRIL_UIM_FILEID_DF_GSM;
    }
    /* if RUIM app is the only one present (& no SIM App), we fetch IMSI_M from DF CDMA */
    else if (qcril_uim_find_app_in_slot(slot, QMI_UIM_APP_RUIM))
    {
      memcpy(read_params_ptr->file_id.path.data_ptr, path[1], 4);
      first_level_df_path = QCRIL_UIM_FILEID_DF_CDMA;
    }
  }
  else
  {
    memcpy(read_params_ptr->file_id.path.data_ptr, path[2], 4);
    first_level_df_path = QCRIL_UIM_FILEID_ADF_USIM_CSIM;
  }

  /* Extract session information */
  err = qcril_uim_extract_session_type(slot,
                                       aid_ptr,
                                       first_level_df_path,
                                       &read_params_ptr->session_info,
                                       NULL,
                                       0);
  if (err != RIL_E_SUCCESS)
  {
    QCRIL_LOG_ERROR("%s", "error processing input params in RIL_REQUEST_GET_IMSI");
    QCRIL_UIM_FREE_IF_NOT_NULL(read_params_ptr->file_id.path.data_ptr);
    return err;
  }

  QCRIL_LOG_INFO( "session_type for RIL_REQUEST_GET_IMSI: %d\n",
                  read_params_ptr->session_info.session_type );

  /* Check session type & update file_id */
  if((read_params_ptr->session_info.session_type == QMI_UIM_SESSION_TYPE_PRI_GW_PROV) ||
     (read_params_ptr->session_info.session_type == QMI_UIM_SESSION_TYPE_SEC_GW_PROV) ||
     (read_params_ptr->session_info.session_type == QMI_UIM_SESSION_TYPE_TER_GW_PROV))
  {
    read_params_ptr->file_id.file_id = QCRIL_UIM_FILEID_EF_IMSI;
  }
  else if ((read_params_ptr->session_info.session_type == QMI_UIM_SESSION_TYPE_PRI_1X_PROV) ||
           (read_params_ptr->session_info.session_type == QMI_UIM_SESSION_TYPE_SEC_1X_PROV) ||
           (read_params_ptr->session_info.session_type == QMI_UIM_SESSION_TYPE_TER_1X_PROV))
  {
    read_params_ptr->file_id.file_id = QCRIL_UIM_FILEID_EF_IMSI_M;
  }
  else
  {
    QCRIL_LOG_ERROR("%s", "Not proper session type for RIL_REQUEST_GET_IMSI");
    QCRIL_UIM_FREE_IF_NOT_NULL(read_params_ptr->file_id.path.data_ptr);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* Read parameters: length and offset */
  read_params_ptr->file_id.path.data_len = QCRIL_UIM_IMSI_PATH_SIZE;
  read_params_ptr->offset = 0;
  read_params_ptr->length = 0;

  return RIL_E_SUCCESS;
} /* qcril_uim_get_imsi_path_for_aid */


/*===========================================================================

  FUNCTION:  qcril_uim_request_get_imsi

===========================================================================*/
/*!
    @brief
    Handler for RIL_REQUEST_GET_IMSI.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_imsi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                 modem_id = QCRIL_MAX_MODEM_ID - 1;
  uint8                              ** in_ptr    = NULL;
  qmi_uim_read_transparent_params_type  read_params;
  qcril_uim_original_request_type     * original_request_ptr = NULL;
  uint8                                 slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  RIL_Errno                             err      = RIL_E_SUCCESS;
#ifndef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  qcril_uim_callback_params_type        callback_params;
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  in_ptr = (uint8 **)(params_ptr->data);
  if(in_ptr == NULL)
  {
    qcril_uim_response(params_ptr->instance_id,
                       params_ptr->t,
                       RIL_E_INVALID_ARGUMENTS,
                       NULL,
                       0,
                       TRUE,
                       "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&read_params, 0, sizeof(qmi_uim_read_transparent_params_type));

  QCRIL_LOG_INFO( "qcril_uim_request_get_imsi(%s)\n",
                  (in_ptr[0] != NULL) ? (const char *)in_ptr[0] : "NULL" );

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("invalid slot value 0x%x", slot);
    qcril_uim_response(params_ptr->instance_id,
                       params_ptr->t,
                       QCRIL_UIM_RIL_E_INTERNAL_ERR,
                       NULL,
                       0,
                       TRUE,
                       NULL);
    QCRIL_ASSERT(0);
    return;
  }

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  err = qcril_uim_get_imsi_path_for_aid(&read_params, (const char *)in_ptr[0], slot);
  if (err != RIL_E_SUCCESS)
  {
    goto send_imsi_error;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id,
                                                         read_params.session_info.session_type);
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_imsi_error;
  }

  /* Proceed with read transparent  */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "read transparent" );
#ifdef FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_READ_TRANSPARENT,
                                   qcril_uim.qmi_handle,
                                   &read_params,
                                   qmi_uim_callback,
                                   (void*)original_request_ptr) >= 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);
    return;
  }
#else
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  if (qcril_qmi_uim_read_transparent(qcril_uim.qmi_handle,
                                     &read_params,
                                     NULL,
                                     (void*)original_request_ptr,
                                     &callback_params.qmi_rsp_data) >= 0)
  {
    callback_params.orig_req_data = original_request_ptr;
    qcril_uim_get_imsi_resp(&callback_params);

    /* Client needs to free the memory for raw data */
    if(callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr)
    {
      qcril_free(callback_params.qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr);
    }
    QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);
    return;
  }
#endif /* FEATURE_QCRIL_UIM_QMI_SIMIO_ASYNC_CALL */
  err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_imsi_error:
  qcril_uim_response(params_ptr->instance_id, params_ptr->t, err,
                     NULL, 0, TRUE, "error in qcril_qmi_uim_read_transparent");
  /* Clean up any original request if allocated */
  if (original_request_ptr)
  {
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }
  QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr)

} /* qcril_uim_request_get_imsi */


/*===========================================================================

  FUNCTION:  qcril_uim_request_isim_authenticate

===========================================================================*/
/*!
    @brief
    Handler for RIL_REQUEST_ISIM_AUTHENTICATION.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_isim_authenticate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                 modem_id             = QCRIL_MAX_MODEM_ID - 1;
  const char                          * in_ptr               = NULL;
  uint8                                 slot                 = 0;
  uint8                                 isim_index           = 0;
  qcril_uim_original_request_type     * original_request_ptr = NULL;
  qmi_uim_authenticate_params_type      auth_params;
  RIL_Errno                             ret_err              = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&auth_params, 0, sizeof(qmi_uim_authenticate_params_type));

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  in_ptr = params_ptr->data;

  QCRIL_LOG_INFO( "qcril_uim_request_isim_authenticate(data: %s)\n", in_ptr != NULL ? in_ptr : "NULL");

  /* Return with error if data was not provided */
  if ((in_ptr == NULL) || (strlen(in_ptr) == 0))
  {
    QCRIL_LOG_ERROR( "%s", " Invalid input for data \n");
    goto isim_auth_error;
  }

  /* Current RIL interface doesnt provide the ISIM AID, so this is what we do:
     Based on the RIL instance, we check if ISIM App is present in that slot
     and use a non-prov session type of that slot */
  slot   = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  switch(slot)
  {
    case 0:
      auth_params.session_info.session_type = QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1;
      break;

    case 1:
      auth_params.session_info.session_type = QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2;
      break;

    default:
      QCRIL_LOG_ERROR("Invalid qmi_uim slot id: 0x%X", slot);
      ret_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
      goto isim_auth_error;
  }

  if ((ret_err = qcril_uim_extract_isim_index(&isim_index, slot)) != RIL_E_SUCCESS)
  {
    QCRIL_LOG_ERROR( "ISIM app not found for instance_id: 0x%x, slot: 0x%x",
                     params_ptr->instance_id, slot);
    goto isim_auth_error;
  }

  /* Update ISIM AID */
  auth_params.session_info.aid.data_len =
    qcril_uim.card_status.card[slot].application[isim_index].aid_len;
  auth_params.session_info.aid.data_ptr =
    (unsigned char *)qcril_uim.card_status.card[slot].application[isim_index].aid_value;

  /* Update auth parameters */
  auth_params.auth_context       = QMI_UIM_AUTH_CONTEXT_IMS_AKA_SECURITY;
  auth_params.auth_data.data_ptr = qcril_uim_alloc_base64string_to_bin(
                                     in_ptr,
                                     &auth_params.auth_data.data_len);

  if (auth_params.auth_data.data_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Unable to convert input ISIM auth data!");
    ret_err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto isim_auth_error;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id,
                                                         auth_params.session_info.session_type);
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_ptr!");
    ret_err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto isim_auth_error;
  }

  /* Proceed with logical channel request */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "authenticate" );
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_AUTHENTICATE,
                                   qcril_uim.qmi_handle,
                                   &auth_params,
                                   qmi_uim_callback,
                                   (void*)original_request_ptr) >= 0)
  {
    /* For success case, free AID & data buffer & return */

    qcril_free(auth_params.auth_data.data_ptr);
    auth_params.auth_data.data_ptr = NULL;
    return;
  }
  ret_err = QCRIL_UIM_RIL_E_MODEM_ERR;

isim_auth_error:
  qcril_uim_response(params_ptr->instance_id, params_ptr->t, ret_err,
                       NULL, 0, TRUE, "error in qcril_qmi_uim_authentication");

  /* Clean up any original request if allocated */
  if (original_request_ptr)
  {
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }

  /* Free data buffer that was allocated */
  if (auth_params.auth_data.data_ptr)
  {
    qcril_free(auth_params.auth_data.data_ptr);
    auth_params.auth_data.data_ptr = NULL;
  }
} /* qcril_uim_request_isim_authenticate */


#ifdef RIL_REQUEST_SIM_AUTHENTICATION
/*===========================================================================

  FUNCTION:  qcril_uim_request_sim_authenticate

===========================================================================*/
/*!
    @brief
    Handler for RIL_REQUEST_SIM_AUTHENTICATION.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_sim_authenticate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                 modem_id             = QCRIL_MAX_MODEM_ID - 1;
  const RIL_SimAuthentication         * in_ptr               = NULL;
  uint8                                 slot                 = 0;
  uint16                                aid_size             = 0;
  qcril_uim_original_request_type     * original_request_ptr = NULL;
  qmi_uim_authenticate_params_type      auth_params;
  uint8                                 computed_aid[QMI_UIM_MAX_AID_LEN];
  RIL_Errno                             ret_err              = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&auth_params, 0, sizeof(qmi_uim_authenticate_params_type));

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  in_ptr = (RIL_SimAuthentication*)params_ptr->data;

  /* Return with error if data was not provided */
  if (in_ptr == NULL || in_ptr->authData == NULL)
  {
    QCRIL_LOG_ERROR( "%s", " Invalid input for data \n");
    ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
    goto sim_auth_error;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);

  aid_size = (in_ptr->aid == NULL) ? 0 : strlen((const char *)in_ptr->aid);

  if (aid_size != 0)
  {
    /* update session_type */
    if ((ret_err = qcril_uim_extract_session_type(slot,
                                       in_ptr->aid,
                                       QCRIL_UIM_FILEID_ADF_USIM_CSIM,
                                       &auth_params.session_info,
                                       computed_aid,
                                       QMI_UIM_MAX_AID_LEN))
        != RIL_E_SUCCESS)
    {
      QCRIL_LOG_ERROR( "app not found for instance_id: 0x%x, slot: 0x%x",
                       params_ptr->instance_id, slot);
      goto sim_auth_error;
    }

    /* Update auth parameters */
    if ((auth_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1 ||
         auth_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2 ||
         auth_params.session_info.session_type == QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3 ) &&
        (qcril_uim_check_aid_with_app_type((const qmi_uim_data_type *)&auth_params.session_info.aid,
                                           QMI_UIM_APP_ISIM)))
    {
      if (in_ptr->authContext == QCRIL_UIM_AUTH_IMS_AKA)
      {
        auth_params.auth_context = QMI_UIM_AUTH_CONTEXT_IMS_AKA_SECURITY;
      }
      else if (in_ptr->authContext == QCRIL_UIM_AUTH_HTTP_DIGEST_SECURITY_CONTEXT)
      {
        auth_params.auth_context = QMI_UIM_AUTH_CONTEXT_HTTP_DIGEST_SECURITY;
      }
      else
      {
        QCRIL_LOG_ERROR("security context not supported for ISIM: 0x%x", in_ptr->authContext);
        ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
        goto sim_auth_error;
      }
    }
    else if (auth_params.session_info.session_type == QMI_UIM_SESSION_TYPE_PRI_GW_PROV ||
             auth_params.session_info.session_type == QMI_UIM_SESSION_TYPE_SEC_GW_PROV ||
             auth_params.session_info.session_type == QMI_UIM_SESSION_TYPE_TER_GW_PROV ||
             (qcril_uim_check_aid_with_app_type((const qmi_uim_data_type *)&auth_params.session_info.aid,
                                                QMI_UIM_APP_USIM)))
    {
      if (in_ptr->authContext == QCRIL_UIM_AUTH_GSM_CONTEXT)
      {
        auth_params.auth_context = QMI_UIM_AUTH_CONTEXT_GSM_SECURITY;
      }
      else if (in_ptr->authContext == QCRIL_UIM_AUTH_3G_CONTEXT)
      {
        auth_params.auth_context = QMI_UIM_AUTH_CONTEXT_3G_SECURITY;
      }
      else if (in_ptr->authContext == QCRIL_UIM_AUTH_VGCS_VBS_SECURITY_CONTEXT)
      {
        auth_params.auth_context = QMI_UIM_AUTH_CONTEXT_VGCS_VBS_SECURITY;
      }
      else
      {
        QCRIL_LOG_ERROR("security context not supported for USIM: 0x%x", in_ptr->authContext);
        ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
        goto sim_auth_error;
      }
    }
    else /* for CSIM and invalid AID */
    {
      QCRIL_LOG_ERROR("authentication not supported for AID: %s at this point", in_ptr->aid);
      ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
      goto sim_auth_error;
    }
  }
  else /* in_ptr->aid == NULL or AID size is 0 then go with 2g card */
  {
    if(qcril_uim_find_app_in_slot(slot, QMI_UIM_APP_SIM))
    {
      /* update session_type */
      qcril_uim_extract_session_type(slot,
                                     NULL,
                                     QCRIL_UIM_FILEID_DF_GSM,
                                     &auth_params.session_info,
                                     NULL,
                                     0);

      if (auth_params.session_info.session_type != QMI_UIM_SESSION_TYPE_PRI_GW_PROV &&
          auth_params.session_info.session_type != QMI_UIM_SESSION_TYPE_SEC_GW_PROV &&
          auth_params.session_info.session_type != QMI_UIM_SESSION_TYPE_TER_GW_PROV)
      {
        QCRIL_LOG_ERROR("provisioning session not available for run GSM ALGOR: 0x%x",
                        auth_params.session_info.session_type);
        ret_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
        goto sim_auth_error;
      }

      /* Check for QCRIL_UIM_AUTH_GSM_CONTEXT also, android telephony didn't have
         support for authContext 0 for 2G cards */
      if (in_ptr->authContext == 0 ||
          in_ptr->authContext == QCRIL_UIM_AUTH_GSM_CONTEXT)
      {
        auth_params.auth_context = QMI_UIM_AUTH_CONTEXT_RUN_GSM_ALGO;
      }
      else
      {
        QCRIL_LOG_ERROR("Invalid authContext for run GSM ALGOR: 0x%x",
                        in_ptr->authContext);
        ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
        goto sim_auth_error;
      }
    }
    else if (qcril_uim_find_app_in_slot(slot, QMI_UIM_APP_RUIM))
    {
      /* update session_type */
      ret_err = qcril_uim_extract_session_type(slot,
                                               NULL,
                                               QCRIL_UIM_FILEID_DF_CDMA,
                                               &auth_params.session_info,
                                               NULL,
                                               0);
      if(ret_err !=  RIL_E_SUCCESS)
      {
        QCRIL_LOG_ERROR("Unable to map session with error: 0x%x",
                        ret_err);
        goto sim_auth_error;
      }

      if (auth_params.session_info.session_type != QMI_UIM_SESSION_TYPE_PRI_1X_PROV &&
          auth_params.session_info.session_type != QMI_UIM_SESSION_TYPE_SEC_1X_PROV &&
          auth_params.session_info.session_type != QMI_UIM_SESSION_TYPE_TER_1X_PROV)
      {
        QCRIL_LOG_ERROR("provisioning session not available for run CAVE: 0x%x",
                        auth_params.session_info.session_type);
        ret_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
        goto sim_auth_error;
      }

      if (in_ptr->authContext == 0)
      {
        auth_params.auth_context = QMI_UIM_AUTH_CONTEXT_RUN_CAVE_ALGO;
      }
      else
      {
        QCRIL_LOG_ERROR("Invalid authContext for run CAVE ALGO: 0x%x",
                        in_ptr->authContext);
        ret_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
        goto sim_auth_error;
      }
    }
  }

  auth_params.auth_data.data_ptr = qcril_uim_alloc_base64string_to_bin(
                                     in_ptr->authData,
                                     &auth_params.auth_data.data_len);

  if (auth_params.auth_data.data_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Unable to convert input SIM auth data!");
    ret_err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto sim_auth_error;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id,
                                                         auth_params.session_info.session_type);
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_ptr!");
    ret_err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto sim_auth_error;
  }

  /* Proceed with logical channel request */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "authenticate" );
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_AUTHENTICATE,
                                   qcril_uim.qmi_handle,
                                   &auth_params,
                                   qmi_uim_callback,
                                   (void*)original_request_ptr) >= 0)
  {
    /* For success case, free AID & data buffer & return */

    qcril_free(auth_params.auth_data.data_ptr);
    auth_params.auth_data.data_ptr = NULL;
    return;
  }
  ret_err = QCRIL_UIM_RIL_E_MODEM_ERR;

sim_auth_error:
  qcril_uim_response(params_ptr->instance_id, params_ptr->t, ret_err,
                       NULL, 0, TRUE, "error in qcril_qmi_uim_authentication");

  /* Clean up any original request if allocated */
  if (original_request_ptr)
  {
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }

  /* Free data buffer that was allocated */
  if (auth_params.auth_data.data_ptr)
  {
    qcril_free(auth_params.auth_data.data_ptr);
    auth_params.auth_data.data_ptr = NULL;
  }
} /* qcril_uim_request_sim_authenticate */
#endif /* RIL_REQUEST_SIM_AUTHENTICATION */


/*===========================================================================

  FUNCTION:  qcril_uim_aid_log

===========================================================================*/
/*!
    @brief
    Responsible to print the aid in the adb log and QXDM logs

    @params
    aid_ptr       : input param
    print_aid_ptr : output param
    print_aid_len : input param, output param one extra byte for null
                    termination

    @return
    RIL_Errno
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_aid_log
(
  const char *aid_ptr,
  char       *print_aid_ptr,
  uint8       print_aid_len
)
{
  if (aid_ptr == NULL ||
      print_aid_ptr == NULL ||
      print_aid_len == 0)
  {
    return QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
  }

  /* Print only when adb log is not on, AID should not get logged in adb logs */
  if (!qcril_log_adb_on)
  {
    (void)strlcpy(print_aid_ptr,
                  aid_ptr,
                  print_aid_len);
  }
  else
  {
    uint8   aid_len    = 0;
    uint8   i          = 0;

    /* In case adb log, print only first two bytes and other bytest as * if aid bytes less than 5
       print first three and last 2 bytes and other as * if greater than 5 */
    aid_len = strlen(aid_ptr);
    if (aid_len > 4)
    {
      /* Copy first 2 bytes of aid_ptr */
      for (i = 0; i < 4 && i < (print_aid_len - 1); i++)
      {
        print_aid_ptr[i] = aid_ptr[i];
      }
      /* length greater than 5 bytes which is 10 charecters */
      if (aid_len > 10)
      {
        /* copy one more byte of aid_ptr */
        for (; i < 6 && i < (print_aid_len - 1); i++)
        {
          print_aid_ptr[i] = aid_ptr[i];
        }
        /* Remaining bytes as * except last two bytes */
        for (; i < (aid_len - 4) && i < (print_aid_len - 1); i++)
        {
          print_aid_ptr[i] = '*';
        }
        for (; i < aid_len && i < (print_aid_len - 1); i++)
        {
          print_aid_ptr[i] = aid_ptr[i];
        }
      } /* if (aid_len > 10) */
    } /* if (aid_len > 4) */
    /* Remaining bytes print as *: this is required in case aid_len <= 4
       (all bytes are printed with '*' or in case aid_len <= 10
       (only first 3 bytes are printed in clear) */
    for (; i < aid_len && i < (print_aid_len - 1); i++)
    {
      print_aid_ptr[i] = '*';
    }
  } /* else of (!qcril_log_adb_on) */
  return RIL_E_SUCCESS;
} /* qcril_uim_aid_log */


/*===========================================================================

  FUNCTION:  qcril_uim_send_open_logical_ch_req

===========================================================================*/
/*!
    @brief
    Responsible to send QMI Open channel request to the modem. Note that
    we are using the QMI_UIM_OPEN_LOGICAL_CHANNEL command now since we have
    to support the opening of logical channel without specifying the AID TLV.

    @return
    RIL_Errno
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_send_open_logical_ch_req
(
  qcril_instance_id_e_type      instance_id,
  const char                  * aid_ptr,
  int                           p2,
  RIL_Token                     token,
  int                           request_id,
  qmi_uim_slot_type             slot
)
{
  qcril_uim_original_request_type        * original_request_ptr = NULL;
  qmi_uim_open_logical_channel_params_type open_logical_ch_params;
  uint16                                   aid_size             = 0;
  uint8                                    aid_buffer[QMI_UIM_MAX_AID_LEN] = {0};
  qcril_uim_fci_value_type                 qcril_fci_value      = QCRIL_UIM_FCI_VALUE_FCP;
  uint8                                    ara_m_aid[]          = {0xA0, 0x00, 0x00, 0x01, 0x51,
                                                                   0x41, 0x43, 0x4C, 0x00};
  uint8                                    slot_id              = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  char                                     print_aid[QMI_UIM_MAX_AID_LEN + 1] = {'\0'};

  memset(&open_logical_ch_params, 0, sizeof(qmi_uim_open_logical_channel_params_type));

  QCRIL_LOG_INFO( "qcril_uim_request_logical_channel(aid: %s)\n",
      (qcril_uim_aid_log(aid_ptr, print_aid, sizeof(print_aid)) != RIL_E_SUCCESS) ? "NULL" : print_aid);

  open_logical_ch_params.slot = slot;

  /* If AID is provided, add the AID info to be sent to modem.
     If AID pointer is NULL or empty string, skip the AID TLV since we need to
     open the channel to MF with no select on any DF. */
  if ((aid_ptr != NULL) && (strlen(aid_ptr) > 0))
  {
    /* Convert AID string into binary */
    aid_size = qcril_uim_hexstring_to_bin(aid_ptr,
                                          aid_buffer,
                                          QMI_UIM_MAX_AID_LEN);
    if (aid_size == 0 || aid_size > QMI_UIM_MAX_AID_LEN)
    {
      QCRIL_LOG_ERROR("%s", "Error converting AID string into binary");
      return QCRIL_UIM_RIL_E_INTERNAL_ERR;
    }

    slot_id = qcril_uim_instance_id_to_slot(instance_id);
    /* If opening ARA-M application, we need to ensure that the provisioning application has
       been activated before accessing as to not delay provisioning of the USIM/CSIM applicaiton. */
    if ((slot_id < QMI_UIM_MAX_CARD_COUNT) &&
        (qcril_uim.card_status.card[slot_id].card_state == QMI_UIM_CARD_STATE_PRESENT) &&
        (qcril_uim_ara_m_delay_timer.timer_id == 0) &&
        (aid_size == sizeof(ara_m_aid)) &&
        strlen(aid_ptr) <= QMI_UIM_MAX_AID_LEN &&
        (memcmp(aid_buffer, ara_m_aid, sizeof(ara_m_aid)) == 0))
    {
      struct timespec       current_time;
      struct timeval        init_time;
      struct timeval        check_time;

      memset(&current_time, 0x00, sizeof(struct timespec));

      /* Get the current time */
      clock_gettime(CLOCK_MONOTONIC, &current_time);
      check_time.tv_sec  = current_time.tv_sec;
      check_time.tv_usec  = current_time.tv_nsec/1000;

      /* Give three seconds for provisioning app initalization following card insertion */
      init_time.tv_sec = qcril_uim.card_inserted_time.tv_sec + QCRIL_UIM_ARA_M_APP_OPEN_WAIT_TIME;
      init_time.tv_usec = qcril_uim.card_inserted_time.tv_nsec/1000;

      QCRIL_LOG_INFO("Init time tv_sec:0x%x, tv_usec:0x%x, current time tv_sec:0x%x, tv_usec:0x%x",
                           init_time.tv_sec, init_time.tv_usec, check_time.tv_sec, check_time.tv_usec);

      /* Card inserted time is zero or current time is lesser than init time then delay ARA-M app open */
      if((qcril_uim.card_inserted_time.tv_sec == 0 && qcril_uim.card_inserted_time.tv_nsec == 0) ||
         (0 > util_timer_compare_times(&check_time, &init_time)))
      {
        QCRIL_LOG_INFO( "Delay the ARA-M APP open request");

        memset(&qcril_uim_ara_m_delay_timer, 0, sizeof(qcril_uim_ara_m_delay_timer));

        /* No Card insert event received yet delay ARA-M app open for three seconds, request might
           received just at the time of card reset */
        if(qcril_uim.card_inserted_time.tv_sec == 0 &&
           qcril_uim.card_inserted_time.tv_nsec == 0)
        {
          qcril_uim_ara_m_delay_timer.ara_m_delay_timer.tv_sec = QCRIL_UIM_ARA_M_APP_OPEN_WAIT_TIME;
          qcril_uim_ara_m_delay_timer.ara_m_delay_timer.tv_usec = 0;
        }
        else
        {
          timersub(&init_time, &check_time, &qcril_uim_ara_m_delay_timer.ara_m_delay_timer);
          QCRIL_LOG_INFO("qcril_uim_ara_m_delay_timer tv_sec:0x%x, tv_usec:0x%x",
                              qcril_uim_ara_m_delay_timer.ara_m_delay_timer.tv_sec,
                              qcril_uim_ara_m_delay_timer.ara_m_delay_timer.tv_usec);
        }
        qcril_uim_ara_m_delay_timer.instance_id              = instance_id;
        qcril_uim_ara_m_delay_timer.token                    = token;
        qcril_uim_ara_m_delay_timer.request_id               = request_id;
        qcril_uim_ara_m_delay_timer.slot                     = slot;
        qcril_uim_ara_m_delay_timer.p2                       = p2;

        memset((void *)&qcril_uim_ara_m_delay_timer.aid_data[0], '\0', sizeof(qcril_uim_ara_m_delay_timer.aid_data));
        (void)strlcpy((char *)&qcril_uim_ara_m_delay_timer.aid_data[0],
                      aid_ptr,
                      sizeof(qcril_uim_ara_m_delay_timer.aid_data));

        if(qcril_setup_timed_callback( instance_id,
                                       QCRIL_DEFAULT_MODEM_ID,
                                       qcril_uim_resend_ara_m_cmd_timer_cb,
                                       &qcril_uim_ara_m_delay_timer.ara_m_delay_timer,
                                       &qcril_uim_ara_m_delay_timer.timer_id ) == 0)
        {
          return RIL_E_SUCCESS;
        }

        /* Timer failed cleanup cache */
        memset(&qcril_uim_ara_m_delay_timer, 0, sizeof(qcril_uim_ara_m_delay_timer));
      }
    }

    /* Update AID info */
    open_logical_ch_params.aid_present = QMI_UIM_TRUE;
    open_logical_ch_params.aid.data_ptr = (unsigned char*)aid_buffer;
    open_logical_ch_params.aid.data_len = (unsigned short)aid_size;
  }

  open_logical_ch_params.file_control_information.is_valid = QMI_UIM_TRUE;

  if(p2 > QCRIL_UIM_INVALID_OPEN_CH_P2)
  {
    RIL_Errno ret = QCRIL_UIM_RIL_E_INTERNAL_ERR;

    ret = qcril_uim_convert_p2_value(p2, &qcril_fci_value);
    if(ret != RIL_E_SUCCESS)
    {
      return ret;
    }
  }
  else
  {
    /* When opening the channel, ask for appropriate template depending upon
       whether the it is an ICC card or a specific AID */
    if (qcril_uim_find_app_in_slot(slot, QMI_UIM_APP_SIM) ||
        qcril_uim_find_app_in_slot(slot, QMI_UIM_APP_RUIM))
    {
      qcril_fci_value = QCRIL_UIM_FCI_VALUE_FCP;
    }
    else
    {
      qcril_fci_value = qcril_uim_determine_select_template_from_aid(aid_ptr);
    }
  }

  open_logical_ch_params.file_control_information.fci_value =
                                 qcril_uim_convert_fci_value(qcril_fci_value);

  /* Allocate original request, it is freed in qmi_uim_callback */
  original_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         QCRIL_MAX_MODEM_ID - 1,
                                                         token,
                                                         request_id,
                                                         0);
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_ptr!");
    return QCRIL_UIM_RIL_E_NO_MEMORY;
  }

  /* Save original request AID data, FCI value and slot in case
     open channel fails due to invalid P1/P2 and a second fallback
     open channel attempt needs to be made. */
  original_request_ptr->data.channel_info.fci_value = qcril_fci_value;
  original_request_ptr->data.channel_info.aid_size = (unsigned short)aid_size;
  original_request_ptr->data.channel_info.slot = open_logical_ch_params.slot;
  memcpy(original_request_ptr->data.channel_info.aid_buffer,
         (unsigned char*)aid_buffer,
         aid_size);

  /* Proceed with logical channel request */
  QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_service", "logical channel" );
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_OPEN_LOGICAL_CHANNEL,
                                   qcril_uim.qmi_handle,
                                   &open_logical_ch_params,
                                   qmi_uim_callback,
                                   (void*)original_request_ptr) >= 0)
  {
    return RIL_E_SUCCESS;
  }

  /* In case or error, clean up any original request allocated */
  if (original_request_ptr)
  {
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }

  return QCRIL_UIM_RIL_E_MODEM_ERR;
} /* qcril_uim_send_open_logical_ch_req */


/*===========================================================================

  FUNCTION: qcril_uim_resend_ara_m_cmd_timer_cb

===========================================================================*/
/*!
    @brief
    Timer callback function responsible to re-send ARA-M Open channel cmd.

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_resend_ara_m_cmd_timer_cb
(
  void *param_ptr
)
{
  (void) param_ptr;

  if(qcril_uim_ara_m_delay_timer.timer_id != 0)
  {
    QCRIL_LOG_INFO("ARA M app open delay timer callback");

    if(qcril_uim_send_open_logical_ch_req(qcril_uim_ara_m_delay_timer.instance_id,
                                          qcril_uim_ara_m_delay_timer.aid_data,
                                          qcril_uim_ara_m_delay_timer.p2,
                                          qcril_uim_ara_m_delay_timer.token,
                                          qcril_uim_ara_m_delay_timer.request_id,
                                          qcril_uim_ara_m_delay_timer.slot) != RIL_E_SUCCESS)
    {
      qcril_uim_response(qcril_uim_ara_m_delay_timer.instance_id, qcril_uim_ara_m_delay_timer.token,
                         QCRIL_UIM_RIL_E_INTERNAL_ERR, NULL, 0, TRUE,
                         "error in qcril_qmi_uim_logical_channel due to card error");
    }

    /* Done with timer request, cleanup cache */
    memset(&qcril_uim_ara_m_delay_timer, 0, sizeof(qcril_uim_ara_m_delay_timer));
  }
} /* qcril_uim_resend_ara_m_cmd_timer_cb */


/*===========================================================================

  FUNCTION:  qcril_uim_send_close_logical_ch_req

===========================================================================*/
/*!
    @brief
    Responsible to send QMI Close channel request to the modem.

    @return
    RIL_Errno
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_send_close_logical_ch_req
(
  qcril_instance_id_e_type      instance_id,
  const char                  * in_ptr,
  RIL_Token                     token,
  int                           request_id,
  qmi_uim_slot_type             slot
)
{
  qcril_uim_original_request_type     * original_request_ptr = NULL;
  qmi_uim_logical_channel_params_type   logical_channel_params;

  /* Return with error if input pointer was not provided */
  if (in_ptr == NULL)
  {
    QCRIL_LOG_ERROR( "%s", " Invalid input pointer \n");
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_logical_channel(channel id: 0x%X)", *in_ptr);

  /* Fill QMI parameter */
  memset(&logical_channel_params, 0, sizeof(qmi_uim_logical_channel_params_type));
  logical_channel_params.slot = slot;
  logical_channel_params.operation_type = QMI_UIM_LOGICAL_CHANNEL_CLOSE;
  logical_channel_params.channel_data.close_channel_info.channel_id = *in_ptr;
  logical_channel_params.channel_data.close_channel_info.terminate_app = FALSE;

  /* Allocate original request, it is freed in qmi_uim_callback */
  original_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         QCRIL_MAX_MODEM_ID - 1,
                                                         token,
                                                         request_id,
                                                         0);
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_ptr!");
    return QCRIL_UIM_RIL_E_NO_MEMORY;
  }

  qcril_uim_remove_select_response_info_entry(
    logical_channel_params.channel_data.close_channel_info.channel_id);

  /* Proceed with logical channel request */
  QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_service", "logical channel" );
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_LOGICAL_CHANNEL,
                                   qcril_uim.qmi_handle,
                                   &logical_channel_params,
                                   qmi_uim_callback,
                                   (void*)original_request_ptr) >= 0)
  {
    return RIL_E_SUCCESS;
  }

  /* In case or error, clean up any original request allocated */
  if (original_request_ptr)
  {
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }

  return QCRIL_UIM_RIL_E_MODEM_ERR;
} /* qcril_uim_send_close_logical_ch_req */


/*===========================================================================

  FUNCTION:  qcril_uim_request_logical_channel

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_SIM_OPEN_CHANNEL or RIL_REQUEST_SIM_CLOSE_CHANNEL
    request from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_logical_channel
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qmi_uim_slot_type      slot   = QMI_UIM_SLOT_1;
  RIL_Errno              result = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  /* Find slot info */
  if ( ril_to_uim_is_tsts_enabled() && (params_ptr->instance_id == QCRIL_THIRD_INSTANCE_ID) )
  {
    slot = QMI_UIM_SLOT_3;
  }
  else if ( (ril_to_uim_is_tsts_enabled() ||
             ril_to_uim_is_dsds_enabled()) &&
            (params_ptr->instance_id == QCRIL_SECOND_INSTANCE_ID) )
  {
    slot = QMI_UIM_SLOT_2;
  }
  else if (params_ptr->instance_id == QCRIL_DEFAULT_INSTANCE_ID)
  {
    slot = QMI_UIM_SLOT_1;
  }
  else
  {
    QCRIL_LOG_ERROR( " Invalid instance_id in input: 0x%x\n", params_ptr->instance_id);
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INTERNAL_ERR,
                       NULL, 0, TRUE, "error in qcril_qmi_uim_logical_channel");
    return;
  }

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Parse input info based on the request type */
  if (params_ptr->event_id == RIL_REQUEST_SIM_OPEN_CHANNEL)
  {
    RIL_OpenChannelParams    * open_ch_p2_ptr = (RIL_OpenChannelParams *)params_ptr->data;

    if (open_ch_p2_ptr == NULL)
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS,
                 NULL, 0, TRUE, "error in qcril_qmi_uim_logical_channel");
      return;
    }

    result = qcril_uim_send_open_logical_ch_req(params_ptr->instance_id,
                                                open_ch_p2_ptr->aidPtr,
                                                open_ch_p2_ptr->p2,
                                                params_ptr->t,
                                                params_ptr->event_id,
                                                slot);
  }
  else if (params_ptr->event_id == RIL_REQUEST_SIM_CLOSE_CHANNEL)
  {
    result = qcril_uim_send_close_logical_ch_req(params_ptr->instance_id,
                                                 params_ptr->data,
                                                 params_ptr->t,
                                                 params_ptr->event_id,
                                                 slot);
  }
  else
  {
    QCRIL_LOG_ERROR( " Invalid input event_id: 0x%x\n", params_ptr->event_id);
  }

  /* Upon failure to send request to modem, send result error back */
  if (result != RIL_E_SUCCESS)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, result,
                       NULL, 0, TRUE, "error in qcril_qmi_uim_logical_channel");
  }
} /* qcril_uim_request_logical_channel */


/*===========================================================================

  FUNCTION:  qcril_uim_send_reselect_req

===========================================================================*/
/*!
    @brief
    Responsible to send QMI reselect request to the modem.

    @return
    TRUE if successful, FALSE otherwise
*/
/*=========================================================================*/
static boolean qcril_uim_send_reselect_req
(
  qcril_instance_id_e_type        instance_id,
  RIL_Token                       token,
  int                             request_id,
  int                             channel_id,
  qmi_uim_slot_type               slot,
  qmi_uim_send_apdu_params_type * apdu_params_ptr
)
{
  qcril_uim_original_request_type     * original_request_ptr = NULL;
  qmi_uim_reselect_params_type          reselect_params;

  if (apdu_params_ptr == NULL)
  {
    return FALSE;
  }

  QCRIL_LOG_INFO( "qcril_uim_send_reselect_req (channel id: 0x%x)", channel_id);

  /* Fill QMI parameter */
  memset(&reselect_params, 0, sizeof(qmi_uim_reselect_params_type));
  reselect_params.slot = slot;
  reselect_params.channel_id = channel_id;
  reselect_params.select_mode = QMI_UIM_SELECT_MODE_NEXT;

  /* Allocate original request, it is freed in qmi_uim_callback */
  original_request_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                         QCRIL_MAX_MODEM_ID - 1,
                                                         token,
                                                         request_id,
                                                         0);
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_ptr!");
    return FALSE;
  }

  /* Update the original req ptr with the send APDU parameters.
     The internal APDU data_ptr will be freed in the reselect cb */
  memcpy(&original_request_ptr->data.send_apdu,
         apdu_params_ptr,
         sizeof(qmi_uim_send_apdu_params_type));

  /* Proceed with logical channel request */
  QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_service", "reselect" );
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_RESELECT,
                                   qcril_uim.qmi_handle,
                                   &reselect_params,
                                   qmi_uim_callback,
                                   (void*)original_request_ptr) >= 0)
  {
    return TRUE;
  }

  /* In case or error, clean up any original request allocated */
  if (original_request_ptr)
  {
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }

  return FALSE;
} /* qcril_uim_send_reselect_req */


/*===========================================================================

  FUNCTION:  qcril_uim_request_send_apdu

===========================================================================*/
/*!
    @brief
    Handles these RIL requests:

    RIL_REQUEST_SIM_APDU
    RIL_REQUEST_SIM_TRANSMIT_CHANNEL
    RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC
    RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_send_apdu
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type             modem_id             = QCRIL_MAX_MODEM_ID - 1;
  uint16                            in_apdu_length       = 0;
  qcril_uim_original_request_type * original_request_ptr = NULL;
  qmi_uim_send_apdu_params_type     send_apdu_params;
  int                               cla                  = 0;
  int                               ins                  = 0;
  int                               p1                   = 0;
  int                               p2                   = 0;
  int                               p3                   = 0;
  const char                      * data_ptr             = NULL;
  int                               channel_id           = 0;
  boolean                           channel_id_present   = FALSE;
  uint8                             i                    = 0;
  boolean                           send_select_rsp_data = FALSE;
  RIL_SIM_IO_Response               ril_response;
  RIL_SIM_IO_Response             * ril_resp_ptr         = NULL;
  size_t                            ril_resp_len         = 0;
  RIL_Errno                         ril_err              = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  uint8                             select_resp_index    = QCRIL_UIM_MAX_SELECT_RESP_COUNT;
  qmi_uim_slot_type                 slot                 = QMI_UIM_SLOT_1;
#if defined(RIL_REQUEST_SIM_APDU) || defined(RIL_REQUEST_SIM_TRANSMIT_CHANNEL)
  boolean                           check_simio_params   = FALSE;
  const RIL_SIM_IO_v6             * request_simio_ptr    = NULL;
#endif /* RIL_REQUEST_SIM_APDU || RIL_REQUEST_SIM_TRANSMIT_CHANNEL */
#if defined(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC) || defined(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
  boolean                           check_apdu_params    = FALSE;
  const RIL_SIM_APDU              * request_apdu_ptr     = NULL;
#endif /* RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC || RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL */
  RIL_SlotStreamApduReq           * slot_apdu_ptr        = NULL;
  uint32                            oem_hook_slot_id     = 0;
  RIL_SIM_APDU                      apdu_data;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  if(params_ptr->event_id == QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ)
  {
    slot_apdu_ptr = (RIL_SlotStreamApduReq *)params_ptr->data;
    if(slot_apdu_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
      QCRIL_ASSERT(0);
      return;
    }
    check_apdu_params  = TRUE;
  }

  memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));
  memset(&send_apdu_params, 0, sizeof(qmi_uim_send_apdu_params_type));

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Note: Due to the way code is strctured below, both types of APIs can coexist */
#if defined(RIL_REQUEST_SIM_APDU) || defined(RIL_REQUEST_SIM_TRANSMIT_CHANNEL)
#if defined(RIL_REQUEST_SIM_APDU)
  if (params_ptr->event_id == RIL_REQUEST_SIM_APDU)
  {
    check_simio_params = TRUE;
  }
#endif /* RIL_REQUEST_SIM_APDU */
#if defined(RIL_REQUEST_SIM_TRANSMIT_CHANNEL)
  if (params_ptr->event_id == RIL_REQUEST_SIM_TRANSMIT_CHANNEL)
  {
    check_simio_params = TRUE;
  }
#endif /* RIL_REQUEST_SIM_TRANSMIT_CHANNEL */
  if (check_simio_params)
  {
    request_simio_ptr = (const RIL_SIM_IO_v6*)params_ptr->data;
    if(request_simio_ptr == NULL)
    {
      ril_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
      goto send_apdu_response;
    }

    /* Update parameters from input pointer */
    cla        = request_simio_ptr->cla;
    ins        = request_simio_ptr->command;
    channel_id = request_simio_ptr->fileid;
    p1         = request_simio_ptr->p1;
    p2         = request_simio_ptr->p2;
    p3         = request_simio_ptr->p3;
    data_ptr   = request_simio_ptr->data;

#if defined(RIL_REQUEST_SIM_TRANSMIT_CHANNEL)
    /* If request is RIL_REQUEST_SIM_TRANSMIT_CHANNEL, update channel_id also */
    if (params_ptr->event_id == RIL_REQUEST_SIM_TRANSMIT_CHANNEL)
    {
      channel_id_present = TRUE;
    }
#endif /* RIL_REQUEST_SIM_TRANSMIT_CHANNEL */
  }
#endif /* RIL_REQUEST_SIM_APDU || RIL_REQUEST_SIM_TRANSMIT_CHANNEL */

#if defined(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC) || defined(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
#if defined(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC)
  if (params_ptr->event_id == RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC)
  {
    check_apdu_params = TRUE;
  }
#endif /* RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC */
#if defined(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
  if (params_ptr->event_id == RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
  {
    check_apdu_params = TRUE;
  }
#endif /* RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL */
  if (check_apdu_params)
  {
    if(params_ptr->event_id == QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ)
    {
      /* Read in the order of the structure RIL_SlotStreamApduReq */
      /* Read slot id */
      oem_hook_slot_id = slot_apdu_ptr->slot_id;
      switch(oem_hook_slot_id)
      {
        case 0:
          slot = QMI_UIM_SLOT_1;
          break;
        case 1:
          slot = QMI_UIM_SLOT_2;
          break;
        case 2:
          slot = QMI_UIM_SLOT_3;
          break;
        default:
          ril_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
          QCRIL_LOG_ERROR("Invalid slot_id in input: 0x%x\n", slot_apdu_ptr->slot_id);
          goto send_apdu_response;
      }

      apdu_data = slot_apdu_ptr->apdu_data;
      request_apdu_ptr   = &apdu_data;

      /* Data is not pointer for this stucture it is stream of bytes
         copy the address of data */
      if(params_ptr->datalen <= (sizeof(RIL_SlotStreamApduReq) - sizeof(request_apdu_ptr->data)))
      {
        /* Input datalen is less than size of RIL_SlotStreamApduReq, data will be NULL */
        data_ptr = NULL;
      }
      else
      {
        data_ptr   = (char *)&request_apdu_ptr->data;
      }
    }
    else
    {
      request_apdu_ptr = (const RIL_SIM_APDU*)params_ptr->data;
      if(request_apdu_ptr == NULL)
      {
        ril_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
        goto send_apdu_response;
      }

      data_ptr   = request_apdu_ptr->data;
    }

    /* Update parameters from input pointer */
    cla        = request_apdu_ptr->cla;
    ins        = request_apdu_ptr->instruction;
    channel_id = request_apdu_ptr->sessionid;
    p1         = request_apdu_ptr->p1;
    p2         = request_apdu_ptr->p2;
    p3         = request_apdu_ptr->p3;

#if defined(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
    /* If request is RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL, update channel_id also */
    if (params_ptr->event_id == RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
    {
      channel_id_present = TRUE;
    }
#endif /* RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL */
  }
#endif /* RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC || RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL */

  /* Note - An ADPU is constructed from input APIs as follows:
     RIL_REQUEST_SIM_APDU or RIL_REQUEST_SIM_TRANSMIT_CHANNEL: RIL_SIM_IO_v6
     RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC or RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: RIL_SIM_APDU
     Mapping for these input structs to CLA + INS + P1 + P2 + P3 + data:
     channel_id: fileid or sessionid, CLA: cla, INS: command or instruction,
     P1: p1, P2: p2, P3: p3, data: data
     Only RIL_REQUEST_SIM_TRANSMIT_CHANNEL & RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL
     have the valid channel_id */
  /* Log only if adb logs not on, apdu logs not get printed in adb logs */
  if (!qcril_log_adb_on)
  {
    QCRIL_LOG_INFO( "qcril_uim_request_send_apdu(0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, %s)\n",
                    cla,
                    ins,
                    channel_id,
                    p1,
                    p2,
                    p3,
                    data_ptr != NULL ? data_ptr : "NULL" );
  }
  else
  {
    /* In case of adb log print only cls ins and last two bytes of data */
    QCRIL_LOG_INFO( "qcril_uim_request_send_apdu(0x%X, 0x%X, **, **, **, **, ***%s)\n",
                    cla,
                    ins,
                    data_ptr == NULL ? "NULL" : ((strlen(data_ptr) <= 4) ? data_ptr : (data_ptr + strlen(data_ptr) - 4)));
  }

  /* Sanity check */
  if ((p1  < 0)    || (p1  > 0xFF) ||
      (p2  < 0)    || (p2  > 0xFF) ||
      (cla < 0)    || (cla > 0xFF) ||
      (ins < 0)    || (ins > 0xFF))
  {
    QCRIL_LOG_ERROR( "Unsupported case, P1: 0x%X, P2: 0x%X, P3: 0x%X, cla: 0x%X, ins: 0x%X \n",
                     p1, p2, p3, cla, ins);
    ril_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
    goto send_apdu_response;
  }

  /* Limitation on instruction byte is applicable only in case of T=0 protocol,
     but QCRIL does not have that information. So, blocking request for telecom
     cards (as in practice they all use T=0 protocol), but allow it for non
     telecom cards: in case, the modem will reject the APDU if needed, based on
     the protocol */
  if ((params_ptr->event_id != QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ) &&
      (ins & 0xF0) == QCRIL_UIM_INVALID_INS_BYTE_MASK)
  {
    QCRIL_LOG_ERROR( "Invalid INS byte 0x%X", ins);
    ril_response.sw1 = QCRIL_UIM_SW1_INS_CODE_NOT_SUPPORTED;
    ril_response.sw2 = QCRIL_UIM_SW2_NORMAL_END;
    ril_err          = RIL_E_SUCCESS;
    ril_resp_ptr     = &ril_response;
    ril_resp_len     = sizeof(RIL_SIM_IO_Response);
    goto send_apdu_response;
  }

  if (channel_id_present == FALSE)
  {
    /* Calculate channel id based on the type of the logical channel:
       Standard logical channel: 0 -  3 in b1 & b2
       Extended logical channel: 4 - 19 in b1 - b4 represented from 0000 - 1111 */
    if (cla & 0x40)
    {
      channel_id = (cla & 0x0F) + 4;
    }
    else
    {
      channel_id = (cla & 0x03);
    }
  }

  /* For QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ slot id comes in the request */
  if(params_ptr->event_id != QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ)
  {
    /* Find slot info */
    if (ril_to_uim_is_tsts_enabled() && (params_ptr->instance_id == QCRIL_THIRD_INSTANCE_ID))
    {
      slot = QMI_UIM_SLOT_3;
    }
    else if ((ril_to_uim_is_tsts_enabled() ||
              ril_to_uim_is_dsds_enabled()) &&
              (params_ptr->instance_id == QCRIL_SECOND_INSTANCE_ID))
    {
      slot = QMI_UIM_SLOT_2;
    }
    else if (params_ptr->instance_id == QCRIL_DEFAULT_INSTANCE_ID)
    {
      slot = QMI_UIM_SLOT_1;
    }
    else
    {
      QCRIL_LOG_ERROR("Invalid instance_id in input: 0x%x\n", params_ptr->instance_id);
      ril_err = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
      goto send_apdu_response;
    }
  }

  /* First, we loop through to find if there is a select response data
     that matches the incoming channel id  */
  for (i = 0; i < QCRIL_UIM_MAX_SELECT_RESP_COUNT; i++)
  {
    if ((qcril_uim.select_response_info[i].in_use) &&
        (qcril_uim.select_response_info[i].channel_id == channel_id))
    {
      select_resp_index = i;
      break;
    }
  }

  /* Next if the APDU request was a GET RESPONSE to a previous OPEN CHANNEL,
     we respond immediately with the stored response select response data. */
  if ((ins               == QCRIL_UIM_INS_BYTE_GET_RESPONSE) &&
      (p1                == 0)                               &&
      (p2                == 0)                               &&
      (p3                == 0)                               &&
      (select_resp_index < QCRIL_UIM_MAX_SELECT_RESP_COUNT))
  {
    memset(&ril_response, 0, sizeof(RIL_SIM_IO_Response));
    ril_err = qcril_uim_update_get_response_apdu(select_resp_index,
                                                 &ril_response);
    send_select_rsp_data = TRUE;
  }

  /* In any case cleanup select response info if it was stored previously
     for a particular channel id */
  if (select_resp_index < QCRIL_UIM_MAX_SELECT_RESP_COUNT)
  {
    /* Free the cached buffer, if available*/
    if (qcril_uim.select_response_info[select_resp_index].select_resp_ptr)
    {
      qcril_free(qcril_uim.select_response_info[select_resp_index].select_resp_ptr);
      qcril_uim.select_response_info[select_resp_index].select_resp_ptr = NULL;
    }
    /* Also clean up the entire entry */
    memset(&qcril_uim.select_response_info[select_resp_index],
           0,
           sizeof(qcril_uim_select_response_info_type));
  }

  if (send_select_rsp_data)
  {
    QCRIL_LOG_DEBUG( "RIL_SIM_IO_Response: sw1=0x%X sw2=0x%X data=%s\n",
                      ril_response.sw1, ril_response.sw2,
                      ril_response.simResponse != NULL ? ril_response.simResponse : "NULL");

    ril_resp_ptr = &ril_response;
    ril_resp_len = sizeof(RIL_SIM_IO_Response);
    goto send_apdu_response;
  }

  /* Compose the Send APDU parameters */
  send_apdu_params.slot = slot;

  /* set procedure bytes */
  send_apdu_params.procedure_bytes = qcril_uim.procedure_bytes;

  if (channel_id_present)
  {
    send_apdu_params.channel_id         = channel_id;
    send_apdu_params.channel_id_present = QMI_UIM_TRUE;
  }
  else
  {
    send_apdu_params.channel_id_present = QMI_UIM_FALSE;
  }

  /* Calculate total buffer size for APDU data */
  if ((data_ptr == NULL) || (strlen(data_ptr) == 0))
  {
    if (p3 < 0)
    {
      in_apdu_length = QCRIL_UIM_APDU_MIN_SIZE;
    }
    else if(p3 < QCRIL_UIM_APDU_MAX_SHORT_APDU_SIZE)
    {
      in_apdu_length = QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3;
    }
    else
    {
      in_apdu_length = QCRIL_UIM_LONG_APDU_MIN_SIZE_PLUS_P3;
    }
  }
  else if(p3 < QCRIL_UIM_APDU_MAX_SHORT_APDU_SIZE)
  {
    in_apdu_length = QCRIL_UIM_APDU_MIN_SIZE_PLUS_P3 + strlen(data_ptr)/2;
  }
  else
  {
    in_apdu_length = QCRIL_UIM_LONG_APDU_MIN_SIZE_PLUS_P3 + strlen(data_ptr)/2;
  }

  /* Allocate memory and compose the raw APDU data */
  send_apdu_params.apdu.data_ptr = (uint8*) qcril_malloc(in_apdu_length);
  if (send_apdu_params.apdu.data_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Unable to allocate buffer for apdu.data_ptr!");
    ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_apdu_response;
  }

  send_apdu_params.apdu.data_len = in_apdu_length;

  if (qcril_uim_compose_apdu_data(&send_apdu_params.apdu,
                                  cla,
                                  ins,
                                  p1,
                                  p2,
                                  p3,
                                  data_ptr) == FALSE)
  {
    QCRIL_LOG_ERROR("%s", "Error composing APDU data!");
    ril_err = QCRIL_UIM_RIL_E_SYSTEM_ERR;
    goto send_apdu_response;
  }

  /* Check for SelectNext request via streaming APDU. If so, use reselect QMI API.
     Also note that for backward compatibility with older modems that doesnt support
     reselect QMI API, this incoming APDU request is stored & used in reselect cb */
  if ((ins         == QCRIL_UIM_INS_BYTE_SELECT)            &&
      (p1          == QCRIL_UIM_P1_VALUE_SELECT_BY_DF_NAME) &&
      ((p2 & 0x03) == QCRIL_UIM_P2_MASK_SELECT_NEXT))
  {
    if (qcril_uim_send_reselect_req(params_ptr->instance_id,
                                    params_ptr->t,
                                    params_ptr->event_id,
                                    channel_id,
                                    slot,
                                    &send_apdu_params))
    {
      return;
    }
    /* Upon error, send error response back to client */
    ril_err = QCRIL_UIM_RIL_E_MODEM_ERR;
    goto send_apdu_response;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id,
                                                         0);
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_ptr!");
    ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
    goto send_apdu_response;
  }

  if(params_ptr->event_id == QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ)
  {
    original_request_ptr->data.slot_id = oem_hook_slot_id;
  }

  /* Proceed with send APDU request */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "send APDU" );
  if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SEND_APDU,
                                   qcril_uim.qmi_handle,
                                   &send_apdu_params,
                                   qmi_uim_callback,
                                   (void*)original_request_ptr) >= 0)
  {
    qcril_free(send_apdu_params.apdu.data_ptr);
    send_apdu_params.apdu.data_ptr = NULL;
    return;
  }
  ril_err = QCRIL_UIM_RIL_E_MODEM_ERR;

send_apdu_response:
  if(params_ptr->event_id == QCRIL_EVT_HOOK_SLOT_SEND_APDU_REQ)
  {
    RIL_SlotSendApduResp              send_slot_apdu_rsp;
    uint32                            resp_len       = 0;

    memset(&send_slot_apdu_rsp, 0, sizeof(send_slot_apdu_rsp));
    send_slot_apdu_rsp.slot_id = slot_apdu_ptr->slot_id;
    send_slot_apdu_rsp.ril_err = ril_err;
    resp_len = sizeof(send_slot_apdu_rsp.slot_id) + sizeof(send_slot_apdu_rsp.ril_err);

    if(ril_resp_ptr != NULL && ril_resp_len > 0)
    {
      send_slot_apdu_rsp.sw1 = ril_resp_ptr->sw1;
      send_slot_apdu_rsp.sw2 = ril_resp_ptr->sw2;
      resp_len += sizeof(send_slot_apdu_rsp.sw1) + sizeof(send_slot_apdu_rsp.sw2);

      if(ril_resp_ptr->simResponse != NULL)
      {
        memcpy(send_slot_apdu_rsp.simResponse,
               ril_resp_ptr->simResponse,
               strlen(ril_resp_ptr->simResponse));
        resp_len += strlen(ril_resp_ptr->simResponse);
      }
    }
    /* Payload will have actual respone */
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, RIL_E_SUCCESS,
                       &send_slot_apdu_rsp, resp_len, TRUE,
                       "error in slot qcril_qmi_uim_send_apdu");
  }
  else
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, ril_err,
                       ril_resp_ptr, ril_resp_len, TRUE,
                       "error in qcril_qmi_uim_send_apdu");
  }

  /* Clean up any sim response if allocated */
  if (ril_response.simResponse)
  {
    qcril_free(ril_response.simResponse);
    ril_response.simResponse = NULL;
  }

  /* Clean up any original request if allocated */
  if (original_request_ptr)
  {
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }

  /* Clean up any APDU buffer if allocated */
  if(send_apdu_params.apdu.data_ptr)
  {
    qcril_free(send_apdu_params.apdu.data_ptr);
    send_apdu_params.apdu.data_ptr = NULL;
  }
} /* qcril_uim_request_send_apdu */


/*=========================================================================

  FUNCTION:  qcril_uim_request_set_apdu_behavior

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ request from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_set_apdu_behavior
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_original_request_type               * original_req_ptr          = NULL;
  qmi_uim_set_apdu_behavior_params_type           set_apdu_behavior_params;
  RIL_SetAPDUBehavior                           * set_apdu_behavior_req_ptr;
  RIL_Errno                                       ril_err                   = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  /* Sanity check */
  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return;
  }

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  if(params_ptr->event_id != QCRIL_EVT_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ)
  {
    goto set_apdu_behavior_req_error;
  }

  memset(&set_apdu_behavior_params, 0, sizeof(qmi_uim_set_apdu_behavior_params_type));

  set_apdu_behavior_req_ptr  = (RIL_SetAPDUBehavior *)params_ptr->data;

  /* Find slot info */
  if ( ril_to_uim_is_tsts_enabled() && (params_ptr->instance_id == QCRIL_THIRD_INSTANCE_ID) )
  {
    set_apdu_behavior_params.slot = QMI_UIM_SLOT_3;
  }
  else if ( (ril_to_uim_is_tsts_enabled() || ril_to_uim_is_dsds_enabled()) &&
            (params_ptr->instance_id == QCRIL_SECOND_INSTANCE_ID) )
  {
    set_apdu_behavior_params.slot = QMI_UIM_SLOT_2;
  }
  else if (params_ptr->instance_id == QCRIL_DEFAULT_INSTANCE_ID)
  {
    set_apdu_behavior_params.slot = QMI_UIM_SLOT_1;
  }
  else
  {
    QCRIL_LOG_ERROR( " Invalid instance_id in input: 0x%x\n", params_ptr->instance_id);
    goto set_apdu_behavior_req_error;
  }

  set_apdu_behavior_params.channel_id = set_apdu_behavior_req_ptr->channel_id;

  switch(set_apdu_behavior_req_ptr->apdu_behavior)
  {
    case RIL_UICC_APDU_BEHAVIOR_NO_GET_RESPONSE_FOR_WARNING:
      set_apdu_behavior_params.apdu_behavior = QMI_UIM_APDU_BEHAVIOR_NO_GET_RESPONSE_FOR_WARNING;
      break;
    case RIL_UICC_APDU_BEHAVIOR_AUTOMATIC_GET_RESPONSE_FOR_WARNING:
      set_apdu_behavior_params.apdu_behavior = QMI_UIM_APDU_BEHAVIOR_AUTOMATIC_GET_RESPONSE_FOR_WARNING;
      break;
    default:
      QCRIL_LOG_ERROR( " Invalid apdu_behavior in input: 0x%x\n", set_apdu_behavior_req_ptr->apdu_behavior);
      goto set_apdu_behavior_req_error;
  }

  /* Allocate original request */
  original_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                     QCRIL_MAX_MODEM_ID - 1,
                                                     params_ptr->t,
                                                     params_ptr->event_id,
                                                     0);

  if (original_req_ptr != NULL)
  {
    if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SET_APDU_BEHAVIOR,
                                     qcril_uim.qmi_handle,
                                     &set_apdu_behavior_params,
                                     qmi_uim_callback,
                                     (void*)original_req_ptr) >= 0)
    {
      return;
    }
    else
    {
      ril_err = QCRIL_UIM_RIL_E_MODEM_ERR;
    }
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
  }

set_apdu_behavior_req_error:
  qcril_uim_response(params_ptr->instance_id, params_ptr->t, ril_err,
                       NULL, 0, TRUE, "error in qcril_uim_request_voltage_status");

  /* Clean up any original request if allocated */
  if (original_req_ptr)
  {
    qcril_free(original_req_ptr);
    original_req_ptr = NULL;
  }
} /* qcril_uim_request_set_apdu_behavior */


/*===========================================================================

  FUNCTION:  qcril_uim_process_internal_verify_pin_command_callback

===========================================================================*/
/*!
    @brief
    Handler for QCRIL_EVT_INTERNAL_UIM_VERIFY_PIN_COMMAND_CALLBACK.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_process_internal_verify_pin_command_callback
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                   modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                                     res;
  RIL_Errno                               ril_err  = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  qcril_uim_pin2_original_request_type  * original_request_ptr = NULL;
  qcril_uim_original_request_type       * callback_request_ptr = NULL;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  original_request_ptr = (qcril_uim_pin2_original_request_type*)params_ptr->data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* In case of PIN error, return immediately */
  if (original_request_ptr->pin2_result != 0)
  {
    int32 num_retries = (int32)original_request_ptr->pin2_num_retries;

    /* Change the error code appropriately */
    switch(original_request_ptr->pin2_result)
    {
      case QMI_ERR_INCORRECT_PIN_V01:
      case QMI_ERR_PIN_PERM_BLOCKED_V01:
        ril_err = RIL_E_PASSWORD_INCORRECT;
        break;

      case QMI_ERR_PIN_BLOCKED_V01:
        ril_err = RIL_E_SIM_PUK2;
        break;

      default:
        ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
        break;
    }

    /* Only for set FDN request, we send number of retries in response. For
       all other requests, we send a simple response */
    if (original_request_ptr->cmd == QCRIL_UIM_ORIG_SET_SERVICE_STATUS_FDN)
    {
      qcril_uim_response(params_ptr->instance_id,
                         (RIL_Token)original_request_ptr->token,
                         ril_err,
                         &num_retries,
                         sizeof(int32),
                         TRUE,
                         "error in pin verification");
    }
    else
    {
      qcril_uim_response(params_ptr->instance_id,
                         (RIL_Token)original_request_ptr->token,
                         ril_err,
                         NULL,
                         0,
                         TRUE,
                         "error in pin verification");
    }

    /* Free memory allocated when the initial command was processed */
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;

    return;
  }

  /* Allocate callback_request_ptr, it is freed in qmi_uim_callback
     Note that session type is populated below for each request type */
  callback_request_ptr = qcril_uim_allocate_orig_request(
                                original_request_ptr->instance_id,
                                original_request_ptr->modem_id,
                                (RIL_Token)original_request_ptr->token,
                                (int)original_request_ptr->cmd,
                                0);
  if (callback_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for callback_request_ptr!");
    return;
  }

  /* Executes the original request */
  switch(original_request_ptr->cmd)
  {
    case QCRIL_UIM_ORIG_SIM_IO_READ_BINARY:
      QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "read transparent" );
      callback_request_ptr->session_type = original_request_ptr->data.read_transparent.session_info.session_type;
      res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_READ_TRANSPARENT,
                                         qcril_uim.qmi_handle,
                                         &original_request_ptr->data.read_transparent,
                                         qmi_uim_callback,
                                         (void*)callback_request_ptr);
      break;

    case QCRIL_UIM_ORIG_SIM_IO_READ_RECORD:
      QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "read record" );
      callback_request_ptr->session_type = original_request_ptr->data.read_record.session_info.session_type;
      res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_READ_RECORD,
                                         qcril_uim.qmi_handle,
                                         &original_request_ptr->data.read_record,
                                         qmi_uim_callback,
                                         (void*)callback_request_ptr);
      break;

    case QCRIL_UIM_ORIG_SIM_IO_UPDATE_BINARY:
      QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "write transparent" );
      callback_request_ptr->session_type = original_request_ptr->data.write_transparent.session_info.session_type;
      res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_WRITE_TRANSPARENT,
                                         qcril_uim.qmi_handle,
                                         &original_request_ptr->data.write_transparent,
                                         qmi_uim_callback,
                                         (void*)callback_request_ptr);
      break;

    case QCRIL_UIM_ORIG_SIM_IO_UPDATE_RECORD:
      QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "write record" );
      callback_request_ptr->session_type = original_request_ptr->data.write_record.session_info.session_type;
      res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_WRITE_RECORD,
                                         qcril_uim.qmi_handle,
                                         &original_request_ptr->data.write_record,
                                         qmi_uim_callback,
                                         (void*)callback_request_ptr);
      break;

    case QCRIL_UIM_ORIG_SET_SERVICE_STATUS_FDN:
      QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "set service" );
      callback_request_ptr->data.fdn_status = original_request_ptr->data.set_service_status.fdn_status;
      callback_request_ptr->session_type    = original_request_ptr->data.set_service_status.session_info.session_type;
      res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SET_FDN,
                                         qcril_uim.qmi_handle,
                                         &original_request_ptr->data.set_service_status,
                                         qmi_uim_callback,
                                         (void*)callback_request_ptr);
      break;

    default:
      QCRIL_LOG_ERROR( "%s: Invalid cmd for internal pin2 verification - %d\n",
                        __FUNCTION__, original_request_ptr->cmd);
      res = -1;
      qcril_free(callback_request_ptr);
      callback_request_ptr = NULL;
      break;
  }

  /* In case of error, send the response immediately */
  if (res < 0)
  {
    qcril_uim_response(params_ptr->instance_id, original_request_ptr->token,
                       QCRIL_UIM_RIL_E_MODEM_ERR, NULL, 0, TRUE, NULL);
  }

  /* Free memory allocated when the initial command was processed */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_process_internal_verify_pin_command_callback */


/*===========================================================================

  FUNCTION:  qcril_uim_cleanup_ara_m_delay_cache_info

===========================================================================*/
/*!
    @brief
    Cleans the global ARA-M APP open cache and sends error response.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_cleanup_ara_m_delay_cache_info
(
  void
)
{
  memset(&qcril_uim.card_inserted_time, 0x00, sizeof(qcril_uim.card_inserted_time));

  /* Stop ARA-M delay timer if running */
  if(qcril_uim_ara_m_delay_timer.timer_id != 0)
  {
    qcril_cancel_timed_callback((void *)(long)qcril_uim_ara_m_delay_timer.timer_id);
    qcril_uim_response(qcril_uim_ara_m_delay_timer.instance_id, qcril_uim_ara_m_delay_timer.token,
                       QCRIL_UIM_RIL_E_INTERNAL_ERR, NULL, 0, TRUE,
                 "error in ARA-M qcril_qmi_uim_logical_channel due to card error or refresh reset");
  }

  /* Done with timer request, cleanup cache */
  memset(&qcril_uim_ara_m_delay_timer, 0, sizeof(qcril_uim_ara_m_delay_timer));
} /* qcril_uim_cleanup_ara_m_delay_cache_info */


/*===========================================================================

  FUNCTION:  qcril_uim_direct_get_imsi

===========================================================================*/
/*!
    @brief
    Returns IMSI of requested aid of a slot

    @return
    None

    Note: imsi_ptr need to delete by client
*/
/*=========================================================================*/
RIL_Errno qcril_uim_direct_get_imsi
(
  uint8            slot,
  const char     * aid_ptr,
  char          ** imsi_ptr
)
{
  qmi_uim_read_transparent_params_type  read_params;
  RIL_Errno                             err        = RIL_E_SUCCESS;
  qmi_uim_rsp_data_type                 qmi_rsp_data;

  if(slot >= QMI_UIM_MAX_CARD_COUNT || imsi_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    return QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
  }

  memset(&read_params, 0, sizeof(qmi_uim_read_transparent_params_type));

  err = qcril_uim_get_imsi_path_for_aid(&read_params, aid_ptr, slot);
  if (err != RIL_E_SUCCESS)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);
    return err;
  }

  /* Proceed with read transparent  */
  memset(&qmi_rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  if (qcril_qmi_uim_read_transparent(qcril_uim.qmi_handle,
                                     &read_params,
                                     NULL,
                                     NULL,
                                     &qmi_rsp_data) >= 0)
  {
    int imsi_len = 0;

    err = qcril_uim_convert_err_value(qmi_rsp_data.qmi_err_code);

    QCRIL_LOG_DEBUG( "qcril_uim_direct_get_imsi_resp: qmi_err_code=%d, session_type=%d \n",
                      qmi_rsp_data.qmi_err_code,
                      read_params.session_info.session_type);

    /* Based on the type of request, fetch appropriate IMSI */
    if (err == RIL_E_SUCCESS)
    {
      if((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_PRI_GW_PROV) ||
         (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_SEC_GW_PROV) ||
         (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_TER_GW_PROV))
      {
        *imsi_ptr = qcril_uim_parse_gw_imsi(
                     qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr,
                     qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_len,
                     &imsi_len);
      }
      else if((read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_PRI_1X_PROV) ||
              (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_SEC_1X_PROV) ||
              (read_params.session_info.session_type == QMI_UIM_SESSION_TYPE_TER_1X_PROV))
      {
        *imsi_ptr = qcril_uim_parse_1x_imsi(
                     qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr,
                     qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_len,
                     &imsi_len);
      }
    }
    if (*imsi_ptr == NULL)
    {
      QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);
      return QCRIL_UIM_RIL_E_INTERNAL_ERR;
    }

    /* Client needs to free the memory for raw data */
    QCRIL_UIM_FREE_IF_NOT_NULL(qmi_rsp_data.rsp_data.read_transparent_rsp.content.data_ptr);
    QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);
    return RIL_E_SUCCESS;
  }
  QCRIL_UIM_FREE_IF_NOT_NULL(read_params.file_id.path.data_ptr);
  return QCRIL_UIM_RIL_E_MODEM_ERR;
} /* qcril_uim_direct_get_imsi */

/*=========================================================================

  FUNCTION:  qcril_uim_request_set_terminal_capability

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_SLOT_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ request
    from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_set_terminal_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_original_request_type               * original_req_ptr   = NULL;
  qmi_uim_terminal_capability_params_type         set_terminal_capability_params;
  RIL_TerminalCapabilityLPATLV                  * set_terminal_capability_req_ptr;
  RIL_Errno                                       ril_err            = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  /* Sanity check */
  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return;
  }

  QCRIL_LOG_INFO("%s\n", __FUNCTION__);

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  if(params_ptr->event_id != QCRIL_EVT_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ)
  {
    goto set_terminal_capability_req_error;
  }

  memset(&set_terminal_capability_params, 0, sizeof(qmi_uim_terminal_capability_params_type));

  set_terminal_capability_req_ptr  = (RIL_TerminalCapabilityLPATLV *)params_ptr->data;

  /* Find slot info */
  if ( ril_to_uim_is_tsts_enabled() && (params_ptr->instance_id == QCRIL_THIRD_INSTANCE_ID) )
  {
    set_terminal_capability_params.slot = QMI_UIM_SLOT_3;
  }
  else if ( (ril_to_uim_is_tsts_enabled() || ril_to_uim_is_dsds_enabled()) &&
            (params_ptr->instance_id == QCRIL_SECOND_INSTANCE_ID) )
  {
    set_terminal_capability_params.slot = QMI_UIM_SLOT_2;
  }
  else if (params_ptr->instance_id == QCRIL_DEFAULT_INSTANCE_ID)
  {
    set_terminal_capability_params.slot = QMI_UIM_SLOT_1;
  }
  else
  {
    QCRIL_LOG_ERROR("Invalid instance_id in input: 0x%x\n", params_ptr->instance_id);
    goto set_terminal_capability_req_error;
  }

  /* We currently support set/get terminal capability only for LPA TLV
     (that has a tag of 0x83), which can be further enhanced to other
     TLVs in the future, based on the requirements */
  set_terminal_capability_params.num_tlvs = (uint8_t) 0x01;
  set_terminal_capability_params.tlvs[0].tag = (uint8_t) QMI_UIM_TERMINAL_CAPABILITY_LPA_TLV_TAG;
  set_terminal_capability_params.tlvs[0].value.data_ptr = &(set_terminal_capability_req_ptr->value);
  set_terminal_capability_params.tlvs[0].value.data_len = (unsigned short) QMI_UIM_TERMINAL_CAPABILITY_LPA_TLV_LEN;
  set_terminal_capability_params.tlvs[0].tlv_present = (uint8_t) set_terminal_capability_req_ptr->tlv_present;

  /* Allocate original request */
  original_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                     QCRIL_MAX_MODEM_ID - 1,
                                                     params_ptr->t,
                                                     params_ptr->event_id,
                                                     0);

  if (original_req_ptr != NULL)
  {
    if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SET_TERMINAL_CAPABILITY,
                                     qcril_uim.qmi_handle,
                                     &set_terminal_capability_params,
                                     qmi_uim_callback,
                                     (void*)original_req_ptr) >= 0)
    {
      return;
    }
    else
    {
      ril_err = QCRIL_UIM_RIL_E_MODEM_ERR;
    }
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
  }

set_terminal_capability_req_error:
  qcril_uim_response(params_ptr->instance_id, params_ptr->t, ril_err,
                     NULL, 0, TRUE, "error in qcril_uim_request_set_terminal_capability");

  /* Clean up any original request if allocated */
  QCRIL_UIM_FREE_IF_NOT_NULL(original_req_ptr);
} /* qcril_uim_request_set_terminal_capability */


/*=========================================================================

  FUNCTION:  qcril_uim_request_get_terminal_capability

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_SLOT_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ request
    from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_terminal_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_original_request_type           * original_req_ptr          = NULL;
  qmi_uim_terminal_capability_params_type     get_terminal_capability_params;
  RIL_Errno                                   ril_err                   = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  /* Sanity check */
  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return;
  }

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  if(params_ptr->event_id != QCRIL_EVT_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ)
  {
    goto get_terminal_capability_req_error;
  }

  /* Find slot info */
  if ( ril_to_uim_is_tsts_enabled() && (params_ptr->instance_id == QCRIL_THIRD_INSTANCE_ID) )
  {
    get_terminal_capability_params.slot = QMI_UIM_SLOT_3;
  }
  else if ( (ril_to_uim_is_tsts_enabled() || ril_to_uim_is_dsds_enabled()) &&
            (params_ptr->instance_id == QCRIL_SECOND_INSTANCE_ID) )
  {
    get_terminal_capability_params.slot = QMI_UIM_SLOT_2;
  }
  else if (params_ptr->instance_id == QCRIL_DEFAULT_INSTANCE_ID)
  {
    get_terminal_capability_params.slot = QMI_UIM_SLOT_1;
  }
  else
  {
    QCRIL_LOG_ERROR( " Invalid instance_id in input: 0x%x\n", params_ptr->instance_id);
    goto get_terminal_capability_req_error;
  }

  /* Allocate original request */
  original_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                     QCRIL_MAX_MODEM_ID - 1,
                                                     params_ptr->t,
                                                     params_ptr->event_id,
                                                     0);

  /* Indicate that it is a GET request. This is done to ensure we handle the
     response from QMIUIM accordingly because there is a single API for
     GET and SET from QMIUIM perspective */
  if (original_req_ptr != NULL)
  {
    if (qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_GET_TERMINAL_CAPABILITY,
                                     qcril_uim.qmi_handle,
                                     &get_terminal_capability_params,
                                     qmi_uim_callback,
                                     (void*)original_req_ptr) >= 0)
    {
      return;
    }
    else
    {
      ril_err = QCRIL_UIM_RIL_E_MODEM_ERR;
    }
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
  }

get_terminal_capability_req_error:
  qcril_uim_response(params_ptr->instance_id, params_ptr->t, ril_err,
                       NULL, 0, TRUE, "error in qcril_uim_request_get_terminal_capability");

  /* Clean up any original request if allocated */
  QCRIL_UIM_FREE_IF_NOT_NULL(original_req_ptr);
} /* qcril_uim_request_get_terminal_capability */

#endif /* defined (FEATURE_QCRIL_UIM_QMI) */
