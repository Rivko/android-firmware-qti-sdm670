/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef FRAMEWORK_QCRIL_EVENT_H_INCLUDED
#define FRAMEWORK_QCRIL_EVENT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdbool.h>
#include <framework/IxErrno.h>
#include <framework/legacy.h>
void qcril_event_start( void );
void qcril_event_suspend();
void qcril_event_resume();

typedef enum {
    QCRIL_STATE_MODEM_OFF                       = 0x01,
    QCRIL_STATE_MODEM_UNAVAILABLE               = 0x02,
    QCRIL_STATE_MODEM_ON                        = 0x04,
    QCRIL_STATE_SIM_OR_RUIM_READY               = 0x10,
} qcril_radio_state_bits_t;

#define QMI_RIL_ALLOWED_IN_ALL_STATES ( 0xffffffff )

/* Bitmask represents the radio states
   Bit 5  - SIM or RUIM ready
   Bit 3  - Modem On
   Bit 2  - Modem Unavailable
   Bit 1  - Modem Off
*/
#define QCRIL_STATE_MASK_ALL_STATES \
    (QCRIL_STATE_MODEM_ON|QCRIL_STATE_MODEM_UNAVAILABLE|QCRIL_STATE_MODEM_OFF)

#define QCRIL_STATE_MASK_ALL_ACTIVE_STATES \
    (QCRIL_STATE_MODEM_OFF          | QCRIL_STATE_MODEM_ON)

#define QCRIL_STATE_MASK_SIM_OR_RUIM_READY_STATES \
    (QCRIL_STATE_SIM_OR_RUIM_READY)

void qcril_event_init( void );

/**
 * Register a new event
 */



IxErrnoType qcril_event_queue_with_data_free
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type modem_id,
  qcril_data_src_e_type data_src,
  qcril_evt_e_type event_id,
  void *data,
  size_t datalen,
  qcril_event_data_free_cb_t datafree,
  RIL_Token t
);

qcril_evt_e_type qcril_event_get_internal_event(int request);

IxErrnoType qcril_event_queue( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id, qcril_data_src_e_type data_src,
                        qcril_evt_e_type event_id, void *data, size_t datalen, RIL_Token t );

errno_enum_type qcril_process_event( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id,
                                     qcril_evt_e_type event_id, void *data, size_t datalen, RIL_Token t );
IxErrnoType qcril_hash_table_lookup
(
  qcril_evt_e_type id,
  qcril_event_id_t **entry_ptr_ptr /*!< OUT table entry ptr, if found */
);

const char *qcril_log_lookup_event_name
(
  qcril_evt_e_type event_id
);

IxErrnoType qcril_dispatch_event
(
  qcril_event_id_t *entry_ptr,
  qcril_request_params_type *params_ptr
);
boolean qcril_is_event_in_group(qcril_evt_e_type event_id, qcril_evt_group group_id);

IxErrnoType qcril_execute_event
(
  qcril_request_params_type *params_ptr,
  int                       *is_dedicated_thread
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
