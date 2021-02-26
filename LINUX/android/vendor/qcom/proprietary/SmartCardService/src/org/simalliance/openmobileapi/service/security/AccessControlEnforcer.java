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
 * Copyright 2012 Giesecke & Devrient GmbH.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package org.simalliance.openmobileapi.service.security;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.Signature;
import android.os.Build;
import android.os.SystemProperties;
import android.util.Log;

import java.io.ByteArrayInputStream;
import java.io.PrintWriter;
import java.security.AccessControlException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.MissingResourceException;
import java.security.cert.Certificate;
import java.security.cert.CertificateEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import org.simalliance.openmobileapi.service.CardException;
import org.simalliance.openmobileapi.service.IChannel;
import org.simalliance.openmobileapi.service.ISmartcardServiceCallback;
import org.simalliance.openmobileapi.service.ITerminal;
import org.simalliance.openmobileapi.service.SmartcardService;
import org.simalliance.openmobileapi.service.security.ChannelAccess.ACCESS;
import org.simalliance.openmobileapi.service.security.ara.AraController;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;
import org.simalliance.openmobileapi.service.security.arf.ArfController;


public class AccessControlEnforcer {

    private PackageManager mPackageManager = null;

    private AraController mAraController = null;
    private boolean mUseAra = true;

    private ArfController mArfController = null;
    private boolean mUseArf = false;

    private AccessRuleCache mAccessRuleCache = null;
    private boolean mRulesRead = false;

    private ITerminal mTerminal = null;

    private ChannelAccess mInitialChannelAccess = new ChannelAccess();
    private boolean mFullAccess = false;

    protected boolean[] mNfcEventFlags = null;

    private final String ACCESS_CONTROL_ENFORCER = "Access Control Enforcer: ";

    public AccessControlEnforcer( ITerminal terminal ) {

        mTerminal = terminal;
        mAccessRuleCache = new AccessRuleCache();
    }

    public PackageManager getPackageManager() {
        return mPackageManager;
    }

    public void setPackageManager(PackageManager packageManager) {
        this.mPackageManager = packageManager;
    }

    public ITerminal getTerminal(){
        return mTerminal;
    }

    public AccessRuleCache getAccessRuleCache(){
        return mAccessRuleCache;
    }

    public static byte[] getDefaultAccessControlAid(){
        return AraController.getAraMAid();
    }

    public synchronized void reset() {
        // Destroy any previous Controler
        // in order to reset the ACE
        Log.i(SmartcardService._TAG, "Reset the ACE for terminal:" + mTerminal.getName());
        mAraController = null;
        mArfController = null;
    }

