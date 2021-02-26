/******************************************************************************

                        QMI_IP.C

******************************************************************************/

/******************************************************************************

  @file    qmi_ip.c
  @brief   Qualcomm mapping interface over IP

  DESCRIPTION


  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
08/30/13   tw         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "ds_string.h"
#include "qmi_ip.h"

/*===========================================================================
                              VARIABLE DECLARATIONS
===========================================================================*/

int multicast_server_finished;
int acceptTCPConnection;
int heartbeat_check;
int heartbeat_response;
int shut_down;
int qmi_ip_server_fd = -1;
int tcp_connection_active;

int                 qmi_handle;
int                 wds_clnt_hndl;
qmi_client_type     qmi_nas_handle;
dsi_call_info_t     dsi_net_hndl;
qmi_ip_conf_t       qmi_ip_conf;
int                 embms_rmnet_link;

int                 device_mode;

pthread_cond_t      cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;

void* init_qti_socket(void);

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*==========================================================================

FUNCTION INIT_QTI_SOCKET()

DESCRIPTION

  Create thread for for receiving the link up/down events from QTI

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None

==========================================================================*/
void* init_qti_socket(void){
    int                     val, ret, len, i;
    struct                  sockaddr_un qti_socket;
    struct timeval          rcv_timeo;
    unsigned int            sockfd, max_sd;
    fd_set                  qti_fd_set;
    char                    buf[MAX_BUF_LEN];
    struct sockaddr_storage their_addr;
    socklen_t               addr_len;
    qmi_ip_event_e          event = 0;
    struct timeval *timeout = NULL;

    timeout = calloc(1, sizeof(struct timeval));
    if(timeout == NULL)
    {
        LOG_MSG_ERROR("Couldn't calloc space for timeout", 0, 0, 0);
        return;

    }
    timeout->tv_sec  = 2 ;
    timeout->tv_usec = 0;

    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        LOG_MSG_ERROR("Error creating socket, errno: %d", errno, 0, 0);
        return;
    }

    fcntl(sockfd, F_SETFD, FD_CLOEXEC);

    rcv_timeo.tv_sec = 0;
    rcv_timeo.tv_usec = 100000;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
    val = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, val);
    /* set bit in os_params */
    FD_ZERO(&qti_fd_set);
    FD_SET(sockfd, &qti_fd_set);

    qti_socket.sun_family = AF_UNIX;
    strlcpy(qti_socket.sun_path, QMI_IP_STA_FILE, sizeof(QMI_IP_STA_FILE));
    unlink(qti_socket.sun_path);
    len = strlen(qti_socket.sun_path) + sizeof(qti_socket.sun_family);

    if (bind(sockfd, (struct sockaddr *)&qti_socket, len) < 0)
    {
        LOG_MSG_ERROR("Error binding the socket, errno: %d", errno, 0, 0);
        return;
    }

    while (1) {
        ret = select(sockfd, &qti_fd_set, NULL, NULL, timeout);

        if (ret = 0 && shutdown) {
            break;
        }

        memset(buf, 0, sizeof(buf));
		event = 0;
        if ( ( i = recvfrom(sockfd, buf, 1 , 0, (struct sockaddr *)&their_addr, &addr_len)) > 0 ){
            if (*buf == LINK_DOWN_STR )
              event = QMI_IP_LINK_DOWN_EVENT;

            if (event != 0 )
                ret= qmi_ip_queue_event(event, device_mode);
            else
                LOG_MSG_ERROR("Error incorrect link event, %s", buf, 0, 0);
        }
    }
    LOG_MSG_ERROR("Shutting down socket to QTI", 0, 0, 0);
    close(sockfd);
    return;
}

