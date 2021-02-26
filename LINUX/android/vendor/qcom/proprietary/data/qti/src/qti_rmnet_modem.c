/******************************************************************************

                        QTI_RMNET_MODEM.C

******************************************************************************/

/******************************************************************************

  @file    qti_rmnet_modem.c
  @brief   Tethering Interface module for RMNET tethering. This file contains
           QTI interaction with modem interconnect for RMNET tethering

  DESCRIPTION
  Implementation file for QTI interaction with modem interconnect for RmNet
  tethering.

  ---------------------------------------------------------------------------
  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------


******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/22/14   sb        Initial version for modem interface for Fusion, LE and MSM.

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/msm_rmnet.h>
#include <errno.h>

#include "qti.h"
#include "data_common_v01.h"
#include "data_port_mapper_v01.h"
#include "qmi_client_instance_defs.h"

#define PRINT_QMI_MSG(msg,len) qti_log_raw_qmi_msg (msg,len)

#define QTI_PLATFORM_NUM_CHARS_PER_BYTE (3) /* 2 hex chars per byte + space */
#define QTI_PLATFORM_NUM_BYTES_PER_LINE (16)

#define QTI_PLATFORM_GET_HEX_CHAR(x)  \
  (((x) > 0x0F) ? '*' : hex_digit_to_char_tbl[(x)])

static const char hex_digit_to_char_tbl[] =
{
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static qti_rmnet_param               * rmnet_state_config = NULL;
extern qmi_client_os_params          qti_dpm_os_params;
static  qti_dpl_param                * dpl_state_config = NULL;
boolean                              is_ssr_in_progress = false;

/*===========================================================================
                          FUNCTION DEFINITIONS

============================================================================*/

/*==========================================================================
FUNCTION RETRIEVE_MHI_FILE()

DESCRIPTION

  This function will dynamically retrieve the mhi file pipe info.
  in Fusion targets.

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
  -1 on FAILURE

SIDE EFFECTS
  None

==========================================================================*/
int retrieve_mhi_file(char **mhi_file_name)
{
  FILE *fp = NULL;
  char mhi_esoc_link_info[MAX_SOC_LINK_INFO_LEN];
  DIR *mhi_dir;
  struct dirent *dir;
  char mhi_file_path[MHI_PATH_STRING_LEN];
  int retry_count = 0;
  int ret = QTI_SUCCESS;

  memset(mhi_esoc_link_info, 0, MAX_SOC_LINK_INFO_LEN * sizeof(char));
  memset(mhi_file_path, 0, MHI_PATH_STRING_LEN * sizeof(char));

  fp = fopen(MHI_ESOC_FILE_PATH, "r");
  if(fp == NULL)
  {
    LOG_MSG_ERROR("Error in opening the ESOC link info file", 0, 0, 0);
    return QTI_FAILURE;
  }
  else if(0 == fscanf(fp, "%s", mhi_esoc_link_info))
  {
    LOG_MSG_ERROR("Error reading mhi_esoc_link_info\n", 0, 0, 0);
    ret = QTI_FAILURE;
    goto bail;
  }
  else
  {
    /******************************************************************
          cat /sys/bus/esoc/devices/esoc0 --> 0304_00.01.00
          ls /sys/bus/mhi/devices/0304_00.01.00_QMI1/mhi_uci
              File name -----> mhi_0304_00.01.00_pipe_16
    ******************************************************************/
    snprintf(mhi_file_path, MHI_PATH_STRING_LEN, "%s/%s%s",
             MDM_FILE_PATH_MHI_START, mhi_esoc_link_info, MDM_FILE_PATH_MHI_END);
    while (retry_count < MHI_DIR_OPEN_MAX_RETRY)
    {
      mhi_dir = opendir(mhi_file_path);
      if(!mhi_dir)
      {
        LOG_MSG_ERROR("Unable to open the dir. Retrying again", 0, 0, 0);
        sleep(ONE_SEC);
        retry_count++;
        continue;
      }
      else
      {
        LOG_MSG_INFO1("Successful in opening the MHI UCI dir", 0, 0, 0);
        while((dir = readdir(mhi_dir)) != NULL)
        {
          if( !strncmp(dir->d_name, ".", strlen(".")) || !strncmp(dir->d_name,"..",strlen("..")) )
            continue;
          else
          {
            strlcpy(*mhi_file_name, dir->d_name, strlen(dir->d_name)+1);
            LOG_MSG_INFO1("MHI pipe name is %s \n", *mhi_file_name, 0, 0);
          }
        }
      }
      closedir(mhi_dir);
      break;
    }
    if (retry_count == MHI_DIR_OPEN_MAX_RETRY)
    {
      LOG_MSG_ERROR("Unable to open the dir even after %d retries",MHI_DIR_OPEN_MAX_RETRY, 0, 0);
      ret = QTI_FAILURE;
    }
  }

bail:
  fclose(fp);
  fp = NULL;

  return ret;
}

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_EVENT_CB()

DESCRIPTION

  This function
  - handles SSR events.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/
static void qti_rmnet_modem_event_cb
(
  qmi_sys_event_type              event_id,
  const qmi_sys_event_info_type   *event_info,
  void                            *user_data
)
{
#ifndef FEATURE_MDM_LE
  int                             ret_val;
  int                             line_state;
  int                             ph_iface_fd = 0;
/*----------------------------------------------------------------------*/
  (void) user_data;

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return;
  }

  LOG_MSG_INFO1("Received QMUXD cb event id %d", event_id, 0, 0);
  if( DS_TARGET_FUSION_SDM845_SDX24 == rmnet_state_config->target ||
      DS_TARGET_FUSION_8084_9X45 == rmnet_state_config->target ||
      DS_TARGET_FUSION4_5_PCIE == rmnet_state_config->target ||
      DS_TARGET_FUSION_8096_9X55 == rmnet_state_config->target )
  {
    if(event_id == QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND)
    {
      LOG_MSG_INFO1("Received QMUXD cb event on conn id %d",
                    event_info->qmi_modem_service_ind.conn_id, 0, 0);

      if(event_info->qmi_modem_service_ind.conn_id ==
         rmnet_state_config->qmux_conn_id)
      {
#ifndef FEATURE_DATA_ADPL_ONLY

        ret_val = qti_rmnet_ph_set_modem_state(MODEM_OUT_OF_SERVICE);
        if (ret_val != QTI_SUCCESS)
        {
          LOG_MSG_ERROR("Failed to set modem state on peripheral driver file", 0, 0, 0);
        }

        if ( rmnet_state_config->ph_iface[PH_DRIVER_TYPE_USB].ph_enabled )
        {
          ph_iface_fd = rmnet_state_config->ph_iface[PH_DRIVER_TYPE_USB].ph_iface_fd;
        }
        else if (rmnet_state_config->ph_iface[PH_DRIVER_TYPE_MHI].ph_enabled)
        {
          ph_iface_fd = rmnet_state_config->ph_iface[PH_DRIVER_TYPE_MHI].ph_iface_fd;
        }
        else
        {
          LOG_MSG_ERROR("Failed to set ph_iface_fd", 0, 0, 0);
          return;
        }

        ret_val = ioctl(ph_iface_fd, FRMNET_CTRL_GET_LINE_STATE, &line_state);
        if (ret_val)
        {
          LOG_MSG_INFO1("Couldn't get FRMNET LINE STATE from driver", 0, 0, 0);
        }
        else if (line_state == 0)
        {
          LOG_MSG_INFO1("USB cable not connected. Dont reset modem rmnet components",
                        0, 0, 0);
          return;
        }
        else if (line_state == 1)
        {

          ret_val = qti_rmnet_data_teardown_bridge(MHI_DATA_INTERFACE, USB_DATA_INTERFACE);
          if (ret_val != QTI_SUCCESS)
          {
            LOG_MSG_ERROR("Failed to teardown data bridge driver", 0, 0, 0);
          }

          ret_val = qti_rmnet_dfs_release();
          if (ret_val != QTI_SUCCESS)
          {
            LOG_MSG_ERROR("Failed to release DFS client", 0, 0, 0);
          }
#endif //FEATURE_DATA_ADPL_ONLY

          if(dpl_state_config->dpl_iface_fd > 0)
          {
            ret_val = ioctl(dpl_state_config->dpl_iface_fd, FRMNET_CTRL_GET_LINE_STATE, &line_state);
            if (ret_val)
            {
              LOG_MSG_INFO1("Couldn't get FRMNET LINE STATE from DPL driver", 0, 0, 0);
            }
            else if (line_state == 0)
            {
              LOG_MSG_INFO1("USB cable not connected. Dont reset modem rmnet components for DPL",
                             0, 0, 0);
             return;
            }
            else if (line_state == 1)
            {
              ret_val = qti_rmnet_data_teardown_bridge(MHI_DPL_DATA_INTERFACE, USB_DPL_DATA_INTERFACE);
              if (ret_val != QTI_SUCCESS)
              {
                LOG_MSG_ERROR("Failed to teardown data bridge driver for DPL", 0, 0, 0);
              }
            }
          }

          LOG_MSG_INFO1("Completed processing modem out of service ind", 0, 0, 0);
#ifndef FEATURE_DATA_ADPL_ONLY
        }
#endif //FEATURE_DATA_ADPL_ONLY

      }
    }
    else if(event_id == QMI_SYS_EVENT_MODEM_IN_SERVICE_IND)
    {
      if(event_info->qmi_modem_service_ind.conn_id ==
         rmnet_state_config->qmux_conn_id)
      {
#ifndef FEATURE_DATA_ADPL_ONLY

        ret_val = qti_rmnet_ph_set_modem_state(MODEM_IN_SERVICE);
        if (ret_val != QTI_SUCCESS)
        {
          LOG_MSG_ERROR("Failed to set modem state on peripheral driver file", 0, 0, 0);
        }

        if ( rmnet_state_config->ph_iface[PH_DRIVER_TYPE_USB].ph_enabled )
        {
          ph_iface_fd = rmnet_state_config->ph_iface[PH_DRIVER_TYPE_USB].ph_iface_fd;
        }
        else if (rmnet_state_config->ph_iface[PH_DRIVER_TYPE_MHI].ph_enabled)
        {
          ph_iface_fd = rmnet_state_config->ph_iface[PH_DRIVER_TYPE_MHI].ph_iface_fd;
        }
        else
        {
          LOG_MSG_ERROR("Failed to set ph_iface_fd", 0, 0, 0);
          return;
        }

        ret_val = ioctl(ph_iface_fd, FRMNET_CTRL_GET_LINE_STATE, &line_state);
        if (ret_val)
        {
          LOG_MSG_INFO1("Couldn't get FRMNET LINE STATE from driver",
                        0, 0, 0);
          return;
        }
        else if (line_state == 0)
        {
          LOG_MSG_INFO1("USB cable not connected. So dont initialize modem components",
                        0, 0, 0);
          return;
        }
        else
        {
          ret_val = qti_rmnet_data_init_bridge(MHI_DATA_INTERFACE, USB_DATA_INTERFACE);
          if (ret_val != QTI_SUCCESS)
          {
            LOG_MSG_ERROR("Failed to init data bridge driver", 0, 0, 0);
          }

          ret_val = qti_rmnet_dfs_init();
          if (ret_val != QTI_SUCCESS)
          {
            LOG_MSG_ERROR("Failed to initialize DFS client", 0, 0, 0);
          }
        }
#endif //FEATURE_DATA_ADPL_ONLY

        if(dpl_state_config->dpl_iface_fd > 0)
        {
          ret_val = ioctl(dpl_state_config->dpl_iface_fd, FRMNET_CTRL_GET_LINE_STATE, &line_state);
          if (ret_val)
          {
            LOG_MSG_INFO1("Couldn't get FRMNET LINE STATE from DPL driver", 0, 0, 0);
          }
          else if (line_state == 0)
          {
            LOG_MSG_INFO1("USB cable not connected. Dont init modem rmnet components for DPL",
                           0, 0, 0);
            return;
          }
          else if (line_state == 1)
          {
            ret_val = qti_rmnet_data_init_bridge(MHI_DPL_DATA_INTERFACE, USB_DPL_DATA_INTERFACE);
            if (ret_val != QTI_SUCCESS)
            {
              LOG_MSG_ERROR("Failed to init data bridge driver for DPL", 0, 0, 0);
            }
          }
        }

        LOG_MSG_INFO1("Completed processing modem in service ind", 0, 0, 0);
      }
    }
    else
    {
      LOG_MSG_INFO1("Ignoring event %d", event_id, 0, 0);
    }
  }
#endif //FEATURE_MDM_LE
}

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_INIT_THRU_QMUXD()