    public synchronized boolean initialize(boolean loadAtStartup, ISmartcardServiceCallback callback) {
        try {

            boolean status = true;
            String denyMsg = "";
            // allow access to set up access control for a channel
            mInitialChannelAccess.setApduAccess(ChannelAccess.ACCESS.ALLOWED);
            mInitialChannelAccess.setNFCEventAccess(ChannelAccess.ACCESS.ALLOWED);
            mInitialChannelAccess.setAccess(ChannelAccess.ACCESS.ALLOWED, "");

            readSecurityProfile();

            if(!mTerminal.getName().startsWith(SmartcardService._UICC_TERMINAL)) {
                // When SE is not the UICC then it's allowed to grant full access if no
                // rules can be retreived.
                mFullAccess = true;
            }

            // 1 - Let's try to use ARA
            if( mUseAra && mAraController == null)
                mAraController = new AraController(this);

            if( mUseAra && mAraController != null ){
                try {
                    mAraController.initialize(loadAtStartup, callback);
                    // disable other access methods

                    Log.i(SmartcardService._TAG, "ARA applet is used for:" + mTerminal.getName());
                    mUseArf = false;
                    mFullAccess = false;

                } catch( Exception e ) {

                 // ARA cannot be used since we got an exception during initialization
                    mUseAra = false;
                    denyMsg = e.getLocalizedMessage();

                     if( e instanceof MissingResourceException ) {
                         if(mTerminal.getName().startsWith(SmartcardService._UICC_TERMINAL)) {
                             // If the SE is a UICC then a possible explanation could simply
                             // be due to the fact that the UICC is old and doesn't
                             // support logical channel (and is not compliant with GP spec).
                             // in this case we should simply act as if no ARA was available
                             Log.w(SmartcardService._TAG, "Got MissingResourceException: Does the UICC support logical channel?");
                             Log.w(SmartcardService._TAG, "Full message: " +  e.getMessage());
                         } else {
                             // If the SE is not a UICC then this exception means that something
                             // wrong has occured!
                             throw new MissingResourceException( e.getMessage(), "", "");
                         }
                    } else if( mAraController.isNoSuchElement() ) {
                         Log.i(SmartcardService._TAG, "No ARA applet found in: " + mTerminal.getName());
                    } else {
                        // ARA is available but doesn't work properly.
                        // We are going to disable everything per security req.
                        Log.i(SmartcardService._TAG, "AccessControlEnforcer - Problem accessing ARA, Access DENIED. " + e.getLocalizedMessage());

                        // access is denied for any terminal if exception during accessing ARA has any other reason.
                        mUseArf = false;
                        mFullAccess = false;
                        status = false;
                    }
                }
            }

            // 2 - Let's try to use ARF since ARA cannot be used
            if(mUseArf && !mTerminal.getName().startsWith(SmartcardService._UICC_TERMINAL)) {
                Log.i(SmartcardService._TAG, "Disable ARF for terminal: " + mTerminal.getName() + " (ARF is only available for UICC)");
                mUseArf = false; // Arf is only supproted on UICC
            }

            if( mUseArf && mArfController == null)
                mArfController = new ArfController(this);

            if( mUseArf && mArfController != null) {
                try {
                    mArfController.initialize(callback);
                    // disable other access methods
                    Log.i(SmartcardService._TAG, "ARF rules are used for:" + mTerminal.getName());
                    mFullAccess = false;
                } catch( Exception e ) {
                    // ARF cannot be used since we got an exception
                    mUseArf = false;
                    status = false;
                    denyMsg = e.getLocalizedMessage();
                    Log.e(SmartcardService._TAG, e.getMessage() );
                }
            }

            /* 3 - Let's grant full access since neither ARA nor ARF can be used */
            if(mFullAccess) {
                mInitialChannelAccess.setApduAccess(ChannelAccess.ACCESS.ALLOWED);
                mInitialChannelAccess.setNFCEventAccess(ChannelAccess.ACCESS.ALLOWED);
                mInitialChannelAccess.setAccess(ChannelAccess.ACCESS.ALLOWED, "");

                Log.i(SmartcardService._TAG, "Full access granted for:" + mTerminal.getName());
            }

            /* 4 - Let's block everything since neither ARA, ARF or fullaccess can be used */
            if(!mUseArf && !mUseAra && !mFullAccess) {
                mInitialChannelAccess.setApduAccess(ChannelAccess.ACCESS.DENIED);
                mInitialChannelAccess.setNFCEventAccess(ChannelAccess.ACCESS.DENIED);
                mInitialChannelAccess.setAccess(ChannelAccess.ACCESS.DENIED, denyMsg);

                Log.i(SmartcardService._TAG, "Deny any access to:" + mTerminal.getName());
            }
            mRulesRead = status;
            return status;
        } finally {

        }
    }

    public static Certificate decodeCertificate(byte[] certData) throws CertificateException {
        CertificateFactory certFactory = CertificateFactory.getInstance("X.509");
        X509Certificate cert = (X509Certificate) certFactory
                .generateCertificate(new ByteArrayInputStream(certData));
        return cert;
    }

    public synchronized boolean Checkx509Certif (String packageName)
        throws PKCS15Exception, SecureElementException, CertificateException{

        ArrayList<X509Certificate> Uiccx509 = null;
        Certificate[] appCerts = null;
        try {
            Uiccx509 = mArfController.getx509Certif();
        } catch (Throwable exp) {
            throw new AccessControlException(exp.getMessage());
        }
        if(Uiccx509==null) return false;

        try {
            appCerts = getAPPCerts(packageName);
            if (appCerts == null || appCerts.length == 0) {
                throw new AccessControlException("Application certificates are invalid or do not exist.");
            }
        } catch (Throwable exp) {
            throw new AccessControlException(exp.getMessage());
        }
        for (int i = 0; i < appCerts.length; i++) {
            if (Uiccx509.contains(appCerts[i])) {
                 Log.i(SmartcardService._TAG, "Uiccx509.contains(appCerts)");
                 return true;
            } else {
                Log.i(SmartcardService._TAG, "Uiccx509 DOES NOT contains(appCerts)");
                return false;
            }
        }
        return false;
    }

