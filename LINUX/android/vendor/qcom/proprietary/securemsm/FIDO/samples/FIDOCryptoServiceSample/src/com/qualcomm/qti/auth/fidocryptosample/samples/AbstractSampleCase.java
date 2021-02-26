/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.samples;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import java.util.Map;
import java.util.HashMap;

import com.qualcomm.qti.auth.fidocryptosample.Guid;
import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoExtension;

public abstract class AbstractSampleCase {

  public static final short TAG_UAFV1_DEREGISTER_CMD = 0x3404;
  public static final short TAG_UAFV1_DEREGISTER_CMD_RESPONSE = 0x3604;
  public static final short TAG_UAFV1_GETINFO_CMD = 0x3401;
  public static final short TAG_UAFV1_GETINFO_CMD_RESPONSE = 0x3601;
  public static final short TAG_UAFV1_OPEN_SETTINGS_CMD = 0x3406;
  public static final short TAG_UAFV1_OPEN_SETTINGS_CMD_RESPONSE = 0x3606;
  public static final short TAG_UAFV1_REGISTER_CMD = 0x3402;
  public static final short TAG_UAFV1_REGISTER_CMD_RESPONSE = 0x3602;
  public static final short TAG_UAFV1_SIGN_CMD = 0x3403;
  public static final short TAG_UAFV1_SIGN_CMD_RESPONSE = 0x3603;
  public static final byte UAF_CMD_STATUS_OK = 0x00;
  public static final short TAG_AAID = 0x2E0B;
  public static final short TAG_API_VERSION = 0x280E;
  public static final short TAG_APPID = 0x2804;
  public static final short TAG_ASSERTION_SCHEME = 0x280A;
  public static final short TAG_ATTESTATION_TYPE = 0x2807;
  public static final short TAG_AUTHENTICATOR_ASSERTION = 0x280F;
  public static final short TAG_AUTHENTICATOR_INDEX = 0x280D;
  public static final short TAG_AUTHENTICATOR_INFO = 0x3811;
  public static final short TAG_AUTHENTICATOR_METADATA = 0x2809;
  public static final short TAG_KEYHANDLE = 0x2801;
  public static final short TAG_KEYHANDLE_ACCESS_TOKEN = 0x2805;
  public static final short TAG_STATUS_CODE = 0x2808;
  public static final short TAG_SUPPORTED_EXTENSION_ID = 0x2812;
  public static final short TAG_TC_DISPLAY_CONTENT_TYPE = 0x280C;
  public static final short TAG_TC_DISPLAY_PNG_CHARACTERISTICS = 0x280B;
  public static final short TAG_TRANSACTION_CONTENT = 0x2810;
  public static final short TAG_USERNAME = 0x2806;
  public static final short TAG_USERNAME_AND_KEYHANDLE = 0x3802;
  public static final short TAG_USERVERIFY_TOKEN = 0x2803;
  public static final short TAG_TRANSACTION_CONFIRMATION = 0x38F4;
  public static final short TAG_ASSERTION_INFO = 0x2E0E;
  public static final short TAG_ATTESTATION_BASIC_FULL = 0x3E07;
  public static final short TAG_ATTESTATION_BASIC_SURROGATE = 0x3E08;
  public static final short TAG_ATTESTATION_CERT = 0x2E05;
  public static final short TAG_AUTHENTICATOR_NONCE = 0x2E0F;
  public static final short TAG_COUNTERS = 0x2E0D;
  public static final short TAG_EXTENSION_CRITICAL = 0x3E11;
  public static final short TAG_EXTENSION_DATA = 0x2E14;
  public static final short TAG_EXTENSION_ID = 0x2E13;
  public static final short TAG_EXTENSION_NON_CRITICAL = 0x3E12;
  public static final short TAG_FINAL_CHALLENGE = 0x2E0A;
  public static final short TAG_KEYID = 0x2E09;
  public static final short TAG_PUB_KEY = 0x2E0C;
  public static final short TAG_SIGNATURE = 0x2E06;
  public static final short TAG_TRANSACTION_CONTENT_HASH = 0x2E10;
  public static final short TAG_UAFV1_AUTH_ASSERTION = 0x3E02;
  public static final short TAG_UAFV1_KRD = 0x3E03;
  public static final short TAG_UAFV1_REG_ASSERTION = 0x3E01;
  public static final short TAG_UAFV1_SIGNED_DATA = 0x3E04;

  public static final String APP_ID_STR = "https://myUrl";
  //public static final String APP_ID_STR = "https://myUrl.com/fJYbXVZgZCYyzualfOkwEiXmtryWMUwtqkGGnhTRmGGCNTJQQMVMkWKwcVrticMzTsOmLCFxidXZEvloJtSNnHQqgRXkJACixnASkJMOXrlTWIwypaADDhNitqVSkZCTSXrTkrAiAaDYxnaxFsQLoNbNDkbQHbAuzPySUzXyRcYfaaYoHkjiRURqBrppDRNvcSIVbwSgduCWYIdgAmilSKMChQDYLmnsjenMOfcmUeFQKLxBZzetPjhBQUXuIXNAqSBSBxIIKrdYkIInZTMKKoiPPspBCyQmtFnUOnDgkRQaYDusrfHHiPZJWuViPxgihspOeNsnjDUgwNiUUCOYvSlMGPVAfIcRnUpajWgpgCQSBIFYgVGkzCvipmfYmdSQHRCFnuWuozifyhAqjOlRtkxkwayYGjGwWmOKdedrBInnBcCheFGPCNFDiTiLDHqfkzioBcGPAPZqVanBDnuAzlmryVFhoqEJTIDtavhXWeJMP";
  public static final byte[] APP_ID = APP_ID_STR.getBytes();
  public static final String TRANSACTION_CONTENT_STR = "I will buy it for 5$";
  public static final String TRANSACTION_CONTENT_PNG = "/data/local/tmp/content.png";

  public static byte getByte(int num, int i) {
    ByteBuffer buffer = ByteBuffer.allocate(4);
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    buffer.putInt(num);
    return buffer.get(i);
  }

  public static byte getByte(short num, int i) {
    ByteBuffer buffer = ByteBuffer.allocate(2);
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    buffer.putShort(num);
    return buffer.get(i);
  }

  static final byte[] good_cookie = new Guid(0x48b07a18, (short) 0xa142,
      (short) 0x4517, new byte[] { (byte) 0xb0, 0x41, 0x5e, (byte) 0xb4, 0x26,
          0x47, 0x2f, (byte) 0xf6 }).toByteArray();

  public static void writeByte(ByteBuffer byteBuffer, short tag, byte value) {
    byteBuffer.putShort(tag);
    byteBuffer.putShort((short) 1);
    byteBuffer.put(value);
  }

  public static void writeByteArray(ByteBuffer byteBuffer, short tag,
      byte[] value) {
    if (value != null) {
      byteBuffer.putShort(tag);
      byteBuffer.putShort((short) value.length);
      byteBuffer.put(value);
    }
  }

  public static void writeShort(ByteBuffer byteBuffer, short tag, short value) {
    byteBuffer.putShort(tag);
    byteBuffer.putShort((short) 2);
    byteBuffer.putShort(value);
  }

  public static final Map useTLocExParam = new HashMap<String, byte[]> ();
  static {
    useTLocExParam.put(FidoCryptoExtension.TRUSTED_LOCATION_KEY, new byte[] {1});
  }
}
