/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm.ui;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.drm.DrmManagerClient;
import android.drm.DrmRights;
import android.net.Uri;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;

import com.oma.drm.R;
import com.android.internal.util.HexDump;

public class DrmRightsInstallerActivity extends Activity {

    private static final String TAG = "OmaDrm:DrmRightsActivity";

    private static final String DRM_MIMETYPE_RIGHTS_XML_STRING = "application/vnd.oma.drm.rights+xml";

    private static final String DRM_MIMETYPE_RIGHTS_WBXML_STRING = "application/vnd.oma.drm.rights+wbxml";

    private static final String TAG_RIGHTS = "o-ex:rights"; // 05
    private static final String TAG_CONTEXT = "o-ex:context"; // 06
    private static final String TAG_VERSION = "o-dd:version"; // 07
    private static final String TAG_UID = "o-dd:uid"; // 08
    private static final String TAG_AGREEMENT = "o-ex:agreement"; // 09
    private static final String TAG_ASSET = "o-ex:asset"; // 0A
    private static final String TAG_KEYINFO = "ds:KeyInfo"; // 0B
    private static final String TAG_KEYVALUE = "ds:KeyValue"; // 0C
    private static final String TAG_PERMISSION = "o-ex:permission"; // 0D
    private static final String TAG_PLAY = "o-dd:play"; // 0E
    private static final String TAG_DISPLAY = "o-dd:display"; // 0F
    private static final String TAG_EXECUTE = "o-dd:execute"; // 10
    private static final String TAG_PRINT = "o-dd:print"; // 11
    private static final String TAG_CONSTRAINT = "o-ex:constraint"; // 12
    private static final String TAG_COUNT = "o-dd:count"; // 13
    private static final String TAG_DATETIME = "o-dd:datetime"; // 14
    private static final String TAG_START = "o-dd:start"; // 15
    private static final String TAG_END = "o-dd:end"; // 16
    private static final String TAG_INTERVAL = "o-dd:interval"; // 17

    private static final String XMLNS_OEX = "xmlns:o-ex"; // 05
    private static final String XMLNS_ODD = "xmlns:o-dd"; // 06
    private static final String XMLNS_DD = "xmlns:ds"; // 07

    private static String mKeyValue = null;

    final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();

