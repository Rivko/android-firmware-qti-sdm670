/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef IQVOPCALLBACK_H
#define IQVOPCALLBACK_H
#include "logfile.h"

typedef int int32_t;

namespace qvop {
    class IQvopCallback  {
    public:
        IQvopCallback() {}
        virtual ~IQvopCallback() {};

        // On any error
        virtual void onError(int32_t error) = 0;

        // On a match or match any successful search
        virtual void onMatchFound(int32_t error,
            const char* user_id,
            const char* identifier,
            int32_t index,
            double sentence_score,
            double ver_score,
            float spoofScore) = 0;

        // On a match or match any failed
        virtual void onMatchNotFound(int32_t error,
            int32_t index,
            double sentence_score,
            double ver_score,
            float spoofScore) = 0;

        // Enrollment was successful
        virtual void onEnrollStatus(int32_t status) = 0;

        // Delete status - user/identifier, depends on request
        virtual void onDeleteStatus(int32_t status,
            const char* user_id,
            const char* identifier) = 0;

        // Delete all voiceprint templates status
        virtual void onDeleteAllStatus(int32_t status) = 0;

        // Version info
        virtual void onVersionInfo(long major, long minor) = 0;

        // Config setting success
        virtual void onConfigStatus(int32_t status) = 0;

        // Rename successful
        virtual void onRenameStatus(int32_t status) = 0;
    };
};
#endif //IQVOPCALLBACK_H
