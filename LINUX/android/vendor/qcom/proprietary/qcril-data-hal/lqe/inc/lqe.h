/******************************************************************************

                                L Q E  . H

******************************************************************************/

/******************************************************************************

  @file    lqe.h
  @brief   LTE downlink quality estimation public header file

  DESCRIPTION
  Public header file for implementation of a library wrapper of
  LTE downlink quality estimation APIs
******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __LQE_H__
#define __LQE_H__

#include <stdint.h>

#define LQE_SUCCESS          0
#define LQE_FAILURE         -1
#define LQE_NOT_SUPPORTED   -2

typedef unsigned int  lqe_client_hdl_t;

typedef enum lqe_ind_events_e {
  LQE_INVALID_IND_EV,
  LQE_DOWNLINK_THROUGHPUT_IND_EV,
  LQE_UPLINK_THROUGHPUT_IND_EV,
  LQE_MAX_IND_EV
} lqe_ind_events_t;

typedef enum
{
  LQE_TURN_OFF_INDICATIONS    = 0x00,
  LQE_TURN_ON_INDICATIONS     = 0x01
}lqe_indications_switch_e;

typedef enum {
  LQE_THRPT_REPORT_ENABLED = 0,
  LQE_THRPT_REPORT_DISABLED_NO_DATA_CALL = 1,
  LQE_THRPT_REPORT_DISABLED_ALL_CALLS_DORMANT = 2,
  LQE_THRPT_REPORT_DISABLED_UNSUPPORTED_RAT = 3,
}lqe_thrpt_status_reason_enum;

typedef struct {
  uint8_t reporting_status_valid;
  lqe_thrpt_status_reason_enum reporting_status;

  uint8_t actual_interval_valid;
  uint32_t actual_interval;
}lqe_downlink_throughput_report_status;

typedef struct {
  uint8_t downlink_allowed_rate_valid;
  uint32_t downlink_allowed_rate;

  uint8_t confidence_level_valid;
  uint8_t confidence_level;

  uint8_t is_suspended_valid;
  uint8_t is_suspended;
}lqe_downlink_throughput_info_ind_t;

typedef struct {
  uint8_t reporting_status_valid;
  lqe_thrpt_status_reason_enum reporting_status;

  uint8_t actual_interval_valid;
  uint32_t actual_interval;
}lqe_uplink_throughput_report_status;

typedef struct {
  uint8_t uplink_allowed_rate_valid;
  uint32_t uplink_allowed_rate;

  uint8_t confidence_level_valid;
  uint8_t confidence_level;

  uint8_t is_suspended_valid;
  uint8_t is_suspended;
}lqe_uplink_throughput_info_ind_t;

typedef union {
  lqe_downlink_throughput_info_ind_t dl_throughput_info;

  lqe_uplink_throughput_info_ind_t   ul_throughput_info;

} lqe_ind_event_info_t;

/*---------------------------------------------------------------------------
  Type definition of callback for clients to register to receive
  asynchronous indications of throughput information from the modem
---------------------------------------------------------------------------*/
typedef void (*lqei_ind_f) (lqe_client_hdl_t       client_hndl,
                           lqe_ind_events_t        event,
                           lqe_ind_event_info_t   *info,
                           void                   *data);

/*===========================================================================
  FUNCTION  lqei_init
===========================================================================*/
/*!
@brief
  Initialize and setup the library resources. Client process should only
  call this API once.

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_init
(
  void
);

/*===========================================================================
  FUNCTION  lqei_release
===========================================================================*/
/*!
@brief
  De-Initialize and releases the library resources. Client process should
  only call this API once.

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_release
(
  void
);

/*===========================================================================
  FUNCTION  lqei_client_register
===========================================================================*/
/*!
@brief
  This function registers a client with the LQE library and returns a handle
  to client as a result. The client is supposed to use this handle with all
  the other APIs. Users can create multiple clients with a max of 5 clients.

@param cb_f: client callback to receive indications from modem.
@param data: cookie data
@param client_hndl: A valid handle to the library on successful return

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_client_register
(
  lqei_ind_f             cb_f,
  void                   *data,
  lqe_client_hdl_t       *client_handle
);

/*===========================================================================
  FUNCTION  lqei_client_release
===========================================================================*/
/*!
@brief
  This function releases a client and its resources from the LQE library.

@param client_hndl: A valid handle to the library

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_client_release
(
  lqe_client_hdl_t       client_handle
);

/*===========================================================================
  FUNCTION  lqei_set_downlink_throughput_indication_frequency
===========================================================================*/
/*!
@brief
  This function registers for downlink throughput indications from the modem.
  It sends a request to the modem to specify the interval at which the
  downlink throughput information is reported. This minimum timer
  interval can be queried using  lqei_get_min_downlink_throughput_frequency().
  The rate at which the indications are generated is an integral multiple
  of @param report_interval_timer and @param min_dl_interval returned in
  lqei_get_min_downlink_throughput_frequency().

  For Example if @param min_dl_interval returned 512 ms and user specifies
  @param report_interval_timer of 2, then the downlink throughput indications
  are generated every 2 * 512 = 1024 ms while the radio is connected.

  A @param indications value of 0 disables the timer on the modem
  and no  more indications are received. By default and in the absence of
  explicit request using this API, the indications are disabled. @param value
  of 1 will enable indications. Even if indications is 0, the current
  downlink rate and confidence level can be queried using
  lqei_get_downlink_throughput_info

  If multiple clients request for this indication using this API, then the
  minimum of the all the timers requested will be used by the modem to send
  the indications.

  A non zero frequency needs to be set to get the correct throughput information.
  An error is returned if the frequency is set to 0.

@param client_handle: handle obtained from lqe_client_register()
@param report_interval_timer: see description above
@param lqe_indications_switch_e: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - This function triggers the modem to start generating indications
    as per user request.
*/
/*=========================================================================*/
int
lqei_set_downlink_throughput_indication_frequency
(
  lqe_client_hdl_t            client_handle,
  int                         report_interval_timer,
  lqe_indications_switch_e    indications
);

