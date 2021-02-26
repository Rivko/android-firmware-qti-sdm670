/*========================================================================
avtimer_client.c

This file contains avtimer client api implementation.

Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/src/avtimer_client.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "avtimer_server.h"


/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                          Function Definitions
** ======================================================================= */


/**
 * Verify attributes passed to avtimer_client_verify_args
 *
 * The avtimer_client_verify_args is used to verfiy all the
 * input arguments to avtimer_timer_create function. Error
 * checks related to minimum and maximum duration and type of
 * timer are done in this function
 *
 * @param[in] attr   Specifies timer duration and timer type
 * @return ADSPResult one of adsp result codes
 */
static inline ADSPResult avtimer_client_verify_args ( const avtimer_attr_t *attr_ptr )
{
    ADSPResult rc = ADSP_EOK;

    if ( NULL != attr_ptr )
    {
        /* Verify state number */
        if ( AVTIMER_ATTR_INIT != attr_ptr->state )
        {
            rc = ADSP_EBADPARAM;
        }

        /* Verify duration with min & max timer duration */
        if ( AVTIMER_MIN_DURATION > attr_ptr->duration ||
                AVTIMER_MAX_DURATION < attr_ptr->duration )
        {
            rc = ADSP_EBADPARAM;
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid duration  0x%llx\n",attr_ptr->duration);
        }

        /* Verify timer type */
        if ( AVTIMER_ONESHOT_DURATION > attr_ptr->type ||
                AVTIMER_PERIODIC < attr_ptr->type )
        {
            rc = ADSP_EBADPARAM;
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid timer type %d\n",attr_ptr->type);
        }

        /* Verify timer expiry */
        if ((AVTIMER_PERIODIC == attr_ptr->type)  && (AVTIMER_DEFAULT_EXPIRY != attr_ptr->expiry) )
        {
            rc = ADSP_EBADPARAM;
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "For periodic timers, expiry must be zero 0x%llx\n",attr_ptr->expiry);

        }

    }
    else
    {
        rc = ADSP_EBADPARAM;
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "null attribute ptr \n");
    }


    return rc;

}

ADSPResult avtimer_timer_create (avtimer_t *timer_ptr, const avtimer_attr_t *attr_ptr,
                 qurt_elite_channel_t *cb_channel_ptr, uint32_t mask)
{
    ADSPResult rc;
    avtimer_msg_t my_msg;
    avtimer_cmd_create_t my_create_cmd;
    qurt_signal2_t ack_sig;

    //verify the input arguments.
    if (ADSP_FAILED(rc = avtimer_client_verify_args(attr_ptr))) {
        return rc;
    }

    //initialize the ack signal
    qurt_signal2_init(&ack_sig);

    //form the create message
    my_msg.msg_type = AVTIMER_SERVER_CREATE_CMD;
    my_msg.create_cmd_ptr = &my_create_cmd;

    my_create_cmd.type = attr_ptr->type;
    my_create_cmd.duration = attr_ptr->duration;
    my_create_cmd.expiry = attr_ptr->expiry;
    my_create_cmd.cb.cb_channel_ptr = cb_channel_ptr;
    my_create_cmd.cb.mask = mask;
    my_create_cmd.ack_ptr = &ack_sig;
    my_create_cmd.timer = 0;
    my_create_cmd.result = ADSP_EOK;

    //push in to the queue
    if (ADSP_FAILED(rc = qurt_elite_queue_push_back((qurt_elite_queue_t *)(avtimer_server_global_inst_ptr->avtimer_server_cmdq),
                          (uint64_t *)&my_msg)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "could not push the msg in to avtimer server queue = %d!!\n", rc);
        return rc;
    }

    //wait for ack from avtimer server.
    qurt_signal2_wait (my_create_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK, QURT_SIGNAL_ATTR_WAIT_ANY);
    qurt_signal2_clear (my_create_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK);

    //destroy the signal.
    qurt_signal2_destroy(&ack_sig);

    //return the created timer object and the result
    *timer_ptr = my_create_cmd.timer;
    return my_create_cmd.result;

}