DESCRIPTION

  This function
  - initializes RMNET modem state parameters.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_modem_init_thru_qmuxd
(
  qti_rmnet_param  * rmnet_state,
  qcmap_sock_thrd_fd_read_f read_f,
  qti_dpl_param          * dpl_state
)
{
  int qmi_err = QMI_NO_ERR;
/*-------------------------------------------------------------------------*/

  if (rmnet_state == NULL)
  {
    LOG_MSG_ERROR("Invalid rmnet state data received", 0, 0, 0);
    return QTI_FAILURE;
  }

  rmnet_state_config = rmnet_state;
  dpl_state_config = dpl_state;

/*------------------------------------------------------------------------
  Initialize a QMUX client and obtain a handle
-------------------------------------------------------------------------*/
  qmi_err = qmi_qmux_if_pwr_up_init_ex((qmi_qmux_if_rx_msg_hdlr_type)read_f,
                                       qti_rmnet_modem_event_cb,
                                       NULL,
                                       &rmnet_state_config->qti_qmux_qmi_handle,
                                       QMI_QMUX_IF_CLNT_MODE_RAW);

  if (qmi_err != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Failed to get a QMUX handle for QTI", 0, 0, 0);
    return QTI_FAILURE;
  }
  else
  {
    LOG_MSG_INFO1("Succeeded to get a QMUX handle for QTI", 0, 0, 0);
  }

  return QTI_SUCCESS;
}

#ifndef FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_GET_QMUX_CONN_ID()

DESCRIPTION

  This function
  - maps to the right QMUXD connection id corresponding to the USB tethered
  data call

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/
static int qti_rmnet_get_qmux_conn_id
(
  qmi_connection_id_type *qmi_conn_id
)
{

/*-------------------------------------------------------------------------*/
  if(qmi_conn_id == NULL)
  {
    LOG_MSG_ERROR("Invalid pointer for connection id", 0, 0, 0);
    return QTI_FAILURE;
  }

  *qmi_conn_id = rmnet_state_config->qmux_conn_id;
  return QTI_SUCCESS;
}