    boolean isFromDMService = false;
    public static final String KEY_FILE_DATA = "FILE_DATA";
    public static final String KEY_DM_SERVICE = "SERVICE";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        try {
            String rightMimeType = getIntent().getType();
            isFromDMService = getIntent()
                    .getBooleanExtra(KEY_DM_SERVICE, false);
            byte[] inputData;
            Uri uri = null;
            if (isFromDMService) {
                inputData = getIntent().getByteArrayExtra(KEY_FILE_DATA);
            } else {
                uri = getIntent().getData();
                InputStream iStream = getContentResolver().openInputStream(uri);
                inputData = toByteArray(iStream);
            }

            File rightsFile = new File(getApplicationContext().getFilesDir(),
                    "rights.xml");
            String tempRightsFilePath = rightsFile.getAbsolutePath();
            // Log.d(TAG, " tempRightsFilePath: " + tempRightsFilePath);
            if (rightsFile.exists()) {
                rightsFile.delete();
            }
            rightsFile.createNewFile();

            if (DRM_MIMETYPE_RIGHTS_XML_STRING.equals(rightMimeType)) {

                FileOutputStream stream = new FileOutputStream(rightsFile);
                stream.write(inputData);
                stream.close();
            } else if (DRM_MIMETYPE_RIGHTS_WBXML_STRING.equals(rightMimeType)) {

                String hexData = HexDump.toHexString(inputData);
                setKeyValue(inputData);
                // Log.d(TAG, "hHexData: " + hexData);
                convertWbxmlToXml(splitEqually(hexData, 2), rightsFile);
            }
            DrmRights drmRights = new DrmRights(tempRightsFilePath,
                    rightMimeType);
            DrmManagerClient drmClient = null;
            try {
                drmClient = new DrmManagerClient(getApplicationContext());

                drmClient.saveRights(drmRights, tempRightsFilePath, null);
                // Delete temp file
                rightsFile.delete();
            } finally {
                if (drmClient != null) {
                    drmClient.release();
                }
            }
            if (!isFromDMService) {
                // Delete DRM rights File
                getContentResolver().delete(uri, null, null);
            }
        } catch (FileNotFoundException e) {
            Log.e(TAG, "FileNotFoundException.");
        } catch (IOException e) {
            Log.e(TAG, "IOException.");
        } catch (Exception ee) {
            Log.d(TAG, "Exception " + ee.getMessage());
        }
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage(R.string.drm_rights_message)
                .setOnCancelListener(new DialogInterface.OnCancelListener() {
                    @Override
                    public void onCancel(DialogInterface dialog) {
                        finish();
                    }
                })
                .setPositiveButton(android.R.string.ok, new OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        finish();
                    }
                });

        final AlertDialog dialog = builder.create();
        dialog.show();
    }

    private void convertWbxmlToXml(List<String> wbxmlData, File rightsfile) {
        Iterator<String> iterator = wbxmlData.iterator();
        DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory
                .newInstance();
        DocumentBuilder documentBuilder = null;
        try {
            documentBuilder = documentBuilderFactory.newDocumentBuilder();
            Document doc = documentBuilder.newDocument();
            Element rights = null;
            Element context = null;
            Element version = null;
            Element uid = null;
            Element agreement = null;
            Element asset = null;
            Element keyinfo = null;
            Element keyvalue = null;
            Element permission = null;
            Element play = null;
            Element display = null;
            Element constraint = null;
            Element count = null;
            Element datetime = null;
            Element start = null;
            Element end = null;
            Element interval = null;

            boolean isVersion = false;
            boolean isKeyValue = false;
            boolean isUid = false;
            boolean isCount = false;
            boolean isStart = false;
            boolean isEnd = false;
            boolean isInterval = false;
            boolean isPlay = false;
            boolean isDisplay = false;
            int i = 0;
            int previous = 0;
            String mTagValue = "";
            while (iterator.hasNext()) {
                String ss = iterator.next();
                int current = Integer.parseInt(ss, 16);
                switch (current) {
                case 0x00: // 00 String Terminator
                    if (isVersion) {
                        isVersion = false;
                        version.appendChild(doc.createTextNode(mTagValue));
                        context.appendChild(version);
                    } else if (isUid) {
                        isUid = false;
                        uid.appendChild(doc.createTextNode(mTagValue));
                        context.appendChild(uid);
                    } else if (isCount) {
                        isCount = false;
                        count.appendChild(doc.createTextNode(mTagValue));
                        constraint.appendChild(count);
                    } else if (isStart) {
                        isStart = false;
                        start.appendChild(doc.createTextNode(mTagValue));
                        datetime.appendChild(start);
                    } else if (isEnd) {
                        isEnd = false;
                        end.appendChild(doc.createTextNode(mTagValue));
                        datetime.appendChild(end);
                        constraint.appendChild(datetime);
                    } else if (isInterval) {
                        isInterval = false;
                        interval.appendChild(doc.createTextNode(mTagValue));
                        constraint.appendChild(interval);
                    }
                    mTagValue = "";
                    break;
                case 0x01: // End Tag
                    if (isKeyValue) {
                        isKeyValue = false;
                        keyvalue.appendChild(doc.createTextNode(mKeyValue));
                        keyinfo.appendChild(keyvalue);
                        asset.appendChild(keyinfo);
                        mTagValue = "";
                    }
                    break;
                case 0xC3:
                    break;
                case 0x10:
                    break;
                case 0x6A:
                    if (i > 2) {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0xC5: // Rights
                    if (i == 4) {
                        rights = doc.createElement(TAG_RIGHTS);
                        doc.appendChild(rights);
                        rights.setAttribute(XMLNS_OEX,
                                "http://odrl.net/1.1/ODRL-EX");
                        rights.setAttribute(XMLNS_ODD,
                                "http://odrl.net/1.1/ODRL-DD");
                        rights.setAttribute(XMLNS_DD,
                                "http://www.w3.org/2000/09/xmldsig#/");
                    }
                    break;
                case 0x46: // Context
                    context = doc.createElement(TAG_CONTEXT);
                    if (i == 12) {
                        rights.appendChild(context);
                    } else if (previous == 74) {
                        asset.appendChild(context);
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x47: // Version
                    if (previous == 70) {
                        version = doc.createElement(TAG_VERSION);
                        isVersion = true;
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x48: // UID
                    if (previous == 70) {
                        uid = doc.createElement(TAG_UID);
                        isUid = true;
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x49: // Agreement
                    if (previous == 1) {
                        agreement = doc.createElement(TAG_AGREEMENT);
                        rights.appendChild(agreement);
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x4A: // Asset
                    if (previous == 73) {
                        asset = doc.createElement(TAG_ASSET);
                        agreement.appendChild(asset);
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x4B: // KeyInfo
                    if (previous == 1) {
                        keyinfo = doc.createElement(TAG_KEYINFO);
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x4C: // KeyValue
                    if (previous == 75) {
                        keyvalue = doc.createElement(TAG_KEYVALUE);
                        isKeyValue = true;
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x4D: // Permission
                    if (previous == 1) {
                        permission = doc.createElement(TAG_PERMISSION);
                        agreement.appendChild(permission);
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x4E: // Play
                    if (previous == 77) {
                        play = doc.createElement(TAG_PLAY);
                        isPlay = true;
                        permission.appendChild(play);
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x4F: // Display
                    if (previous == 77) {
                        display = doc.createElement(TAG_DISPLAY);
                        isDisplay = true;
                        permission.appendChild(display);
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x50: // Execute
                    mTagValue = mTagValue + (char) current;
                    break;
                case 0x51: // Print
                    mTagValue = mTagValue + (char) current;
                    break;
                case 0x52: // Constraint
                    if (previous == 78 || previous == 79) {
                        constraint = doc.createElement(TAG_CONSTRAINT);
                        if (isPlay) {
                            play.appendChild(constraint);
                            isPlay = false;
                        } else if (isDisplay) {
                            display.appendChild(constraint);
                            isDisplay = false;
                        }
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x53: // Count
                    if (previous == 82) {
                        count = doc.createElement(TAG_COUNT);
                        isCount = true;
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x54: // DateTime
                    if (previous == 1 || previous == 82) {
                        datetime = doc.createElement(TAG_DATETIME);
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x55: // Start
                    if (previous == 84) {
                        start = doc.createElement(TAG_START);
                        isStart = true;
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x56: // End
                    if (previous == 1) {
                        end = doc.createElement(TAG_END);
                        isEnd = true;
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x57: // Interval
                    if (previous == 1 || previous == 82) {
                        interval = doc.createElement(TAG_INTERVAL);
                        isInterval = true;
                        mTagValue = "";
                    } else {
                        mTagValue = mTagValue + (char) current;
                    }
                    break;
                case 0x21:
                case 0x22:
                case 0x23:
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x27:
                case 0x28:
                case 0x29:
                case 0x2A:
                case 0x2B:
                case 0x2C:
                case 0x2D:
                case 0x2E:
                case 0x2F:
                case 0x30:
                case 0x31:
                case 0x32:
                case 0x33:
                case 0x34:
                case 0x35:
                case 0x36:
                case 0x37:
                case 0x38:
                case 0x39:
                case 0x3A:
                case 0x3B:
                case 0x3C:
                case 0x3D:
                case 0x3E:
                case 0x3F:
                case 0x40:
                case 0x41:
                case 0x42:
                case 0x43:
                case 0x44:
                case 0x45:
                case 0x58:
                case 0x59:
                case 0x5A:
                case 0x5B:
                case 0x5C:
                case 0x5D:
                case 0x5E:
                case 0x5F:
                case 0x60:
                case 0x61:
                case 0x62:
                case 0x63:
                case 0x64:
                case 0x65:
                case 0x66:
                case 0x67:
                case 0x68:
                case 0x69:
                case 0x6B:
                case 0x6C:
                case 0x6D:
                case 0x6E:
                case 0x6F:
                case 0x70:
                case 0x71:
                case 0x72:
                case 0x73:
                case 0x74:
                case 0x75:
                case 0x76:
                case 0x77:
                case 0x78:
                case 0x79:
                case 0x7A:
                case 0x7B:
                case 0x7C:
                case 0x7D:
                case 0x7E:
                case 0x7F:
                    mTagValue = mTagValue + (char) current;
                    break;
                default:
                    break;
                }
                previous = current;
                i += 1;
            }

            TransformerFactory transformerFactory = TransformerFactory
                    .newInstance();
            Transformer transformer = transformerFactory.newTransformer();
            DOMSource source = new DOMSource(doc);
            StreamResult result = new StreamResult(rightsfile);
            transformer.transform(source, result);
        } catch (Exception e) {
            Log.d(TAG, "Error while convertWbxmlToXml. " + e.getMessage());
        }
    }

    private void setKeyValue(byte[] val) {
        try {
            int start = 0, end = 0, curr = 0, prev = 0;
            StringBuilder sb = new StringBuilder();
            for (int j = 0; j < val.length; j++) {
                curr = (val[j] & 0xFF);
                if (curr == 16 && prev == 195) {
                    start = j;
                    start += 1;
                    end = start + 16;
                }
                prev = curr;
            }
            byte[] encoded = Base64.encode(Arrays.copyOfRange(val, start, end),
                    Base64.NO_WRAP);
            for (byte bb : encoded) {
                sb.append((char) bb);
            }
            mKeyValue = sb.toString();
        } catch (Exception e) {
            Log.d(TAG, "Error while reading keyValue. ");
        }
    }

    public static List<String> splitEqually(String text, int size) {
        List<String> ret = new ArrayList<String>((text.length() + size - 1)
                / size);

        for (int start = 0; start < text.length(); start += size) {
            ret.add(text.substring(start, Math.min(text.length(), start + size)));
        }
        return ret;
    }

    private byte[] toByteArray(InputStream inputStream) throws IOException {
        ByteArrayOutputStream byteBuffer = new ByteArrayOutputStream();
        int bufferSize = 1024;
        byte[] buffer = new byte[bufferSize];

        int len = 0;
        while ((len = inputStream.read(buffer)) != -1) {
            byteBuffer.write(buffer, 0, len);
        }
        return byteBuffer.toByteArray();
    }
}
