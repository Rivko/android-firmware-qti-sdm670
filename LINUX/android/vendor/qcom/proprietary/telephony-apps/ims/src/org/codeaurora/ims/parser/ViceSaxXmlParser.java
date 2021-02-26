/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */

package org.codeaurora.ims.parser;

import android.util.Log;
import java.io.InputStream;
import java.util.List;
import java.text.ParseException;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;

public class ViceSaxXmlParser{
    private static ViceSaxXmlParser mXmlparser = null;
    private static ViceSaxXmlHandler mSaxHandler;

    private ViceSaxXmlParser() {

    }

    synchronized public static Element parse(InputStream is) {
        try {
            // create a XMLReader from SAXParser
            XMLReader xmlReader = SAXParserFactory.newInstance().newSAXParser()
                    .getXMLReader();
            // create a SAXXMLHandler for VICE
            mSaxHandler = new ViceSaxXmlHandler();
            // store handler in XMLReader
            xmlReader.setContentHandler(mSaxHandler);
            // TODO: Encoding is set in xml itself
            // but unless xml is parsed, cannot determine the encoding
            // Hardcoding it for now.
            InputSource inputSource = new InputSource(is);
            inputSource.setEncoding("utf-8");
            // the process starts
            xmlReader.parse(inputSource);
        } catch (Exception ex) {
            if (ex instanceof InvalidConfVersionException) {
                Log.d("ViceSaxxParser", "Exception caught at SAXParser");
            } else if (ex instanceof ParseException) {
                Log.d("ViceSaxxParser", "InValid Format , Exception in Parser");
                ex.printStackTrace();
            }
        }
        return mSaxHandler.getConferenceInfoLatestMessage();
    }

    public static ViceSaxXmlParser getInstance() {
        if (mXmlparser == null) {
            mXmlparser = new ViceSaxXmlParser();
        }
        return mXmlparser;
    }

    public List<ImsViceParser.DialogInfo> getDialogInfo() {
        if (mSaxHandler != null) {
            return mSaxHandler.dumpDialogInfo();
        }
        return null;
    }

}

