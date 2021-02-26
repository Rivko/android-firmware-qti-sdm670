/*========================================================================*/
/**
@file SwitchSvc.h

@brief Switch service public header.

This file contains switch service belonging to the Elite framework.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "audio_decoder" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/switch_svc/inc/SwitchSvc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/04/10   RP      Created file.
========================================================================== */


#ifndef SWITCHSVC_H
#define SWITCHSVC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */


/** @addtogroup audio_decoder
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** Switch service initialization params */
typedef struct
{
   volatile uint32_t *delay_ptr;
   /**< shared delay variable */

   void *pCbData;
      /**<Pointer to the ASM callback struct
       * This is needed to generate mark buffer events.
       */
   uint32_t max_out_bufs;
      /** number of output bufs to create */

} switch_svc_init_params;

/**
* Creates an instance of the Elite Switch service.

  @param [in] inputParam   Pointer to switch_svc_init_params_t, expressed
                           as a uint32_t parameter.
  @param [out] handle      Service entry handle returned to the caller.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
*/
ADSPResult switch_svc_create (uint32_t init_params, void **handle);


/** \brief Opcode for  sitch get service handle message */
#define ELITEMSG_CUSTOM_SWITCH_GET_SVC_HANDLE      0x00290001

/** \brief Payload format for Matrix Config Input Ports
    message */
typedef struct elite_msg_custom_switch_get_svc_handle elite_msg_custom_switch_get_svc_handle;

/**
*   This is the payload structure for Get switch service handle
*   message. This custom message can be used to get the service
*   handle containing pointer to vacant input data port of the
*   switch service
*/
struct elite_msg_custom_switch_get_svc_handle
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t        unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t        unResponseResult; /**< Response result that the server
                                           sends back to the client. */
    uint32_t        unSecOpCode;      /**< This is the secondary opcode indicating
                                           the format for the rest of payload */

    elite_svc_handle_t *pSvcHandle;   /**< Handle to the peer with which to
                                           connect or disconnect. */
};


/** \brief Opcode for switch vacate port message */
#define ELITEMSG_CUSTOM_SWITCH_VACATE_PORT      0x00290002

/** \brief Payload format for switch vacate port message */
typedef struct elite_msg_custom_switch_vacate_port elite_msg_custom_switch_vacate_port;

/**
*   This is the payload structure for switch vacate  port
*   function. This is used to signal the switch that the decoder
*   attached to particular port is about to be disconnected.
*/
struct elite_msg_custom_switch_vacate_port
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t        unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t        unResponseResult; /**< Response result that the server
                                           sends back to the client. */
    uint32_t        unSecOpCode;      /**< This is the secondary opcode indicating
                                           the format for the rest of payload */

    elite_svc_handle_t *pSvcHandle;   /** service handle of the port to be vacated. */
};


/** \brief Opcode for Switch Flush message */
#define ELITEMSG_CUSTOM_SWITCH_FLUSH          0x00290003

/** \brief Payload format for Matrix Flush message */
typedef struct elite_msg_custom_switch_flush elite_msg_custom_switch_flush;

/**
*   This is the payload structure for switch flush cmd. Since
*   this is a port-specific message, the client should specify
*   the port ID.
*/
struct elite_msg_custom_switch_flush
{
    qurt_elite_queue_t    *pBufferReturnQ;     /**< This is the queue to which this payload
                                               buffer needs to be returned */
    qurt_elite_queue_t    *pResponseQ;         /**< This is the queue to send the ACK to.
                                               NULL indicates no response is required */
    uint32_t         unClientToken;       /**< Token that should be given in the ACK. This is different
                                               than the unResponeResult and can be used to identify
                                               who send the ACK back by the server. */
    uint32_t         unResponseResult;    /**< This is to be filled with ACK results by the client. */
    uint32_t         unSecOpCode;         /**< This is the secondary opcode indicating
                                               the format for the rest of payload */
    elite_svc_handle_t *pSvcHandle;   /** service handle of the port to be vacated. */

};
/** @} */ /* end_addtogroup audio_decoder */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef SWITCHSVC_H

