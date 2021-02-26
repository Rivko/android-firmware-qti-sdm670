/* st_hw_extn.h
 * Interface for sound trigger hal and st extn
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef SOUND_TRIGGER_HW_EXTN_H
#define SOUND_TRIGGER_HW_EXTN_H

#ifndef ST_EXTN_ENABLED
int sthw_extn_process_detection_event_keyphrase(
    st_session_t *st_ses __unused, uint64_t timestamp __unused,
    int detect_status __unused, void *payload __unused,
    size_t payload_size __unused,
    struct sound_trigger_phrase_recognition_event **event __unused) {
    return -ENOSYS;
}
bool sthw_extn_check_process_det_ev_support() {
    return false;
}
#else
int sthw_extn_process_detection_event_keyphrase(
    st_session_t *st_ses, uint64_t timestamp, int detect_status,
    void *payload, size_t payload_size,
    struct sound_trigger_phrase_recognition_event **event);
bool sthw_extn_check_process_det_ev_support();
#endif /* ST_EXTN_ENABLED */

#endif /* SOUND_TRIGGER_HW_EXTN_H */