    public synchronized void checkCommand(IChannel channel, byte[] command) {

        ChannelAccess ca = channel.getChannelAccess();
        if (ca == null) {

            throw new AccessControlException(ACCESS_CONTROL_ENFORCER + "Channel access not set");
        }

        String reason = ca.getReason();
        if (reason.length() == 0) {
            reason = "Command not allowed!";
        }

        if (ca.getAccess() != ACCESS.ALLOWED ) {

            throw new AccessControlException(ACCESS_CONTROL_ENFORCER + reason);
        }
        if (ca.isUseApduFilter()) {
            ApduFilter[] accessConditions = ca.getApduFilter();
            if (accessConditions == null || accessConditions.length == 0) {

                throw new AccessControlException(ACCESS_CONTROL_ENFORCER + "Access Rule not available: " + reason);
            }
            for (ApduFilter ac : accessConditions) {
                if (CommandApdu.compareHeaders(command, ac.getMask(), ac.getApdu())) {

                    return;
                }
            }

            throw new AccessControlException(ACCESS_CONTROL_ENFORCER + "Access Rule does not match: " + reason);
        }
        if (ca.getApduAccess() == ChannelAccess.ACCESS.ALLOWED) {

            return;
        } else {

            throw new AccessControlException(ACCESS_CONTROL_ENFORCER + "APDU access NOT allowed" );
        }
    }

    public ChannelAccess setUpChannelAccess(
            byte[] aid,
            String packageName,
            boolean checkRefreshTag,
            ISmartcardServiceCallback callback) {
        ChannelAccess channelAccess = null;

        // check result of channel access during initialization procedure
        if( mInitialChannelAccess.getAccess() == ChannelAccess.ACCESS.DENIED ){
            throw new AccessControlException( ACCESS_CONTROL_ENFORCER + "access denied: " + mInitialChannelAccess.getReason() );
        }
        // this is the new GP Access Control Enforcer implementation
        if( mUseAra || mUseArf ){

            try {
                channelAccess = internal_setUpChannelAccess(aid, packageName, checkRefreshTag, callback);
            } catch( Exception e ) {
                if( e instanceof MissingResourceException ) {
                    throw new MissingResourceException( ACCESS_CONTROL_ENFORCER + e.getMessage(), "", "");
                } else {
                    // access is denied for any terminal if exception during accessing ARA has any other reason.
                    throw new AccessControlException( ACCESS_CONTROL_ENFORCER + "access denied: " + e.getMessage() );
                }
            }
        }

        if( channelAccess == null || // precautionary check
            (channelAccess.getApduAccess() != ChannelAccess.ACCESS.ALLOWED &&
             channelAccess.isUseApduFilter() == false)) {

            if( this.mFullAccess == true ){
                // if full access is set then we reuse the initial channel access,
                // since we got so far it allows everything with a descriptive reason.
                channelAccess = mInitialChannelAccess;
            } else {
                throw new AccessControlException( ACCESS_CONTROL_ENFORCER + "no APDU access allowed!" );
            }
        }

        channelAccess.setPackageName(packageName);

        return channelAccess.clone();
    }

    private synchronized ChannelAccess internal_setUpChannelAccess(byte[] aid, String packageName, boolean checkRefreshTag,
            ISmartcardServiceCallback callback) {

        ChannelAccess channelAccess = new ChannelAccess();
        if (packageName == null || packageName.isEmpty()) {
            throw new AccessControlException("package names must be specified");
        }
        if (aid == null || aid.length == 0) {
            throw new AccessControlException("AID must be specified");
        }
        if (aid.length < 5 || aid.length > 16) {
            throw new AccessControlException("AID has an invalid length");
        }

        try {
            // estimate SHA-1 hash value of the device application's certificate.
            Certificate[] appCerts = getAPPCerts(packageName);

            // APP certificates must be available => otherwise Exception
            if (appCerts == null || appCerts.length == 0) {
                throw new AccessControlException("Application certificates are invalid or do not exist.");
            }

            if (checkRefreshTag)
                updateAccessRuleIfNeed(callback);

            channelAccess = getAccessRule(aid, appCerts, callback );

        } catch (Throwable exp) {


            throw new AccessControlException(exp.getMessage());
        }

        return channelAccess;
    }

