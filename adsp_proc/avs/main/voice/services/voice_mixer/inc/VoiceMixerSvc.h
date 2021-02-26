/*========================================================================

*//** @file VoiceMixerSvc.h

Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file provides interfaces to create and control Matrix mixer for voice graph. An instance of this service is created statically in tx and rx paths.
The matrix mixer allows routing of arbitrary inputs to arbitrary outputs.


*//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_mixer/inc/VoiceMixerSvc.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  05/01/10   ksa     Created file

  ========================================================================== */

#ifndef VMX_H
#define VMX_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** Enumeration to create new Matrix instance */
#define VMX_RX_INSTANCE_ID 1
#define VMX_TX_INSTANCE_ID 2
/** Enumeration to indicate input port in stop/run commands */
#define VMX_INPUT_DIRECTION  0
/** Enumeration to indicate output port in stop/run commands */
#define VMX_OUTPUT_DIRECTION 1

/** Enumeration to indicate that an input port is of low priority.

    In case of multiple input ports feeding a single output port,
    low priority inputs:
    1. Will not stop the output port from sending data, in case data is
    available from another port which is high priority
    2. Will be able to accumulate data to the output port, but no output
    data will be sent out unless data is also available from another port
    which is not low priority
 */
#define VMX_INPUT_LOW_PRIO   0
/** Enumeration to indicate that an input port is of medium priority.

    In case of multiple input ports feeding a single output port,
    medium priority inputs:
    1. Will not stop the output port from sending data, in case data is
    available from another port which is high priority
    2. Will be able to have an output deliver data even if it comes only from
    medium priority port(s) provided no high priority ports are connected
 */
#define VMX_INPUT_MED_PRIO   1
/** Enumeration to indicate that an input port is of high priority.

    In case of multiple input ports feeding a single output port,
    high priority inputs:
    1. Will stop an output port from delivering data, unless data from all high
    priority inputs connected to that output is available.
    2. Will be able to have an output deliver data at all times, regardless of
    availability of data from any input ports except high priority
 */
#define VMX_INPUT_HIGH_PRIO  2

/** Allows creation, destruction and reconfiguration of input ports. */
#define ELITEMSG_CUSTOM_VMX_CFG_INPUT_PORTS  0x00270001
/** Allows creation, destruction and reconfiguration of output ports. */
#define ELITEMSG_CUSTOM_VMX_CFG_OUTPUT_PORTS 0x00270002
/** Allows input and output ports to be moved to run state.

    -In the case of output ports, this command indicates that output port will
    now accumulate input data and send output buffers.
    -In case of input ports, this command indicates that the input port is
    ready. A media type message must still be sent to the input port before
    it will accept any data buffers.
*/
#define ELITEMSG_CUSTOM_VMX_RUN               0x00270003
/** Allows input and output ports to be moved to stop state.

    -In the case of output ports, this command indicates that output port will
    no longer accumulate input data and send output buffers.
    -In case of input ports, this command indicates that the input port is
    will no longer accept any incoming data path messages and they will all be
    dropped.
*/
#define ELITEMSG_CUSTOM_VMX_STOP                0x00270004

typedef struct vmx_inport_handle_t vmx_inport_handle_t;

/** @brief Structure for handle to an input port.

    Required for sending data buffers to the input port
*/
struct vmx_inport_handle_t
{
   uint32_t            inport_id;
   /**< Unique ID of the input port */
   elite_svc_handle_t  port_handle;
   /**< Elite service handle to input port, for getting access to the input data queue */
};

/** @brief Structure for handle to an output port.
*/
typedef struct vmx_outport_handle_t vmx_outport_handle_t;
/** @brief Structure for handle to an output port.
*/
struct vmx_outport_handle_t
{
   uint32_t outport_id; /**< Unique ID of the output port */
};

/** @brief Structure that is used to confgure the matrix mixer
when it is being created.
*/
typedef struct VMtMxParams_t VMtMxParams_t;
/** @brief Structure that is used to confgure the matrix mixer
when it is being created.
*/
struct VMtMxParams_t
{
   uint32_t outPortsFrameSize;
   /**< Common output frame size across all output ports */
   uint32_t maxInPorts;
   /**< Maximum number of input ports required */
   uint32_t maxOutPorts;
   /**< Maximum number of output ports required */
};

/** Enumeration for creation of a new input port */
#define VMX_NEW_INPUT            1
/** Enumeration for closing an open input port */
#define VMX_CLOSE_INPUT          2
/** Enumeration for changing the sampling rate of an input port */
#define VMX_RECONFIGURE_INPUT    3
/** Enumeration for reassigning input port to different output port(s) */
#define VMX_REASSIGN_INPUT       4

/** @brief Structure of parameters used to configure input ports.
*/
typedef struct vmx_inport_cfg_params_t vmx_inport_cfg_params_t;
/** @brief Structure of parameters used to configure input ports.
*/
struct vmx_inport_cfg_params_t
{
   int32_t               index;
   /**< Unique ID of port being addressed, -1 if creating a new input */
   uint32_t              configuration;
   /**< Enumeration to indicate the action that needs to be taken */
   uint32_t              input_mapping_mask;
   /**< Mask indicating output ports that this input should feed to */
   vmx_inport_handle_t** ack_ptr;
   /**< Return pointer when creating a new input port */
   uint32_t              priority;
   /**< Enumeration to indicate priority of the input port */
};

