/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Contributed by: Giesecke & Devrient GmbH.
 */

package org.simalliance.openmobileapi.service.security.arf.PKCS15;

import java.util.Arrays;

import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.ASN1;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;

import android.util.Log;

public class EF {

    public static final String TAG = "SmartcardService ACE ARF";

    public static final int APDU_SUCCESS = 0x9000;
    private static final int BUFFER_LEN = 253;

    // 2012-04-13
    private static final short EF = 0x04;
    private static final short TRANSPARENT = 0x00;
    private static final short LINEAR_FIXED = 0x01;
    private static final short UNKNOWN = 0xFF;

    // Selected file parameters
    private short mFileType = UNKNOWN,mFileStructure = UNKNOWN, mFileNbRecords;
    private int mFileID,mFileSize,mFileRecordSize;
    private String mFilePath;


    // Handle to "Secure Element" object
    protected SecureElement mSEHandle=null;

    public EF( SecureElement handle ){
        mSEHandle = handle;
    }

    public int getFileId(){
        return mFileID;
    }
    public String getFilePath(){
        return mFilePath;
    }

    private void decodeFileProperties(byte[] data )
        throws SecureElementException
    {
        if( data != null ){
            // check if first byte is the FCP tag
            // then do USIM decoding
            if( data[0] == 0x62 ){
                decodeUSIMFileProps(data);
            } else {
                // otherwise sim decoding
                decodeSIMFileProps(data);
            }
        }
    }

    /**
     * Decodes file properties (SIM cards)
     * @param data TS 51.011 encoded characteristics
     */
    private void decodeSIMFileProps(byte[] data)
        throws SecureElementException
    {
        if ((data==null)||(data.length<15))
            throw new SecureElementException("Invalid Response data");

        // 2012-04-13
        // check type of file
        if( (short)(data[6] & 0xFF) == (short)0x04  ){
            mFileType = EF;
        } else {
            mFileType = UNKNOWN; // may also be DF or MF, but we are not interested in them.
        }
        if( (short)(data[13]&0xFF) == (short)0x00 ) {
            mFileStructure = TRANSPARENT;
        } else  if( (short)(data[13]&0xFF) == (short)0x01 ) {
            mFileStructure = LINEAR_FIXED;
        } else {
            mFileStructure = UNKNOWN; // may also be cyclic
        }
        mFileSize=((data[2] & 0xFF)<<8)|(data[3] & 0xFF);

        // check if file is cyclic or linear fixed
        if (mFileType == EF && // is EF ?
                mFileStructure != TRANSPARENT ) {
            mFileRecordSize=data[14] & 0xFF;
            mFileNbRecords=(short)(mFileSize/mFileRecordSize);
        }
    }

    /**
     * Decodes file properties (USIM cards)
     * @param data TLV encoded characteristics
     */
    private void decodeUSIMFileProps(byte[] data)
        throws SecureElementException
    {
        try {
            byte[] buffer=null;
            DERParser DER=new DERParser(data);

            DER.parseTLV(ASN1.TAG_FCP);
            while(!DER.isEndofBuffer()) {
                switch(DER.parseTLV()) {
                case (byte)0x80: // File size
                    buffer=DER.getTLVData();
                    if ((buffer!=null)&&(buffer.length>=2))
                        mFileSize=((buffer[0] & 0xFF)<<8)|(buffer[1] & 0xFF);
                    break;
                case (byte)0x82: // File descriptor
                    buffer=DER.getTLVData();
                    if ((buffer!=null)&&(buffer.length>=2)) {
                        if( (short)(buffer[0] & 0x07) == (short)0x01 ) {
                            mFileStructure = TRANSPARENT;
                        } else if( (short)(buffer[0] & 0x07) == (short)0x02 ) {
                            mFileStructure = LINEAR_FIXED;
                        } else {
                            mFileStructure = UNKNOWN; // may also be cyclic
                        }

                        // check if bit 4,5,6 are set
                        // then this is a DF or ADF, but we mark it with UNKNOWN,
                        // since we are only interested in EFs.
                        if( (short)(buffer[0] & 0x38) == (short)0x38 ) {
                            mFileType=UNKNOWN;
                        } else {
                            mFileType=EF;
                        }
                        if (buffer.length==5) {
                            mFileRecordSize=buffer[3] & 0xFF;
                            mFileNbRecords=(short)(buffer[4] & 0xFF);
                        }
                    } break;
                default:
                    DER.skipTLVData(); break;
                }
            }
        } catch(Exception e) {
            throw new SecureElementException("Invalid GetResponse");
        }
    }