    public ChannelAccess getAccessRule( byte[] aid, Certificate[] appCerts, ISmartcardServiceCallback callback  ) throws AccessControlException, CardException, CertificateEncodingException {

        ChannelAccess channelAccess = null;

        // if read all is true get rule from cache.
        if( mRulesRead ){
            // get rules from internal storage
            channelAccess = mAccessRuleCache.findAccessRule( aid, appCerts );
        }

        // if no rule was found return an empty access rule
        // with all access denied.
        if( channelAccess == null ){
            channelAccess = new ChannelAccess();
            channelAccess.setAccess(ChannelAccess.ACCESS.DENIED, "no access rule found!" );
            channelAccess.setApduAccess(ChannelAccess.ACCESS.DENIED);
            channelAccess.setNFCEventAccess(ChannelAccess.ACCESS.DENIED);
        }
        return channelAccess;
    }


    /**
     * Returns Certificate chain for one package.
     *
     * @param packageName
     * @return
     * @throws CertificateException
     * @throws NoSuchAlgorithmException
     * @throws AccessControlException
     * @throws CardException
     */
    public Certificate[] getAPPCerts(String packageName)
             throws CertificateException, NoSuchAlgorithmException, AccessControlException {

         if(packageName == null || packageName.length() == 0)
             throw new AccessControlException("Package Name not defined");

         PackageInfo foundPkgInfo;

         try {
             foundPkgInfo = mPackageManager.getPackageInfo(packageName,
                                                     PackageManager.GET_SIGNATURES);
         } catch (NameNotFoundException ne) {
             throw new AccessControlException("Package does not exist");
         }

         if (foundPkgInfo == null) {
                 throw new AccessControlException("Package does not exist");
             }

         ArrayList<Certificate> appCerts = new ArrayList<Certificate>();

         for (Signature signature : foundPkgInfo.signatures) {
             appCerts.add(decodeCertificate(signature.toByteArray()));
         }
         return appCerts.toArray(new Certificate[appCerts.size()]);
    }

    public static byte[] getAppCertHash(Certificate appCert) throws CertificateEncodingException
    {
        /**
         * Note: This loop is needed as workaround for a bug in Android 2.3.
         * After a failed certificate verification in a previous step the
         * MessageDigest.getInstance("SHA") call will fail with the
         * AlgorithmNotSupported exception. But a second try will normally
         * succeed.
         */
        MessageDigest md = null;
        for (int i = 0; i < 10; i++) {
            try {
                md = MessageDigest.getInstance("SHA");
                break;
            } catch (Exception e) {
            }
        }
        if (md == null) {
            throw new AccessControlException("Hash can not be computed");
        }
        return md.digest(appCert.getEncoded());
    }

    public synchronized boolean[] isNFCEventAllowed(
            byte[] aid,
            String[] packageNames,
            boolean checkRefreshTag,
            ISmartcardServiceCallback callback)
                    throws CardException
    {
        if( mUseAra || mUseArf ){
            return internal_isNFCEventAllowed(aid, packageNames, checkRefreshTag, callback);
        } else {
            // 2012-09-27
            // if ARA and ARF is not available and terminal DOES NOT belong to a UICC -> mFullAccess is true
            // if ARA and ARF is not available and terminal belongs to a UICC -> mFullAccess is false
            boolean[] ret = new boolean[packageNames.length];
            for( int i = 0; i < ret.length; i++ ){
                ret[i] = this.mFullAccess;
            }
            return ret;
        }
    }

