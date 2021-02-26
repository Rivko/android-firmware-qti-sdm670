/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Guid {
  public final int data1;
  public final short data2;
  public final short data3;
  public final byte[] data4;

  public Guid(int data1, short data2, short data3, byte[] data4) {
    this.data1 = data1;
    this.data2 = data2;
    this.data3 = data3;
    this.data4 = data4;
  }

  public byte[] toByteArray() {
    ByteBuffer byteBuffer = ByteBuffer.allocate(16);
    byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
    byteBuffer.putInt(data1);
    byteBuffer.putShort(data2);
    byteBuffer.putShort(data3);
    byteBuffer.put(data4);
    return byteBuffer.array();
  }
}