/*===========================================================================

FUNCTION QTI_RMNET_QMUX_BUF_ALLOC()

DESCRIPTION

  This function
  - allocates buffer to store the QMI QMUX packet

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure


SIDE EFFECTS
  None

=========================================================================*/

static int qti_rmnet_qmux_buf_alloc
(
  qti_qmux_buf_s *buf,
  uint32_t        size
)
{
  void *data_ptr;
/*-------------------------------------------------------------------------*/
  if(buf == NULL)
  {
    LOG_MSG_ERROR("Invalid pointer for buf passed", 0, 0, 0);
    return QTI_FAILURE;
  }

/*--------------------------------------------------------------------------
  Protect against memory leaks via successive calls to buf_alloc
-------------------------------------------------------------------------- */
  if (buf->data != NULL)
  {
    LOG_MSG_INFO1("Attempted double-alloc of buffer! Old size %d new size %d",
                  buf->size, size, 0);
    free(buf->data);
    buf->data = NULL;
    buf->size = 0;
  }

/*--------------------------------------------------------------------------
  Allocate data for storing QMI packet
--------------------------------------------------------------------------*/
  data_ptr = malloc(size);
  if (data_ptr == NULL)
  {
    LOG_MSG_ERROR("Could not allocate data_ptr ", 0, 0, 0);
    buf->size = 0;
    return QTI_FAILURE;
  }
  else
  {
    memset(data_ptr, 0, size);
    buf->data = data_ptr;
    buf->size = size;
  }

  return QTI_SUCCESS;
}

/*===========================================================================

FUNCTION QTI_LOG_RAW_QMI_MSG()

DESCRIPTION

  This function
  - Logs the hex dumps to QXDM/File

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void qti_log_raw_qmi_msg
(
  const unsigned char  *msg,
  int                  msg_len
)
{
  int i, j;
  char buff[QTI_PLATFORM_NUM_CHARS_PER_BYTE*QTI_PLATFORM_NUM_BYTES_PER_LINE+1] = "";
  char command[QTI_MAX_COMMAND_STR_LEN] = "";
  unsigned char upper_half;
  unsigned char lower_half;
  const unsigned char *data = msg;

  if (NULL == msg)
  {
    return;
  }

  for (i = 1, j = 0; i <= msg_len; ++i, ++data)
  {
    upper_half = (*data) >> 4;
    lower_half = (*data) & 0x0F;
    buff[j++]  = QTI_PLATFORM_GET_HEX_CHAR(upper_half);
    buff[j++]  = QTI_PLATFORM_GET_HEX_CHAR(lower_half);
    buff[j++]  = ' ';

    if (i % QTI_PLATFORM_NUM_BYTES_PER_LINE == 0)
    {
      buff[j] = '\0';
      HEXDUMP_QXDM_MSG("%s\n", buff, 0, 0);
      HEXDUMP_LOG_MSG("%s \n", buff);
      j = 0;
    }
  }

  /* Print any remaining data */
  if (j > 0)
  {
    buff[j] = '\0';
    HEXDUMP_QXDM_MSG ("%s\n", buff, 0, 0);
    HEXDUMP_LOG_MSG("%s \n", buff);
  }
}

/*===========================================================================

FUNCTION QTI_HEX_DUMPS_LOGGING()

DESCRIPTION

  This function
  - Logs the hex dumps to QXDM

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void qti_hex_dumps_logging
(
   void         *data,
   uint32_t     len
)
{
  qti_qmux_msg_s               *qmux_msg;
  qti_qmux_buf_s               qmi_qmux_if_buf;
  int                          actual_data_size;

  if( hex_dumps_file_mode || hex_dumps_qxdm_mode )
  {
    actual_data_size = len - QTI_QMUX_HDR_LEN_BYTES;
    memset(&qmi_qmux_if_buf, 0 , sizeof(qti_qmux_buf_s));

    if (qti_rmnet_qmux_buf_alloc(&qmi_qmux_if_buf,
                                 ((len - QTI_QMUX_HDR_LEN_BYTES) +
                                 QMI_QMUX_IF_MSG_HDR_SIZE)) != QTI_SUCCESS)
    {
      LOG_MSG_ERROR("Couldn't allocate buffer of size %d for qmi_qmux_if",
                    ((len - QTI_QMUX_HDR_LEN_BYTES) + QMI_QMUX_IF_MSG_HDR_SIZE), 0, 0);
    }
    else
    {
      memcpy(((uint8_t *) qmi_qmux_if_buf.data + QMI_QMUX_IF_MSG_HDR_SIZE),
             (uint8_t *)((char*)data + sizeof(qti_qmux_if_type_t) + sizeof(qti_qmux_hdr_s)),
             actual_data_size);

      PRINT_QMI_MSG ((unsigned char *) qmi_qmux_if_buf.data + QMI_QMUX_IF_MSG_HDR_SIZE - QMI_QMUX_HDR_SIZE, (int)actual_data_size + QMI_QMUX_HDR_SIZE);

      free(qmi_qmux_if_buf.data);
      qmi_qmux_if_buf.data = NULL;
      qmi_qmux_if_buf.size = 0;
    }
  }
  return;
}
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
)
{
  int                          ret;
  qti_qmux_buf_s               qmi_qmux_if_buf;
  qmi_connection_id_type       qmi_conn_id;
  qti_qmux_msg_s               *qmux_msg;
  boolean                      ret_val = QTI_FAILURE;
  int                          rc;
  int                          actual_data_size;
/*-----------------------------------------------------------------------*/

  if (data == NULL)
  {
    LOG_MSG_ERROR("Invalid data received", 0, 0, 0);
    return;
  }

  if(DS_TARGET_FUSION_SDM845_SDX24 == rmnet_state_config->target ||
     DS_TARGET_FUSION_8084_9X45 == rmnet_state_config->target ||
     DS_TARGET_FUSION4_5_PCIE == rmnet_state_config->target ||
	 DS_TARGET_FUSION_8096_9X55 == rmnet_state_config->target)
  {
    ret = qti_rmnet_process_qmi_tx_to_modem((qti_qmux_msg_s *)data, len);
    if(ret != QTI_SUCCESS)
    {
      LOG_MSG_ERROR("QMI message not parsed correctly", 0, 0, 0);
      return;
    }
  }
  LOG_MSG_INFO1("Will send QMI message of size %d to modem", len, 0 , 0);

  HEXDUMP_LOG_MSG("Will send QMI message of size %d to modem", len);
  qti_hex_dumps_logging(data, len);

  memset(&qmi_qmux_if_buf, 0 , sizeof(qti_qmux_buf_s));

  if (rmnet_state_config->qti_qmux_qmi_handle == 0)
  {
    LOG_MSG_ERROR("Tried to send QMUX message to modem, but qmux handle is invalid",
                   0, 0, 0);
  }
  else if (len < QTI_QMUX_MIN_MSG_LEN_BYTES)
  {
    LOG_MSG_ERROR("Not sending short QMUX message to the modem! Got %d bytes, min %d",
                  len, QTI_QMUX_MIN_MSG_LEN_BYTES, 0);
  }
  else if (qti_rmnet_get_qmux_conn_id(&qmi_conn_id)!= QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Couldn't find connection ID", 0, 0, 0);
  }
  else if (qti_rmnet_qmux_buf_alloc(&qmi_qmux_if_buf,
                                    ((len - QTI_QMUX_HDR_LEN_BYTES) +
                                    QMI_QMUX_IF_MSG_HDR_SIZE)) != QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Couldn't allocate buffer of size %d for qmi_qmux_if",
                  ((len - QTI_QMUX_HDR_LEN_BYTES) + QMI_QMUX_IF_MSG_HDR_SIZE), 0, 0);
  }
  else
  {
    qmux_msg = (qti_qmux_msg_s *)data;
    actual_data_size = len - QTI_QMUX_HDR_LEN_BYTES;
    if (qmux_msg->qmux_hdr.svc_type == QTI_QMUX_SVC_TYPE_QMI_CTL)
    {
/*----------------------------------------------------------------------------
  The qmi_qmux_if APIs expect to be passed a buffer that has
  QMI_QMUX_IF_MSG_HDR_SIZE bytes of free space available in front of the
  buffer containing the SDU.
-----------------------------------------------------------------------------*/
      memcpy(((uint8_t *) qmi_qmux_if_buf.data + QMI_QMUX_IF_MSG_HDR_SIZE),
             (uint8_t *)((char*)data + sizeof(qti_qmux_if_type_t) + sizeof(qti_qmux_hdr_s)),
             actual_data_size);

      rc = qmi_qmux_if_send_raw_qmi_cntl_msg(
                rmnet_state_config->qti_qmux_qmi_handle,
                qmi_conn_id,
                ((unsigned char *) qmi_qmux_if_buf.data + QMI_QMUX_IF_MSG_HDR_SIZE),
                actual_data_size);
    }
/*----------------------------------------------------------------------------
 Send QMI_SDU message
-----------------------------------------------------------------------------*/
    else
    {

/*----------------------------------------------------------------------------
  The qmi_qmux_if APIs expect to be passed a buffer that has
  QMI_QMUX_IF_MSG_HDR_SIZE bytes of free space available in front of the
  buffer containing the SDU.
-----------------------------------------------------------------------------*/
       memcpy(((uint8_t *) qmi_qmux_if_buf.data + QMI_QMUX_IF_MSG_HDR_SIZE),
              (uint8_t *)((char*)data + sizeof(qti_qmux_if_type_t) + sizeof(qti_qmux_hdr_s)),
               actual_data_size);

       rc = qmi_qmux_if_send_qmi_msg(
               rmnet_state_config->qti_qmux_qmi_handle,
               qmi_conn_id,
               (qmi_service_id_type) qmux_msg->qmux_hdr.svc_type,
               (qmi_client_id_type) qmux_msg->qmux_hdr.client_id,
               ((unsigned char *) qmi_qmux_if_buf.data + QMI_QMUX_IF_MSG_HDR_SIZE),
               actual_data_size);
    }

    if (rc != QMI_NO_ERR)
    {
      LOG_MSG_ERROR("Error %d when sending QMI message for svc %d",
                     rc, qmux_msg->qmux_hdr.svc_type, 0);
    }
    else
    {
      LOG_MSG_INFO1("Successfully sent QMI message to modem", 0, 0, 0);
      ret_val = QTI_SUCCESS;
    }

    free(qmi_qmux_if_buf.data);
    qmi_qmux_if_buf.data = NULL;
    qmi_qmux_if_buf.size = 0;
  }

  return;

}
#endif //FEATURE_DATA_ADPL_ONLY


