/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/*!
  @file
  qmi_ril_file_observer.h

  @brief
    monitor the state of file and inform the changes
    to the interested clients

*/


#ifndef QMI_RIL_FILE_OBSERVER_H
#define QMI_RIL_FILE_OBSERVER_H

#ifdef  __cplusplus
extern "C" {
#endif

#define QMI_RIL_FILE_OBSERVER_FILE_NAME_SIZE    (150)

typedef enum
{
    QMI_RIL_FILE_OBSERVER_CHANGE_CREATED = 0,
    QMI_RIL_FILE_OBSERVER_CHANGE_OPENED,
    QMI_RIL_FILE_OBSERVER_CHANGE_MODIFIED,
    QMI_RIL_FILE_OBSERVER_CHANGE_CLOSE_WRITE,
    QMI_RIL_FILE_OBSERVER_CHANGE_CLOSE_NO_WRITE,
    QMI_RIL_FILE_OBSERVER_CHANGE_DELETED,
    QMI_RIL_FILE_OBSERVER_CHANGE_INVALID,
    QMI_RIL_FILE_OBSERVER_CHANGE_MAX
}qmi_ril_file_observer_change_event;

typedef void (*qmi_ril_file_observer_state_change_handler)
(
    const qmi_ril_file_observer_change_event event
);

int qmi_ril_file_observer_init();
int qmi_ril_file_observer_register_for_change_event
(
    char *file_name,
    qmi_ril_file_observer_state_change_handler hdlr
);

int qmi_ril_file_observer_deregister_for_change_event
(
    char *file_name,
    qmi_ril_file_observer_state_change_handler hdlr
);

#ifdef  __cplusplus
}
#endif

#endif /* QMI_RIL_FILE_OBSERVER_H */
