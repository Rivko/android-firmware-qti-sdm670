/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.server.wigigapp;

import android.content.Context;
import android.preference.Preference;
import android.view.View;
import android.widget.TextView;

public class P2pPeer extends Preference {

    public enum PeerStatus {PeerAvailable, PeerConnected, PeerFailed, PeerInvited, PeerUnavailable}

    private String mName;
    private String mAddress;
    private PeerStatus mStatus;

    private boolean showSummary = true;
    private TextView mSummaryView;

    P2pPeer(Context context, String name, String addr, PeerStatus status) {
        super(context);
        mName = name;
        mAddress = addr;
        mStatus = status;
        refresh();
    }

    public String getName() {
        return mName;
    }

    public String getAddress() {
        return mAddress;
    }

    public PeerStatus getStatus() {
        return mStatus;
    }

    @Override
    protected void onBindView(View view) {
        super.onBindView(view);
        updateIcon();

        mSummaryView = new TextView(getContext());
        mSummaryView.setVisibility(showSummary ? View.VISIBLE : View.GONE);

        notifyChanged();
    }

    protected void updateIcon() {
        // TODO: Choose the icon for P2P
        setIcon(R.drawable.ic_wigig_lock4);
    }

    @Override
    public int compareTo(Preference preference) {
        if (!(preference instanceof P2pPeer)) {
            return 1;
        }
        P2pPeer other = (P2pPeer) preference;
        // Show connected peers first
        if (mStatus == PeerStatus.PeerConnected && !(other.mStatus == PeerStatus.PeerConnected))
            return -1;
        if (!(mStatus == PeerStatus.PeerConnected) && other.mStatus == PeerStatus.PeerConnected)
            return 1;
        // Then show invited
        if (mStatus == PeerStatus.PeerInvited && !(other.mStatus == PeerStatus.PeerInvited))
            return -1;
        if (!(mStatus == PeerStatus.PeerInvited) && other.mStatus == PeerStatus.PeerInvited)
            return 1;
        // Finally sort by name.
        return mName.compareToIgnoreCase(other.mName);
    }

    @Override
    public boolean equals(Object other) {
        if (!(other instanceof P2pPeer)) return false;
        return (this.compareTo((P2pPeer) other) == 0);
    }

    @Override
    public int hashCode() {
        int result = mName.hashCode();
        return result;
    }

    /*
     Shows or Hides the Summary of an P2P peer.
    */
    public void setShowSummary(boolean showSummary) {
        this.showSummary = showSummary;
        if (mSummaryView != null) {
            mSummaryView.setVisibility(showSummary ? View.VISIBLE : View.GONE);
        } // otherwise, will be handled in onBindView.
    }

    /*
     Updates the title and summary; may indirectly call notifyChanged().
     */
    private void refresh() {
        setTitle(mName);
        final Context context = getContext();
        updateIcon();

        // Force new summary
        setSummary(null);

        // Update to new summary
        StringBuilder summary = new StringBuilder();

        switch (mStatus) {
            case PeerConnected:
                summary.append(context.getString(R.string.wfd_peer_connected));
                break;
            case PeerAvailable:
                summary.append(context.getString(R.string.wfd_peer_available));
                break;
            case PeerInvited:
                summary.append(context.getString(R.string.wfd_peer_invited));
                break;
            case PeerUnavailable:
                summary.append(context.getString(R.string.wfd_peer_not_available));
                break;
            case PeerFailed:
                summary.append(context.getString(R.string.wfd_peer_failed));
                break;
            default:
                break;
        }

        if (summary.length() > 0) {
            setSummary(summary.toString());
            setShowSummary(true);
        } else {
            setShowSummary(false);
        }
    }
}