/*===========================================================================
  FUNCTION nas_ind_cb
==========================================================================*/
/*!
@brief
  Handle NAS indications

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void nas_ind_cb
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
)
{
    int ret;
    unsigned char *msg_ptr;
    unsigned char *tmp_msg_ptr;

    if (-1 == qmi_ip_server_fd)
    {
      LOG_MSG_ERROR("TCP connection is not open. Ignore call back.",0,0,0);
      return;
    }

    int msg_ptr_len = ind_buf_len + QMI_HEADER_SIZE;
    msg_ptr = malloc(msg_ptr_len);

    if (msg_ptr == NULL){
        LOG_MSG_ERROR("MALLOC failure",0,0,0);
        return;
    }

    tmp_msg_ptr = msg_ptr;

    WRITE_QMI_HEADER(tmp_msg_ptr, msg_ptr_len, QMI_NAS_SERVICE_TYPE, msg_id,
                     QMI_IND_CTRL_FLAGS, QMI_IND_TX_ID, msg_ptr_len - QMI_HEADER_SIZE);
    memcpy(tmp_msg_ptr, ind_buf, ind_buf_len);

    ret = send(qmi_ip_server_fd, msg_ptr, msg_ptr_len, 0);
    free(msg_ptr);

    return;
}


/*===========================================================================
  FUNCTION netmgr_ind_cb
==========================================================================*/
/*!
@brief
  Handle NETMGR indications

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_ind_cb
(
  netmgr_nl_events_t event,
  netmgr_nl_event_info_t * info,
  void * data
)
{
  int rc;
  dsi_call_tech_type tech;

  if (NULL == info)
  {
      LOG_MSG_ERROR("NULL Info rcvd", 0,0,0);
      return;
  }

  if (((int) info->link) != embms_rmnet_link) //may need to do a mapping between netmgr_link_id_e and qmi_client_qmux_instance_type
  {
      LOG_MSG_ERROR("Event is not for eMBMS link", 0,0,0);
      return;
  }

  switch( event )
  {
  case NET_PLATFORM_UP_EV:
  case NET_PLATFORM_RECONFIGURED_EV:
    netmgr_status = UP;
    LOG_MSG_INFO1("Netmgr UP, %d", event,0,0);
    break;
  case NET_PLATFORM_DOWN_EV:
    LOG_MSG_INFO1("Netmgr DOWN", 0,0,0);
    netmgr_status = DOWN;
    break;
  }

  return;
}

int check_heartbeat(int* err, char* buf);
/*==========================================================================

FUNCTION CHECK_HEARTBEAT()

DESCRIPTION

 Check to see if there is an active connection with the GW

DEPENDENCIES
  None.

RETURN VALUE
  0: Connection is active
  -1: Connection is down


SIDE EFFECTS
  None

==========================================================================*/
int check_heartbeat(int* err, char* buf){
    int ret, rc;
    LOG_MSG_INFO1("Check QMI_IP connection to GW",0,0,0);
    ret = send(qmi_ip_server_fd, "HEARTBEAT", strlen("HEARTBEAT"), 0);

    if(ret > 0){
        /* Link is still up */
        ret = recv(qmi_ip_server_fd, buf, 4095, 0);

        if(ret > 0){
            rc = pthread_mutex_lock(&mutex);
            heartbeat_response = -1;
            rc = pthread_cond_broadcast(&cond);
            rc = pthread_mutex_unlock(&mutex);

            *err = ret;
            LOG_MSG_INFO1("QMI_IP connection to GW is still active",0,0,0);
            return 0;
        }
    }

    rc = pthread_mutex_lock(&mutex);
    heartbeat_response = 1;
    rc = pthread_cond_broadcast(&cond);
    rc = pthread_mutex_unlock(&mutex);

    acceptTCPConnection = 0;

    LOG_MSG_ERROR("QMI_IP connection to GW is down",0,0,0);

    return -1;
}

