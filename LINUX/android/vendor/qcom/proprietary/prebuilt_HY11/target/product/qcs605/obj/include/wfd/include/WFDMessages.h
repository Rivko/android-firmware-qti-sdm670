#ifndef __WFDMESSAGES_H
#define __WFDMESSAGES_H
/*==============================================================================
*       WFDMessages.h
*
*  DESCRIPTION:
*       Class declaration for WFDMessages. This provides async messaging
*   services for WFD MM Modules
*
*
*  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
/*==============================================================================
                             Edit History
================================================================================
   When            Who           Why
-----------------  ------------  -----------------------------------------------
09/23/2014         SK            InitialDraft
================================================================================
*/

/*==============================================================================
**               Includes and Public Data Declarations
**==============================================================================
*/

/* =============================================================================

                     INCLUDE FILES FOR MODULE

================================================================================
*/
#include "WFDMMThreads.h"
#include "WFDMMLogs.h"
#include <functional>


class SignalQueue;

#define MAX_CMD_DATA 30

class WFDMessage
{
public:
    WFDMessage();
    ~WFDMessage();
    void          Acquire();
    void          Release();
    bool          WaitForResponse(int *);
    bool          NotifyResponse(int);
    void          SetNoResponse();
    bool          isNoResponseSet(){return mbNoResponse;};
    void          setCallBackFunc(std::function<void()> cback)
                                  {callback = cback;}
    std::function<void()>& getCallBackFunc()
                                  {return callback;}

    int           mnCmd;
    void*         mpResp;
    int           mnRetCode;
    unsigned int  nNumArgs;
    uint64_t      &operator[](int i);
private:

    MM_HANDLE     mSignalQ;
    MM_HANDLE     mSignal;
    bool          mbRecipientActive;
    bool          isValid;
    bool          mbNoResponse;
    uint64_t      cmdData[MAX_CMD_DATA];
    std::function<void()> callback;
};


//! Callback function if client needs Message Service to provide
//! Async message delivery
typedef void (*WFDMessageCbType)(void*, WFDMessage*);

class WFDMessageService
{
public:
    WFDMessageService(void* clientData, WFDMessageCbType pFn);

    ~WFDMessageService();

    WFDMessage*      getMessageObject();

    bool             SendMessage(WFDMessage *pMsg);

    bool             recvMessage(WFDMessage**);

    bool             recvMessage(WFDMessage** pMsg, int nTimeOutMs);


    /*This function internally composes a message using the arguments
      supplied. In the message handler callback, client will receive
      a message with arguments in same order and command.
      Return value reflects the status of the command processed by the
      client in message handler thread*/
    bool              sendCmdAndWait(int nCmd, const uint64_t *pArgs,
                                    int nNumArgs, int *pRespCode);

    /*This function internally composes a message using the arguments
      supplied. In the message handler callback client will receive
      a message with arguments in same order and command.
      Return value reflects whether a message is sent. Doesnt block until
      message is processed*/
    bool              sendCmd(int nCmd, const uint64_t *pArgs,
                                    int nNumArgs);

    /*Sometime message service is used just to execute something in a
      different thread. This API facilitates user to just have a
      predefined function called by messageservice in handler thread.
      This function will always be blocking until the execution in
      handler thread is finished unless specified by the user.
      RespCode will not be populated.
    */
    bool              executeInHandlerThread(std::function<void()> func,
                                             bool wait = true);

private:
    static void      ThreadEntry(void* handle, unsigned int code);
    int              Thread(unsigned int);
    WFDMMThreads    *mpThread;
    SignalQueue     *mMsgQ;
    bool             isValid;
    bool             mbActive;
    WFDMessageCbType mpFn;
    void            *mClientData;
};
#endif /*__WFDMESSAGES_H*/
