/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_radio_config_socket.cc
  @brief   qcril qmi - radio config socket implementation

  DESCRIPTION
    Handles radio config socket traffic.

******************************************************************************/


extern "C" {
#define __STDC_FORMAT_MACROS 1
#define __STDC_LIMIT_MACROS 1
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pb_decode.h>
#include "qcrili.h"
}

#define TAG "Radio_Config_Socket"

#include <framework/Log.h>

#include "radio_config_interface.pb.h"
#include "qcril_qmi_radio_config_socket.h"

#ifdef QMI_RIL_UTF
#include "unistd.h"
#include "netinet/ip.h"
extern "C" uint32 qcril_get_time_milliseconds();
#endif


#if defined(FEATURE_TARGET_GLIBC_x86) || defined(QMI_RIL_UTF)
    extern "C" size_t strlcat(char *, const char *, size_t);
    extern "C" size_t strlcpy(char *, const char *, size_t);
#endif

extern "C" {
	#include "qcril_qmi_radio_config_packing.h"
	#include "qcril_qmi_radio_config_misc.h"
}
#include "qcril_qmi_singleton_agent.h"

#define QCRIL_QMI_RADIO_CONFIG_SOCKET_PATH "/dev/socket/qmux_radio/qcril_radio_config"

#define QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_CONNECTIONS 5

class qcril_qmi_radio_config_socket_agent : public qcril_qmi_singleton_agent<qcril_qmi_radio_config_socket_agent>
{
public:
	boolean is_socket_server_started() { return (listen_sid != 0); }
	boolean is_socket_connected() { return (conn_sid >= 0); }
	boolean send_message(
         RIL_Token token,
         com_qualcomm_qti_radioconfiginterface_MessageType type,
         com_qualcomm_qti_radioconfiginterface_MessageId message_id,
         bool has_error,
         com_qualcomm_qti_radioconfiginterface_Error error,
         const void* msg, int msg_len);

	void close_socket_connection();

	qcril_qmi_radio_config_socket_agent();

private:
	boolean init_socket_listenfd();
	void recv_thread_handler();
	boolean process_incoming_message();
	int get_message_size();
	void thread_func();

private: //data members
    pthread_t thread_id;
    int listen_sid;
    int conn_sid;
    static const int MSG_OFFSET = 4;

    int recvd_byte_num;
    uint8_t recv_buffer[QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_BUF_SIZE];
    uint8_t send_buffer[QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_BUF_SIZE];
};

//===========================================================================
// qcril_qmi_radio_config_socket_agent::qcril_qmi_radio_config_socket_agent
//===========================================================================
qcril_qmi_radio_config_socket_agent::qcril_qmi_radio_config_socket_agent() :
    qcril_qmi_singleton_agent<qcril_qmi_radio_config_socket_agent>(QMI_RIL_RADIO_CONFIG_SOCKET_THREAD_NAME),
    listen_sid(0),
    conn_sid(0),
    recvd_byte_num(0)
{} // qcril_qmi_radio_config_socket_agent::qcril_qmi_radio_config_socket_agent

//===========================================================================
// qcril_qmi_radio_config_socket_agent::thread_func
//===========================================================================
void qcril_qmi_radio_config_socket_agent::thread_func()
{
	QCRIL_LOG_FUNC_ENTRY();

	if( listen_sid == 0 )
	{
		if( !init_socket_listenfd() )
		{
			recv_thread_handler();
		}
		else
		{
			QCRIL_LOG_ERROR("Failed to initialize socket!");
			listen_sid = 0;
		}
	}
	else
	{
		QCRIL_LOG_ERROR("Socket radio_config already initialized");
	}
	QCRIL_LOG_FUNC_RETURN();
}// qcril_qmi_radio_config_socket_agent::thread_func