/*===========================================================================

FUNCTION QTI_RMNET_MODEM_RECV_MSG_THRU_QMUXD()

DESCRIPTION

  This function
  - receives QMI messages from modem

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
)
{
#ifndef FEATURE_DATA_ADPL_ONLY
  int ret;
  int                    ret_val;
  qti_qmux_buf_s         buf;
  qti_qmux_msg_s         *qmux_msg;


 /*-------------------------------------------------------------------------*/

  if (rx_msg == NULL)
  {
    LOG_MSG_ERROR("Invalid RX data received", 0, 0, 0);
    return;
  }

  LOG_MSG_INFO1("Received QMUX data buffer with size %d on connection ID %d",
                 rx_msg_len, conn_id, 0);

  if (service_id == QMI_CTL_SERVICE &&
      rx_msg_len < (int)sizeof(qti_qmux_qmi_ctl_sdu_s))
  {
    LOG_MSG_ERROR("Received short QMI payload: %d bytes (minimum %d)",
                   rx_msg_len, sizeof(qti_qmux_qmi_ctl_sdu_s), 0);
    return;
  }
  else if (service_id != QMI_CTL_SERVICE &&
           rx_msg_len < (int)sizeof(qti_qmux_sdu_s))
  {
    LOG_MSG_ERROR("Received short regular QMUX payload: %d bytes (minimum %d)",
                   rx_msg_len, sizeof(qti_qmux_sdu_s), 0);
    return;
  }
  else if ( conn_id != rmnet_state_config->qmux_conn_id)
  {
    LOG_MSG_ERROR("Received message on invalid connection id %d",
                   conn_id, 0, 0);
    return;
  }
  else
  {
    memset(&buf, 0 , sizeof(qti_qmux_buf_s));
    if (qti_rmnet_qmux_buf_alloc(&buf, (rx_msg_len + QTI_QMUX_HDR_LEN_BYTES)) != QTI_SUCCESS)
    {
      LOG_MSG_ERROR("Couldn't allocate buffer of size %d bytes for QMUX response!",
                    (rx_msg_len + QTI_QMUX_HDR_LEN_BYTES), 0, 0);
    }
    else
    {

/*----------------------------------------------------------------------------
  Add the QMUX header
---------------------------------------------------------------------------- */
      qmux_msg = (qti_qmux_msg_s *) buf.data;
      qmux_msg->if_type = QTI_QMUX_IF_TYPE_QMUX;
      qmux_msg->qmux_hdr.length    = (rx_msg_len + sizeof(qti_qmux_hdr_s));
      qmux_msg->qmux_hdr.ctl_flags = (uint8_t) control_flags;
      qmux_msg->qmux_hdr.svc_type  = (uint8_t) service_id;
      qmux_msg->qmux_hdr.client_id = (uint8_t) client_id;
      memcpy(&(qmux_msg->sdu), rx_msg, rx_msg_len);

      if( DS_TARGET_FUSION_SDM845_SDX24 == rmnet_state_config->target ||
          DS_TARGET_FUSION_8084_9X45 == rmnet_state_config->target ||
          DS_TARGET_FUSION4_5_PCIE == rmnet_state_config->target ||
          DS_TARGET_FUSION_8096_9X55 == rmnet_state_config->target )
      {
        ret_val = qti_rmnet_process_qmi_rx_from_modem(
                           (qti_qmux_msg_s *)qmux_msg,
                           (rx_msg_len + QTI_QMUX_HDR_LEN_BYTES));
        if(ret_val != QTI_SUCCESS)
        {
          LOG_MSG_ERROR("QMI message not parsed correctly", 0, 0, 0);
        }
      }
/*----------------------------------------------------------------------------
  Write QMI message into peripheral device file
---------------------------------------------------------------------------- */
      HEXDUMP_LOG_MSG("Received QMUX data buffer with size %d on connection ID %d",
                       rx_msg_len, conn_id);
      qti_hex_dumps_logging((qti_qmux_msg_s *)qmux_msg,
                            (rx_msg_len + QTI_QMUX_HDR_LEN_BYTES));

      qti_rmnet_ph_send_msg((qti_qmux_msg_s *)qmux_msg,
                            (rx_msg_len + QTI_QMUX_HDR_LEN_BYTES));

      free(buf.data);
      buf.data = NULL;
      buf.size = 0;
    }
    return;
  }
 #endif //FEATURE_DATA_ADPL_ONLY
}

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_INIT()

