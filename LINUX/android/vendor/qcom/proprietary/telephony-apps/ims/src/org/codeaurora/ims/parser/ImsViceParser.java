/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */

package org.codeaurora.ims.parser;

import android.content.Context;
import android.net.Uri;
import android.os.Parcel;
import android.telecom.VideoProfile;
import android.telephony.SubscriptionManager;
import android.util.Log;
import com.android.ims.ImsExternalCallState;
import com.android.ims.ImsCallProfile;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

/*
 * ImsViceParser will maintain context of previous Vice info element and present element
 * to do further validation part before updating Vice data to UI.
 */
public class ImsViceParser extends ConfInfo {
    // Member variable
    private Context mContext;

    // Local variables
    private static ViceSaxXmlHandler sHandler;
    private String mViceDialogStr = null;
    private List<DialogInfo> mDialogIds;
    private static ArrayList<Element> mDialogList;
    private ViceSaxXmlParser mViceparser;
    private static String LOGTAG = "ImsViceParser";
    private static final String LOCAL_TARGET_PARAM_NAME = "+sip.rendering";
    private static final String LOCAL_TARGET_PARAM_PVAL_NO = "no";

    //DialogIds structure will hold all required information
    //for a particular dialog
    public static class DialogInfo {
        public String dialogId;
        public String callId;
        public String direction;
        public String exclusive;
        public String state;
        public String paramName;
        public String paramVal;
        public String remoteIdentity;
        public List<MediaAttribute> mediaAttributes = new ArrayList<>();
    }

    //MediaAttribute structure will hold media attributes
    //for a particular DialogId
    public static class MediaAttribute {
        public String mediaType;
        public String mediaDirection;
        public String mediaPort;
    }

    // Values are as per RFC 4235 received as part of DEP XML
    private static final String STATE_TERMINATED = "terminated";
    private static final String STATE_CONFIRMED = "confirmed";
    private static final String MEDIA_TYPE_AUDIO = "audio";
    private static final String MEDIA_TYPE_VIDEO = "video";
    private static final String MEDIA_DIRECTION_SENDRECV = "sendrecv";
    private static final String MEDIA_DIRECTION_SENDONLY = "sendonly";
    private static final String MEDIA_DIRECTION_RECVONLY = "recvonly";
    private static final String MEDIA_DIRECTION_INACTIVE = "inactive";

    // Not part of RFC but defined as an intermediate value between Parser
    // and interface
    private static final String CALL_TYPE_VOICE_ACTIVE = "volteactive";
    private static final String CALL_TYPE_VOICE_HELD = "volteheld";
    private static final String CALL_TYPE_VIDEO_TX_RX = "vttxrx";
    private static final String CALL_TYPE_VIDEO_TX = "vttx";
    private static final String CALL_TYPE_VIDEO_RX = "vtrx";
    private static final String CALL_TYPE_VIDEO_HELD = "vtheld";

    public ImsViceParser(Context context) {
        mContext = context;
        preparePartialList();
    }

    public static void setSAXHandler(ViceSaxXmlHandler handler) {
        sHandler = handler;
    }

