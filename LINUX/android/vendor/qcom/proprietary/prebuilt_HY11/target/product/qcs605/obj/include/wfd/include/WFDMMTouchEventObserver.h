/* =======================================================================
                              WFDMMTouchEventObserver.h
DESCRIPTION

Header file for WFDMMTouchEventObserver.cpp file

Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */


/* =======================================================================
                             Edit History
   When            Who           Why
-----------------  ------------  -----------------------------------------------
17/03/2015                       InitialDraft
========================================================================== */

#ifndef _WFDMMTEO_H_
#define _WFDMMTEO_H_

/*========================================================================
 *                             Include Files
 *==========================================================================*/
class WFDMMThreads;
class SignalQueue;
struct pollfd;

namespace wfdUtils
{
    typedef enum TEOCmd{
        CHANGE_POLL_TIMEOUT_VAL,
        CHANGE_SCREEN_TIMEOUT_VAL,
    } TEOCmd;

    typedef struct TEOMsg{
        TEOCmd cmd;
        long value;
    } TEOMsg;
}

class WFDMMTouchEventObserver
{
public:
    typedef enum TEOUserProfile {
        UHID = 0,
        DS
    } TEOUserProfile;

    WFDMMTouchEventObserver( TEOUserProfile prof = DS);
    ~WFDMMTouchEventObserver();
    int start();
    int stop();
    int poke(wfdUtils::TEOMsg&);
    bool isWFDHIDDeviceFound();
private:

    /*------------------------------------------------------------------
     Variables
    --------------------------------------------------------------------
    */

    WFDMMThreads* m_hThread;
    unsigned long m_nScreenTimeout;
    int  m_nPollTimeout;
    int  m_nBrightnessCache;
    bool m_bPollForData;
    bool m_bDisplayOn;
    bool m_bWFDHIDFound;
    TEOUserProfile m_eUserProfile;
    pollfd* m_pPollFds;
    int  m_nPollFds;
    int  m_nWatchFd;
    char** m_pDeviceNames;
    SignalQueue* m_pCmdQ;
    /*------------------------------------------------------------------
     Functions
    --------------------------------------------------------------------
    */

    int createResources();
    void releaseResources();
    static void eventObserverCb(void* pClientData, unsigned int signal);
    int eventObserver(int signal);
    int managePollFd(const char* devName, size_t devNameSize, bool add);
    int scanDirectory(const char *dirname);
    int checkDevice(const char * devName, int fd);
    void handleDirChange(const char *dirname);
    int inline turnDisplayOn(bool turnOn);
};
#endif //_WFDMMTEO_H_
