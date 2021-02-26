/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import com.qti.debugreport.*;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.LinkedHashMap;
import java.io.PrintWriter;
import java.lang.StringBuilder;

import android.app.Activity;
import android.app.PendingIntent;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.text.TextUtils;

import android.widget.Button;
import android.widget.TextView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.AdapterView;
import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.view.View;
import android.view.LayoutInflater;
import android.graphics.Color;
import android.location.Location;

import com.qti.location.sdk.IZatManager;
import com.qti.location.sdk.IZatDebugReportingService;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.BadParcelableException;


public class DebugReportActivity extends Activity {

    private static String TAG = "DebugReportActivity";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private IZatManager mIzatMgr;
    private IZatDebugReportingService mDebugReportingService;
    private DebugReportCallback mDebugreportCb;
    private static final Object sReportLogLock = new Object();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "oncreate");
        setContentView(R.layout.activity_debugreport);
        doInit();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.v(TAG, "ondestroy");

        if (mDebugreportCb != null) {
            mDebugReportingService.deregisterForDebugReports(mDebugreportCb);
            mDebugreportCb.saveReport();
            mDebugreportCb = null;
        }

        mIzatMgr.disconnectDebugReportingService(mDebugReportingService);
    }

    private void doInit() {
        mIzatMgr = IZatManager.getInstance(getApplicationContext());
        mDebugReportingService = mIzatMgr.connectDebugReportingService();

        String version = mIzatMgr.getVersion();
        if (VERBOSE) {
            Log.v(TAG, "SDK and Service Version:" + version);
        }

        final Button registerForStatus = (Button) findViewById(R.id.registerBtn);
        final Button unregisterForStatus = (Button) findViewById(R.id.unregisterBtn);
        final Button getStatusReport = (Button) findViewById(R.id.getReportBtn);

        registerForStatus.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            if (VERBOSE)
                Log.v(TAG, "Register for Status");

                if (mDebugreportCb == null) {
                    // register for debug reports
                    mDebugreportCb = new DebugReportCallback(DebugReportActivity.this);
                    mDebugReportingService.registerForDebugReports(mDebugreportCb);
                }
            }
        });

        unregisterForStatus.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (VERBOSE) {
                    Log.v(TAG, "Unregister for Status");
                }

                if (mDebugreportCb != null) {
                    mDebugReportingService.deregisterForDebugReports(mDebugreportCb);
                    mDebugreportCb.saveReport();
                    mDebugreportCb = null;
                }
            }
        });

        getStatusReport.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (VERBOSE) {
                    Log.v(TAG, "getReport");
                }

                Bundle reportObj =  mDebugReportingService.getDebugReport();
                printReports(reportObj, null);
              }
        });
    }

    void printReports(Bundle reportObj, PrintWriter fileWriter) {
        synchronized(sReportLogLock) {
            if (reportObj == null) {
                return;
            }

            reportObj.setClassLoader(IZatEphmerisDebugReport.class.getClassLoader());
            ArrayList<IZatEphmerisDebugReport> ephmerisReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_EPH_STATUS_KEY);

            reportObj.setClassLoader(IZatFixStatusDebugReport.class.getClassLoader());
            ArrayList<IZatFixStatusDebugReport> fixStatusReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_FIX_STATUS_KEY);

            reportObj.setClassLoader(IZatLocationReport.class.getClassLoader());
            ArrayList<IZatLocationReport> epiReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_EXTERNAL_POSITION_INJECTION_KEY);
            ArrayList<IZatLocationReport> bestLocationReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_BEST_POSITION_KEY);

            reportObj.setClassLoader(IZatGpsTimeDebugReport.class.getClassLoader());
            ArrayList<IZatGpsTimeDebugReport> gpsTimeReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_GPS_TIME_KEY);

            reportObj.setClassLoader(IZatXoStateDebugReport.class.getClassLoader());
            ArrayList<IZatXoStateDebugReport> xoStateReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_XO_STATE_KEY);

            reportObj.setClassLoader(IZatRfStateDebugReport.class.getClassLoader());
            ArrayList<IZatRfStateDebugReport> rfStateReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_RF_STATE_KEY);

            reportObj.setClassLoader(IZatErrorRecoveryReport.class.getClassLoader());
            ArrayList<IZatErrorRecoveryReport> errorRecoveries =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_LAST_ERROR_RECOVERIES_KEY);

            reportObj.setClassLoader(IZatPDRDebugReport.class.getClassLoader());
            ArrayList<IZatPDRDebugReport> pdrReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_PDR_INFO_KEY);

            reportObj.setClassLoader(IZatSVHealthDebugReport.class.getClassLoader());
            ArrayList<IZatSVHealthDebugReport> svReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_SV_HEALTH_KEY);

            reportObj.setClassLoader(IZatXTRADebugReport.class.getClassLoader());
            ArrayList<IZatXTRADebugReport> xtraReports =
                    reportObj.getParcelableArrayList(
                    IZatDebugConstants.IZAT_DEBUG_XTRA_STATUS_KEY);

            if (ephmerisReports != null) {
                for (IZatEphmerisDebugReport ephmerisReport : ephmerisReports) {
                    logEphReport(ephmerisReport, fileWriter);
                }
            }

            if (svReports != null) {
                for (IZatSVHealthDebugReport svReport : svReports) {
                    logSvReport(svReport, fileWriter);
                }
            }

            if (xtraReports != null) {
                for (IZatXTRADebugReport xtraReport : xtraReports) {
                    logXtraReport(xtraReport,fileWriter);
                }
            }

            if (epiReports != null) {
                for (IZatLocationReport epiReport : epiReports) {
                    logEpiReport(epiReport, fileWriter);
                }
            }

            if (bestLocationReports != null) {
                for (IZatLocationReport bestLocationReport : bestLocationReports) {
                    logBestPosReport(bestLocationReport, fileWriter);
                }
            }

            if (fixStatusReports != null) {
                for (IZatFixStatusDebugReport fixStatusReport : fixStatusReports) {
                    logFixStatusReport(fixStatusReport, fileWriter);
                }
            }

            if (pdrReports != null) {
                for (IZatPDRDebugReport pdrReport : pdrReports) {
                    logPdrReport(pdrReport, fileWriter);
                }
            }

            if (gpsTimeReports != null) {
                for (IZatGpsTimeDebugReport gpsTimeReport : gpsTimeReports) {
                    logGpstimeReport(gpsTimeReport, fileWriter);
                }
            }

            if (xoStateReports != null) {
                for (IZatXoStateDebugReport xoStateReport : xoStateReports) {
                    logXoStatusReport(xoStateReport, fileWriter);
                }
            }

            if (rfStateReports != null) {
                for (IZatRfStateDebugReport rfStateReport : rfStateReports) {
                    logRfStateReport(rfStateReport, fileWriter);
                }
            }

            if (errorRecoveries != null) {
                for (IZatErrorRecoveryReport errorRecovery : errorRecoveries) {
                    logErrorRecoveries(errorRecovery, fileWriter);
                }
            }
        }
    }

    void printReport(Bundle reportObj, PrintWriter fileWriter) {
        synchronized(sReportLogLock) {
            if (reportObj == null) {
                return;
            }

            reportObj.setClassLoader(IZatEphmerisDebugReport.class.getClassLoader());
            IZatEphmerisDebugReport ephmerisReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_EPH_STATUS_KEY);

            reportObj.setClassLoader(IZatFixStatusDebugReport.class.getClassLoader());
            IZatFixStatusDebugReport fixStatusReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_FIX_STATUS_KEY);

            reportObj.setClassLoader(IZatLocationReport.class.getClassLoader());
            IZatLocationReport epiReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_EXTERNAL_POSITION_INJECTION_KEY);
            IZatLocationReport bestLocationReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_BEST_POSITION_KEY);

            reportObj.setClassLoader(IZatGpsTimeDebugReport.class.getClassLoader());
            IZatGpsTimeDebugReport gpsTimeReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_GPS_TIME_KEY);

            reportObj.setClassLoader(IZatXoStateDebugReport.class.getClassLoader());
            IZatXoStateDebugReport xoStateReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_XO_STATE_KEY);

            reportObj.setClassLoader(IZatRfStateDebugReport.class.getClassLoader());
            IZatRfStateDebugReport rfStateReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_RF_STATE_KEY);

            reportObj.setClassLoader(IZatErrorRecoveryReport.class.getClassLoader());
            IZatErrorRecoveryReport errorRecovery =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_LAST_ERROR_RECOVERIES_KEY);

            reportObj.setClassLoader(IZatPDRDebugReport.class.getClassLoader());
            IZatPDRDebugReport pdrReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_PDR_INFO_KEY);

            reportObj.setClassLoader(IZatSVHealthDebugReport.class.getClassLoader());
            IZatSVHealthDebugReport svReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_SV_HEALTH_KEY);

            reportObj.setClassLoader(IZatXTRADebugReport.class.getClassLoader());
            IZatXTRADebugReport xtraReport =
                    reportObj.getParcelable(IZatDebugConstants.IZAT_DEBUG_XTRA_STATUS_KEY);

            logEphReport(ephmerisReport, fileWriter);
            logSvReport(svReport, fileWriter);
            logXtraReport(xtraReport,fileWriter);
            logEpiReport(epiReport, fileWriter);
            logBestPosReport(bestLocationReport, fileWriter);
            logFixStatusReport(fixStatusReport, fileWriter);
            logPdrReport(pdrReport, fileWriter);
            logGpstimeReport(gpsTimeReport, fileWriter);
            logXoStatusReport(xoStateReport, fileWriter);
            logRfStateReport(rfStateReport, fileWriter);
            logErrorRecoveries(errorRecovery, fileWriter);
         }
    }

    private void logEphReport(IZatEphmerisDebugReport ephmerisReport,
                              PrintWriter fileWriter) {
        if (ephmerisReport != null) {
            IZatUtcSpec timespec = ephmerisReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = ephmerisReport.getLastReportedUTCTime();
            StringBuilder s = new StringBuilder();

            s.append("Ephmeris LastUpdated: UTC (secs):" + timespec.getSeconds()
                       + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n ");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                       + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            int gpsEphDataValidity = ephmerisReport.getEphmerisForGPS();
            int glonassEphDataValidity = ephmerisReport.getEphmerisForGlonass();
            long bdsEphDataValidity = ephmerisReport.getEphmerisForBDS();
            long galEphDataValidity = ephmerisReport.getEphmerisForGal();
            byte qzssEphDataValidity = ephmerisReport.getEphmerisForQzss();

            s.append(" GPS:");
            s.append(getBits(gpsEphDataValidity));

            s.append(" GLONASS:");
            s.append(getBits(glonassEphDataValidity));

            s.append(" BDS:");
            s.append(getBits(bdsEphDataValidity));

            s.append(" GAL:");
            s.append(getBits(galEphDataValidity));

            s.append(" QZSS:");
            s.append(getBits(qzssEphDataValidity));

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logSvReport(IZatSVHealthDebugReport svReport, PrintWriter fileWriter) {
        if (svReport != null) {
            IZatUtcSpec timespec = svReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = svReport.getLastReportedUTCTime();
            StringBuilder s = new StringBuilder();
            s.append("SV Health Report LastUpdated: UTC (secs):" + timespec.getSeconds() +
                     " UTC (ns):" + timespec.getNanoSeconds() + "\r\n ");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                   + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            s.append(" GPS:");
            List<IZatSVHealthDebugReport.IzatSVHealthState> gpsSVHealthList =
                    svReport.getSVHealthForGPS();
            s.append(TextUtils.join(", ", gpsSVHealthList) + "\r\n");

            s.append(" Glonass:");
            List<IZatSVHealthDebugReport.IzatSVHealthState> glonassSVHealthList =
                    svReport.getSVHealthForGlonass();
            s.append(TextUtils.join(", ", glonassSVHealthList) + "\r\n");

            s.append( " BDS:");
            List<IZatSVHealthDebugReport.IzatSVHealthState> bdsSVHealthList =
                    svReport.getSVHealthForBDS();
            s.append(TextUtils.join(", ", bdsSVHealthList) + "\r\n");

            s.append(" Gal:");
            List<IZatSVHealthDebugReport.IzatSVHealthState> galSVHealthList =
                    svReport.getSVHealthForGal();
            s.append(TextUtils.join(", ", galSVHealthList) + "\r\n");

            s.append(" QZSS:");
            List<IZatSVHealthDebugReport.IzatSVHealthState> qzssSVHealthList =
                    svReport.getSVHealthForQzss();
            s.append(TextUtils.join(", ", qzssSVHealthList) + "\r\n");

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logXtraReport(IZatXTRADebugReport xtraReport, PrintWriter fileWriter) {
        if (xtraReport != null) {
            IZatUtcSpec timespec = xtraReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = xtraReport.getLastReportedUTCTime();
            StringBuilder s = new StringBuilder();
            s.append("XTRA Report LastUpdated: UTC (secs):" + timespec.getSeconds()
                    + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                    + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            if (xtraReport.hasGpsXtraInfo()) {
                IZatXTRADebugReport.GpsXtraValidityInfo gpsXtra =
                        xtraReport.getXtraDataValidityForGPS();
                int ageGps = gpsXtra.getXtraAge();
                int gpsXtraMask = gpsXtra.getGpsXtraValidity();

                s.append(" GPS : Age: " + ageGps);
                s.append(" State:" + getBits(gpsXtraMask));
            } else {
                s.append(" GPS XTRA: Invalid ");
            }

            if (xtraReport.hasGlonassXtraInfo()) {
                IZatXTRADebugReport.GlonassXtraValidityInfo glonassXtra =
                        xtraReport.getXtraDataValidityForGlonass();
                int ageGlonass = glonassXtra.getXtraAge();
                int glonassXtraMask = glonassXtra.getXtraValidity();

                s.append(" GLONASS : Age: " + ageGlonass);
                s.append(" State:"+ getBits(glonassXtraMask));
            } else {
                s.append(" GLONASS XTRA: Invalid ");
            }

            if (xtraReport.hasBdsXtraInfo()) {
                IZatXTRADebugReport.BdsXtraValidityInfo bdsXtra =
                        xtraReport.getXtraDataValidityForBDS();
                int ageBds = bdsXtra.getXtraAge();
                long bdsXtraMask = bdsXtra.getXtraValidity();

                s.append(" BDS : Age: " + ageBds);
                s.append(" State:"+  getBits(bdsXtraMask));
            } else {
                s.append(" BDS XTRA: Invalid ");
            }

            if (xtraReport.hasGalXtraInfo()) {
                IZatXTRADebugReport.GalXtraValidityInfo galXtra =
                        xtraReport.getXtraDataValidityForGal();
                int ageGal = galXtra.getXtraAge();
                long galXtraMask = galXtra.getXtraValidity();

                s.append(" GAL : Age:" + ageGal);
                s.append(" State:"+ getBits(galXtraMask));
            } else {
                s.append(" GAL XTRA: Invalid ");
            }

            if (xtraReport.hasQzssXtraInfo()) {
                IZatXTRADebugReport.QzssXtraValidityInfo qzssXtra =
                        xtraReport.getXtraDataValidityForQzss();
                int ageQzss = qzssXtra.getXtraAge();
                byte qzssXtraMask = qzssXtra.getXtraValidity();

                s.append(" QZSS : Age:" + ageQzss);
                s.append(" State:"+ getBits(qzssXtraMask));
            } else {
                s.append(" QZSS XTRA: Invalid ");
            }

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logEpiReport(IZatLocationReport epiReport, PrintWriter fileWriter) {
        if (epiReport != null) {
            IZatUtcSpec timespec = epiReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = epiReport.getLastReportedUTCTime();
            StringBuilder s = new StringBuilder();
            s.append("EPI Report LastUpdated: UTC (secs):" + timespec.getSeconds()
                       + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                       + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            s.append("EPI Validity: Horizontal: " + epiReport.hasHorizontalFix() +
                " Vertical: " + epiReport.hasVerticalFix() + "\r\n");

            if (epiReport.hasHorizontalFix()) {
                s.append(" Lat:" + epiReport.getLatitude());
                s.append(" Lon:" + epiReport.getLongitude());
                s.append(" Horizontal Accuracy:" + epiReport.getAccuracy());
            }
            if (epiReport.hasVerticalFix()) {
                s.append(" Alt:" + epiReport.getAltitude());
                s.append(" Alt Uncertainity:" + epiReport.getAltitudeUncertainity());
            }
            if (epiReport.hasSource()) {
                s.append(" Source:" + epiReport.getSource());
            }

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logBestPosReport(IZatLocationReport bestLocationReport,
                                  PrintWriter fileWriter) {
        if (bestLocationReport != null) {
            StringBuilder s = new StringBuilder();
            IZatUtcSpec timespec = bestLocationReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = bestLocationReport.getLastReportedUTCTime();
            s.append("Best Location Report LastUpdated: UTC (secs):" + timespec.getSeconds()
                    + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                    + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            if (bestLocationReport.hasHorizontalFix()) {
                s.append(" Lat:" + bestLocationReport.getLatitude());
                s.append(" Lon:" + bestLocationReport.getLongitude());
                s.append(" Horizontal Accuracy:" + bestLocationReport.getAccuracy());
            }
            if (bestLocationReport.hasVerticalFix()) {
                s.append(" Alt:" + bestLocationReport.getAltitude());
                s.append(" Alt Uncertainity:" +
                        bestLocationReport.getAltitudeUncertainity());
            }
            if (bestLocationReport.hasSource()) {
                s.append(" Source:" + bestLocationReport.getSource());
            }

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logFixStatusReport(IZatFixStatusDebugReport fixStatusReport,
                                    PrintWriter fileWriter) {
        if (fixStatusReport != null) {
            StringBuilder s = new StringBuilder();
            IZatUtcSpec timespec = fixStatusReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = fixStatusReport.getLastReportedUTCTime();
            s.append("FixStatus Report LastUpdated: UTC (secs):" + timespec.getSeconds()
                    + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                    + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            s.append(" FixStatus: " + fixStatusReport.getFixStatus());
            s.append(" HEPE Limit: " + fixStatusReport.getHEPELimit());

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logPdrReport(IZatPDRDebugReport pdrReport, PrintWriter fileWriter) {
        if (pdrReport != null) {
            StringBuilder s = new StringBuilder();
            IZatUtcSpec timespec = pdrReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = pdrReport.getLastReportedUTCTime();
            s.append("PDR Report LastUpdated: UTC (secs):" + timespec.getSeconds()
                    + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                    + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            s.append(" PDR Engaged:" + pdrReport.isPDREngaged());
            s.append(" PDR Mag Calibrated:" + pdrReport.isPDRMagCalibrated());
            s.append(" Heading Filter Engaged:" + pdrReport.isHDGFilterEngaged());
            s.append(" INS Filter Engaged:" + pdrReport.isINSFilterEngaged());

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logGpstimeReport(IZatGpsTimeDebugReport gpsTimeReport,
                                  PrintWriter fileWriter) {
        if (gpsTimeReport != null) {
            StringBuilder s = new StringBuilder();
            IZatUtcSpec timespec = gpsTimeReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = gpsTimeReport.getLastReportedUTCTime();
            s.append("GPS Time Report LastUpdated: UTC (secs):" + timespec.getSeconds()
                    + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                    + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            s.append(" GPS Week:" + gpsTimeReport.getGpsWeek());
            s.append(" GPS TOW (ms):" + gpsTimeReport.getGpsTimeOfWeek());

            s.append(" TimeValid:" + gpsTimeReport.IsTimeValid());
            s.append(" TimeSource:" + gpsTimeReport.getTimeSource());
            s.append(" TimeUnc:" + gpsTimeReport.getTimeUncertainity());

            s.append( " Clock Frequency Bias:" +
                    gpsTimeReport.getClockFrequencyBias());
            s.append( " Clock Frequency Bias Unc:" +
                    gpsTimeReport.getClockFrequencyBiasUncertainity());

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logXoStatusReport(IZatXoStateDebugReport xoStateReport,
                                   PrintWriter fileWriter) {
        if (xoStateReport != null) {
            StringBuilder s = new StringBuilder();
            IZatUtcSpec timespec = xoStateReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = xoStateReport.getLastReportedUTCTime();
            s.append("XO State Report LastUpdated: UTC (secs):" + timespec.getSeconds()
                    + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append(" LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                    + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");
            s.append(" XOState:" + xoStateReport.getXoState());

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logRfStateReport(IZatRfStateDebugReport rfStateReport, PrintWriter fileWriter) {
        if (rfStateReport != null) {
            StringBuilder s = new StringBuilder();
            IZatUtcSpec timespec = rfStateReport.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = rfStateReport.getLastReportedUTCTime();
            s.append("RF State Report LastUpdated: UTC (secs):" + timespec.getSeconds()
                    + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append("  LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                    + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            s.append(" PGA Gain:" + rfStateReport.getPGAGain());
            s.append(" GPS BP AmplI:" + rfStateReport.getGPSBPAmpI());
            s.append(" GPS BP AmplQ:" + rfStateReport.getGPSBPAmpQ());
            s.append(" ADC AmplI:" + rfStateReport.getADCAmplitudeI());
            s.append( " ADC AmplQ:" + rfStateReport.getADCAmplitudeQ());
            s.append(" Jammer Metric GPS:" + rfStateReport.getJammerMetricGPS());
            s.append(" Jammer Metric Glonass:" +
                          rfStateReport.getJammerMetricGlonass());
            s.append(" Jammer Metric BDS:" + rfStateReport.getJammerMetricBds());
            s.append(" Jammer Metric Gal:" + rfStateReport.getJammerMetricGal());
            s.append(" GLO BP AmpI:" + rfStateReport.getGLOBPAmpI());
            s.append(" GLO BP AmpQ:" + rfStateReport.getGLOBPAmpQ());
            s.append(" BDS BP AmpI:" + rfStateReport.getBDSBPAmpI());
            s.append(" BDS BP AmpQ:" + rfStateReport.getBDSBPAmpQ());
            s.append(" GAL BP AmpI:" + rfStateReport.getGALBPAmpI());
            s.append(" GAL BP AmpQ:" + rfStateReport.getGALBPAmpQ());

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private void logErrorRecoveries(IZatErrorRecoveryReport errorRecovery, PrintWriter fileWriter) {
        if (errorRecovery != null) {
            StringBuilder s = new StringBuilder();
            IZatUtcSpec timespec = errorRecovery.getUTCTimestamp();
            IZatUtcSpec timespecLastReported = errorRecovery.getLastReportedUTCTime();
            s.append("Error Recoveries LastUpdated: UTC (secs):" + timespec.getSeconds()
                    + " UTC (ns):" + timespec.getNanoSeconds() + "\r\n");
            s.append("  LastReported: UTC (secs):" + timespecLastReported.getSeconds()
                    + " UTC (ns):" + timespecLastReported.getNanoSeconds() + "\r\n ");

            if (fileWriter != null) {
                fileWriter.println(s);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, s.toString());
                }
            }
        }
    }

    private String getBits(int mask) {
        return getBits((long) mask, (byte) 32);
    }

    private String getBits(long mask) {
        return getBits(mask, (byte) 64);
    }

    private String getBits(byte mask) {
        return getBits((long) mask, (byte) 8);
    }

    private String getBits(long mask, byte size) {
        size = (byte) Math.min((int) size, 64);
        String tmpStr = "";
        for (int i = 0; i < size; i++) {
            tmpStr += (((1L << i & mask) != 0) ? "1" : "0");
        }
        return tmpStr;
    }
}
