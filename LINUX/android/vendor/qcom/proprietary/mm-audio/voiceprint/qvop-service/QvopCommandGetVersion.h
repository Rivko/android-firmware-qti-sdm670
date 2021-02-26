/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_GETVERSION_H__
#define __QVOP_COMMAND_GETVERSION_H__

#include "QvopCommandService.h"
#include "logfile.h"

namespace qvop {
    /**
     * Command to retrieve VoicePrint algorithm version.
     */
    class QvopCommandGetVersion : public QvopCommandService {
    public:
        QvopCommandGetVersion() { }

    protected:
        virtual void execute(IQvopCallback& callback) {
            logfile_print_v("%s: entry", QVOP_FN);

            QvopStatus res = tryInitSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("%s: tryInitSession failed", QVOP_FN);
                callback.onError(res);
            }

            logfile_print_v("%s: TZ loaded", QVOP_FN);

            res = QvopGetVersion(&version_info);

            if (res == QVOP_SUCCESS) {
                logfile_print_d("QvopGetVersion success res=%d, major=%d, minor=%d",
                    res, version_info.major, version_info.minor);
                callback.onVersionInfo(static_cast<long>(version_info.major),
                    static_cast<long>(version_info.minor));
            }
            else  {
                logfile_print_e("%s: QvopGetVersion failed res=%d", QVOP_FN, res);
                callback.onError(res);
            }

            res = tryCloseSession();
            if (QVOP_SUCCESS != res) {
                logfile_print_e("%s: tryCloseSession failed", QVOP_FN);
                callback.onError(res);
            }
        }

    private:
        struct qvop_version_info version_info;
    };
}
#endif  // __QVOP_COMMAND_GETVERSION_H__