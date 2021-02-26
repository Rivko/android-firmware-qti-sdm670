/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */

package org.codeaurora.ims.parser;

import android.util.Log;
import java.util.ArrayList;
import java.util.List;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;


public class ViceSaxXmlHandler extends DefaultHandler {
    private StringBuilder builder;
    private Element mViceDialogInfoElement;
    private Element mViceDialogElement;
    private Element mViceDialogLocalElement;
    private Element mViceDialogLocalIdentityElement;
    private Element mViceDialogLocalTargetElement;
    private Element mViceDialogMediaTypeElement;
    private Element mViceDialogRemoteElement;
    private Element mViceDialogMediaAttriElement;
    private Element mViceDialogMediaDirElement;
    private Element mViceDialogRemoteIdentityElement;
    private Element mViceDialogMediaPortElement;
    private Element mViceDialogReferredElement;
    private Element mViceDialogRouteElement;
    private Element mViceDialogHopElement;
    private List<Element> mViceDialogInfoSubList;
    private List<Element> mViceDialogSubList;
    private List<Element> mViceDialogLocalSubList;
    private List<Element> mViceDialogRemoteSubList;
    private List<Element> mViceDialogLocalTargetSubList;
    private List<Element> mViceDialogMediaAttriSubList;
    private List<Element> mViceDialogRouteSubList;

    private boolean mDoesDialogHaveSubElement = false;
    private boolean mDoesDialogLocalHaveSubElement = false;
    private boolean mDoesRouteSetHaveSubElement = false;

    private List<ImsViceParser.DialogInfo> mDialogInfoList = new ArrayList<>();
    private ImsViceParser.DialogInfo mDialogInfo;
    private ImsViceParser.MediaAttribute mMediaAttribute;

    private final String LOGTAG = "ViceSaxxLHandler";

    public ViceSaxXmlHandler() {
        Log.d(LOGTAG, "ViceSaxxHandler: constructor");
        mViceDialogInfoElement = new Element();
    }

    public Element getConferenceInfoLatestMessage() {
        return mViceDialogInfoElement;
    }

    @Override
    public void startDocument() throws SAXException {
        super.startDocument();
        builder = new StringBuilder();
    }

