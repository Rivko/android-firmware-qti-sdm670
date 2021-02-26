/* ==============================================================================
 * WFDServiceUtils.java
 *
 * Contains utility function implementations
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================== */

package com.qualcomm.wfd;

import android.util.Log;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

/**
 * Created by ichakrab on 25-05-15.
 */
public class WFDServiceUtils {

    private static final String TAG = "WFDServiceUtils";

    private static final String configFileLocation = "/system/etc/wfdconfig.xml";

    private static DocumentBuilder sDocumentBuilder = null;

    private static Document sDocument = null;

    private static File sConfigFile = null;

    public static int setup() {
        setupXML();
        return 0;
    }

    public static int destroy() {
        releaseFileReferences();
        return 0;
    }

    private static void setupXML() {
        try {
            sDocumentBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
        } catch (ParserConfigurationException e) {
            e.printStackTrace();
            releaseFileReferences();
            return;
        }
        sConfigFile = new File(configFileLocation);
        if (sConfigFile.isFile()) {
            try {
                sDocument = sDocumentBuilder.parse(sConfigFile);
            } catch (SAXException e) {
                e.printStackTrace();
                releaseFileReferences();
                return;
            } catch (IOException e) {
                e.printStackTrace();
                releaseFileReferences();
                return;
            }
        } else {
            Log.e(TAG, sConfigFile + " is not on the filesystem");
            releaseFileReferences();
        }
    }

    private static Node findNodeByName(Node node, String name) {
        if (node != null && name != null) {
            if (name.equalsIgnoreCase(node.getNodeName())) {
                return node;
            } else {
                for (Node n = node.getFirstChild(); n != null;n = n
                        .getNextSibling()) {
                    Node found = findNodeByName(n, name);
                    if (found != null) {
                        return found;
                    }
                }
            }
        }
        return null;
    }

    /**
     * This method is a helper to determine the value of a key in the config file.
     *
     * @param key The String whose value is to be looked up
     * @return The value of the key or "Not Found" if not found
     */
    public static String getValue(String key) {
        Node value = findNodeByName(sDocument, key);
        if(value == null) {
            //may be null if invalid key is provided
            Log.e(TAG,key + "not found in config file");
            return "Not Found";
        }
        return value.getTextContent();
    }

    private static void releaseFileReferences() {
        Log.d(TAG, "releaseFileReferences()");
        sDocumentBuilder = null;
        sDocument = null;
        sConfigFile = null;
    }
}