DESCRIPTION

  This function
  - initializes RMNET modem state parameters.
  - opens the modem file node.
  - maps the modem file node with its read function.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/

int qti_rmnet_modem_init
(
  qti_rmnet_param  * rmnet_state,
  qti_dpl_param    * dpl_state
)
{
  int ret_val;
/*-------------------------------------------------------------------------*/

  if (rmnet_state == NULL || dpl_state == NULL)
  {
    LOG_MSG_ERROR("Invalid rmnet state data received", 0, 0, 0);
    return QTI_FAILURE;
  }

  rmnet_state_config = rmnet_state;
  dpl_state_config = dpl_state;
#ifndef FEATURE_DATA_ADPL_ONLY

  LOG_MSG_INFO1("Open mode interface to send QMI messages", 0, 0, 0);

  ret_val = qti_file_open(rmnet_state_config->modem_iface_device_file,
                          &(rmnet_state_config->modem_iface_fd));

  if(ret_val == QTI_FAILURE)
  {
    LOG_MSG_ERROR("Failed to open modem interface device file. Abort. Error %d",
                   errno, 0, 0);
    return QTI_FAILURE;
  }
  else
  {
    LOG_MSG_INFO1("Opened file's fd is %d", rmnet_state_config->modem_iface_fd, 0, 0);
    if( qcmap_nl_addfd_map(rmnet_state_config->fd_set,
                           rmnet_state_config->modem_iface_fd,
                           qti_rmnet_modem_recv_msg,
                           MAX_NUM_OF_FD) != QCMAP_UTIL_SUCCESS)
    {
      LOG_MSG_ERROR("Failed to map fd with the read function",0,0,0);
      close(rmnet_state_config->modem_iface_fd);
      return QCMAP_UTIL_FAILURE;
    }
  }
#endif //FEATURE_DATA_ADPL_ONLY

  return QTI_SUCCESS;
}
#ifndef FEATURE_DATA_ADPL_ONLY
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


int qti_rmnet_modem_reset()
{
  int   ret_val;
/*----------------------------------------------------------------------*/

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return QTI_FAILURE;
  }

  FD_CLR(rmnet_state_config->modem_iface_fd,
         &(rmnet_state_config->fd_set->fdset));
  ret_val = qti_clear_fd(rmnet_state_config->fd_set,
                         rmnet_state_config->modem_iface_fd);
  if (ret_val != QTI_SUCCESS)
  {
    LOG_MSG_ERROR("Failed to clear fd from fd set", 0, 0, 0);
  }
  close(rmnet_state_config->modem_iface_fd);
  rmnet_state_config->modem_iface_fd = 0;
  LOG_MSG_INFO1("Closing the modem fd ",
                 0, 0, 0);

  return QTI_SUCCESS;
}



/*===========================================================================

FUNCTION QTI_RMNET_MODEM_SEND_MSG()

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

void qti_rmnet_modem_send_msg
(
   void      *data,
   int       len
)
{
  int                          ret;
  char                         command[QTI_MAX_COMMAND_STR_LEN];
/*-----------------------------------------------------------------------*/

  if (data == NULL)
  {
    LOG_MSG_ERROR("Invalid data received", 0, 0, 0);
    return;
  }

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return;
  }

  HEXDUMP_LOG_MSG("Hex dumps received from peripheral: \n");
  qti_hex_dumps_logging(data, len);

  if( (DS_TARGET_FUSION_SDM845_SDX24 == rmnet_state_config->target) ||
      (DS_TARGET_FUSION4_5_PCIE == rmnet_state_config->target) ||
      (DS_TARGET_JOLOKIA == rmnet_state_config->target) ||
      (DS_TARGET_LYKAN == rmnet_state_config->target) ||
      (DS_TARGET_FEERO == rmnet_state_config->target))
  {
    ret = qti_rmnet_process_qmi_tx_to_modem(data, len );
    if(ret != QTI_SUCCESS)
    {
      LOG_MSG_ERROR("QMI message not parsed correctly", 0, 0, 0);
      return;
    }

    HEXDUMP_LOG_MSG("Hex dumps received from peripheral after EP translation: \n");
    HEXDUMP_QXDM_MSG("Hex dumps received from peripheral after EP translation", 0, 0, 0);
    qti_hex_dumps_logging(data, len);
  }
  LOG_MSG_INFO1("Will send QMI message of size %d to modem", len, 0 , 0);

  if (rmnet_state_config->modem_iface_fd == 0)
  {
    LOG_MSG_ERROR("Tried to send QMI message to modem, but modem interface not inited",
                   0, 0, 0);
    return;
  }

  ret = write(rmnet_state_config->modem_iface_fd, (char*)data, len);
  if (ret == -1)
  {
    LOG_MSG_ERROR("Couldn't send message to modem. Errno %d", errno, 0, 0);
  }
  else if (ret != len)
  {
    LOG_MSG_ERROR_6("Unexpected return value when writing to device file: got %d,"
                    "expected %d",
                    ret,
                    len,
                    0, 0, 0, 0);
  }
  else
  {
    LOG_MSG_INFO1("Successfully sent message to modem size %d on fd %d",
                  ret, rmnet_state_config->modem_iface_fd, 0);
  }

  return;

}

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_RECV_MSG()

DESCRIPTION

  This function
  - receives QMI messages from modem

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
)
{
  int                   buf_length;
  int                   ret_val;
  char                  command[QTI_MAX_COMMAND_STR_LEN];
  char                  *modem_rx_buf = NULL;
/*-------------------------------------------------------------------------*/

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return QTI_FAILURE;
  }

  modem_rx_buf = (char*)malloc(RMNET_MAX_TRANSFER_SIZE);
  if(modem_rx_buf == NULL)
  {
    LOG_MSG_ERROR("No memory to allocate buffer", 0, 0, 0);
    return QTI_FAILURE;
  }
  memset(modem_rx_buf, 0, RMNET_MAX_TRANSFER_SIZE);

  buf_length = read(rmnet_modem_fd, modem_rx_buf, RMNET_MAX_TRANSFER_SIZE);

  LOG_MSG_INFO1("Read %d data from modem device file", buf_length, 0, 0);

  if (buf_length < 0)
  {
    LOG_MSG_ERROR("Failed to read from the dev file. Errno %d error msg=%s",
                   errno, strerror (errno), 0);
    if (errno == QTI_ENETRESET )
    {
      if (rmnet_state_config->modem_iface_fd)
      {
        ret_val = qti_rmnet_modem_reset();
        if (ret_val != QTI_SUCCESS)
        {
          LOG_MSG_ERROR("Failed to reset modem interface node", 0, 0, 0);
        }
      }
    }
    free(modem_rx_buf);
    return QTI_FAILURE;
  }
  else if (buf_length > RMNET_MAX_TRANSFER_SIZE)
  {
    LOG_MSG_ERROR("Received data larger than dest buff %d",
                   errno, 0, 0);
    free(modem_rx_buf);
    return QTI_FAILURE;
  }

  HEXDUMP_LOG_MSG("Hex dumps of data received from modem: \n");
  qti_hex_dumps_logging(modem_rx_buf, buf_length);

