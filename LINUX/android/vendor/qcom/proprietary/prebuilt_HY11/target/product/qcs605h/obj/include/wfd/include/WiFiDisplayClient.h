#ifndef _WFD_WIFIDISPLAYCLIENT_H_
#define _WFD_WIFIDISPLAYCLIENT_H_

/*==============================================================================
*       WiFiDisplayClient.h
*
*  DESCRIPTION:
*       Class declaration WiFiDisplayClient
*
*
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
/*==============================================================================
                             Edit History
================================================================================
   When            Who           Why
-----------------  ------------  -----------------------------------------------
11/06/2014                    InitialDraft
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

#include "IWiFiDisplayService.h"
#include <jni.h>
#include <sys/types.h>
#include "UIBCDefs.h"

struct WfdDevice;
struct WFD_uibc_event_t;
struct uibc_Fields;

typedef void (*stringarray_cb)(const char* eName,
    int numObjects, char strArray[][256],const jobject&);

typedef bool (*uibc_send_event_cb)(WFD_uibc_event_t* const,
    const void * const pClientData, const jobject&, const jobject&);

typedef bool (*uibc_hid_event_cb)(const uint8_t* const HIDPacket, uint8_t packetLen,
    const HIDDataType& dataType,const jobject&);

namespace android {
    class InputEvent;
    class KeyEvent;
    class Surface;
    class IBinder;
}

class WiFiDisplayClient
{

public:

    typedef struct screenDimensions{
        uint16_t height;
        uint16_t width;
        uint16_t Orientation;
        float aspectRatio;
        char* dump();
        screenDimensions() = default;
        private:
            static const int NUM_DIG_UINT16_t = 5;
            char dumpStr[4*NUM_DIG_UINT16_t + 30];
    }screenDimensions;

    WiFiDisplayClient( stringarray_cb,uibc_send_event_cb,
    uibc_hid_event_cb);

    WiFiDisplayClient( stringarray_cb,uibc_send_event_cb,
    uibc_hid_event_cb,JNIEnv*,const jobject&);

    int createWfdSession (WfdDevice *localDevice, WfdDevice *peerDevice);

    int stopWfdSession (int sessId, JNIEnv* env);

    int play_rtsp (int sessId, bool secureFlag);

    int pause_rtsp (int sessId, bool secureFlag);

    int standby_rtsp(int sessId);

    int teardown_rtsp(int sessId, bool isRTSP);

    int setRtpTransport(int transportType);

    int queryTCPTransportSupport();

    int setDecoderLatency(int latency);

    int tcpPlaybackControl(int cmdType, int cmdVal);

    int negotiateRtpTransport(int TransportType,int BufferLenMs,
                int portNum);

    int executeRuntimeCommand(int cmd);

    int setSurface(android::Surface* surface);

    int getConfigItems(int** configItems, size_t* length);

    uibc_Fields* getUIBCField () const{return m_pUIBCFields;}

    int setUIBC(int sessId);

    int enableUIBC(int sessId);

    int disableUIBC(int sessId);

    int startUIBC();//For clients oblivious of JNI

    int startUIBC(JNIEnv*, jfieldID, const jobject&, const jobject&);

    int stopUIBC();

    int sendUIBCEvent(android::InputEvent* ie, WFD_uibc_event_t* ev);

    int setAVPlaybackMode(int mode);

    //! @param codec    - 0 for H.264, 1 for H.265
    //! @param profile  - H.264 0-CBP 1-RHP 2-RHP2 3-BP 4-MP 5-HP / H.265 - 0 Main
    //! @param level    - 0 - level 3.1, 1- 3.2, 2 -4, 3 -4.1, 4 - 4.2, 5 - 5.0, 6 - 5.1, 7 - 5.2
    //! @param formatType - 3 - CEA, 4 - VESA, 5 - HH
    //! @param value    - 0x1, 0x2, 0x4 and so on
    //! @param resParams - {width, height, framerate}
    //! @param len - length of resParams
    //!
    //! @return int
    int sendAvFormatChange(int codec, int profile, int level, int formatType, int value, int* resParams, int len);

    int setBitrate(int value);

    int setResolution (int formatType, int value, int* resParams, int len);

    int getResolution(int32_t* width, int32_t* height);

    int getCommonResolution(uint32_t** bitmap, int32_t* numProf);

    int getNegotiatedResolution(uint32_t* bitmap, size_t &len);

    int getUIBCSupportCategory();

    void setSurfaceProp(const jint*, jsize, const jint*, jsize);

    void sendIDRRequest();

    const screenDimensions& screenDims() const {return m_sScreenDims;}

    ~WiFiDisplayClient();

    class WifiDisplayListener;

private:

    FORBID_CONSTRUCTORS(WiFiDisplayClient);

    static const android::sp<IWiFiDisplayService>& getWiFiDisplayService();

    static android::sp<IWiFiDisplayService> sWiFiDisplayService;

    class WiFiDisplayServiceDeathRecepient
        : public android::IBinder::DeathRecipient
    {
        public:
        virtual ~WiFiDisplayServiceDeathRecepient();
        virtual void binderDied(const android::wp<android::IBinder>& who);
    };

    typedef struct sessionResolution{
        int32_t height;
        int32_t width;
        float aspectRatio;
        sessionResolution() = default;
        char* dump();
        private:
            static const int NUM_DIG_INT32_t = 10;
            char dumpStr[3*NUM_DIG_INT32_t + 30];
    }sessionResolution;

    void convertToWFDFormat(android::InputEvent *ie, WFD_uibc_event_t *ev);
    void getWFDHIDEvent(android::KeyEvent *ke, WFD_uibc_event_t *ev);
    int getWFDFormatKeyCode(int androidKeyCode,int metaState);

    static android::sp<WiFiDisplayClient::WiFiDisplayServiceDeathRecepient> sDeathNotifier;
    android::sp<IWiFiDisplaySession> m_pWFDSession;
    android::sp<WifiDisplayListener> m_pListener;
    WfdDevice*                       m_pLocalDevice;
    WfdDevice*                       m_pPeerDevice;
    sessionResolution                m_sSessionResolution;
    screenDimensions                 m_sScreenDims;
    uibc_Fields*                     m_pUIBCFields;
};
#endif // _WFD_WIFIDISPLAYCLIENT_H_
