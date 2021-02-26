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

package org.simalliance.openmobileapi.service.security.arf.PKCS15;

import org.simalliance.openmobileapi.internal.ByteArrayConverter;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFCDF;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;
import java.security.cert.X509Certificate;
import java.security.cert.CertificateEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.Certificate;
import android.util.Log;
import java.util.Arrays;
import java.util.ArrayList;
import java.io.ByteArrayInputStream;

public class EFTCF extends EF {

    public static final String TAG = "SmartcardService - EFTCF";
    // DT ID for EF_TCF file
    public static final byte[] EFTCF_PATH = { 0x20,0x03 };
    public ArrayList<X509Certificate> x509certificates = new ArrayList<X509Certificate>();
    public ArrayList<byte[]> couples = new ArrayList<byte[]>();
    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     */
    public EFTCF(SecureElement handle) throws PKCS15Exception,SecureElementException, CertificateException {
        super(handle);
    }

    public ArrayList<X509Certificate> analyseFile(ArrayList<byte[]> x509buff)  throws PKCS15Exception,SecureElementException, CertificateException {
        couples = x509buff;
        byte[] buff = null;
        byte[] TCF_File = null;
        int Startbuff;
        int Sizebuff;
        byte[] path = null;
        Certificate UiccCert;
        Log.i(TAG,"Analysing EF_TCF...");

        for (int i = 0; i < couples.size(); i++) {

            Startbuff = bytesToInt(couples.get(i)[0],couples.get(i)[1]);
            Sizebuff = bytesToInt(couples.get(i)[2],couples.get(i)[3]);
            path = new byte[]{ couples.get(i)[4], couples.get(i)[5]};
            if (path == null) {
                path = EFTCF_PATH;
            }
            if (selectFile(path)!= APDU_SUCCESS) {
                Log.e(TAG, "EF_TCF not found with path : " + ByteArrayConverter.byteArrayToHexString(path));
                return null;
            }

            TCF_File = readBinary(0,Util.END);

            /* Handle Startbuff and/or Sizebuff == zero */
            if (Sizebuff == 0) {
                Sizebuff = TCF_File.length - Startbuff;
                Log.v(TAG, "Sizebuff not provided from EF-CDF - assume whole file.");
            }

            UiccCert = decodeCertificate (Arrays.copyOfRange(TCF_File, Startbuff,Startbuff + Sizebuff));
        }
        if (x509certificates.size()>0) {
            return x509certificates;
        } else {
            Log.i(TAG, "EF_TCF returning x509NULL");
            return null;
        }
    }

    public Certificate decodeCertificate(byte[] certData) throws CertificateException {
        CertificateFactory certFactory = CertificateFactory.getInstance("X.509");
        X509Certificate cert = (X509Certificate) certFactory
                .generateCertificate(new ByteArrayInputStream(certData));
        x509certificates.add(cert);
        return cert;
    }

    public static String byteArrayToHex(byte[] a) {
       StringBuilder sb = new StringBuilder(a.length * 2);
       for(byte b: a)
          sb.append(String.format("%02x", b & 0xff));
       return sb.toString();
    }

    private int bytesToInt(byte a, byte b) {
        String ab = String.format("%02X", a) + String.format("%02X", b);
        return Integer.parseInt(ab, 16);
    }

}