/*----------------------------------------------------------------------------
  Check to see if QMI response message needs to altered wrt EP info
----------------------------------------------------------------------------*/
  if((DS_TARGET_FUSION_SDM845_SDX24 == rmnet_state_config->target ) ||
     (DS_TARGET_FUSION4_5_PCIE == rmnet_state_config->target ) ||
     (DS_TARGET_JOLOKIA == rmnet_state_config->target ) ||
     (DS_TARGET_LYKAN == rmnet_state_config->target) ||
     (DS_TARGET_FEERO == rmnet_state_config->target))
  {
    ret_val = qti_rmnet_process_qmi_rx_from_modem(
                      modem_rx_buf,
                      buf_length);
    if(ret_val != QTI_SUCCESS)
    {
      LOG_MSG_ERROR("QMI message not parsed correctly", 0, 0, 0);
    }

    HEXDUMP_LOG_MSG("Hex dumps received from modem after EP translation: \n");
    HEXDUMP_QXDM_MSG("Hex dumps received from modem after EP translation", 0, 0, 0);
    qti_hex_dumps_logging(modem_rx_buf, buf_length);
  }

/*----------------------------------------------------------------------------
  Process and send QMI message to host
----------------------------------------------------------------------------*/
  qti_rmnet_ph_send_msg(modem_rx_buf, buf_length);

  free(modem_rx_buf);
  return QTI_SUCCESS;
}

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_SET_DTR()

DESCRIPTION

  This function
  - toggles DTR.

DEPENDENCIES
  None.

RETURN VALUE
  QTI_SUCCESS on success
  QTI_FAILURE on failure

SIDE EFFECTS
  None

=========================================================================*/
int qti_rmnet_modem_set_dtr(uint8_t set)
{
  int             fd = 0;
  int             dtr_sig;
  int             ret = QTI_FAILURE;
  int             close_on_exit = FALSE;
  char            *mhi_file_name=NULL;
  char            mhi_file[MHI_PATH_STRING_LEN];

/*-------------------------------------------------------------------------
  In case modem fd is not open open it and make sure we close on it on
  function exit. If modem fd is already open use that and there will be
  no need to close it since it will be tracked outside of this function.
-------------------------------------------------------------------------*/
  memset(mhi_file, 0, MHI_PATH_STRING_LEN * sizeof(char));

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return ret;
  }

  if (!rmnet_state_config->modem_iface_fd)
  {
    close_on_exit = TRUE;
    if(DS_TARGET_FUSION_SDM845_SDX24 == rmnet_state_config->target)
    {
      if ( rmnet_state_config->modem_iface_device_file != ' ')
      {
        mhi_file_name = (char*)calloc(1,sizeof(char)*MHI_PATH_STRING_LEN);
        if (mhi_file_name == NULL)
        {
          LOG_MSG_ERROR("Failed in allocating memory to mhi_file_name", 0, 0, 0);
          return QTI_FAILURE;
        }
        if (retrieve_mhi_file(&mhi_file_name)!= QTI_FAILURE)
        {
          snprintf(mhi_file, MHI_PATH_STRING_LEN, "%s%s", DEV_FOLDER, mhi_file_name);
          memcpy(rmnet_state_config->modem_iface_device_file,
                 mhi_file,
                 strlen(mhi_file));
          free(mhi_file_name);
        }
        else
        {
           LOG_MSG_ERROR("Unable to retrieve the MHI file name from esoc link info",0,0,0);
           free(mhi_file_name);
           return QTI_FAILURE;
        }
      }
    }
    fd = open(rmnet_state_config->modem_iface_device_file, O_RDWR);
    if (fd < 0)
    {
      LOG_MSG_ERROR("Opening the device file failed. errno %d", errno, 0, 0);
      return QTI_FAILURE;
    }
  }
  else
  {
    fd = rmnet_state_config->modem_iface_fd;
  }

  LOG_MSG_INFO1("Request to set DTR %d", set, 0, 0);

/*-------------------------------------------------------------------------
  Set DTR high
-------------------------------------------------------------------------*/
  if (set == SET_DTR_HIGH)
  {
    dtr_sig = 0;

    LOG_MSG_INFO1("Setting DTR bit HIGH",0, 0, 0);

/*--------------------------------------------------------------------------
TIOCM_DTR - Data Terminal Ready
TIOCM_RTS - Request To Send
TIOCM_CD - Data Carrier Detect
----------------------------------------------------------------------------*/

    dtr_sig |= (TIOCM_DTR|TIOCM_RTS|TIOCM_CD);

    LOG_MSG_INFO1("DTR bit will be set to dtr_sig:%d ",
                  dtr_sig, 0, 0);

    if((ioctl(fd, TIOCMSET, (void *)&dtr_sig)) == -1)
    {
      LOG_MSG_ERROR("Ioctl call to set DTR bit failed. DTR %d error num=%d error=%s",
                     dtr_sig, errno,strerror (errno));
    }
    LOG_MSG_INFO1("Success Setting DTR bit HIGH ",0, 0, 0);

    ret=QTI_SUCCESS;

  }
/*-------------------------------------------------------------------------
  Set DTR low
-------------------------------------------------------------------------*/
  else if (set == SET_DTR_LOW)
  {
    dtr_sig = 0;

    LOG_MSG_INFO1("Setting DTR bit LOW", 0, 0, 0);

    dtr_sig |= (~(TIOCM_DTR |TIOCM_RTS |TIOCM_CD));

    LOG_MSG_INFO1("Will clear DTR bit. DTR %d", dtr_sig, 0, 0);

    if(ioctl(fd, TIOCMSET, (void *)&dtr_sig) == -1)
    {
      LOG_MSG_ERROR("Ioctl call to reset DTR bit failed. DTR %d error num=%d error=%s",
                     dtr_sig, errno,strerror (errno));
    }

    LOG_MSG_INFO1("Success Setting DTR bit LOW ",0, 0, 0);
    ret=QTI_SUCCESS;

  }

  if (close_on_exit && (fd > 0))
  {
    LOG_MSG_INFO1("Closing fd after DTR set", 0, 0, 0);
    close(fd);
  }

  return ret;
}

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
)
{
  int fd;
  int rval = QTI_FAILURE;
  struct ifreq ifr;
/*-----------------------------------------------------------------------*/

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return rval;
  }

  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 )
  {
    LOG_MSG_ERROR("ifioctl_set: socket failed", 0, 0, 0);
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  strlcpy(ifr.ifr_name,
          data_iface,
          strlen(MHI_DATA_INTERFACE)+1);

  ifr.ifr_data = (void*)(intptr_t)mtu;

  /* Set if MTU for the device */
  if( ioctl( fd, SIOCSIFMTU, &ifr ) < 0 )
  {
    LOG_MSG_ERROR("ifioctl_set: SIOCSIFMTU ioctl failed err %d", errno, 0, 0);
    close(fd);
    goto error;
  }

  /* Close temporary socket */
  close(fd);

  rval = QTI_SUCCESS;

  error:
  return rval;

}
#endif //FEATURE_DATA_ADPL_ONLY

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
)
{
  int fd;
  int rval = QTI_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;

/*-----------------------------------------------------------------------*/
  if(ep_id == NULL)
  {
    LOG_MSG_ERROR("Got invalid EP ID", 0, 0, 0);
    return QTI_FAILURE;
  }

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return rval;
  }

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("get_epid: socket failed", 0, 0, 0);
    goto error;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg)
  {
    LOG_MSG_ERROR("get_epid: malloc for ifru_data failed", 0, 0, 0);
    close(fd);
    goto error;
  }

  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  strlcpy(ifr.ifr_name,
          data_iface,
          strlen(MHI_DATA_INTERFACE)+1);
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_GET_EPID;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0)
  {
    LOG_MSG_ERROR("get_epid: ioctl RMNET_IOCTL_GET_EPID failed err %d", errno, 0, 0);
    close(fd);
    free(ext_ioctl_arg);
    goto error;
  }

  *ep_id = ext_ioctl_arg->u.data;
  LOG_MSG_INFO1("get_epid: ioctl RMNET_IOCTL_GET_EPID as %d",
                ext_ioctl_arg->u.data, 0, 0);

  /* close temporary socket */
  close(fd);
  free(ext_ioctl_arg);
  rval = QTI_SUCCESS;

