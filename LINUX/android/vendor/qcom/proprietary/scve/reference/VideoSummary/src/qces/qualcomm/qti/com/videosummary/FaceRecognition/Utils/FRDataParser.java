/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.Utils;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by mithun on 12/18/15.
 */
public class FRDataParser {
    final String FR_ID_HEADER = "$#FR";
    final String ASD_HEADER = "$#SD";
    final String ED_HEADER = "%%ED";//End of data

    final int UNKNOWN = -1;
    final int FACE_IDS = 1;
    final int ASD = 2;
    final int END_OF_DATA = 3;


    List<byte[]> faceIdlist;
    List<Byte> asdCategoryList;

    byte[] mData;

    public FRDataParser(byte[] data) {
        this.mData = data;
        parse();
    }

    /**
     * Parse method will parse the entire data byte array and will load the meaning full data into its corresponding dataStructure
     * After parse returns true, use the getter methods to get the faceIDList, asdCategoryList
     *
     * @return
     */
    public boolean parse() {
        faceIdlist = null;

        int length = mData.length;

        int i = 0;
        while ((i + 3) < length) {//i+3 is used because we should have atleast 4 bytes which is either a header or end of data

            int headerID = getHeaderID(i);
            i += 4;//increament i by 4 bytes to jump the 4 byte header
            switch (headerID) {
                case FACE_IDS:
                    byte[] faceId = get20ByteFaceID(i);
                    i += 20;//increment by 20 bytes for FR ID
                    if (faceIdlist == null) {
                        faceIdlist = new ArrayList<byte[]>();
                    }
                    faceIdlist.add(faceId);
                    break;
                case ASD:
                    Byte asdCategory = getOneByteASDCategory(i);
                    i += 1;//increment by 1 byte for ASD category
                    if (asdCategoryList == null) {
                        asdCategoryList = new ArrayList<Byte>();
                    }
                    asdCategoryList.add(asdCategory);
                    break;
                case END_OF_DATA:
                    return true;
                default:
                    throw new UnsupportedOperationException("header unknown during parsing data at index " + i +
                            " 4 byte data = " + (char) mData[i] + "" + (char) mData[i + 1] + "" + (char) mData[i + 2] + "" + (char) mData[i + 3]);
            }
        }
        return true;
    }

    private byte[] get20ByteFaceID(int i) {
        if (mData.length <= i + 19) {
            throw new IndexOutOfBoundsException("insufficifient data to extract 20 byte Face ID at index " + i + " with data.length = " + mData.length);
        }
        byte[] faceID = new byte[20];
        for (int j = 0; j < 20; j++) {
            faceID[j] = mData[j + i];
        }

        return faceID;
    }

    private byte getOneByteASDCategory(int i) {
        if (mData.length <= i) {
            throw new IndexOutOfBoundsException("insufficifient data to extract ASD Cateogry at index " + i + " with data.length = " + mData.length);
        }
        return mData[i];
    }

    private int getHeaderID(int i) {
        byte[] header = new byte[4];
        int retVal = UNKNOWN;

        //copy the 4 byte header and convert to string for easy string comparision
        for (int j = 0; j < 4; j++) {
            header[j] = mData[i + j];
        }

        String headerString = new String(header);
        if (headerString.compareTo(FR_ID_HEADER) == 0) {
            retVal = FACE_IDS;
        } else if (headerString.compareTo(ASD_HEADER) == 0) {
            retVal = ASD;
        } else if (headerString.compareTo(ED_HEADER) == 0) {
            retVal = END_OF_DATA;
        }

        return retVal;
    }

    public List<byte[]> getFaceIDList() {
        return faceIdlist;
    }

    public List<Byte> getASDCategoryList() {
        return asdCategoryList;
    }

    /**
     * Used only for testing this parser to feed data.
     *
     * @return
     */
    public static byte[] getDummyData() {
        String data = new String("FR#$12345678901234567890");
        return data.getBytes();
    }
}
