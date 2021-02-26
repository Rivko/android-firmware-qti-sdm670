/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QFP_COMMAND_RENAME_H__
#define __QFP_COMMAND_RENAME_H__

#include "QvopCommandService.h"
#include "logfile.h"
namespace qvop {

    /**
     * Command for VoicePrint Rename calls.
     */
    class QvopCommandRename : public QvopCommandService {
    public:
        QvopCommandRename(const char* user_id, const char * old_id, const char * new_id) {
            if (user_id != 0) {
                strlcpy(mUser, user_id, QVOP_MAX_ENROLLEE_ID_LEN);
            }
            if (old_id != 0) {
                strlcpy(mOldId, old_id, QVOP_MAX_PHRASEID_LEN);
            }

            if (new_id != 0) {
                strlcpy(mNewId, new_id, QVOP_MAX_PHRASEID_LEN);
            }
        }

    protected:
        virtual void execute(IQvopCallback& callback) {

            QvopStatus res = tryInitSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_v("QVOPQvopSessionInit failed");
                callback.onError(res);
                return;
            }

            logfile_print_v("%s: TZ loaded.", QVOP_FN);

            res = QvopRenameTemplate(mUser, mOldId, mNewId);
            logfile_print_v("QvopRenameTemplate res=%d", res);

            callback.onRenameStatus(res);

            res = tryCloseSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("%s: tryCloseSession failed", QVOP_FN);
                callback.onError(res);
            }
        }

    private:
        char mUser[QVOP_MAX_ENROLLEE_ID_LEN];
        char mOldId[QVOP_MAX_PHRASEID_LEN];
        char mNewId[QVOP_MAX_PHRASEID_LEN];
    };
}
#endif  // __QFP_COMMAND_RENAME_H__
