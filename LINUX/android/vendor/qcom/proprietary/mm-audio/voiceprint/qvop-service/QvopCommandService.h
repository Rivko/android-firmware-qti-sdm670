/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QVOP_COMMAND_SERVICE_H__
#define __QVOP_COMMAND_SERVICE_H__

#include "IQvopCallback.h"
#include "logfile.h"

#define AUDIO_SAMPLE_BIT_DEPTH_16 16
#define AUDIO_SAMPLE_BIT_DEPTH_24 24
#define AUDIO_SAMPLE_BIT_DEPTH_32 32
#define AUDIO_SAMPLE_DEFAULT_BIT_DEPTH AUDIO_SAMPLE_BIT_DEPTH_16
#define AUDIO_SAMPLE_SIZE_16 2
#define AUDIO_SAMPLE_SIZE_24 3
#define AUDIO_SAMPLE_SIZE_32 4
#define AUDIO_SAMPLE_DEFAULT_SIZE AUDIO_SAMPLE_SIZE_16
#define ENCODING_PCM_16BIT 2 // AudioFormat.ENCODING_PCM_16BIT = 2 -> from java

#define AUDIO_DEFAULT_SAMPLE_RATE 16000
#define AUDIO_DEFAULT_NUMBER_CHANNELS 1
#define AUDIO_DEFAULT_FORMAT ENCODING_PCM_16BIT

extern "C"{
    #include "qvop_qsee_interface.h"
    namespace qvop {

        /**
         * Abstract base class for service commands which handles these
         * common functions:
         *
         *  - Managing the callback lifetime
         *  - Registering and handling callback death notification
         *  - Keeping track of any cancellation request
         *
         */
        class QvopCommandService {
        public:
            /**
             * Constructor
             */
            QvopCommandService();
            virtual ~QvopCommandService();

            // common fn then forwards execution to the derived class
            virtual void executeCmd(IQvopCallback& callback);

        protected:
            /**
             * Execute the target. Override in the derived class.
             *
             * @param callback Callback, 0 if none
             */
            virtual void execute(IQvopCallback& callback) = 0;

            /**
             * Check for cancel request
             *
             * @return bool True if thread cancellation has been requested.
             */
            bool isCanceled() const;


            // Try opening a TZ session
            QvopStatus tryInitSession();

            // Try closing a TZ session
            QvopStatus tryCloseSession();

        private:

            // flags
            bool mComplete;
            volatile bool mCanceled;
        };
    }
}
#endif  // __QVOP_COMMAND_SERVICE_H__