void get_odu_mode();
/*==========================================================================

FUNCTION GET_ODU_MODE()

DESCRIPTION

 Retrieves the current ETH mode.

DEPENDENCIES
  QCMAP_MSGR_HANDLE.

RETURN VALUE
  None.


SIDE EFFECTS
  None

==========================================================================*/
void get_odu_mode(){
    qmi_error_type_v01 qmi_error;
    qcmap_msgr_get_odu_mode_resp_msg_v01 get_mode_resp;
    memset(&get_mode_resp,0,
           sizeof(qcmap_msgr_get_odu_mode_resp_msg_v01));

    qmi_error = qmi_client_send_msg_sync(qmi_ip_conf.qmi_ip_qcmap_msgr_handle,
                       QMI_QCMAP_MSGR_GET_ODU_MODE_REQ_V01,
                       NULL,
                       0,
                       (void*)&get_mode_resp,
                       sizeof(qcmap_msgr_get_odu_mode_resp_msg_v01),
                       QMI_TIMEOUT);

    LOG_MSG_INFO1("qmi_client_send_msg_sync(get_odu_mode): error %d result %d",
      qmi_error, get_mode_resp.resp.result, 0);

    if ((qmi_error != QMI_NO_ERR) ||
          (get_mode_resp.resp.result != QMI_NO_ERR) ||
          (get_mode_resp.mode_valid == FALSE))
    {
      LOG_MSG_ERROR("Can not get ETH mode %d : %d. Setting default mode",
                    qmi_error, get_mode_resp.resp.error,0);
    }
    else if ( get_mode_resp.mode != QCMAP_MSGR_ODU_ROUTER_MODE_V01 ){
      LOG_MSG_ERROR("Only Router mode is supported now, ETH mode Returned %d.",
                    get_mode_resp.mode, 0, 0);
    }
    device_mode = ROUTER_MODE;
}

