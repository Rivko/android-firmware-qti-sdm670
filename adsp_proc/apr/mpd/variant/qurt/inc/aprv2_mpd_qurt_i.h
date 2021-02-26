#ifndef __APRV2_MPD_QURT_API_H__
#define __APRV2_MPD_QURT_API_H__

/*
  Copyright (C) 2012,2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/mpd/variant/qurt/inc/aprv2_mpd_qurt_i.h#2 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "aprv2_packet.h"
#include "qurt_signal.h"
#include "qurt_qdi_driver.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/**
 * Name of the kernel APR driver. Client must use this name when opening an 
 * instance to the kernel APR driver.
 */
#define APRV2_MPD_DEVICE_NAME ( "qc_apr" )

/**
 * Driver defined methods
 */
#define APRV2_MPD_CMDID_GET_RX_SIGNAL     (0+QDI_PRIVATE)
#define APRV2_MPD_CMDID_REGISTER          (1+QDI_PRIVATE)
#define APRV2_MPD_CMDID_DEREGISTER        (2+QDI_PRIVATE)
#define APRV2_MPD_CMDID_ASYNC_SEND        (3+QDI_PRIVATE)
#define APRV2_MPD_CMDID_PEEK_RX_PACKET    (4+QDI_PRIVATE)
#define APRV2_MPD_CMDID_READ_RX_PACKET    (5+QDI_PRIVATE)
#define APRV2_MPD_CMDID_SET_UPD_STATE     (6+QDI_PRIVATE)


/**
 * APR MPD Signals
 */
#define APRV2_MPD_SIG_RX_PACKET           0x00000001

/*****************************************************************************
 * Service Registration                                                      *
 ****************************************************************************/

/**
 * Registers a service with the kernel APR driver.
 *
 * \return APR_EOK when successful.
 */
static inline int32_t __aprv2_mpd_cmd_register(int handle, uint16_t addr)
{

   if (0 == addr)
      return APR_EBADPARAM;

   return qurt_qdi_handle_invoke(handle, APRV2_MPD_CMDID_REGISTER, addr);
}

/**
 * DeRegisters a service with the kernel APR driver.
 *
 * \return APR_EOK when successful.
 */
static inline int32_t __aprv2_mpd_cmd_deregister(int handle, uint16_t addr)
{

   if (0 == addr)
      return APR_EBADPARAM;

   return qurt_qdi_handle_invoke(handle, APRV2_MPD_CMDID_DEREGISTER, addr);
}


/*****************************************************************************
 * Rx Packet Retrieval                                                       *
 ****************************************************************************/

/**
 * Retrieves the header of the Rx packet. This allows the client to allocate
 * the correct buffer size for reading the next Rx packet by calling 
 * __aprv2_mpd_cmd_read_rx_packet().
 *
 * \return APR_EOK When successful.
 * \return APR_ENOTREADY When there are no rx packets available. The client
 *                       should not retry until an Rx signal is received.
 */
static inline int32_t __aprv2_mpd_cmd_peek_rx_packet(
   int handle,
   aprv2_packet_t *ret_packet_header
)
{

   if (NULL == ret_packet_header)
      return APR_EBADPARAM;

   return qurt_qdi_handle_invoke(handle,
                            APRV2_MPD_CMDID_PEEK_RX_PACKET, ret_packet_header);
}

static inline int32_t __aprv2_mpd_cmd_set_upd_state(int handle, int addr)
{

   return qurt_qdi_handle_invoke(handle, APRV2_MPD_CMDID_SET_UPD_STATE, addr);
}
/**
 * Reads the next Rx packet into the client provided buffer.
 *
 * \return APR_EOK when successful.
 * \return APR_ENOTREADY When there are no rx packets available. 
 * \notes The client should call this API when an Rx signal is received.
 */
static inline int32_t __aprv2_mpd_cmd_read_rx_packet(
   int handle,
   uint8_t *ret_packet,            /* aprv2_packet_t * */
   uint32_t size,
   uint32_t *ret_bytes_read
)
{

   if (NULL == ret_packet || 0 == size || NULL == ret_bytes_read)
      return APR_EBADPARAM;

   return qurt_qdi_handle_invoke(handle,
            APRV2_MPD_CMDID_READ_RX_PACKET, ret_packet, size, ret_bytes_read);
}

/*****************************************************************************
 * Async Send message                                                        *
 ****************************************************************************/

/**
 * Send packet to KDRV from UDRV. KDRV receives the packet and 
 * routes it based on its routing table. Payload for this 
 * command is of type aprv2_packet_t and contains the APR packet 
 * to send to Kernel.  
 *
 * \return APR_EOK when successful.
 * \return APR_EBADPARAM if the input parameter is invalid.
 */
static inline int32_t __aprv2_mpd_cmd_async_send(
   int handle,
   uint8_t *packet,         /* aprv2_packet_t * */
   uint32_t size
)
{

   if (NULL == packet || size < sizeof(aprv2_packet_t))
      return APR_EBADPARAM;

   return qurt_qdi_handle_invoke(handle,
                   APRV2_MPD_CMDID_ASYNC_SEND, packet, size);
}


/*****************************************************************************
 * Rx Signal Setup                                                           *
 ****************************************************************************/

/**
 * Retrieve the rx signal from the kernel APR driver. Client can then wait on
 * the signal by calling qurt_signal_wait(rx_signal). The signal will be
 * raised by the kernel APR driver when one or more rx packets are available. 
 * The signal is not queued. Upon receiving this signal, the client must retrieve 
 * all the available packets in a loop. When the client is no longer interested 
 * in the rx signal, it must call qdi_close( client_handle ) to release the signal 
 * handle.
 *
 * \return APR_EOK when successful.
 * \return APR_EBADPARAM if the input parameter is invalid.
 */
static inline int32_t __aprv2_mpd_cmd_get_rx_signal(
   int handle,
   qurt_signal_t **ret_rx_signal
)
{

   if (NULL == ret_rx_signal)
      return APR_EBADPARAM;

   return qurt_qdi_handle_invoke(handle,
                   APRV2_MPD_CMDID_GET_RX_SIGNAL, ret_rx_signal);
}

#endif /* __APRV2_MPD_QURT_API_H__ */