/** Enumeration for creation of a new output port */
#define VMX_NEW_OUTPUT           1
/** Enumeration for closing an open output port */
#define VMX_CLOSE_OUTPUT         2
/** Enumeration for changing the sampling rate of an output port */
#define VMX_RECONFIGURE_OUTPUT   3
/** Enumeration for reassigning output port to different input port(s) */
#define VMX_REASSIGN_OUTPUT      4

/** @brief Structure of parameters used to configure output ports.
*/
typedef struct vmx_outport_cfg_params_t vmx_outport_cfg_params_t;
/** @brief Structure of parameters used to configure output ports.
*/
struct vmx_outport_cfg_params_t
{
   int32_t                    index;
   /**< Unique ID of port being addressed, -1 if creating a new output */
   uint32_t                 configuration;
   /**< Enumeration to indicate the action that needs to be taken */
   uint32_t                 num_output_bufs;
   /**< Number of output buffers the port is required to have */
   uint32_t                 max_bufq_capacity;
   /**< Capacity of the output port buffer queue */
   elite_svc_handle_t*     svc_handle_ptr;
   /**< Handle to downstream peer that this output port is connecting to */
   uint32_t                 output_mapping_mask;
   /**< Mask indicating input ports that should feed to this output */
   uint32_t                 out_sample_rate;
   /**< Sampling rate of the output port  */
   vmx_outport_handle_t** ack_ptr;
   /**< Return pointer when creating a new output port */
};


/** @brief Structure of parameters used to configure input ports.
*/
typedef struct elite_msg_custom_vmx_cfg_inports_t elite_msg_custom_vmx_cfg_inports_t;
/** @brief Structure of parameters used to configure input ports.
*/
struct elite_msg_custom_vmx_cfg_inports_t
{
   qurt_elite_queue_t           *pBufferReturnQ;
   /**< Queue to which this payload buffer needs to be returned*/
   qurt_elite_queue_t           *pResponseQ;
   /**< Queue to send the ACK to, NULL indicates no response is required */
   uint32_t                    unClientToken;
   /**< Unique token to match a response with a request */
   uint32_t                    unResponseResult;
   /**< Result of the command */
   uint32_t                    unSecOpCode;
   /**< Secondary opcode indicating the format for the rest of payload */
   vmx_inport_cfg_params_t    cfgInPortsPayload;
   /**< Configuration parameters for input port */
};


/** @brief Structure of payload used to stop or run input and output ports.
*/
typedef struct elite_msg_custom_vmx_stop_run_t elite_msg_custom_vmx_run_t;
/** @brief Structure of payload used to stop or run input and output ports.
*/
typedef struct elite_msg_custom_vmx_stop_run_t elite_msg_custom_stop_t;
/** @brief Structure of payload used to stop or run input and output ports.
*/
struct elite_msg_custom_vmx_stop_run_t
{
   qurt_elite_queue_t *pBufferReturnQ;
   /**< Queue to which this payload buffer needs to be returned*/
   qurt_elite_queue_t *pResponseQ;
   /**< Queue to send the ACK to, NULL indicates no response is required */
   uint32_t      unClientToken;
   /**< Unique token to match a response with a request */
   uint32_t      unResponseResult;
   /**< Result of the command */
   uint32_t      unSecOpCode;
   /**< Secondary opcode indicating the format for the rest of payload */
   uint32_t      unPortID;
   /**< Unique ID of input or output port */
   uint32_t      unDirection;
   /**< Enumeration specifying whether the port is an input port or an output port */
};

/** @brief Structure of parameters used to configure output ports.
*/
typedef struct elite_msg_custom_vmx_cfg_outports_t elite_msg_custom_vmx_cfg_outports_t;

/** @brief Structure of parameters used to configure output ports.
*/
struct elite_msg_custom_vmx_cfg_outports_t
{
   qurt_elite_queue_t            *pBufferReturnQ;
   /**< Queue to which this payload buffer needs to be returned*/
   qurt_elite_queue_t            *pResponseQ;
   /**< Queue to send the ACK to, NULL indicates no response is required */
   uint32_t                 unClientToken;
   /**< Unique token to match a response with a request */
   uint32_t                 unResponseResult;
   /**< Result of the command */
   uint32_t                 unSecOpCode;
   /**< Secondary opcode indicating the format for the rest of payload */
   vmx_outport_cfg_params_t cfgOutPortsPayload;
   /**< Configuration parameters for output port */
};

/** Function to create an instance of this service
    @param[in] inputParam - Dummy input parameter
    @param[out] handle - Return handle of created instance
    @param[in] pVmtMxParams - Parameters required for creation of the instance
*/
ADSPResult CreateVoiceMatrixMixerSvc (uint32_t inputParam, void **handle, VMtMxParams_t* pVMtMxParams,uint32_t instance_id);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif //VMX_H
