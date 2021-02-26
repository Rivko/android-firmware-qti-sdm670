/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

class TcpCommand {
    static final String TAG = "TcpCommand";

    public static final int TYPE_REQUEST_CONNECTION = 0;

    private final int type; // byte, 1 byte
    private final int length; // byte, 1 byte
    private final byte[] value; // length bytes

    public TcpCommand(int type, int length, byte[] value) {
        this.type = type;
        this.length = length;
        this.value = value;
    }

    public TcpCommand(byte[] buf) throws IOException {
        this(new ByteArrayInputStream(buf));
    }

    public TcpCommand(InputStream inputStream) throws IOException {
        DataInputStream input = new DataInputStream(inputStream);
        type = input.readUnsignedByte();
        length = input.readUnsignedByte();
        value = new byte[length];
        readAllBytes(input, value);
    }

    private void readAllBytes(InputStream input, byte[] buffer) throws IOException {
        int rest = buffer.length;
        while (rest != 0) {
            int size = input.read(buffer, buffer.length - rest, rest);
            if (size == -1) {
                Log.e(TAG, "Reach EOS without enough contents");
                throw new EOFException();
            }
            rest -= size;
        }
    }

    public int getType() {
        return type;
    }

    public int getLength() {
        return length;
    }

    public byte[] getValue() {
        return value;
    }

    public byte[] toBuffer() {
        ByteBuffer buf  = ByteBuffer.allocate(1 + 2 + value.length);
        buf.put((byte)type);
        buf.put((byte)length);
        buf.put(value);
        return buf.array();
    }

    public static TcpCommand newConnectoinRequestCommand(int port) {
        ByteBuffer buf = ByteBuffer.allocate(2);
        buf.putShort((short)port);
        return new TcpCommand(TYPE_REQUEST_CONNECTION, 2, buf.array());
    }

    public String toString() {
        return "TcpCommand type: " + type + ", length: " + length
                + ", value: " + Arrays.toString(value);
    }
}