/*===========================================================================
  FUNCTION  lqei_get_min_downlink_throughput_frequency
===========================================================================*/
/*!
@brief
 This function queries the modem for the minimum downlink interval timer.
 The returned value in @param min_dl_interval is in milliseconds. This
 value specifies the minimum interval in which the downlink throughput
 indications are generated by the modem. These indications are received
 via the client registered callback function.

@param client_handle: handle obtained from lqe_client_register()
@param min_dl_interval: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_get_min_downlink_throughput_frequency
(
  lqe_client_hdl_t                        client_handle,
  lqe_downlink_throughput_report_status  *status
);

/*===========================================================================
  FUNCTION  lqei_get_downlink_throughput_info
===========================================================================*/
/*!
@brief
 This function queries the modem for the current downlink rate and the
 confidence level. This is a synchronous call.
 The returned value in @param dl_rate is the current downlink rate in Kbps.
 The returned value in @param confidence_level is the current confidence
 level. It gives the degree of accuracy at which the current downlink
 rate is generated. The accuracy is on a scale of 0-7, 0 being the least
 accurate while 7 being the most accurate.

 The downlink throughput indication frequency needs to be set for this
 API to work. A non zero frequency needs to be set to get the correct
 throughput information. An error is returned if the frequency is set to 0.

@param client_handle: handle obtained from lqe_client_register()
@param dl_rate: see description above
@param confidence_level: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_get_downlink_throughput_info
(
  lqe_client_hdl_t       client_handle,
  int                    *dl_rate,
  int                    *confidence_level
);

/*===========================================================================
  FUNCTION  lqei_set_uplink_throughput_indication_frequency
===========================================================================*/
/*!
@brief
  This function registers for uplink throughput indications from the modem.
  It sends a request to the modem to specify the interval at which the
  uplink throughput information is reported. This minimum timer
  interval can be queried using  lqei_get_min_uplink_throughput_frequency().
  The rate at which the indications are generated is an integral multiple
  of @param report_interval_timer and @param min_ul_interval returned in
  lqei_get_min_uplink_throughput_frequency().

  For Example if @param min_ul_interval returned 512 ms and user specifies
  @param report_interval_timer of 2, then the uplink throughput indications
  are generated every 2 * 512 = 1024 ms while the radio is connected.

  A @param indications value of 0 disables the timer on the modem
  and no  more indications are received. By default and in the absence of
  explicit request using this API, the indications are disabled. @param value
  of 1 will enable indications. Even if indications is 0, the current
  uplink rate and confidence level can be queried using
  lqei_get_uplink_throughput_info

  If multiple clients request for this indication using this API, then the
  minimum of the all the timers requested will be used by the modem to send
  the indications.

  A non zero frequency needs to be set to get the correct throughput information.
  An error is returned if the frequency is set to 0.

@param client_hanule: hanule obtained from lqe_client_register()
@param report_interval_timer: see description above
@param lqe_indications_switch_e: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - This function triggers the modem to start generating indications
    as per user request.
*/
/*=========================================================================*/
int
lqei_set_uplink_throughput_indication_frequency
(
  lqe_client_hdl_t            client_hanule,
  int                         report_interval_timer,
  lqe_indications_switch_e    indications
);

/*===========================================================================
  FUNCTION  lqei_get_min_uplink_throughput_frequency
===========================================================================*/
/*!
@brief
 This function queries the modem for the minimum uplink interval timer.
 The returned value in @param min_ul_interval is in milliseconds. This
 value specifies the minimum interval in which the uplink throughput
 indications are generated by the modem. These indications are received
 via the client registered callback function.

@param client_hanule: hanule obtained from lqe_client_register()
@param min_ul_interval: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_get_min_uplink_throughput_frequency
(
  lqe_client_hdl_t                        client_hanule,
  lqe_uplink_throughput_report_status    *status
);

/*===========================================================================
  FUNCTION  lqei_get_uplink_throughput_info
===========================================================================*/
/*!
@brief
 This function queries the modem for the current uplink rate and the
 confidence level. This is a synchronous call.
 The returned value in @param ul_rate is the current uplink rate in Kbps.
 The returned value in @param confidence_level is the current confidence
 level. It gives the degree of accuracy at which the current uplink
 rate is generated. The accuracy is on a scale of 0-7, 0 being the least
 accurate while 7 being the most accurate.

 The uplink throughput indication frequency needs to be set for this
 API to work. A non zero frequency needs to be set to get the correct
 throughput information. An error is returned if the frequency is set to 0.

@param client_hanule: hanule obtained from lqe_client_register()
@param ul_rate: see description above
@param confidence_level: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_get_uplink_throughput_info
(
  lqe_client_hdl_t       client_hanule,
  int                    *ul_rate,
  int                    *confidence_level
);

#define LQE_MALLOC(ptr,size) \
  do \
  { \
    ptr = malloc(size); \
    if (NULL != ptr) \
    { \
      memset(ptr, 0, size); \
      LQE_LOG_DEBUG("LQE: malloc'ed [%p]",ptr); \
    } \
  } while (0)

#define LQE_FREE(ptr) \
  do \
  { \
    if (NULL != ptr) \
    { \
      LQE_LOG_DEBUG("LQE: freeing [%p]",ptr); \
      free(ptr); \
      ptr = NULL; \
    } \
  } while (0)

#endif /*__LQE_H__*/
