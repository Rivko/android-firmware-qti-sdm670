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
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 *
 * Not a Contribution.
 */

/*
 * Copyright (C) 2011 Deutsche Telekom, A.G.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Contributed by: Giesecke & Devrient GmbH.
 */

package org.simalliance.openmobileapi.service.security.arf.PKCS15;

import java.security.AccessControlException;
import java.util.MissingResourceException;

import org.simalliance.openmobileapi.service.IChannel;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFACMain;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFACRules;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFDIR;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFDODF;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFODF;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFCDF;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFTCF;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import android.os.Build;
import android.os.SystemProperties;
import android.util.Log;
import java.util.ArrayList;
/**
 * Handles PKCS#15 topology
 ***************************************************/
public class PKCS15Handler {

    public static final String TAG = "SmartcardService ACE ARF";

    // AID of the GPAC Applet/ADF
    public static final byte[] GPAC_ARF_AID =
        {(byte)0xA0,0x00,0x00,0x00,0x18,0x47,0x50,0x41,0x43,0x2D,0x31,0x35};
    // AID of the PKCS#15 ADF
    public static final byte[] PKCS15_AID =
       { (byte)0xA0,0x00,0x00,0x00,0x63,0x50,0x4B,0x43,0x53,0x2D,0x31,0x35 };

    // AIDs of "Access Control Rules" containers
    public static final byte[][] CONTAINER_AIDS= {
        PKCS15_AID,
        GPAC_ARF_AID,
        null
    };

    // Handle to "Secure Element"
    private SecureElement mSEHandle;
    // "Secure Element" label
    private String mSELabel=null;

    // Handle to "Logical Channel" allocated by the SE
    private IChannel mArfChannel=null;

    // "EF Access Control Main" object
    private EFACMain mACMainObject=null;
    // EF AC Rules object
    private EFACRules mACRulesObject=null;

    private byte[] mPkcs15Path = null;
    private byte[] mACMainPath = null;
    EFTCF TCFObject = null;
    // SIM Allowed modes:
    private boolean mSimIoAllowed;
    private boolean mSimAllianceAllowed;
    private boolean mX509found = false;
    private boolean mACMFfound = true;
    public ArrayList<X509Certificate> CertifList = null;

    /**
     * Updates "Access Control Rules"
     */
    private boolean updateACRules()
        throws Exception, PKCS15Exception, SecureElementException
    {
        byte[] ACRulesPath=null;
        if (!mACMFfound) {
            mSEHandle.resetAccessRules();
            mACMainPath  = null;
            if (mArfChannel!=null)
                mSEHandle.closeArfChannel();
            this.initACEntryPoint();
        }
        try {
            ACRulesPath=mACMainObject.analyseFile();
            mACMFfound = true;
        } catch (Exception e) {
            Log.i(TAG, "ACMF Not found !");
            mACMainObject=null;
            mSEHandle.resetAccessRules();
            mACMFfound = false;
            throw e;
        }
        // Check if rules must be updated
        if (ACRulesPath != null) {
            Log.i(TAG, "Access Rules needs to be updated...");
            if (mACRulesObject==null) {
                mACRulesObject=new EFACRules(mSEHandle);
            }
            mSEHandle.clearAccessRuleCache();
            mACMainPath  = null;
            if (mArfChannel!=null)
                mSEHandle.closeArfChannel();

            this.initACEntryPoint();

            try {
                mACRulesObject.analyseFile(ACRulesPath);
            } catch (Exception e) {
                Log.i(TAG, "Exception: clear access rule cache and refresh tag");
                mSEHandle.resetAccessRules();
                throw e;
            }
            return true;
        } else {
            Log.i(TAG, "Refresh Tag has not been changed...");
            return false;
        }
    }

    public ArrayList<X509Certificate> getCertif ()
        throws PKCS15Exception, SecureElementException, CertificateException {
            if (mX509found) {
                Log.i(TAG, "returning CertifList");
                return CertifList;
            } else {
                throw new PKCS15Exception("Cannot get x509 certificates via EF Dir");
            }
    }
    /**
     * Initializes "Access Control" entry point [ACMain]
     */
    private void initACEntryPoint()
        throws PKCS15Exception, SecureElementException, CertificateException
    {

        byte[] DODFPath=null;

        readAllowedSimMode();

        for(int ind=0;ind<CONTAINER_AIDS.length;ind++) {
            if (selectACRulesContainer(CONTAINER_AIDS[ind])) {

                byte[] acMainPath = null;
                if( mACMainPath==null){
                    EFODF ODFObject=new EFODF(mSEHandle);
                    DODFPath=ODFObject.analyseFile(mPkcs15Path);
                    EFDODF DODFObject=new EFDODF(mSEHandle);
                    acMainPath=DODFObject.analyseFile(DODFPath);

                    EFCDF CDFObject=new EFCDF(mSEHandle);
                    byte[] mCDFPath = ODFObject.getCDFPath();
                    ArrayList<byte[]> x509buff = CDFObject.checkCDF(mCDFPath);
                    if (x509buff != null) {
                        TCFObject=new EFTCF(mSEHandle);
                        CertifList = TCFObject.analyseFile(x509buff);
                        mX509found = true;
                    }

                    mACMainPath = acMainPath;
                } else {
                    if( mPkcs15Path != null ) {
                        acMainPath = new byte[mPkcs15Path.length + mACMainPath.length];
                        System.arraycopy(mPkcs15Path, 0, acMainPath, 0, mPkcs15Path.length);
                        System.arraycopy(mACMainPath, 0, acMainPath, mPkcs15Path.length, mACMainPath.length );

                    } else {
                        acMainPath = mACMainPath;
                    }
                }
                mACMainObject=new EFACMain(mSEHandle,acMainPath);
                break;
            }
        }
    }

