/*
 * Copyright 2012 Giesecke & Devrient GmbH.
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

package org.simalliance.openmobileapi.service.security;


public class ChannelAccess {

    public enum ACCESS {
        ALLOWED, DENIED, UNDEFINED;
    }

    protected String CHANNEL_ACCESS_TAG = "ChannelAccess";

    protected String mPackageName = "";

    protected ACCESS mAccess = ACCESS.UNDEFINED;

    protected ACCESS mApduAccess = ACCESS.UNDEFINED;

    protected boolean mUseApduFilter = false;

    protected int mCallingPid = 0;

    protected String mReason = "no access by default";

    protected ACCESS mNFCEventAccess = ACCESS.UNDEFINED;

    protected ApduFilter[] mApduFilter = null;

    public ChannelAccess clone(){
        ChannelAccess ca = new ChannelAccess();
        ca.setAccess(this.mAccess, this.mReason);
        ca.setPackageName( this.mPackageName);
        ca.setApduAccess(this.mApduAccess);
        ca.setCallingPid(this.mCallingPid);
        ca.setNFCEventAccess(this.mNFCEventAccess);
        ca.setUseApduFilter(this.mUseApduFilter);
        if( this.mApduFilter != null ) {
            ApduFilter[] apduFilter = new ApduFilter[this.mApduFilter.length];
            int i = 0;
            for( ApduFilter filter : mApduFilter ){
                apduFilter[i++] = filter.clone();
            }
               ca.setApduFilter(apduFilter);
        } else {
            ca.setApduFilter(null);
        }
        return ca;
    }

    public String getPackageName(){
        return mPackageName;
    }

    public void setPackageName( String name ){
        this.mPackageName = name;
    }

    public ACCESS getApduAccess() {
        return mApduAccess;
    }

    public void setApduAccess(ACCESS apduAccess) {
        this.mApduAccess = apduAccess;
    }


    public ACCESS getAccess() {
        return mAccess;
    }

    public void setAccess(ACCESS access, String reason) {
        this.mAccess = access;
        this.mReason = reason;
    }

    public boolean isUseApduFilter() {
        return mUseApduFilter;
    }

    public void setUseApduFilter(boolean useApduFilter) {
        this.mUseApduFilter = useApduFilter;
    }

    public void setCallingPid(int callingPid) {
        this.mCallingPid = callingPid;
    }

    public int getCallingPid() {
        return mCallingPid;
    }

    public String getReason() {
        return mReason;
    }
    public ApduFilter[] getApduFilter() {
        return mApduFilter;
    }

    public void setApduFilter(ApduFilter[] accessConditions) {
        mApduFilter = accessConditions;
    }
    public ACCESS getNFCEventAccess() {
        return mNFCEventAccess;
    }

    public void setNFCEventAccess(ACCESS access) {
        this.mNFCEventAccess = access;
    }

    @Override
    public String toString(){
        StringBuilder sb = new StringBuilder();
        sb.append(this.getClass().getName());
        sb.append("\n [mPackageName=");
        sb.append(mPackageName);
        sb.append(", mAccess=");
        sb.append(mAccess);
        sb.append(", mApduAccess=");
        sb.append(mApduAccess);
        sb.append(", mUseApduFilter=");
        sb.append(mUseApduFilter);
        sb.append(", mApduFilter=");
        if( mApduFilter != null ){
            for( ApduFilter f : mApduFilter ){
                sb.append(f.toString());
                sb.append(" ");
            }
        } else {
            sb.append("null");
        }
        sb.append(", mCallingPid=");
        sb.append(mCallingPid);
        sb.append(", mReason=");
        sb.append(mReason);
        sb.append(", mNFCEventAllowed=");
        sb.append(mNFCEventAccess);
        sb.append("]\n");

        return sb.toString();

    }
}