ADSPResult avtimer_timer_stop (avtimer_t timer)
{
    ADSPResult rc;
    avtimer_msg_t my_msg;
    avtimer_cmd_stop_t my_stop_cmd;

    qurt_signal2_t ack_sig;

    //initialize the ack signal
    qurt_signal2_init(&ack_sig);

    //form the stop message
    my_msg.msg_type = AVTIMER_SERVER_STOP_CMD;
    my_msg.stop_cmd_ptr = &my_stop_cmd;

    my_stop_cmd.timer = timer;
    my_stop_cmd.result = ADSP_EOK;
    my_stop_cmd.ack_ptr = &ack_sig;

    //push in to the queue
    if (ADSP_FAILED(rc = qurt_elite_queue_push_back((qurt_elite_queue_t *)(avtimer_server_global_inst_ptr->avtimer_server_cmdq),
                          (uint64_t *)&my_msg)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "could not push the msg in to avtimer server queue = %d!!\n", rc);
        return rc;
    }

    //wait for ack from avtimer server.
    qurt_signal2_wait (my_stop_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK, QURT_SIGNAL_ATTR_WAIT_ANY);
    qurt_signal2_clear (my_stop_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK);

    //destroy the signal.
    qurt_signal2_destroy(&ack_sig);
    //return the result
    return my_stop_cmd.result;

}

ADSPResult avtimer_timer_delete(avtimer_t timer){

    ADSPResult rc;
    avtimer_msg_t my_msg;
    avtimer_cmd_delete_t my_delete_cmd;
    qurt_signal2_t ack_sig;

    //initialize the ack signal
    qurt_signal2_init(&ack_sig);

    //form the delete command message
    my_msg.msg_type = AVTIMER_SERVER_DELETE_CMD;
    my_msg.delete_cmd_ptr = &my_delete_cmd;

    my_delete_cmd.timer = timer;
    my_delete_cmd.result = ADSP_EOK;
    my_delete_cmd.ack_ptr = &ack_sig;

    //push in to the queue
    if (ADSP_FAILED(rc = qurt_elite_queue_push_back((qurt_elite_queue_t *)(avtimer_server_global_inst_ptr->avtimer_server_cmdq),
                          (uint64_t *)&my_msg)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "could not push the msg in to avtimer server queue = %d!!\n", rc);
        return rc;
    }

    //wait for ack from avtimer server.
    qurt_signal2_wait (my_delete_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK, QURT_SIGNAL_ATTR_WAIT_ANY);
    qurt_signal2_clear (my_delete_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK);

    //destroy the signal.
    qurt_signal2_destroy(&ack_sig);
    //return the result
    return my_delete_cmd.result;

}

ADSPResult avtimer_timer_restart (avtimer_t timer, avtimer_duration_t duration){

    ADSPResult rc;
    avtimer_msg_t my_msg;
    avtimer_cmd_restart_t my_restart_cmd;
    qurt_signal2_t ack_sig;

    //initialize the ack signal
    qurt_signal2_init(&ack_sig);

    //form the restart message
    my_msg.msg_type = AVTIMER_SERVER_RESTART_CMD;
    my_msg.restart_cmd_ptr = &my_restart_cmd;

    my_restart_cmd.duration = duration;
    my_restart_cmd.ack_ptr = &ack_sig;
    my_restart_cmd.timer = timer;
    my_restart_cmd.result = ADSP_EOK;

    //push in to the queue
    if (ADSP_FAILED(rc = qurt_elite_queue_push_back((qurt_elite_queue_t *)(avtimer_server_global_inst_ptr->avtimer_server_cmdq),
                          (uint64_t *)&my_msg)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "could not push the msg in to avtimer server queue = %d!!\n", rc);
        return rc;
    }
    //wait for ack from avtimer server.
    qurt_signal2_wait (my_restart_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK, QURT_SIGNAL_ATTR_WAIT_ANY);
    qurt_signal2_clear (my_restart_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK);

    //destroy the signal.
    qurt_signal2_destroy(&ack_sig);

    //return the result to the caller
    return my_restart_cmd.result;

}