    /**
     * Selects "Access Control Rules" container
     * @param AID Identification of the GPAC Applet/PKCS#15 ADF;
     *                    <code>null</code> for EF_DIR file
     * @return <code>true</code> when container is active;
     *             <code>false</code> otherwise
     */
    private boolean selectACRulesContainer(byte[] aid)
        throws PKCS15Exception,SecureElementException
    {
        boolean isActiveContainer=true;

        if (aid==null) {
            mArfChannel = null;

            // some devices use logical channels to access filesystem directly. This is done with an empty byte array.
            // if open logical channel does not work, last fallback is using SIM_IO (AT-CRSM).
            // 2012-11-08
            if(mSimAllianceAllowed)
                mArfChannel = mSEHandle.openLogicalArfChannel(new byte[]{});

            if (mArfChannel != null) {
                Log.i(TAG, "Logical channels are used to access to PKC15");
                mSEHandle.setSeInterface(SecureElement.SIM_ALLIANCE);
            }
            else {
                if(mSimIoAllowed) {
                    // Since ARF gets only active if the terminal belongs to a SIM/UICC
                    // we have to switch to SIM_IO
                    Log.i(TAG, "Fall back into ARF with SIM_IO");
                    mSEHandle.setSeInterface(SecureElement.SIM_IO);
                }
                else {
                    Log.i(TAG, "SIM IO is not allowed: cannot access to ARF");
                    isActiveContainer = false;
                }
            }

            if(isActiveContainer && mPkcs15Path == null ) { // estimate PKCS15 path only if it is not known already.
                mACMainPath = null;
                // EF_DIR parsing
                EFDIR DIRObject=new EFDIR(mSEHandle);
                mPkcs15Path=DIRObject.lookupAID(PKCS15_AID);
                if( mPkcs15Path == null ) {
                    Log.i(TAG, "Cannot use ARF: cannot select PKCS#15 directory via EF Dir");
                    // TODO: Here it might be possible to set a default path
                    // so that SIMs without EF-Dir could be supported.
                    throw new PKCS15Exception("Cannot select PKCS#15 directory via EF Dir");
                }
            }
        }
        // if an AID is given use logical channel.
        else {
            if(!mSimAllianceAllowed) {
                isActiveContainer = false;
            }
            else {
                // Selection of Applet/ADF via AID is done via SCAPI and logical Channels
                mSEHandle.setSeInterface(SecureElement.SIM_ALLIANCE);
                if ((mArfChannel=mSEHandle.openLogicalArfChannel(aid))==null) {
                    isActiveContainer=false;
                    Log.w(TAG,"GPAC/PKCS#15 ADF not found!!");
                }
                else {
                    // ARF is selected via AID.
                    if( mPkcs15Path != null ){ // if there is a change from path selection to AID selection, then reset AC Main path.
                        mACMainPath = null;
                    }
                    mPkcs15Path = null; // selection is done via AID
                }
            }
        }
        return isActiveContainer;
    }

    /**
     * Constructor
     * @param handle Handle to "Secure Element"
     */
    public PKCS15Handler(SecureElement handle) {
        mSEHandle=handle;
    }

    /**
     * Loads "Access Control Rules" from container
     * @return false if access rules where not read due to constant refresh tag.
     */
    public synchronized boolean loadAccessControlRules(String secureElement) {
        mSELabel=secureElement;
        Log.i(TAG,"- Loading "+mSELabel+" rules...");
        try {
            initACEntryPoint();
            return updateACRules();
        } catch (Exception e) {
            if( e instanceof MissingResourceException ){
                // this indicates that no channel is left for accessing the SE element
                throw (MissingResourceException)e;
            }
            Log.e(TAG,mSELabel+" rules not correctly initialized! " + e.getLocalizedMessage());
            throw new AccessControlException(e.getLocalizedMessage());
        } finally {
            // Close previously opened channel
            if (mArfChannel!=null)
                mSEHandle.closeArfChannel();
        }
    }

    /**
     * Read security allowed sim mode
     */
    private void readAllowedSimMode() {
        if(!Build.IS_DEBUGGABLE) {
            mSimIoAllowed = true;
            mSimAllianceAllowed = true;
        } else {
            String level = SystemProperties.get("service.seek.arf", "simio simalliance");
            level = SystemProperties.get("persist.service.seek.arf", level);

            if(level.contains("simio")) mSimIoAllowed = true; else mSimIoAllowed = false;
            if(level.contains("simalliance")) mSimAllianceAllowed = true; else mSimAllianceAllowed = false;
        }

        Log.i(TAG, "Allowed SIM mode: SimIo=" + mSimIoAllowed + " SimAlliance=" + mSimAllianceAllowed );
    }
}
