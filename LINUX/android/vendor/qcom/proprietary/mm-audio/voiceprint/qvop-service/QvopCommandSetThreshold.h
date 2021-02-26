/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QFP_COMMAND_SET_THRESHOLD_H__
#define __QFP_COMMAND_SET_THRESHOLD_H__

#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {

    class QvopCommandSetThreshold : public QvopCommandService {
    public:
        QvopCommandSetThreshold(const char* user_id, const char * identifier, int32_t threshold) {
            if (user_id != 0) {
                strlcpy(param.user_id, user_id, QVOP_MAX_ENROLLEE_ID_LEN);
            }
            if (identifier != 0) {
                strlcpy(param.keyphrase_id, identifier, QVOP_MAX_PHRASEID_LEN);
            }

            param.auth_level = threshold;
        }

    protected:
        virtual void execute(IQvopCallback& callback) {

            QvopStatus res = tryInitSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("QVOPQvopSessionInit failed");
                callback.onError(res);
            }

            logfile_print_v("%s: TZ loaded.", QVOP_FN);

            res = QvopConfigAuthParams(&param);
            logfile_print_v("QvopConfigAuthParams res=%d", res);

            res = tryCloseSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("%s: tryCloseSession failed", QVOP_FN);
                callback.onError(res);
            }
        }

    private:
        struct qvop_config_auth_params param;
    };
}
#endif  // __QFP_COMMAND_SET_THRESHOLD_H__
