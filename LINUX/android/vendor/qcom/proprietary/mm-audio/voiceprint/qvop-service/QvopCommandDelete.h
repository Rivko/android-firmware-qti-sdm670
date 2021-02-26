/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_DELETE_H__
#define __QVOP_COMMAND_DELETE_H__

#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command for VoicePrint Delete calls.
     */
    class QvopCommandDelete : public QvopCommandService {
    public:
        QvopCommandDelete(const char* user = 0, const char * identifier = 0, bool all = false) :
            deleteAll(false)
        {
            deleteAll = all;

            if (user != 0) {
                strlcpy(user_id, user, QVOP_MAX_ENROLLEE_ID_LEN);
            }

            if (identifier != 0) {
                strlcpy(keyphrase_id, identifier, QVOP_MAX_PHRASEID_LEN);
            }
        }

    protected:
        virtual void execute(IQvopCallback& callback) {
            logfile_print_v("%s: QvopCommandDelete: Running", QVOP_FN);
            QvopStatus res = tryInitSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("%s: QVOPQvopSessionInit failed", QVOP_FN);
                callback.onError(res);
            }

            if (deleteAll) {
                logfile_print_i("%s: Attempt to delete all", QVOP_FN);
                res = QvopDeleteAllTemplates();
                logfile_print_i("QvopDeleteAllTemplates res=%d", res);
                callback.onDeleteAllStatus(res);
            }
            else {
                logfile_print_i("%s: Attempt to delete %s=", QVOP_FN, keyphrase_id);
                res = QvopDeleteTemplate(user_id, keyphrase_id);
                logfile_print_i("%s: QvopDeleteTemplate res=%d", QVOP_FN, res);
                callback.onDeleteStatus(res, user_id, keyphrase_id);
            }

            res = tryCloseSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("%s: tryCloseSession failed", QVOP_FN);
            }
        }

    private:
        bool deleteAll;
        char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
        char keyphrase_id[QVOP_MAX_PHRASEID_LEN];
    };
}

#endif  // __QVOP_COMMAND_DELETE_H__