    // Event Handlers
    @Override
    public void startElement(String uri, String localName, String qName,
                             Attributes attributes) throws SAXException {
        super.startElement(uri, localName, qName, attributes);
        Log.d(LOGTAG, "uri = " + uri + ", localname = " + localName + ", qName = " + qName +
                ", attri = " + attributes);
        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_INFO)) {
            mViceDialogInfoElement = new Element();
            mViceDialogInfoElement.setTagName(Element.VICE_DIALOG_INFO);
            mViceDialogInfoElement.setMapAttribute();
            mViceDialogInfoElement.setParentTag(null);
            mViceDialogInfoElement.setAttributValue(Element.VICE_VERSION,
                    attributes.getValue(Element.VICE_VERSION));
            mViceDialogInfoElement.setAttributValue(Element.VICE_DIALOG_INFO_STATE,
                    attributes.getValue(Element.VICE_DIALOG_INFO_STATE));
            mViceDialogInfoElement.setAttributValue(Element.VICE_ENTITY,
                    attributes.getValue(Element.VICE_ENTITY));
            mViceDialogInfoSubList = new ArrayList<>();
            mViceDialogInfoElement.setSubElementList(mViceDialogInfoSubList);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogInfoElement.getTagName());
            Log.d(LOGTAG, "attribute vals = "
                    + mViceDialogInfoElement.getAttributeValue(Element.VICE_VERSION) + ", "
                    + mViceDialogInfoElement.getAttributeValue(Element.VICE_DIALOG_INFO_STATE) +
                    ", " + mViceDialogInfoElement.getAttributeValue(Element.VICE_ENTITY));
        }
        if (qName.equalsIgnoreCase(Element.VICE_DIALOG)) {
            mDialogInfo= new ImsViceParser.DialogInfo();
            mViceDialogElement = new Element();
            mViceDialogElement.setTagName(Element.VICE_DIALOG);
            mViceDialogElement.setParentTag(Element.VICE_DIALOG_INFO);
            mViceDialogElement.setMapAttribute();
            mViceDialogInfoSubList.add(mViceDialogElement);
            mViceDialogSubList = new ArrayList<>();
            mViceDialogElement.setSubElementList(mViceDialogSubList);
            mViceDialogElement.setAttributValue(Element.VICE_DIALOG_ID,
                    attributes.getValue(Element.VICE_DIALOG_ID));
            mViceDialogElement.setAttributValue(Element.VICE_DIALOG_CALLID,
                    attributes.getValue(Element.VICE_DIALOG_CALLID));
            mViceDialogElement.setAttributValue(Element.VICE_DIALOG_LOCAL_TAG,
                    attributes.getValue(Element.VICE_DIALOG_LOCAL_TAG));
            mViceDialogElement.setAttributValue(Element.VICE_DIALOG_REMOTE_TAG,
                    attributes.getValue(Element.VICE_DIALOG_REMOTE_TAG));
            mViceDialogElement.setAttributValue(Element.VICE_DIALOG_DIRECTION,
                    attributes.getValue(Element.VICE_DIALOG_DIRECTION));
            mDoesDialogHaveSubElement = true;

            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogElement.getTagName());
            Log.d(LOGTAG, "attribute vals = "
                    + mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_ID) +
                    ", " + mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_CALLID) +
                    ", " + mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_LOCAL_TAG) +
                    ", " + mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_REMOTE_TAG)
                    + ", " +
                    mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_DIRECTION));

            mDialogInfo= new ImsViceParser.DialogInfo();
            mDialogInfo.dialogId =  mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_ID);
            mDialogInfo.direction =  mViceDialogElement.getAttributeValue(
                    Element.VICE_DIALOG_DIRECTION);
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_SA) ||
                (qName.equalsIgnoreCase(Element.VICE_DIALOG_STATE)) ||
                (qName.equalsIgnoreCase(Element.VICE_DIALOG_DURATION))) {
            mDoesDialogHaveSubElement = true;
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_REPLACES)) {
            Element viceDialogReplacesElement = new Element();
            viceDialogReplacesElement.setTagName(Element.VICE_DIALOG_REPLACES);
            viceDialogReplacesElement.setParentTag(Element.VICE_DIALOG);
            viceDialogReplacesElement.setMapAttribute();
            mViceDialogSubList.add(viceDialogReplacesElement);
            viceDialogReplacesElement.setAttributValue(Element.VICE_DIALOG_REPLACES_CALLID,
                    attributes.getValue(Element.VICE_DIALOG_REPLACES_CALLID));
            viceDialogReplacesElement.setAttributValue(Element.VICE_DIALOG_REPLACES_LOCALTAG,
                    attributes.getValue(Element.VICE_DIALOG_REPLACES_LOCALTAG));
            viceDialogReplacesElement.setAttributValue(Element.VICE_DIALOG_REPLACES_REMOTETAG,
                    attributes.getValue(Element.VICE_DIALOG_REPLACES_REMOTETAG));
            Log.d(LOGTAG, "Root Tag Name:" + viceDialogReplacesElement.getTagName());
            Log.d(LOGTAG, "attribute vals = "
                    + attributes.getValue(Element.VICE_DIALOG_REPLACES_CALLID) +
                    ", " + attributes.getValue(Element.VICE_DIALOG_REPLACES_LOCALTAG) +
                    ", " + attributes.getValue(Element.VICE_DIALOG_REPLACES_REMOTETAG));
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_REFERRED_BY)) {
            mViceDialogReferredElement = new Element();
            mViceDialogReferredElement.setTagName(Element.VICE_DIALOG_REFERRED_BY);
            mViceDialogReferredElement.setParentTag(Element.VICE_DIALOG);
            mViceDialogReferredElement.setMapAttribute();
            mViceDialogSubList.add(mViceDialogReferredElement);
            mDoesDialogHaveSubElement = true;
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogReferredElement.getTagName());
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_ROUTE_SET)) {
            mViceDialogRouteElement = new Element();
            mViceDialogRouteElement.setTagName(Element.VICE_DIALOG_ROUTE_SET);
            mViceDialogRouteElement.setParentTag(Element.VICE_DIALOG);
            mViceDialogRouteElement.setMapAttribute();
            mViceDialogSubList.add(mViceDialogRouteElement);
            mViceDialogRouteSubList = new ArrayList<>();
            mViceDialogRouteElement.setSubElementList(mViceDialogRouteSubList);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogRouteElement.getTagName());
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_ROUTE_SET_HOP)) {
            mViceDialogHopElement = new Element();
            mViceDialogHopElement.setTagName(Element.VICE_DIALOG_ROUTE_SET_HOP);
            mViceDialogHopElement.setParentTag(Element.VICE_DIALOG_ROUTE_SET);
            mViceDialogHopElement.setMapAttribute();
            mViceDialogRouteSubList.add(mViceDialogHopElement);
            mDoesRouteSetHaveSubElement = true;
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogHopElement.getTagName());
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_LOCAL)) {
            mViceDialogLocalElement = new Element();
            mViceDialogLocalElement.setTagName(Element.VICE_DIALOG_LOCAL);
            mViceDialogLocalElement.setParentTag(Element.VICE_DIALOG);
            mViceDialogLocalElement.setMapAttribute();
            mViceDialogSubList.add(mViceDialogLocalElement);
            mViceDialogLocalSubList = new ArrayList<>();
            mViceDialogLocalElement.setSubElementList(mViceDialogLocalSubList);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogLocalElement.getTagName());
            mDoesDialogLocalHaveSubElement = true;
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_LOCAL_IDENTITY)
                && mDoesDialogLocalHaveSubElement) {
            mViceDialogLocalIdentityElement = new Element();
            mViceDialogLocalIdentityElement.setTagName(Element.VICE_DIALOG_LOCAL_IDENTITY);
            mViceDialogLocalIdentityElement.setParentTag(Element.VICE_DIALOG_LOCAL);
            mViceDialogLocalIdentityElement.setMapAttribute();
            mViceDialogLocalSubList.add(mViceDialogLocalIdentityElement);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogLocalIdentityElement.getTagName());
            mDoesDialogLocalHaveSubElement = true;
        }

        if (qName.equalsIgnoreCase(Element.VICE_LOCAL_TARGET)) {
            mViceDialogLocalTargetElement = new Element();
            mViceDialogLocalTargetElement.setTagName(Element.VICE_LOCAL_TARGET);
            mViceDialogLocalTargetElement.setParentTag(Element.VICE_DIALOG_LOCAL);
            mViceDialogLocalTargetElement.setMapAttribute();
            mViceDialogLocalSubList.add(mViceDialogLocalTargetElement);
            mViceDialogLocalTargetSubList = new ArrayList<>();
            mViceDialogLocalTargetElement.setSubElementList(mViceDialogLocalTargetSubList);
            mDoesDialogLocalHaveSubElement = true;
            mViceDialogLocalTargetElement.setAttributValue(
                    Element.VICE_LOCAL_TARGET_URI,
                    attributes.getValue(Element.VICE_LOCAL_TARGET_URI));
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogLocalTargetElement.getTagName());
            Log.d(LOGTAG, "attribute vals = " + mViceDialogLocalTargetElement.getAttributeValue(
                    Element.VICE_LOCAL_TARGET_URI));
        }

        if (qName.equalsIgnoreCase(Element.VICE_LOCAL_TARGET_PARAM)) {
            Element viceDialogLocalParamElement = new Element();
            viceDialogLocalParamElement.setTagName(Element.VICE_LOCAL_TARGET_PARAM);
            viceDialogLocalParamElement.setParentTag(Element.VICE_LOCAL_TARGET);
            viceDialogLocalParamElement.setMapAttribute();
            mViceDialogLocalTargetSubList.add(viceDialogLocalParamElement);
            mDoesDialogLocalHaveSubElement = true;
            viceDialogLocalParamElement.setAttributValue(Element.VICE_LOCAL_TARGET_PARAM_NAME,
                    attributes.getValue(Element.VICE_LOCAL_TARGET_PARAM_NAME));
            viceDialogLocalParamElement.setAttributValue(Element.VICE_LOCAL_TARGET_PARAM_VAL,
                    attributes.getValue(Element.VICE_LOCAL_TARGET_PARAM_VAL));
            Log.d(LOGTAG, "Root Tag Name:" + viceDialogLocalParamElement.getTagName());
            Log.d(LOGTAG, "attribute vals = "
                    + attributes.getValue(Element.VICE_LOCAL_TARGET_PARAM_NAME) +
                    ", " + attributes.getValue(Element.VICE_LOCAL_TARGET_PARAM_VAL));

            mDialogInfo.paramName =  viceDialogLocalParamElement.getAttributeValue(
                    Element.VICE_LOCAL_TARGET_PARAM_NAME);
            mDialogInfo.paramVal =  viceDialogLocalParamElement.getAttributeValue(
                    Element.VICE_LOCAL_TARGET_PARAM_VAL);
        }

        if (qName.equalsIgnoreCase(Element.VICE_MEDIA_ATTRI)) {
            mViceDialogMediaAttriElement = new Element();
            mViceDialogMediaAttriElement.setTagName(Element.VICE_MEDIA_ATTRI);
            mViceDialogMediaAttriElement.setParentTag(Element.VICE_DIALOG_LOCAL);
            mViceDialogMediaAttriElement.setMapAttribute();
            mViceDialogLocalSubList.add(mViceDialogMediaAttriElement);
            mViceDialogMediaAttriSubList = new ArrayList<>();
            mViceDialogMediaAttriElement.setSubElementList(mViceDialogMediaAttriSubList);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogMediaAttriElement.getTagName());
            mDoesDialogLocalHaveSubElement = true;
            mMediaAttribute = new ImsViceParser.MediaAttribute();
        }

        if (qName.equalsIgnoreCase(Element.VICE_MEDIA_TYPE)) {
            mViceDialogMediaTypeElement = new Element();
            mViceDialogMediaTypeElement.setTagName(Element.VICE_MEDIA_TYPE);
            mViceDialogMediaTypeElement.setParentTag(Element.VICE_MEDIA_ATTRI);
            mViceDialogMediaTypeElement.setMapAttribute();
            mViceDialogMediaAttriSubList.add(mViceDialogMediaTypeElement);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogMediaTypeElement.getTagName());
            mDoesDialogLocalHaveSubElement = true;
        }

        if (qName.equalsIgnoreCase(Element.VICE_MEDIA_DIRECTION)) {
            mViceDialogMediaDirElement = new Element();
            mViceDialogMediaDirElement.setTagName(Element.VICE_MEDIA_DIRECTION);
            mViceDialogMediaDirElement.setParentTag(Element.VICE_MEDIA_ATTRI);
            mViceDialogMediaDirElement.setMapAttribute();
            mViceDialogMediaAttriSubList.add(mViceDialogMediaDirElement);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogMediaDirElement.getTagName());
            mDoesDialogLocalHaveSubElement = true;
        }

        if (qName.equalsIgnoreCase(Element.VICE_MEDIA_PORT)) {
            mViceDialogMediaPortElement = new Element();
            mViceDialogMediaPortElement.setTagName(Element.VICE_MEDIA_PORT);
            mViceDialogMediaPortElement.setParentTag(Element.VICE_MEDIA_ATTRI);
            mViceDialogMediaPortElement.setMapAttribute();
            mViceDialogMediaAttriSubList.add(mViceDialogMediaPortElement);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogMediaPortElement.getTagName());
            mDoesDialogLocalHaveSubElement = true;
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_REMOTE)) {
            mViceDialogRemoteElement = new Element();
            mViceDialogRemoteElement.setTagName(Element.VICE_DIALOG_REMOTE);
            mViceDialogRemoteElement.setParentTag(Element.VICE_DIALOG);
            mViceDialogRemoteElement.setMapAttribute();
            mViceDialogSubList.add(mViceDialogRemoteElement);
            mViceDialogRemoteSubList = new ArrayList<>();
            mViceDialogRemoteElement.setSubElementList(mViceDialogRemoteSubList);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogRemoteElement.getTagName());
            mDoesDialogHaveSubElement = true;
        }

        if (qName.equalsIgnoreCase(Element.VICE_DIALOG_REMOTE_IDENTITY) &&
                !mDoesDialogLocalHaveSubElement) {
            mViceDialogRemoteIdentityElement = new Element();
            mViceDialogRemoteIdentityElement.setTagName(Element.VICE_DIALOG_REMOTE_IDENTITY);
            mViceDialogRemoteIdentityElement.setParentTag(Element.VICE_DIALOG_REMOTE);
            mViceDialogRemoteIdentityElement.setMapAttribute();
            mViceDialogRemoteSubList.add(mViceDialogRemoteIdentityElement);
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogRemoteIdentityElement.getTagName());
            mDoesDialogHaveSubElement = true;
        }
    }

    @Override
    public void characters(char[] ch, int start, int length)
            throws SAXException {
        super.characters(ch, start, length);
        builder.append(ch, start, length);
    }

    @Override
    public void endElement(String uri, String localName, String qName)
            throws SAXException {
        super.endElement(uri, localName, qName);
        if (mViceDialogInfoElement != null) {
            Log.d(LOGTAG, "endelment:inside");
            if(mDoesDialogHaveSubElement) {
                if (qName.equalsIgnoreCase(Element.VICE_DIALOG_SA)) {
                    mViceDialogElement.setAttributValue(
                            Element.VICE_DIALOG_SA, builder.toString().trim());
                    Log.d(LOGTAG, "VICE_DIALOG_SA attribute vals = "
                            + mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_SA));

                    mDialogInfo.exclusive =
                            mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_SA);
                } else if (qName.equalsIgnoreCase(Element.VICE_DIALOG_STATE)) {
                    mViceDialogElement.setAttributValue(Element.VICE_DIALOG_STATE,
                            builder.toString().trim());
                    Log.d(LOGTAG, "VICE_DIALOG_STATE attribute vals = "
                            + mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_STATE));

                    mDialogInfo.state =
                            mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_STATE);
                } else if (qName.equalsIgnoreCase(Element.VICE_DIALOG_DURATION)) {
                    mViceDialogElement.setAttributValue(
                            Element.VICE_DIALOG_DURATION, builder.toString().trim());
                    Log.d(LOGTAG, "VICE_DIALOG_DURATION attribute vals = "
                            + mViceDialogElement.getAttributeValue(Element.VICE_DIALOG_DURATION));
                } else if (qName.equalsIgnoreCase(Element.VICE_DIALOG_REMOTE_IDENTITY)) {
                    mViceDialogRemoteIdentityElement.setAttributValue(
                            Element.VICE_DIALOG_REMOTE_IDENTITY, builder.toString().trim());
                    Log.d(LOGTAG, "VICE_DIALOG_REMOTE_IDENTITY attribute vals = "
                            + mViceDialogRemoteIdentityElement.getAttributeValue(
                            Element.VICE_DIALOG_REMOTE_IDENTITY));

                    mDialogInfo.remoteIdentity =
                            mViceDialogRemoteIdentityElement.getAttributeValue(
                                    Element.VICE_DIALOG_REMOTE_IDENTITY);
                } else if (qName.equalsIgnoreCase(Element.VICE_DIALOG_REFERRED_BY)) {
                    mViceDialogReferredElement.setAttributValue(
                            Element.VICE_DIALOG_REFERRED_BY, builder.toString().trim());
                    Log.d(LOGTAG, "VICE_DIALOG_REFERRED_BY attribute vals = "
                            + mViceDialogReferredElement.getAttributeValue(
                            Element.VICE_DIALOG_REFERRED_BY));
                }
                mDoesDialogHaveSubElement = false;
            }
            if (mDoesRouteSetHaveSubElement) {
                if (qName.equalsIgnoreCase(Element.VICE_DIALOG_ROUTE_SET_HOP)) {
                    mViceDialogHopElement.setAttributValue(
                            Element.VICE_DIALOG_ROUTE_SET_HOP, builder.toString().trim());
                    Log.d(LOGTAG, "VICE_DIALOG_ROUTE_SET_HOP attribute vals = "
                            + mViceDialogHopElement.getAttributeValue(
                            Element.VICE_DIALOG_ROUTE_SET_HOP));
                }
                mDoesRouteSetHaveSubElement = false;
            }
            if (mDoesDialogLocalHaveSubElement) {
                if (qName.equalsIgnoreCase(Element.VICE_DIALOG_LOCAL_IDENTITY)) {
                    mViceDialogLocalIdentityElement.setAttributValue(
                            Element.VICE_DIALOG_LOCAL_IDENTITY, builder.toString().trim());
                    Log.d(LOGTAG, "VICE_DIALOG_LOCAL_IDENTITY attribute vals = "
                            + mViceDialogLocalIdentityElement.getAttributeValue(
                            Element.VICE_DIALOG_LOCAL_IDENTITY));
                } else if (qName.equalsIgnoreCase(Element.VICE_MEDIA_TYPE)) {
                    mViceDialogMediaTypeElement.setAttributValue(Element.VICE_MEDIA_TYPE,
                            builder.toString().trim());
                Log.d(LOGTAG, "VICE_MEDIA_TYPE attribute vals = "
                        + mViceDialogMediaTypeElement.getAttributeValue(
                        Element.VICE_MEDIA_TYPE));

                    mMediaAttribute.mediaType = mViceDialogMediaTypeElement.
                            getAttributeValue(Element.VICE_MEDIA_TYPE);
                } else if (qName.equalsIgnoreCase(Element.VICE_MEDIA_DIRECTION)) {
                    mViceDialogMediaDirElement.setAttributValue(Element.VICE_MEDIA_DIRECTION,
                            builder.toString().trim());
                    Log.d(LOGTAG, "VICE_MEDIA_DIRECTION attribute vals = "
                            + mViceDialogMediaDirElement.getAttributeValue(
                            Element.VICE_MEDIA_DIRECTION));

                    mMediaAttribute.mediaDirection = mViceDialogMediaDirElement.
                            getAttributeValue(Element.VICE_MEDIA_DIRECTION);
                } else if (qName.equalsIgnoreCase(Element.VICE_MEDIA_PORT)) {
                    mViceDialogMediaPortElement.setAttributValue(Element.VICE_MEDIA_PORT,
                            String.valueOf(true));
                    Log.d(LOGTAG, "VICE_MEDIA_PORT attribute vals = "
                            + mViceDialogMediaPortElement.getAttributeValue(
                            Element.VICE_MEDIA_PORT));

                    mMediaAttribute.mediaPort = mViceDialogMediaPortElement.
                            getAttributeValue(Element.VICE_MEDIA_PORT);
                }
                mDoesDialogLocalHaveSubElement = false;
            }

            if (qName.equalsIgnoreCase(Element.VICE_MEDIA_ATTRI)) {
                mDialogInfo.mediaAttributes.add(mMediaAttribute);
            }

            if (qName.equalsIgnoreCase(Element.VICE_DIALOG)) {
                mDialogInfoList.add(mDialogInfo);
            }
            builder.setLength(0);
        }
    }

    @Override
    public void endDocument() {
        try {
            super.endDocument();
            Log.d(LOGTAG, "Root Tag Name:" + mViceDialogInfoElement.getTagName());
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }

    public List<ImsViceParser.DialogInfo> dumpDialogInfo() {
        if (mDialogInfoList != null && !mDialogInfoList.isEmpty()) {
            int size = mDialogInfoList.size();
            for (int i = 0; i < size; i++) {
                Log.d(LOGTAG, "dumpDialogInfo[" + i + "] -> index "
                        + ", dialog ID = " + mDialogInfoList.get(i).dialogId
                        + ", exclusive = " + mDialogInfoList.get(i).exclusive
                        + ", direction = " +  mDialogInfoList.get(i).direction
                        + ", state = " + mDialogInfoList.get(i).state
                        + ", param name = " + mDialogInfoList.get(i).paramName
                        + ", param val = " + mDialogInfoList.get(i).paramVal
                        + ", remote identity = " + mDialogInfoList.get(i).remoteIdentity);
                if (mDialogInfoList.get(i).mediaAttributes != null &&
                        !mDialogInfoList.get(i).mediaAttributes.isEmpty()) {
                    int mediaattri = mDialogInfoList.get(i).mediaAttributes.size();
                    for (int j = 0; j < mediaattri; j++) {
                        Log.d(LOGTAG, "dumpDialogInfo[" + i + "] -> index "
                                + ", dialog media type = "
                                + mDialogInfoList.get(i).mediaAttributes.get(j).mediaType
                                + ", dialog media direction = "
                                + mDialogInfoList.get(i).mediaAttributes.get(j).mediaDirection
                                + ", dialog media port = "
                                + mDialogInfoList.get(i).mediaAttributes.get(j).mediaPort
                        );
                    }
                }
            }
            return mDialogInfoList;
        }
        return null;
    }
}