    private synchronized boolean[] internal_isNFCEventAllowed(byte[] aid,
               String[] packageNames,
               boolean checkRefreshTag,
               ISmartcardServiceCallback callback)
                       throws CardException
    {
         // the NFC Event Flags boolean array is created and filled in internal_enableAccessConditions.
         mNfcEventFlags = new boolean[packageNames.length];
         int i=0;
         ChannelAccess channelAccess = null;

         if (checkRefreshTag)
             updateAccessRuleIfNeed(callback);

         for( String packageName : packageNames ) {
             // estimate SHA-1 hash value of the device application's certificate.
                Certificate[] appCerts;
                try {
                    appCerts = getAPPCerts(packageName);

                    // APP certificates must be available => otherwise Exception
                    if (appCerts == null || appCerts.length == 0) {
                        throw new AccessControlException("Application certificates are invalid or do not exist.");
                    }

                    channelAccess = getAccessRule(aid, appCerts, callback);
                    mNfcEventFlags[i] = (channelAccess.getNFCEventAccess() == ChannelAccess.ACCESS.ALLOWED);

                } catch (Exception e) {
                    Log.w(SmartcardService._TAG, " Access Rules for NFC: " + e.getLocalizedMessage());
                    mNfcEventFlags[i] = false;
                }
                i++;
         }
        return mNfcEventFlags;
    }

    public synchronized boolean hasCertificate(String packageName)
    {
        // check result of channel access during initialization procedure
        if( mInitialChannelAccess.getAccess() == ChannelAccess.ACCESS.DENIED ){
            Log.e(SmartcardService._TAG, "access denied: " + mInitialChannelAccess.getReason() );
            return false;
        }

        if (packageName == null || packageName.isEmpty()) {
            Log.e(SmartcardService._TAG, "package names must be specified");
            return false;
        }

        if( mUseAra || mUseArf ){
            Certificate[] appCerts;
            try {
                appCerts = getAPPCerts(packageName);
            } catch (Throwable exp) {
                Log.e(SmartcardService._TAG, "cannot get Certs:" + exp.getMessage());
                return false;
            }

            // APP certificates must be available => otherwise Exception
            if (appCerts == null || appCerts.length == 0) {
                Log.e(SmartcardService._TAG, "Application certificates are invalid or do not exist.");
                return false;
            }

            // if read all is true get rule from cache.
            if( mRulesRead ){
                // get rules from internal storage
                return mAccessRuleCache.findCertificates(appCerts);
            }

            Log.e(SmartcardService._TAG, "no access rule found!");
            return false;
        } else {
            // if ARA and ARF is not available and terminal DOES NOT belong to a UICC -> mFullAccess is true
            // if ARA and ARF is not available and terminal belongs to a UICC -> mFullAccess is false
            return this.mFullAccess;
        }
    }


    public void updateAccessRuleIfNeed(ISmartcardServiceCallback callback) {
        if( mUseAra && mAraController != null ){
            try {
                mAraController.initialize(true, callback);
                mUseArf = false;
                mFullAccess = false;
            } catch( Exception e ) {
                Log.e(SmartcardService._TAG, e.getMessage() );
                if( e instanceof MissingResourceException ) {
                    throw new MissingResourceException( e.getMessage(), "", "");
                }
                if( mAraController.isNoSuchElement() ) {
                    throw new AccessControlException ("No ARA applet found in: " + mTerminal.getName());
                }
            }
        } else if( mUseArf && mArfController != null) {
            try {
                mArfController.initialize(callback);
            } catch( Exception e ) {
                Log.e(SmartcardService._TAG, e.getMessage() );
            }
        }
    }

    public void dump(PrintWriter writer, String prefix) {
       writer.println(prefix + SmartcardService._TAG + ":");
       prefix += "  ";

       writer.println(prefix + "mUseArf: " + mUseArf);
       writer.println(prefix + "mUseAra: " + mUseAra);
       writer.println(prefix + "mInitialChannelAccess:");
       writer.println(prefix + "  " + mInitialChannelAccess.toString());
       writer.println();

       /* Dump the access rule cache */
       if(mAccessRuleCache != null) mAccessRuleCache.dump(writer, prefix);
    }

    private void readSecurityProfile() {
        if(!Build.IS_DEBUGGABLE) {
            mUseArf = true;
            mUseAra = true;
            mFullAccess = false; // Per default we don't grant full access.
        } else {
            String level = SystemProperties.get("service.seek", "useara usearf");
            level = SystemProperties.get("persist.service.seek", level);

            if(level.contains("usearf")) mUseArf = true; else mUseArf = false;
            if(level.contains("useara")) mUseAra = true; else mUseAra = false;
            if(level.contains("fullaccess")) mFullAccess = true; else mFullAccess = false;
        }
        Log.i(SmartcardService._TAG, "Allowed ACE mode: ara=" + mUseAra + " arf=" + mUseArf + " fullaccess=" + mFullAccess );
    }

}
