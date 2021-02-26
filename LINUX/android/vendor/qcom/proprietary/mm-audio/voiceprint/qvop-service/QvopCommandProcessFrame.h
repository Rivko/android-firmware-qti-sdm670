/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_PROCESS_FRAME_H__
#define __QVOP_COMMAND_PROCESS_FRAME_H__

#include "QvopCommandService.h"
#include "QvopCommandMatchGetResult.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command for VoicePrint audio frame processing calls.
     */

    class QvopCommandProcessFrame : public QvopCommandService {
    public:
        QvopCommandProcessFrame(int64_t timestamp,
            uint64_t frameId,
            int32_t sampleRate,
            int32_t numberOfChannels,
            int32_t audioFormat,
            size_t bufferlen,
            int8_t const * buffer) {

            logfile_print_v("%s: entry", QVOP_FN);

            pcmPkt.num_pcm_samples = (audioFormat == ENCODING_PCM_16BIT) ? bufferlen / sizeof(int16_t) : 0;
            pcmPkt.eos_flag = QVOP_PCM_NO_EOS;
            pcmPkt.pcm_format = (audioFormat == ENCODING_PCM_16BIT) ? 16 : 0;
            mCurrentIndex = frameId;
            memcpy(pcmPkt.pcm_data, buffer, bufferlen);
        }

    protected:
        virtual void execute(IQvopCallback& callback) {

            logfile_print_v("%s: entry", QVOP_FN);
            QvopStatus res = QvopSendPCM(&pcmPkt);
            logfile_print_v("%s: ret=%d", QVOP_FN, res);

            if (QVOP_PCM_DATA_REQUEST == res) {
                // More data requested. This is valid.
                logfile_print_v("ret=%d [QVOP_PCM_DATA_REQUEST]", res);
            }
            else if (QVOP_SUCCESS == res) {
                // Okay to continue
            }
            else if (QVOP_MATCH_COMPLETE == res || QVOP_MATCH_FAILURE == res) {
                logfile_print_d("%s: Found a match within this frame id=%d", QVOP_FN, mCurrentIndex);

                struct qvop_match_results match_results;
                QvopStatus matchStatus = QvopMatchGetResult(&match_results);
                if (matchStatus == QVOP_SUCCESS || matchStatus == QVOP_MATCH_SUCCESS) {
                    logfile_print_i("%s: Match Found: id=%d, byteIndex=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
                        QVOP_FN,
                        match_results.resp_id,
                        match_results.index,
                        match_results.sentence_score,
                        match_results.verification_score,
                        match_results.as_fSvmScore);
                    callback.onMatchFound(matchStatus, match_results.user_id,
                        match_results.keyphrase_id,
                        match_results.index,
                        match_results.sentence_score,
                        match_results.verification_score,
                        match_results.as_fSvmScore);
                }
                else {
                    logfile_print_i("%s: Match Not Found: id=%d, byteIndex=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
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
            }
            else {
                logfile_print_e("Error while processing frame.");
                callback.onError(res);
            }

            logfile_print_v("%s: exit", QVOP_FN);
        }

    private:
        struct qvop_pcm_packet pcmPkt;
        int32_t mCurrentIndex;
    };
}
#endif  // __QVOP_COMMAND_PROCESS_FRAME_H__