    /**
     * Selects a file (INS 0xA4)
     * @param path Path of the file
     * @return Command status code [sw1 sw2]
     */
    public int selectFile(byte[] path)
        throws SecureElementException
    {
        if ((path==null) || (path.length==0) || ((path.length%2)!=0))
            throw new SecureElementException("Incorrect path");

        int index;
        int length=path.length;
        if ((mSEHandle.getSeInterface() == SecureElement.SIM_IO)&&(length>2)) {
            index=length-2; // Only FileID is usefull
            mFilePath=Util.bytesToString(path,0,index,"");
        } else {
            index=0;
            mFilePath="";
        }


        byte[] data=null;
        byte[] cmd= new byte[]{ 0x00,(byte)0xA4,0x00,0x04,0x02,0x00,0x00 };

        mFileType=UNKNOWN;
        mFileStructure = UNKNOWN;
        mFileSize=0;
        mFileRecordSize=0;
        mFileNbRecords=0;

        // iterate through path
        for(int sw1;index<length;index+=2) {
            mFileID=((path[index]&0xFF)<<8) | (path[index+1] & 0xFF);
            cmd[5]=(byte)(mFileID>>8);
            cmd[6]=(byte)mFileID;

            data=mSEHandle.exchangeAPDU( this, cmd);

            // Check ADPU status
            sw1=data[data.length-2] & 0xFF;
            if ( (sw1!=0x62) &&
                 (sw1!=0x63) &&
                 (sw1!=0x90) &&
                 (sw1!=0x91) )
            {
                return (sw1<<8) | (data[data.length-1] & 0xFF);
            }
        }

        // Analyse file properties
        decodeFileProperties(data);

        return APDU_SUCCESS;
    }

    /**
     * Reads data from the current selected file (INS 0xB0)
     * @param offset Offset at which to start reading
     * @param nbBytes Number of bytes to read
     * @return Data retreived from the file
     */
    public byte[] readBinary(int offset,int nbBytes)
    throws SecureElementException {
        if (mFileSize==0) return null;
        if (nbBytes==-1) nbBytes=mFileSize;
        if (mFileType != EF)
            throw new SecureElementException("Incorrect file type");
        if (mFileStructure != TRANSPARENT )
            throw new SecureElementException("Incorrect file structure");

        int length,pos=0;
        byte[] result=new byte[nbBytes];
        byte[] cmd={ 0x00,(byte)0xB0,0x00,0x00,0x00 };

        while (nbBytes!=0) {
            if (nbBytes<BUFFER_LEN)
                length=nbBytes;
            else
                length=BUFFER_LEN; // Set to max buffer size

            cmd[2]=(byte)(offset>>8);
            cmd[3]=(byte)offset;
            cmd[4]=(byte)length;
            System.arraycopy(mSEHandle.exchangeAPDU(this, cmd),0,result,pos,length);
            nbBytes-=length; offset+=length; pos+=length;
        }
        return result;
    }

    /**
     * Reads a record from the current selected file (INS 0xB2)
     * @param record Record ID [0..n]
     * @return Data from requested record
     */
    public byte[] readRecord(short record)
    throws SecureElementException {
        // Check the type of current selected file
        if (mFileType != EF)
            throw new SecureElementException("Incorrect file type");
        if (mFileStructure != LINEAR_FIXED)
            throw new SecureElementException("Incorrect file structure");

        // Check if requested record is valid
        if ((record<0) || (record>mFileNbRecords))
            throw new SecureElementException("Incorrect record number");

        Log.i(TAG,"ReadRecord ["+record+"/"+mFileRecordSize+"b]");
        byte[] cmd= { 0x00,(byte)0xB2,(byte)record,0x04,(byte)mFileRecordSize };

        return Arrays.copyOf(mSEHandle.exchangeAPDU(this, cmd),mFileRecordSize);
    }

    /**
     * Returns the number of records in the current selected file
     * @return Number of records [0..n]
     */
    public short getFileNbRecords()
    throws SecureElementException {
        // Check the type of current selected file
        if (mFileNbRecords<0)
            throw new SecureElementException("Incorrect file type");
        return mFileNbRecords;
    }


}
