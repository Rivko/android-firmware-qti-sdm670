/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_MATCH_H__
#define __QVOP_COMMAND_MATCH_H__

#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command for VoicePrint Match calls.
     */

    class QvopCommandMatch : public QvopCommandService {
    public:
        QvopCommandMatch(const char* user_id, const char * identifier) {

            if (user_id != NULL) {
                logfile_print_v("user_id=%s", user_id);
                //int length = strlen(user_id);
                strlcpy(match_params.user_id, user_id, QVOP_MAX_ENROLLEE_ID_LEN);
            }
            if (identifier != NULL) {
                logfile_print_v("identifier=%s", identifier);
                //int length = strlen(identifier);
                strlcpy(match_params.keyphrase_id, identifier, QVOP_MAX_PHRASEID_LEN);
            }
        }

    protected:
        virtual void execute(IQvopCallback& callback) {
            logfile_print_v("%s: execute", QVOP_FN);

            QvopStatus res = tryInitSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("QVOPQvopSessionInit failed");
                callback.onError(res);
                return;
            }

            logfile_print_v("%s: TZ loaded", QVOP_FN);

            res = QvopMatchBegin(&match_params);

            if (res == QVOP_SUCCESS || res == QVOP_PCM_DATA_REQUEST) {
                logfile_print_i("QvopMatchBegin initiated successfully res=%d", res);
            }
            else  {
                logfile_print_e("QvopMatchBegin failed res=%d", res);
                callback.onError(res);
            }
        }

    private:
        struct qvop_match_params match_params;

    };
}
#endif  // __QVOP_COMMAND_MATCH_H__