//===========================================================================
// qcril_qmi_radio_config_socket_agent::init_socket_listenfd
//===========================================================================
//TODO: Multi client architecture!!!
boolean qcril_qmi_radio_config_socket_agent::init_socket_listenfd()
{
	QCRIL_LOG_FUNC_ENTRY();
	struct sockaddr_un server_addr;

	/* create the socket
	 * AF_UNIX - Local communication
	 * SOCK_STREAM - sequenced, reliable, 2-way, connection based byte streams
	 */
	if((listen_sid = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		QCRIL_LOG_ERROR("Error creating qcril_radio_config socket");
		return 1;
	}

	/* configuring server address structure */
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	snprintf(server_addr.sun_path, sizeof(server_addr.sun_path),
	        "%s%d", QCRIL_QMI_RADIO_CONFIG_SOCKET_PATH, qmi_ril_get_process_instance_id());
	unlink(server_addr.sun_path);

	QCRIL_LOG_INFO("binding to socket path %s", server_addr.sun_path);

	/* binding our socket to the service port */
	if( bind(listen_sid, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 )
	{
		QCRIL_LOG_ERROR("Error binding to the radio_config socket!");
		return 1;
	}

	/* Convert the socket to a listening socket */
	if( listen(listen_sid, QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_CONNECTIONS) < 0 )
	{
		QCRIL_LOG_ERROR("Error converting radio_config socket to a listening socket!");
		return 1;
	}	
	QCRIL_LOG_INFO("Listen Socket(radio_config) initialization done");

	QCRIL_LOG_FUNC_RETURN();
	return 0;
}// qcril_qmi_radio_config_socket_agent::init_socket_listenfd

//===========================================================================
// qcril_qmi_radio_config_socket_agent::recv_thread_handler
//===========================================================================
void qcril_qmi_radio_config_socket_agent::recv_thread_handler()
{
	QCRIL_LOG_FUNC_ENTRY();
	
	struct sockaddr_un client_addr;
	socklen_t client_len = sizeof(client_addr);

	int current_recvd_byte_num;
	int msg_size;

	do
	{
		if( (conn_sid = accept(listen_sid, (struct sockaddr*) &client_addr, &client_len)) < 0 )
		{
			//TODO: Error handling?

			QCRIL_LOG_ERROR("Accept failed. conn_sid: %d", conn_sid);
		}
		else
		{
			QCRIL_LOG_INFO("Client connected with conn_sid: %d", conn_sid);
			recvd_byte_num = 0;

			while(TRUE)
			{
				current_recvd_byte_num = recv(conn_sid,
					                          recv_buffer + recvd_byte_num,
					                          sizeof(recv_buffer) - recvd_byte_num,
					                          0);


				if(current_recvd_byte_num <= 0)
				{
					QCRIL_LOG_ERROR("recv returned negative: %d, for conn_sid %d", current_recvd_byte_num, conn_sid);
					QCRIL_LOG_ERROR("closing the socket");
					close(conn_sid);
					break;
				}
				else
				{
					//recieved certain bytes through socket
					recvd_byte_num += current_recvd_byte_num;

					//process chunks of messages as per valid sizes
					while( !process_incoming_message() )
					{
						//if the bytes are processed. remove them from the buffer.
						msg_size = get_message_size();
						recvd_byte_num = recvd_byte_num - msg_size - MSG_OFFSET;
						if(recvd_byte_num > 0)
						{
							memcpy(recv_buffer, recv_buffer + msg_size + MSG_OFFSET, recvd_byte_num);
						}
					} // while
				} //else
			}// while(TRUE)
		}
	} while(TRUE);

	QCRIL_LOG_FUNC_RETURN();
}// qcril_qmi_radio_config_socket_agent::recv_thread_handler

//===========================================================================
// qcril_qmi_radio_config_socket_agent::get_message_size
//===========================================================================
int qcril_qmi_radio_config_socket_agent::get_message_size()
{
	QCRIL_LOG_FUNC_ENTRY();
	int size = -1;
	if( recvd_byte_num >= MSG_OFFSET )
	{
		size = 0;
		//message size will be in 4 bytes(MSG_OFFSET) big-endian format
		for(int i = 0; i < MSG_OFFSET; i++)
		{
			size = (size << 8) + recv_buffer[i];
		}
	}
	QCRIL_LOG_FUNC_RETURN_WITH_RET(size);
	return size;
}// qcril_qmi_radio_config_socket_agent::get_message_size

//===========================================================================
// qcril_qmi_radio_config_socket_agent::process_incoming_message
//===========================================================================
boolean qcril_qmi_radio_config_socket_agent::process_incoming_message()
{
	QCRIL_LOG_FUNC_ENTRY();

	QCRIL_LOG_INFO("received %d bytes", recvd_byte_num);

	//Can we change this to add a new nanopb printing?
	qcril_qmi_print_hex(recv_buffer, recvd_byte_num);

	boolean err = FALSE;
	int size = get_message_size();

	do
	{
		//why int_max - msg offset? if so, it wouldnt even be present in "int size"
		if( (size < 0) || (size > INT_MAX - MSG_OFFSET) )
		{
			err = TRUE;
			break;
		}
		if( recvd_byte_num < size + MSG_OFFSET )
		{
			err = TRUE;
			break;
		}

		size_t unpacked_msg_size = 0;

		com_qualcomm_qti_radioconfiginterface_MessageTag *msg_tag_ptr = NULL;
		void *msg_data_ptr = NULL;

		msg_tag_ptr = qcril_qmi_radio_config_unpack_msg_tag(recv_buffer + MSG_OFFSET, size);

		qcril_evt_e_type radio_config_req_evt;

		if(msg_tag_ptr)
		{
			QCRIL_LOG_INFO("token: %d, msg id: %d, type: %d, error: %d",
				            msg_tag_ptr->token,
				            msg_tag_ptr->id,
				            msg_tag_ptr->type,
				            msg_tag_ptr->error );

			qcril_qmi_radio_config_parse_packed_msg( msg_tag_ptr->type,
				                                     msg_tag_ptr->id,
				                                     msg_tag_ptr->payload,
				                                     &msg_data_ptr,//will contain the unpacked payload
				                                     &unpacked_msg_size,
				                                     &radio_config_req_evt );


			qcril_event_queue( qmi_ril_get_process_instance_id(),
				               QCRIL_DEFAULT_MODEM_ID,
				               QCRIL_DATA_NOT_ON_STACK,
				               radio_config_req_evt,
				               msg_data_ptr,
				               unpacked_msg_size,
				               qcril_qmi_radio_config_convert_to_ril_token(msg_tag_ptr->token));

			QCRIL_LOG_INFO("Queued up the event %d! Freeing the tag ptr", radio_config_req_evt);
			qcril_free(msg_tag_ptr);
		}
		else
		{
			QCRIL_LOG_ERROR("Message tag returned from unpack is null.");
		}

	} while(FALSE);
	QCRIL_LOG_FUNC_RETURN_WITH_RET(err);
	QCRIL_LOG_ERROR("Message tag returned from unpack is null.");
	return err;
}// qcril_qmi_radio_config_socket_agent::process_incoming_message

//===========================================================================
// qcril_qmi_radio_config_socket_agent::send_message
// ??return needs to be checked?
//===========================================================================
boolean qcril_qmi_radio_config_socket_agent::send_message(RIL_Token token,
                                                          com_qualcomm_qti_radioconfiginterface_MessageType type,
                                                          com_qualcomm_qti_radioconfiginterface_MessageId message_id,
                                                          bool has_error,
                                                          com_qualcomm_qti_radioconfiginterface_Error error,
                                                          const void* msg, int msg_len)
{
    QCRIL_LOG_FUNC_ENTRY();

    boolean ret = 0;
    QCRIL_LOG_INFO("socket send message: type: %d, message_id: %d, error: %d", type, message_id, error);

    if(is_socket_connected())
    {
      size_t msg_size = qcril_qmi_radio_config_pack_msg_and_tag( msg, msg_len,
                                      qcril_qmi_radio_config_convert_ril_token_to_send(token),
                                      type, message_id, has_error, error,
                                      send_buffer + 4, sizeof(send_buffer) - 4);

      QCRIL_LOG_INFO("packed message tag size: %d", msg_size);

      unsigned char *tmp = (unsigned char*) send_buffer;
      tmp[0] = (unsigned char) (msg_size >> 24);
      tmp[1] = (unsigned char) ((msg_size >> 16) & 0xff);
      tmp[2] = (unsigned char) ((msg_size >> 8) & 0xff);
      tmp[3] = (unsigned char) (msg_size & 0xff);

      qcril_qmi_print_hex(send_buffer, msg_size + 4);
      QCRIL_LOG_INFO("Sending response...");
      send(conn_sid, send_buffer, msg_size + 4, 0);
    }
    else
    {
      QCRIL_LOG_ERROR("Socket is not connected!");
      ret = 1;
    }
    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

void qcril_qmi_radio_config_socket_init()
{
    QCRIL_LOG_FUNC_ENTRY();
    char* local_thread_name = (char*)qcril_malloc(QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_THREAD_NAME_SIZE);

    if(local_thread_name)
    {
        snprintf(local_thread_name, QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_THREAD_NAME_SIZE,
                "%s%d", QMI_RIL_RADIO_CONFIG_SOCKET_THREAD_NAME, qmi_ril_get_process_instance_id());

        QCRIL_LOG_INFO("...Created Radio Config thread on SUB %d with name: %s",
                qmi_ril_get_process_instance_id(), local_thread_name);

        qcril_qmi_radio_config_socket_agent::get_instance()->init(local_thread_name);
        qcril_free(local_thread_name);
    }
    else
    {
        QCRIL_LOG_ERROR("Failed to allocate memory!");
    }
    QCRIL_LOG_FUNC_RETURN();
}

void qcril_qmi_radio_config_socket_send
(
  RIL_Token token,
  com_qualcomm_qti_radioconfiginterface_MessageType type,
  com_qualcomm_qti_radioconfiginterface_MessageId message_id,
  bool has_error,
  com_qualcomm_qti_radioconfiginterface_Error error,
  const void* msg, int msg_len
)
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_radio_config_socket_agent::get_instance()->send_message(token, type,
                                                                    message_id,
                                                                    has_error,
                                                                    error,
                                                                    msg, msg_len);
  QCRIL_LOG_FUNC_RETURN();
}


void qcril_qmi_radio_config_socket_send_error_response
(
  RIL_Token token,
  com_qualcomm_qti_radioconfiginterface_MessageId message_id
)
{
  QCRIL_NOTUSED(token);
  QCRIL_NOTUSED(message_id);

}
