/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_MATCH_GET_RESULT_H__
#define __QVOP_COMMAND_MATCH_GET_RESULT_H__

#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command for VoicePrint Match Get Result calls.
     */

    class QvopCommandMatchGetResult : public QvopCommandService {
    public:
        QvopCommandMatchGetResult() {}

    protected:
        virtual void execute(IQvopCallback& callback) {
            logfile_print_v("%s: execute", QVOP_FN);

            // Send EOS flag to start processing
            struct qvop_pcm_packet pcmPkt;
            pcmPkt.eos_flag = QVOP_PCM_EOS;
            logfile_print_d("%s: Sending EOS!", QVOP_FN);
            QvopStatus res = QvopSendPCM(&pcmPkt);
            if (QVOP_PCM_DATA_REQUEST == res) {
                logfile_print_w("%s: Why are you asking for more? QVOP_PCM_DATA_REQUEST", QVOP_FN);
            }
            logfile_print_v("%s: ret=%d", QVOP_FN, res);
            // Get the results from the process.
            struct qvop_match_results match_results;
            QvopStatus matchStatus = QvopMatchGetResult(&match_results);
            if (matchStatus == QVOP_SUCCESS || matchStatus == QVOP_MATCH_SUCCESS) {
                logfile_print_i("%s: Match Found: id=%d, index=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
                    QVOP_FN,
                    match_results.resp_id,
                    match_results.index,
                    match_results.sentence_score,
                    match_results.verification_score,
                    match_results.as_fSvmScore);
                callback.onMatchFound(matchStatus, match_results.user_id, match_results.keyphrase_id,
                    match_results.index, match_results.sentence_score,
                    match_results.verification_score,
                    match_results.as_fSvmScore);


            }
            else {
                logfile_print_i("%s: Match Not Found: id=%d, index=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
                    QVOP_FN,
                    match_results.resp_id,
                    match_results.index,
                    match_results.sentence_score,
                    match_results.verification_score,
                    match_results.as_fSvmScore);
                callback.onMatchNotFound(matchStatus,
                    match_results.index,
                    match_results.sentence_score,
                    match_results.verification_score,
                    match_results.as_fSvmScore);

            }
            logfile_print_v("%s: ret=%d", QVOP_FN, matchStatus);

            res = QvopSessionEnd();
            if (QVOP_SUCCESS != res) {
                logfile_print_v("%s:QvopSessionEnd failed", QVOP_FN);
            }
            else {
                logfile_print_v("%s:QvopSessionEnd success", QVOP_FN);
            }

            logfile_print_v("%s: exit ret=%d", QVOP_FN,res);
        }

    private:

    };
}
#endif  // __QVOP_COMMAND_MATCH_GET_RESULT_H__