/*===========================================================================
  FUNCTION  MAIN
===========================================================================*/
/*!
@brief

  Main function for QMI_IP.

@return

  - Starts the multicast listener
  - Handles packets from GW

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int main(int argc, char *argv[]){
    pthread_t           multicast_thread, qti_thread;
    int                 err;
    int                 qmi_error;
    int                 ret;
    int                 sd;
    int                 rc;
    int                 j = 0;
    int                 listen_sd;
    char                ipstr[1024];
    size_t              client_len;
    char*               str;
    unsigned char       buf[4096];
    struct sockaddr_in6 sa_serv;
    struct sockaddr_in  sa_cli;
    int                 msg_buf_size;
    unsigned char       *tmp_buf;
    unsigned char       *send_buf;
    unsigned char       temp_char;
    int                 reuse = 1;
    int                 flags;
    int                 shutdown_count;
    qmi_idl_service_object_type nas_qmi_idl_service_object;
    qmi_service_info            info[10];
    uint32_t num_services = 0, num_entries = 0;
    qmi_cci_os_signal_type     qmi_nas_os_params;
    qmi_client_type            qmi_nas_notifier;
    struct timeval             rcv_timeo;
    fd_set                     fd_set;
    netmgr_client_hdl_t  netmgr_hndl;

    multicast_server_finished = 1;
    acceptTCPConnection = 0;
    heartbeat_check = 0;
    shut_down = 0;
    tcp_connection_active = 0;

    FILE *pswd_fp;
    char gw_password[MAX_PSWD_SIZE];
    char odu_password[MAX_PSWD_SIZE];

    LOG_MSG_INFO2("QMI_IP initialize", 0, 0, 0);

    qmi_ip_cmdq_init();

    /* Get QCMAP handle */
    memset(&qmi_ip_conf, 0, sizeof(qmi_ip_conf_t));
    qmi_ip_conf.state = QMI_IP_LINK_UP_WAIT;

    if(qmi_ip_qcmap_init(&qmi_ip_conf) == QMI_IP_ERROR)
    {
        LOG_MSG_ERROR("QCMAP unable to initialize, exiting",0,0,0);
        exit(2);
    }

    /* Now that we have the qcmap handle get the current ETH mode*/
    get_odu_mode();
    LOG_MSG_INFO1("device mode = %d", device_mode,0,0);

    ret = pthread_create(&qti_thread, NULL, (void (*)(void))init_qti_socket, NULL);

    /* Initialize DSI now, wait to get handle */
    if (DSI_SUCCESS != dsi_init(DSI_MODE_GENERAL))
    {
        LOG_MSG_ERROR("dsi_init failed, exit",0,0,0);
        exit(1);
    }

    if(netmgr_client_register(netmgr_ind_cb, NULL, &netmgr_hndl) == NETMGR_FAILURE){
        LOG_MSG_ERROR("netmgr_client_register failed",0,0,0);
        exit(1);
    }

    /* Create Multicast listener thread */
    ret = pthread_create(&multicast_thread, NULL, (void (*)(void))multicast_listener, NULL);

    while (!shut_down) {

        /* Only accept a TCP connection after the Multicast
           thread verfies the connection */
        rc = pthread_mutex_lock(&mutex);
        while (!acceptTCPConnection) {
            rc = pthread_cond_wait(&cond, &mutex);
        }
        rc = pthread_mutex_unlock(&mutex);

        if (-1 != qmi_ip_server_fd)
        {
          LOG_MSG_ERROR("Previous TCP Conn have not closed yet. Continue",
                        0,0,0);
          continue;
        }

        /* stop listening for multicast connections while
           setting up the TCP connection */
        LOG_MSG_INFO1("TCP connection successfullyestablished.",0,0,0);
        /* ----------------------------------------------- */
        /* Prepare TCP socket for receiving connections */
        memset(&ipstr[0], 0, sizeof(ipstr));
        listen_sd = socket (AF_INET6, SOCK_STREAM, 0);
        CHK_ERR(listen_sd, "socket");

        /* We want to be able to reuse this address */
        if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR,(char*)&reuse, sizeof(int)) == -1)
        {
            LOG_MSG_ERROR("setsockopt FAILED",0,0,0);
        }

        memset (&sa_serv, '\0', sizeof(sa_serv));
        sa_serv.sin6_family      = AF_INET6;
        sa_serv.sin6_addr        = in6addr_any;
        sa_serv.sin6_port        = htons (5006);
        err = bind(listen_sd, (struct sockaddr*) &sa_serv, sizeof (sa_serv));
        CHK_ERR(err, "bind");

        /* Receive a TCP connection. */
        err = listen (listen_sd, 1);
        CHK_ERR(err, "listen");

        inet_ntop(AF_INET6, &sa_serv.sin6_addr, ipstr, 46);
        client_len = sizeof(sa_cli);
        //this needs to be non-blocking.. with a timeout of 5 seconds
        sd = accept (listen_sd, (struct sockaddr*) &sa_cli, &client_len);

        /* We want to be able to reuse this address */
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,(char*)&reuse, sizeof(int)) == -1)
        {
            LOG_MSG_ERROR("setsockopt FAILED",0,0,0);
        }
        /* set timeout value */
        rcv_timeo.tv_sec = 0;
        rcv_timeo.tv_usec = 100000;
        setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
        flags = fcntl(sd, F_GETFL, 0);
        fcntl(sd, F_SETFL, flags);

        close (listen_sd);

        LOG_MSG_INFO1("Connection from %lx, port %x\n", sa_cli.sin_addr.s_addr, sa_cli.sin_port,0);

        /* ----------------------------------------------- */
        /* TCP connection is ready. Store FD. */
        qmi_ip_server_fd = sd;

        /* Initialize NAS */
        nas_qmi_idl_service_object = nas_get_service_object_v01();
        if (nas_qmi_idl_service_object == NULL)
        {
            LOG_MSG_ERROR("nas_qmi_idl_service_object failed!",0,0,0);
            goto retry_connection;
        }
        qmi_error = qmi_client_notifier_init(nas_qmi_idl_service_object,
                                           &qmi_nas_os_params,
                                           &qmi_nas_notifier);

        /* We must wait for the service object to be populated before getting NAS handle */
        while(1)
        {
            qmi_error = qmi_client_get_service_list(nas_qmi_idl_service_object,
                                                    NULL,
                                                    NULL,
                                                    &num_services);
            LOG_MSG_INFO1("qmi_client_get_service_list: %d", qmi_error,0,0);
            if(qmi_error == QMI_NO_ERR)
              break;
            /* wait for server to come up */
            QMI_CCI_OS_SIGNAL_WAIT(&qmi_nas_os_params, 0);
        }

        if (num_services == 0)
        {
            LOG_MSG_ERROR("Num services 0",0,0,0);
            goto retry_connection;
        }

        /* Num entries must be equal to num services */
        num_entries = num_services;
        qmi_error = qmi_client_get_service_list(nas_qmi_idl_service_object,
                                              info,
                                              &num_entries,
                                              &num_services);

        /* Now that the service object is populated get the NAS handle */
        qmi_error = qmi_client_init(&info[0],
                                  nas_qmi_idl_service_object,
                                  nas_ind_cb,
                                  NULL,
                                  NULL,
                                  &qmi_nas_handle);

        LOG_MSG_INFO1("Successfully initialized the NAS client",0,0,0);

        /* Get WDS handles */
        /* embedded instance always needed for eMBMS */
        ret = qmi_ip_wds_init(&qmi_ip_conf, &qmi_ip_server_fd,
                                  QMI_CLIENT_INSTANCE_ANY);
        if ( ret == QMI_IP_ERROR )
        {
            LOG_MSG_ERROR(" qmi_ip_wds_init failed with QMI error %d",ret,0,0);
        }
        else
        {
            //call IOCTL RMNET_IOCTL_GET_EPID and bind this to the logical rmnet link-id
            int epid_type = -1;
            int epid = -1;
            wds_bind_mux_data_port_req_msg_v01 req;
            wds_bind_mux_data_port_resp_msg_v01 resp;

            #ifdef FEATURE_DATA_TARGET_MDM9607
              ds_get_epid(DEVICE_RMNET_DATA, &epid_type, &epid);
            #else
              ds_get_epid(DEVICE_RMNET_IPA, &epid_type, &epid);
            #endif

            memset (&req, 0, sizeof(req));
            memset (&resp, 0, sizeof(resp));

            req.ep_id_valid = (epid != -1);
            req.ep_id.ep_type = (data_ep_type_enum_v01) epid_type;
            req.ep_id.iface_id = (uint32_t) epid;

            j = 0;
            do{
                //get the logical link_id. mux_id = logical link id +1
                embms_rmnet_link = (int) (QMI_CLIENT_QMUX_RMNET_INSTANCE_7 - QMI_CLIENT_QMUX_RMNET_INSTANCE_0 - j++);
                LOG_MSG_INFO1("WDS embedded logical instance %d",
                             embms_rmnet_link,0,0);

                req.mux_id = (uint8_t)(embms_rmnet_link + 1);
                req.mux_id_valid = (req.mux_id != -1);

                ret = qmi_client_send_msg_sync(qmi_ip_conf.qmi_ip_v4_wds_handle,
                                               QMI_WDS_BIND_MUX_DATA_PORT_REQ_V01,
                                               (void *)&req,
                                               sizeof(wds_bind_mux_data_port_req_msg_v01),
                                               (void*)&resp,
                                               sizeof(wds_bind_mux_data_port_resp_msg_v01),
                                               QMI_TIMEOUT);

                if((ret != QMI_NO_ERR) || (resp.resp.result != QMI_NO_ERR ))
                {
                    LOG_MSG_ERROR("Error while sending QMI_WDS_BIND_MUX_DATA_PORT_REQ message: %d %d",
                                  ret, resp.resp.result,0);
                }
                else
                    break;

            }while(j <= MAX_QMI_EMB_INSTANCES);

            if (j > MAX_QMI_EMB_INSTANCES && ((ret != QMI_NO_ERR) || (resp.resp.result != QMI_NO_ERR )))
                LOG_MSG_ERROR(" Exceeded WDS eMBMS instances %d with QMI error %d",j,ret,0);
        }

        tcp_connection_active = 1;
        while(tcp_connection_active)
        {
            memset(buf, 0, 4096);
            if (heartbeat_check && check_heartbeat(&err, buf) == -1)
                break;
            else
            {
                memset(buf, 0, sizeof(buf));
                err = recv(qmi_ip_server_fd, buf, sizeof(buf) - 1, 0);
            }

            if (err > 0) {
                tmp_buf = buf;

                unsigned long rx_iface;
                unsigned long rx_client_id;
                unsigned long rx_length;
                unsigned long rx_service_type;
                unsigned long rx_ctrl_flags;
                unsigned long rx_tx_id;
                unsigned long qmi_message_id;
                unsigned long qmi_msg_length;

                //change this to a macro
                READ_8_BIT_VAL(tmp_buf, temp_char); //should be hardcoded to 0x00
                rx_iface = (unsigned long) temp_char;
                READ_16_BIT_VAL(tmp_buf, temp_char);
                rx_length = (unsigned long) temp_char;
                READ_8_BIT_VAL(tmp_buf, temp_char);
                rx_ctrl_flags = (unsigned long) temp_char; //should be hardcoded to 0x00
                READ_8_BIT_VAL(tmp_buf, temp_char);
                rx_service_type = (unsigned long) temp_char;
                READ_8_BIT_VAL(tmp_buf, temp_char);
                rx_client_id = (unsigned long) temp_char;
                READ_8_BIT_VAL(tmp_buf, temp_char);
                rx_ctrl_flags = (unsigned long) temp_char; //00-req,01-resp,10-ind
                READ_16_BIT_VAL(tmp_buf, temp_char);
                rx_tx_id = (unsigned long) temp_char;
                READ_16_BIT_VAL(tmp_buf, temp_char);
                qmi_message_id = (unsigned long) temp_char;
                READ_16_BIT_VAL(tmp_buf, temp_char);
                qmi_msg_length = (int) temp_char;

                ret = 0;
                /* Process the message based on the service type */
                switch (rx_service_type) {
                case QMI_WDS_SERVICE_TYPE:
                    ret = proccess_wds_message(&tmp_buf, qmi_message_id, (int)qmi_msg_length, rx_tx_id);
                    break;
                case QMI_NAS_SERVICE_TYPE:
                    ret = proccess_nas_message(&tmp_buf, qmi_message_id, (int)qmi_msg_length, rx_tx_id);
                    break;
                default:
                    LOG_MSG_ERROR("INCORRECT SERVICE_TYPE: 0x%02x\n", rx_service_type,0,0);
                    break;
                }

                if (ret == -1) {
                    LOG_MSG_ERROR("Processing of message failed, type %d", rx_service_type,0,0);
                }
            }
            else {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                  continue;
                LOG_MSG_ERROR("TCP Connection Closed %d", errno,0,0);
                tcp_connection_active = 0;
            }
        }

        /* Clean up. */
retry_connection:
       shutdown_count = 0;
        LOG_MSG_INFO1("Tear down connection",0,0,0);

       close (qmi_ip_server_fd);
       LOG_MSG_INFO1("Successfully released and cleared qmi_ip_server_fd",
                     0,0,0);
       qmi_ip_server_fd = -1;
        /* Tear down eMBMS call if UP */
        if(embms_call_state == UP)
        {
          disable_embms();
        }

        /* Release the eMBMS WDS client */
        qmi_ip_wds_release(&qmi_ip_conf);

        acceptTCPConnection = 0;

        /* Release NAS client*/
        qmi_error = qmi_client_release(qmi_nas_handle);
        qmi_nas_handle = NULL;

        if(qmi_error != QMI_NO_ERR)
        {
          LOG_MSG_ERROR("Can not release client nas handle %d",qmi_error,0, 0);
        }
        else
        {
          LOG_MSG_INFO1("Successfully released the nas client handle",0,0,0);
        }
        tcp_connection_active = 0;
        LOG_MSG_INFO1("Connection closed",0,0,0);
    }

    if(netmgr_client_release(netmgr_hndl) == NETMGR_FAILURE){
        LOG_MSG_ERROR("netmgr_client_release failed",0,0,0);
    }
}