    public void updateViceXmlBytes(byte[] vicexml) {
        InputStream is = new ByteArrayInputStream(vicexml);
        mDialogIds = new ArrayList<>();
        try {
            mViceDialogStr = new String(vicexml, "UTF-8");
            Log.d(LOGTAG,"VICE XML in string = " + mViceDialogStr);
            mViceparser = ViceSaxXmlParser.getInstance();
            mNewElement = mViceparser.parse(is);
            debugLog("*************New Vice Notification*****************");
            updateNotification(null, mCachedElement, mNewElement, -1);
            mDialogIds = getDialogInfo();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /*
     * If state not present to a tag , default state will be full otherwise
     * resultant state will be value of new element of corresponding new
     * notification
     */
    private String checkElementState(Element element) {
        String resultantState = Element.STATE_FULL;
        String elementState;
        elementState = element.getAttributeValue(Element.VICE_DIALOG_INFO_STATE);
        if (elementState != null) {
            resultantState = elementState;
        }
        debugLog("Notification state is" + resultantState + " element state " + elementState);
        return resultantState;
    }

    /*
     * Look up table to maintain table partial state supporting tags
     */
    private void preparePartialList() {
        allowedPartial.add(Element.VICE_DIALOG_STATE);
    }

    public List<DialogInfo> getDialogInfo() {
        return mViceparser.getDialogInfo();
    }

    public String getViceDialogInfoAsString() {
        return mViceDialogStr;
    }

    private static int getCallState(DialogInfo dialog) {
        if (dialog != null) {
            if (dialog.state.equalsIgnoreCase(STATE_CONFIRMED)) {
                return ImsExternalCallState.CALL_STATE_CONFIRMED;
            } else if (dialog.state.equalsIgnoreCase(STATE_TERMINATED)) {
                return ImsExternalCallState.CALL_STATE_TERMINATED;
            }
        } else {
            Log.d(LOGTAG, "getCallState Dialog null!");
        }
        return ImsExternalCallState.CALL_STATE_TERMINATED;
    }

    /**
     * Audio      Video         port        Type of Call    Pullable
     * ==============================================================
     * sendrcv    NA/anyvalue    NA/Present  VolteActive     Yes
     * sendonly   NA/anyvalue    NA/Present  VolteHeld       No
     * rcvonly    NA/anyvalue    NA/Present  VolteHeld       No
     * inactive   NA/anyvalue    NA/Present  VolteHeld       No
     * sendrcv    sendrcv        NA          VT-TX-RX        Yes
     * sendrcv    sendonly       NA          VT-TX           Yes
     * sendrcv    rcvonly        NA          VT-RX           Yes
     * sendrcv    inactive       NA          VTHeld          No
     * sendonly   anyvalue       NA          VTHeld          No
     * rcvonly    anyvalue       NA          VTHeld          No
     * inactive   anyvalue       NA          VTHeld          No
     * =============================================================
     */
    private static String getCallType(DialogInfo dialog) {
        String value = null;
        String audioState = null;
        String videoState = null;
        boolean portPresent = false;

        if (dialog != null && dialog.state.equalsIgnoreCase(STATE_CONFIRMED)) {
            int size = dialog.mediaAttributes.size();
            for (int i = 0; i < size; i++) {
                MediaAttribute media = dialog.mediaAttributes.get(i);
                if ((media != null) && (media.mediaType != null) &&
                        (media.mediaDirection != null)) {
                    if (media.mediaType.equalsIgnoreCase(MEDIA_TYPE_AUDIO)) {
                        if (media.mediaDirection.equalsIgnoreCase(
                                MEDIA_DIRECTION_SENDRECV)) {
                            audioState = CALL_TYPE_VOICE_ACTIVE;
                        } else if (media.mediaDirection.equalsIgnoreCase(
                                MEDIA_DIRECTION_SENDONLY) ||
                                media.mediaDirection.equalsIgnoreCase(
                                MEDIA_DIRECTION_RECVONLY) ||
                                media.mediaDirection.equalsIgnoreCase(
                                MEDIA_DIRECTION_INACTIVE)) {
                            audioState = CALL_TYPE_VOICE_HELD;
                        }
                    } else if (media.mediaType.equalsIgnoreCase(MEDIA_TYPE_VIDEO)) {
                        if (media.mediaDirection.equalsIgnoreCase(
                                MEDIA_DIRECTION_SENDRECV)) {
                            videoState = CALL_TYPE_VIDEO_TX_RX;
                        } else if (media.mediaDirection.equalsIgnoreCase(
                                MEDIA_DIRECTION_SENDONLY)) {
                            videoState = CALL_TYPE_VIDEO_TX;
                        } else if (media.mediaDirection.equalsIgnoreCase(
                                MEDIA_DIRECTION_RECVONLY)) {
                            videoState = CALL_TYPE_VIDEO_RX;
                        } else if (media.mediaDirection.equalsIgnoreCase(
                                MEDIA_DIRECTION_INACTIVE)) {
                            videoState = CALL_TYPE_VIDEO_HELD;
                        }
                        if ((media.mediaType.equalsIgnoreCase(
                                MEDIA_TYPE_VIDEO)) && (media.mediaPort != null) &&
                                (!media.mediaPort.isEmpty())) {
                            portPresent = true;
                        }
                    }
                } else {
                    Log.e(LOGTAG, "Media Parameter incorrect!!");
                }
                Log.d(LOGTAG, "audioState = " + audioState + ", videoState = " + videoState
                        + ", portPresent = " + portPresent);
            }

            if (audioState != null) {
                if (audioState.equalsIgnoreCase(CALL_TYPE_VOICE_ACTIVE)) {
                    if ((videoState == null) || ((videoState != null) && portPresent)) {
                        return CALL_TYPE_VOICE_ACTIVE;
                    } else if (videoState.equalsIgnoreCase(CALL_TYPE_VIDEO_TX_RX)) {
                        return CALL_TYPE_VIDEO_TX_RX;
                    } else if (videoState.equalsIgnoreCase(CALL_TYPE_VIDEO_TX)) {
                        return CALL_TYPE_VIDEO_TX;
                    } else if (videoState.equalsIgnoreCase(CALL_TYPE_VIDEO_RX)) {
                        return CALL_TYPE_VIDEO_RX;
                    } else if (videoState.equalsIgnoreCase(CALL_TYPE_VIDEO_HELD)) {
                        return CALL_TYPE_VIDEO_HELD;
                    }
                } else if (audioState.equalsIgnoreCase(CALL_TYPE_VOICE_HELD)) {
                    if ((videoState == null) || ((videoState != null) && portPresent)) {
                        return CALL_TYPE_VOICE_HELD;
                    } else if ((videoState != null) && !portPresent) {
                        return CALL_TYPE_VIDEO_HELD;
                    }
                }
            } else {
                Log.e(LOGTAG, "audioState null!!");
            }
        }
        return null;
    }

    private static int mapViceCallTypeToImsCallProfileCallType(String callType) {
        if (callType == null) {
            Log.e(LOGTAG, "mapViceCallToImsCallProfileCallType callType null");
            return ImsCallProfile.CALL_TYPE_VOICE;
        }

        switch (callType) {
            case CALL_TYPE_VOICE_ACTIVE:
            case CALL_TYPE_VOICE_HELD:
                return ImsCallProfile.CALL_TYPE_VOICE;

            case CALL_TYPE_VIDEO_TX_RX:
            case CALL_TYPE_VIDEO_HELD:
                return ImsCallProfile.CALL_TYPE_VT;

            case CALL_TYPE_VIDEO_TX:
                return ImsCallProfile.CALL_TYPE_VT_TX;

            case CALL_TYPE_VIDEO_RX:
                return ImsCallProfile.CALL_TYPE_VT_RX;

            default:
                Log.e(LOGTAG, "mapViceCallToImsCallProfileCallType unknown callType = " + callType);
                return ImsCallProfile.CALL_TYPE_VOICE;
        }
    }

    private static boolean isCallHeld(String callType) {
        if (callType == null) {
            Log.e(LOGTAG, "mapViceCallToImsCallProfileCallType callType null");
            return false;
        }

        switch (callType) {
            case CALL_TYPE_VOICE_HELD:
            case CALL_TYPE_VIDEO_HELD:
                return true;

            default:
                return false;
        }
    }

    /**
     * Call should be pullable when
     * In XML, Sa:Exclusive = false
     * Feature capability is present
     * When state is either active/sendonly/recvonly
     * RFC 4235 Section 5.2, call cannot be pulled if
     * If paramname present, its value is "+sip.rendering" and
     * corresponding paramval value is "no" - this implies call is on hold
     * if paramval is "yes", then call pullable
     */
    private static boolean isCallPullable(DialogInfo dialog,
            boolean isVideoSupported, boolean isVolteSupported) {
        boolean pullable = false;
        if (dialog == null) {
            return false;
        }
        String callType = getCallType(dialog);
        if (callType == null) {
            return false;
        }

        final boolean canVtBePulled = callType.equalsIgnoreCase(CALL_TYPE_VIDEO_TX_RX) ||
                callType.equalsIgnoreCase(CALL_TYPE_VIDEO_TX) ||
                callType.equalsIgnoreCase(CALL_TYPE_VIDEO_RX);

        if ((dialog.exclusive.equalsIgnoreCase("false")) &&
                ((getCallType(dialog).equalsIgnoreCase(CALL_TYPE_VOICE_ACTIVE)
                && isVolteSupported) ||
                (canVtBePulled && isVideoSupported))) {
            pullable = true;

            if (dialog.paramName != null &&
                    dialog.paramName.equalsIgnoreCase(LOCAL_TARGET_PARAM_NAME) &&
                    dialog.paramVal != null &&
                    dialog.paramVal.equalsIgnoreCase(LOCAL_TARGET_PARAM_PVAL_NO)) {
                pullable = false;
            }
        }
        return pullable;
    }

    public List<ImsExternalCallState> getCallPullInfo(
            boolean isVideoSupported, boolean isVolteSupported) {
        if (isDialogIdsEmpty()) {
            Log.d(LOGTAG, "getCallPullInfo mDialogIds null");
            return null;
        }

        ImsExternalCallState extCallState;
        List<ImsExternalCallState> extCallStateList = new ArrayList<ImsExternalCallState>();

        for (int i = 0; i < mDialogIds.size(); i++) {

            final int callId = Integer.parseInt(mDialogIds.get(i).dialogId);
            final Uri address = Uri.parse(mDialogIds.get(i).remoteIdentity);
            final boolean isPullable = isCallPullable(mDialogIds.get(i),
                    isVideoSupported, isVolteSupported);
            final int callState = getCallState(mDialogIds.get(i));
            final int callType = mapViceCallTypeToImsCallProfileCallType(
                    getCallType(mDialogIds.get(i)));
            final boolean isHeld = isCallHeld(getCallType(mDialogIds.get(i)));

            extCallState = new ImsExternalCallState(callId, address, isPullable,
                    callState, callType, isHeld);

            extCallStateList.add(extCallState);
        }
        return extCallStateList;
    }

    private boolean isDialogIdsEmpty() {
        return (mDialogIds == null || mDialogIds.isEmpty());
    }
}