ADSPResult avtimer_get_attr(avtimer_t timer, avtimer_attr_t *attr_ptr)
{

    ADSPResult rc;
    avtimer_msg_t my_msg;
    avtimer_cmd_getattr_t my_getattr_cmd;
    qurt_signal2_t ack_sig;

    //initialize the ack signal
    qurt_signal2_init(&ack_sig);

    //form the get_attr message
    my_msg.msg_type = AVTIMER_SERVER_GET_ATTR_CMD;
    my_msg.getattr_cmd_ptr = &my_getattr_cmd;

    my_getattr_cmd.timer = timer;
    my_getattr_cmd.attr_ptr = attr_ptr;
    my_getattr_cmd.ack_ptr = (qurt_signal2_t *)&ack_sig;
    my_getattr_cmd.result = ADSP_EOK;

    //push in to the queue
    if (ADSP_FAILED(rc = qurt_elite_queue_push_back((qurt_elite_queue_t *)(avtimer_server_global_inst_ptr->avtimer_server_cmdq),
                          (uint64_t *)&my_msg)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "could not push the msg in to avtimer server queue = %d!!\n", rc);
        return rc;
    }

    //wait for ack from avtimer server.
    qurt_signal2_wait (my_getattr_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK, QURT_SIGNAL_ATTR_WAIT_ANY);
    qurt_signal2_clear (my_getattr_cmd.ack_ptr, AVTIMER_CLIENT_ACK_SIGMASK);

    //destroy the signal.
    qurt_signal2_destroy(&ack_sig);

    //return the result
    return my_getattr_cmd.result;

}


void avtimer_attr_init(avtimer_attr_t *attr_ptr)
{
    //intialize the attribute data struct with default parameters
    attr_ptr->state        = AVTIMER_ATTR_INIT;
    attr_ptr->type         = AVTIMER_DEFAULT_TYPE;
    attr_ptr->duration     = AVTIMER_DEFAULT_DURATION;
    attr_ptr->expiry       = AVTIMER_DEFAULT_EXPIRY;
    attr_ptr->remaining    = AVTIMER_DEFAULT_DURATION;

}


void avtimer_attr_setexpiry(avtimer_attr_t *attr_ptr, avtimer_time_t time)
{
    //expiry and duration are mutually exclusive.
    if (AVTIMER_DEFAULT_DURATION != attr_ptr->duration ) {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "set expiry failed since duration is not equal to default value \n");
        return;

    }
    attr_ptr->expiry = time;
    return;
}

void avtimer_attr_setduration(avtimer_attr_t *attr_ptr, avtimer_duration_t duration)
{
    //expiry and duration are mutually exclusive.
    if (AVTIMER_DEFAULT_EXPIRY != attr_ptr->expiry ) {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "set duration failed since expiry is not equal default value \n");
        return;

    }
    attr_ptr->duration = duration;
    attr_ptr->remaining = duration;
    return;
}

void avtimer_attr_getduration(avtimer_attr_t *attr_ptr, avtimer_duration_t *duration_ptr)
{
    assert(duration_ptr);

    *duration_ptr = attr_ptr->duration;
    return;

}

void avtimer_attr_getremaining(avtimer_attr_t *attr_ptr, avtimer_duration_t *remaining_ptr)
{
    assert(remaining_ptr);

    *remaining_ptr = attr_ptr->remaining;
    return;
}


void avtimer_attr_settype(avtimer_attr_t *attr_ptr, avtimer_type_t type)
{
    attr_ptr->type = type;
    return;

}

void avtimer_attr_gettype(avtimer_attr_t *attr_ptr, avtimer_type_t *type_ptr)
{
    assert(type_ptr);

    *type_ptr = attr_ptr->type;
    return;

}






