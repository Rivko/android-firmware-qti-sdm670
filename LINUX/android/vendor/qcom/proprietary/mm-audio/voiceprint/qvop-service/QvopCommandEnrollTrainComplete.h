/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_ENROLL_CAPTURE_COMPLETE_H__
#define __QVOP_COMMAND_ENROLL_CAPTURE_COMPLETE_H__

#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command for VoicePrint training session complete. Model should be processed.
     */
    class QvopCommandEnrollTrainComplete : public QvopCommandService {
    public:
        QvopCommandEnrollTrainComplete() {}

    protected:
        virtual void execute(IQvopCallback& callback) {
            struct qvop_pcm_packet pcmPkt;
            pcmPkt.eos_flag = QVOP_PCM_EOS;

            // Workaround
            // Put empty data in there since qvop_qsee is failing with the flag only.
            int16_t buffer[1] = { 0 };
            pcmPkt.num_pcm_samples = 1;
            memcpy(pcmPkt.pcm_data, buffer, 1);

            logfile_print_i("%s: Sending EOS!", QVOP_FN);
            QvopStatus res = QvopSendPCM(&pcmPkt);
            if (QVOP_PCM_DATA_REQUEST == res) {
                logfile_print_w("%s: Why are you asking for more? QVOP_PCM_DATA_REQUEST", QVOP_FN);
            }
            logfile_print_v("%s: ret=%d", QVOP_FN, res);

            // handle error
            if (res == QVOP_ENROLLMENT_COMPLETE ||
                res == QVOP_SUCCESS) {
                logfile_print_i("%s: Capture is successful.", QVOP_FN);
                callback.onEnrollStatus(QVOP_SUCCESS);
            }
            else {
                logfile_print_e("%s: Capture has failed.", QVOP_FN);
                callback.onEnrollStatus(res);
            }
        }

    private:

    };
}
#endif  // __QVOP_COMMAND_ENROLL_CAPTURE_COMPLETE_H__