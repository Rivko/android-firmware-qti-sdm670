/*
    Copyright (c) 2014, The Linux Foundation. All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials provided
          with the distribution.
        * Neither the name of The Linux Foundation nor the names of its
          contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package org.simalliance.openmobileapi.service.security.gpac.dataobjects;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Arrays;

import android.util.Log;

public class CLF_FILTER_DO extends BerTlv {

    private String CLF_FILTER_TAG = "CLF_FILTER";

    public final static int _TAG = 0xFE;
    public final static int _TAG_FILTER_ENTRY = 0xA1;
    public final static int _TAG_AID_RANGE = 0xC2;
    public final static int _TAG_AID_MASK = 0xC3;
    public final static int _TAG_ROUTING_MODE = 0xB2;
    public final static int _TAG_FILTER_COND = 0xD2;
    public final static int _TAG_PERMISSION = 0xE3;

    public final static byte[] CLF_FILTER_AID = new byte[] {
        (byte)0x89, (byte)0x14, (byte)0x80, (byte)0xFF, (byte)0x00, (byte)0x56, (byte)0x5A, (byte)0x57,
        (byte)0x5F, (byte)0x52, (byte)0x4F, (byte)0x43, (byte)0x4B, (byte)0x53, (byte)0x00, (byte)0x01
    };

    private byte[] mAidRange = null;
    private byte[] mAidMask = null;
    private byte mRoutingMode = 0x07;
    private byte mFilterCond = (byte)0xF1;
    private byte mPermission = 0x00;

    public CLF_FILTER_DO(byte[] rawData, int valueIndex, int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);
    }

    @Override
    public String toString(){
        StringBuilder b = new StringBuilder();
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        b.append("CLF_FILTER_DO: ");
        try {
            this.build(out);
            b.append(BerTlv.toHex(out.toByteArray()));
        } catch (Exception e ){
            b.append(e.getLocalizedMessage());
        }
        return b.toString();
    }

    public byte[] getAidRange(){
        return mAidRange;
    }

    public byte[] getAidMask(){
        return mAidMask;
    }

    public byte getRoutingMode(){
        return mRoutingMode;
    }

    public byte getFilterCond(){
        return mFilterCond;
    }

    public byte getPermission(){
        return mPermission;
    }

    @Override
    public void interpret()
        throws ParserException {

        mAidRange = null;
        mAidMask = null;

        byte[] data = getRawData();
        int index = getValueIndex();

        if( index + getValueLength() > data.length){
            throw new ParserException( "Not enough data for CLF-FILTER-DO!");
        }

        BerTlv temp = BerTlv.decode(data, index);
        if( temp.getTag() == _TAG_FILTER_ENTRY ) {
            index = temp.getValueIndex();
        } else {
            throw new ParserException( "Missing FILTER-ENTRY-DO");
        }

        do {
            temp = BerTlv.decode(data, index);
            if( index + temp.getValueLength() > data.length){
                throw new ParserException( "Not enough data during parsing CLF-FILTER-DO");
            }

            if( temp.getTag() == _TAG_AID_RANGE ) {
                mAidRange = new byte[temp.getValueLength()];
                System.arraycopy(data, temp.getValueIndex(), mAidRange, 0, temp.getValueLength());
            } else if( temp.getTag() == _TAG_AID_MASK ) {
                mAidMask = new byte[temp.getValueLength()];
                System.arraycopy(data, temp.getValueIndex(), mAidMask, 0, temp.getValueLength());
            } else if( temp.getTag() == _TAG_ROUTING_MODE ) {
                mRoutingMode = data[temp.getValueIndex()];
            } else if( temp.getTag() == _TAG_FILTER_COND ) {
                mFilterCond = data[temp.getValueIndex()];
            } else if( temp.getTag() == _TAG_PERMISSION ) {
                mPermission = data[temp.getValueIndex()];
            } else {
                // uncomment following line if a more restrictive
                // behavior is necessary.
                //throw new ParserException("Invalid DO in CLF-FILTER-DO!");
            }
            index = temp.getValueIndex() + temp.getValueLength();
        } while( getValueIndex() + getValueLength() > index );

        if (mAidRange == null) {
            throw new ParserException( "No AID Range for CLF-FILTER-DO!");
        }
        if (mAidMask == null) {
            throw new ParserException( "No AID Mask for CLF-FILTER-DO!");
        }
    }

    @Override
    public void build(ByteArrayOutputStream stream)
        throws DO_Exception {

        //TODO extended length
        byte filterEntryLength = (byte)(2 + mAidRange.length + 2 + mAidMask.length + 9);
        byte clfFilterLength = (byte)(filterEntryLength + 2);

        stream.write(getTag());
        stream.write(clfFilterLength);
        stream.write(_TAG_FILTER_ENTRY);
        stream.write(filterEntryLength);
        stream.write(_TAG_AID_RANGE);
        stream.write(mAidRange.length);
        try {
            stream.write(mAidRange);
        } catch( IOException ioe ){
            throw new DO_Exception("AID Range could not be written!");
        }
        stream.write(_TAG_AID_MASK);
        stream.write(mAidMask.length);
        try {
            stream.write(mAidMask);
        } catch( IOException ioe ){
            throw new DO_Exception("AID Mask could not be written!");
        }
        stream.write(_TAG_ROUTING_MODE);
        stream.write((byte)0x01);
        stream.write(mRoutingMode);
        stream.write(_TAG_FILTER_COND);
        stream.write((byte)0x01);
        stream.write(mFilterCond);
        stream.write(_TAG_PERMISSION);
        stream.write((byte)0x01);
        stream.write(mPermission);
    }
}