error:
  return rval;
}

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
)
{
  int fd;
  int rval = QTI_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;
/*-----------------------------------------------------------------------*/

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return rval;
  }

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("set_mtu: socket failed", 0, 0, 0);
    goto error;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg)
  {
    LOG_MSG_ERROR("set_mru: malloc for ifru_data failed", 0, 0, 0);
    close(fd);
    goto error;
  }


  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  strlcpy(ifr.ifr_name,
          data_iface,
          strlen(MHI_DATA_INTERFACE)+1);
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_SET_MRU;
  ext_ioctl_arg->u.data = mru;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0)
  {
    LOG_MSG_ERROR("set_mru: ioctl RMNET_IOCTL_SET_MRU failed err %d", errno, 0, 0);
    close(fd);
    free(ext_ioctl_arg);
    goto error;
  }

  LOG_MSG_INFO1("set_mru: ioctl RMNET_IOCTL_SET_MRU as %d",
                ext_ioctl_arg->u.data, 0, 0);

  /* close temporary socket */
  close(fd);
  free(ext_ioctl_arg);
  rval = QTI_SUCCESS;

error:
  return rval;
}
#ifndef FEATURE_DATA_ADPL_ONLY

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
)
{
  int fd;
  int rval = QTI_FAILURE;
  struct ifreq ifr;
  struct rmnet_ioctl_extended_s *ext_ioctl_arg;
/*-----------------------------------------------------------------------*/

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return rval;
  }

  /* Open a datagram socket to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("change_sleep_state: socket failed", 0, 0, 0);
    goto error;
  }

  /* Allocate the extended IOCTL argument */
  ext_ioctl_arg = (void *) malloc (sizeof(struct rmnet_ioctl_extended_s));
  if (NULL == ext_ioctl_arg)
  {
    LOG_MSG_ERROR("change_sleep_state: malloc for ifru_data failed", 0, 0, 0);
    close(fd);
    goto error;
  }

  /* Initialize the ioctl req struct */
  memset(&ifr, 0, sizeof(ifr));
  strlcpy(ifr.ifr_name,
          rmnet_state_config->modem_data_iface_name,
          strlen(MHI_DATA_INTERFACE)+1);
  ifr.ifr_ifru.ifru_data = ext_ioctl_arg;
  memset(ext_ioctl_arg, 0, sizeof(struct rmnet_ioctl_extended_s));
  ext_ioctl_arg->extended_ioctl = RMNET_IOCTL_SET_SLEEP_STATE;
  ext_ioctl_arg->u.data = sleep_state;

  /* Get current if flags for the device */
  if (ioctl(fd, RMNET_IOCTL_EXTENDED, &ifr) < 0)
  {
    LOG_MSG_ERROR("change_sleep_state: ioctl for change sleep state failed err %d",
                  errno, 0, 0);
    close(fd);
    free(ext_ioctl_arg);
    goto error;
  }

  LOG_MSG_INFO1("change_sleep_state: ioctl set sleep state as %d",
                sleep_state, 0, 0);

  /* close temporary socket */
  close(fd);
  free(ext_ioctl_arg);
  rval = QTI_SUCCESS;

error:
  return rval;
}
#endif // FEATURE_DATA_ADPL_ONLY

/*===========================================================================

FUNCTION QTI_RMNET_MODEM_NOT_IN_SERVICE()

DESCRIPTION
  Function invoked during SSR functionality, when modem is not in service.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

==========================================================================*/
void qti_rmnet_modem_not_in_service()
{
  int        ret_val;
  qmi_idl_service_object_type dpm_svc_obj;
  int rc = QMI_INTERNAL_ERR;

#ifndef FEATURE_MDM_LE

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    return;
  }

  if( (DS_TARGET_MSM8994 == rmnet_state_config->target) ||
      (DS_TARGET_JOLOKIA == rmnet_state_config->target) ||
      (DS_TARGET_MSM8992 == rmnet_state_config->target) ||
      (DS_TARGET_MSM8996 == rmnet_state_config->target) ||
      (DS_TARGET_MSM8998 == rmnet_state_config->target) ||
      (DS_TARGET_MSMNILE == rmnet_state_config->target) ||
      (DS_TARGET_SDM845 == rmnet_state_config->target) ||
      (DS_TARGET_TABASCO == rmnet_state_config->target) ||
      (DS_TARGET_ELDARION == rmnet_state_config->target)||
      (DS_TARGET_FEERO == rmnet_state_config->target) ||
      (DS_TARGET_JACALA == rmnet_state_config->target) ||
      (DS_TARGET_FEERO6 == rmnet_state_config->target)||
      (DS_TARGET_FEEROLITE6 == rmnet_state_config->target)||
      (DS_TARGET_STARLORD == rmnet_state_config->target) ||
      (DS_TARGET_STARHAWK == rmnet_state_config->target) ||
      (DS_TARGET_WARLOCK  == rmnet_state_config->target))
  {
    dpm_svc_obj = dpm_get_service_object_v01();
    if( dpm_svc_obj == NULL )
    {
      LOG_MSG_ERROR("Failed to get dpm service object", 0, 0, 0);
      return;
    }

  #ifndef FEATURE_DATA_ADPL_ONLY
    if (rmnet_state_config->modem_iface_fd)
    {
      ret_val = qti_rmnet_modem_reset();
      if (ret_val != QTI_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to reset modem interface node", 0, 0, 0);
      }
    }

    ret_val = qti_rmnet_ph_set_modem_state(MODEM_OUT_OF_SERVICE);
    if (ret_val != QTI_SUCCESS)
    {
      LOG_MSG_ERROR("Failed to set modem state on peripheral driver file", 0, 0, 0);
    }
#endif //FEATURE_DATA_ADPL_ONLY
    ret_val = qti_rmnet_dpm_release();
    if (ret_val != QTI_SUCCESS)
    {
      LOG_MSG_ERROR("Failed to release DPM client", 0, 0, 0);
    }

    is_ssr_in_progress = true;
    LOG_MSG_INFO1("qti rmnet modem not in service"
                  " going to register DPM call back\n",0, 0,0);
/*----------------------------------------------------------------------------
    Register icallback to get notified when modem is in service
---------------------------------------------------------------------------- */
    (void) qmi_client_notifier_init(dpm_svc_obj, &qti_dpm_os_params, &dpm_notifier);
    rc = qmi_client_register_notify_cb(dpm_notifier,dpm_service_available_cb , (void *)NULL);
    if (QMI_NO_ERR != rc)
    {
      LOG_MSG_ERROR("failed to register dpm notifier. clnt_hndl=%p rc=%d!\n",
                    dpm_notifier, rc,0);
    }

    LOG_MSG_INFO1("Completed processing modem out of service ind", 0, 0, 0);
  }

