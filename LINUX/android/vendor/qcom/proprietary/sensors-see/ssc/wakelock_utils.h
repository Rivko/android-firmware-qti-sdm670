/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <pthread.h>
#include <string>
#include "sensors_log.h"

const int FD_COUNT = 2;

class sns_wakelock {
public:
    /**
     * @brief creates wakelock instace to aquire/release wakelock
       with the name provided by string
     */
    sns_wakelock(std::string S);
    ~sns_wakelock();

    /**
     * @brief get holds the wakelock by calling write on
      *          wakelock fd
     * @note subsequent gets increaments the count as
     *           wakelock is already aquired.
     */
   void get();

   /**
    * @brief release the wakelock by calling write on
     *          release wakelock fd if count is "0"
                 if brelease is false then just decreaments the count
    * @note it will not release the wakelock if more elements
    *          in queue and releases only when all gets removed.
    */
   void put(bool brelease);

   private:
    int32_t _set_cnt;
    pthread_mutex_t _lock;
    /*each wakeup source in kernel needs a name to be associated*/
    std::string _name;
    /*FDs to hold below
        "/sys/power/wake_lock",
       "/sys/power/wake_unlock",
     */
    int _fds[FD_COUNT];
    bool _heldwakelock;
};