#endif
}

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

void qti_rmnet_modem_in_service()
{
  int                        ret_val;
  int                        line_state;
  qmi_client_error_type      qmi_error;
  int                        ph_iface_fd;

#ifndef FEATURE_MDM_LE

  if (rmnet_state_config == NULL)
  {
    LOG_MSG_ERROR("Rmnet state config not set", 0, 0, 0);
    goto bail;
  }

  if( (DS_TARGET_MSM8994 == rmnet_state_config->target) ||
      (DS_TARGET_JOLOKIA == rmnet_state_config->target) ||
      (DS_TARGET_MSM8992 == rmnet_state_config->target) ||
      (DS_TARGET_MSM8996 == rmnet_state_config->target) ||
      (DS_TARGET_MSM8998 == rmnet_state_config->target) ||
      (DS_TARGET_SDM845 == rmnet_state_config->target) ||
      (DS_TARGET_TABASCO == rmnet_state_config->target) ||
      (DS_TARGET_ELDARION == rmnet_state_config->target)||
      (DS_TARGET_FEERO == rmnet_state_config->target) ||
      (DS_TARGET_JACALA == rmnet_state_config->target) ||
      (DS_TARGET_FEERO6 == rmnet_state_config->target)||
      (DS_TARGET_FEEROLITE6 == rmnet_state_config->target)||
      (DS_TARGET_STARLORD == rmnet_state_config->target) ||
      (DS_TARGET_STARHAWK == rmnet_state_config->target) ||
      (DS_TARGET_WARLOCK  == rmnet_state_config->target))
  {
#ifndef FEATURE_DATA_ADPL_ONLY
    ret_val = qti_rmnet_ph_set_modem_state(MODEM_IN_SERVICE);
    if (ret_val != QTI_SUCCESS)
    {
      LOG_MSG_ERROR("Failed to set modem state on peripheral driver file", 0, 0, 0);
    }

    if ( rmnet_state_config->ph_iface[PH_DRIVER_TYPE_USB].ph_enabled )
    {
      ph_iface_fd = rmnet_state_config->ph_iface[PH_DRIVER_TYPE_USB].ph_iface_fd;
    }
    else if (rmnet_state_config->ph_iface[PH_DRIVER_TYPE_MHI].ph_enabled)
    {
      ph_iface_fd = rmnet_state_config->ph_iface[PH_DRIVER_TYPE_MHI].ph_iface_fd;
    }
    else
    {
      LOG_MSG_ERROR("Failed to set ph_iface_fd", 0, 0, 0);
      /*This means Modem is UP but may be USB is not yet connected or in RNDIS mode*/
      goto bail;
    }

    ret_val = ioctl(ph_iface_fd, FRMNET_CTRL_GET_LINE_STATE, &line_state);
    if (ret_val)
    {
      LOG_MSG_INFO1("Couldn't get FRMNET LINE STATE from driver",
                     0, 0, 0);
      goto bail;
    }
    else if (line_state == 0)
    {
      LOG_MSG_INFO1("USB cable not connected. So dont initialize modem components",
                    0, 0, 0);
      goto bail;
    }
    else if (line_state == 1)
    {
      ret_val = qti_rmnet_dpm_port_open();
      if (ret_val != QTI_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to open SMD port through DPM", 0, 0, 0);
        goto bail;
      }
/*----------------------------------------------------------------------------
  SMD/modem channels are opened from Apps
----------------------------------------------------------------------------*/
      if(qti_rmnet_modem_init(rmnet_state_config,
                              dpl_state_config) < 0)
      {
        LOG_MSG_ERROR("QTI failed to init modem interface", 0, 0, 0);
        goto bail;
      }
/*----------------------------------------------------------------------------
  Inform USB driver about modem in service.
----------------------------------------------------------------------------*/
      ret_val = qti_rmnet_ph_set_modem_state(MODEM_IN_SERVICE);
      if (ret_val != QTI_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to set modem state on peripheral driver file", 0, 0, 0);
        goto bail;
      }

      if(qti_rmnet_modem_set_dtr(SET_DTR_HIGH)<0)
      {
        LOG_MSG_ERROR("QTI failed to signal DTR high ", 0, 0, 0);
        goto bail;
      }
      else
      {
        LOG_MSG_INFO1("QTI succeeded to signal DTR high",0, 0, 0);
      }
      rmnet_state_config->dtr_enabled = 1;
    }
#endif //FEATURE_DATA_ADPL_ONLY
    ret_val = ioctl(dpl_state_config->dpl_iface_fd, FRMNET_CTRL_GET_LINE_STATE, &line_state);
    if (ret_val)
    {
      LOG_MSG_INFO1("Couldn't get LINE STATE from DPL driver",
                     0, 0, 0);
      goto bail;
    }
    else if (line_state == 0)
    {
      LOG_MSG_INFO1("USB DPL cable not connected. So dont initialize DPL components",
                    0, 0, 0);
      goto bail;
    }
    else if (line_state == 1)
    {
      ret_val = qti_dpl_dpm_port_open();
      if (ret_val != QTI_SUCCESS)
      {
        LOG_MSG_ERROR("Failed to open DPL port through DPM", 0, 0, 0);
        goto bail;
      }
    }

    LOG_MSG_INFO1("Completed processing modem in service ind", 0, 0, 0);
    bail:
     is_ssr_in_progress = false;
     return ;
  }

#endif
}

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
)
{
  int fd;
  int rval = FALSE;
  struct ifreq ifr;
/*-----------------------------------------------------------------------*/
  /* Open a datagram socket to use for issuing the ioctl */
  if( (fd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 )
  {
    LOG_MSG_ERROR("ifioctl: socket failed", 0, 0, 0);
    goto error;
  }

  /* Initialize the ioctl req struct to null */
  memset(&ifr, 0, sizeof(ifr));

  /* Set device name in ioctl req struct */
  strlcpy(ifr.ifr_name,
          data_iface,
          strlen(MHI_DATA_INTERFACE)+1);


  /* Check if iface is up*/
  if( ioctl( fd, SIOCGIFFLAGS, &ifr ) < 0 )
  {
    LOG_MSG_ERROR("ifioctl_set: SIOCGIFFLAGS ioctl failed err %d", errno, 0, 0);
    close(fd);
    goto error;
  }

  if(ifr.ifr_flags & IFF_UP)
  {
    rval = TRUE;
  }

  /* Close temporary socket */
  close(fd);

  error:
  return rval;

}
