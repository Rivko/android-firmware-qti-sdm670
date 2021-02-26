/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import java.util.ArrayList;
import java.util.Map;
import java.util.LinkedHashMap;

import android.app.Activity;
import android.app.PendingIntent;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;

import android.widget.Button;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.AdapterView;
import android.view.View;
import android.view.LayoutInflater;
import android.graphics.Color;
import android.location.Location;

import com.qti.location.sdk.IZatManager;
import com.qti.location.sdk.IZatGeofenceService;


public class FullscreenActivity extends Activity {

    private static String TAG = "GeofenceTest";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private IZatGeofenceService.IZatGeofenceHandle mHandle = null;
    private IZatManager mIzatMgr;
    private IZatGeofenceService mGfService;
    int mGeofenceSelectedIdx = -1;

    Map<IZatGeofenceService.IZatGeofenceHandle,
            IZatGeofenceService.IzatGeofence>  mGeofenceHandleDataMap =
            new LinkedHashMap<IZatGeofenceService.IZatGeofenceHandle,
            IZatGeofenceService.IzatGeofence> ();
    ArrayList<String> mGeofenceListNames = new ArrayList<String>();
    ArrayAdapter<String> mGeofenceListadapter;

    EditText mEditName, mEditLatitude, mEditLongitude, mEditRadius, mEditTransition;
    EditText mEditDwellType, mEditDwellTime, mEditConfidence, mEditResponsiveness;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fullscreen);
        doInit();
    }

    @Override
    protected void onNewIntent(Intent intent) {
        if (intent != null) {
            int transition;
            Location location;
            String strCtx = intent.getStringExtra("context-data");
            if (strCtx != null) {
                Log.v(TAG, "Context:" + strCtx);
            } else {
                Bundle bundleObj = intent.getBundleExtra("context-data");
                if (bundleObj != null) {
                    bundleObj.setClassLoader(TestParcelable.class.getClassLoader());
                    TestParcelable testObj = bundleObj.getParcelable("bundle-obj");
                    if (testObj != null) {
                        if (VERBOSE) {
                            Log.v(TAG, "Context: i = " + testObj.mitest + " s = " + testObj.mstest);
                        }
                    }
                }
            }
            location = intent.getParcelableExtra("transition-location");
            transition = intent.getIntExtra("transition-event", 0);

            if (location != null) {
                Log.v(TAG, "transition:" + transition + " location:" +
                location.toString());
            }
        }
    }

    private void doInit() {
        mIzatMgr = IZatManager.getInstance(getApplicationContext());
        mGfService = mIzatMgr.connectGeofenceService();
        final BreachCallback breachCb = new BreachCallback();

        String version = mIzatMgr.getVersion();
        if (VERBOSE) {
            Log.v(TAG, "SDK and Service Version:" + version);
        }

        // register callback
        mGfService.registerForGeofenceCallbacks(breachCb);

        // register pending intent for this activity
        Intent activitIntent =
                new Intent(getApplicationContext(), FullscreenActivity.class);
        PendingIntent geofenceIntent = PendingIntent.getActivity(
                getApplicationContext(), 0, activitIntent, 0);
        mGfService.registerPendingIntent(geofenceIntent);

        final Button addGeofence = (Button) findViewById(R.id.addBtn);
        final Button editGeofence = (Button) findViewById(R.id.editBtn);
        final Button delGeofence = (Button) findViewById(R.id.delBtn);
        final Button pauseGeofence = (Button) findViewById(R.id.pauseBtn);
        final Button resumeGeofence = (Button) findViewById(R.id.resumeBtn);
        final Button recoverAllGeofence = (Button) findViewById(R.id.recoverAllBtn);

        final ListView lstView = (ListView) findViewById(R.id.geofenceList);
        mGeofenceListadapter = new ArrayAdapter<String>(
                this, android.R.layout.simple_list_item_1, mGeofenceListNames);
        lstView.setAdapter(mGeofenceListadapter);

        lstView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> a, View v, int position,
                    long id) {
                mGeofenceSelectedIdx = (int)id;
            }
        });

        addGeofence.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showAddDialog();
            }
        });

        editGeofence.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mGeofenceSelectedIdx != -1) {
                    showEditDialog();
                }
            }
        });

        delGeofence.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mGeofenceSelectedIdx != -1) {
                    Object gfHandleArray[] =
                            mGeofenceHandleDataMap.keySet().toArray();
                    IZatGeofenceService.IZatGeofenceHandle gfHandle =
                            (IZatGeofenceService.IZatGeofenceHandle)
                            gfHandleArray[mGeofenceSelectedIdx];

                    if (gfHandle != null) {
                        mGfService.removeGeofence(gfHandle);
                        mGeofenceHandleDataMap.remove(gfHandle);
                        mGeofenceListNames.remove(mGeofenceSelectedIdx);
                        mGeofenceListadapter.notifyDataSetChanged();
                    }
                }
            }
        });

        pauseGeofence.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mGeofenceSelectedIdx != -1) {
                    Object gfHandleArray[] =
                            mGeofenceHandleDataMap.keySet().toArray();
                    IZatGeofenceService.IZatGeofenceHandle gfHandle =
                            (IZatGeofenceService.IZatGeofenceHandle)
                            gfHandleArray[mGeofenceSelectedIdx];

                    if (gfHandle != null) {
                        gfHandle.pause();
                    }

                    if (VERBOSE) {
                        Log.v(TAG, "After pause, pause status is: " + gfHandle.isPaused());
                    }
                }
            }
        });

        resumeGeofence.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mGeofenceSelectedIdx != -1) {
                    showResumeDialog();
                }
            }
        });

        recoverAllGeofence.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGeofenceHandleDataMap.clear();
                mGeofenceListNames.clear();

                mGeofenceHandleDataMap.putAll(mGfService.recoverGeofences());
                for (Map.Entry<IZatGeofenceService.IZatGeofenceHandle,
                        IZatGeofenceService.IzatGeofence>
                        entry : mGeofenceHandleDataMap.entrySet()) {
                    IZatGeofenceService.IZatGeofenceHandle gfHandle = entry.getKey();
                    Object geofenceCtx = gfHandle.getContext();
                    if ((geofenceCtx != null) && (geofenceCtx instanceof String)) {
                        mGeofenceListNames.add(geofenceCtx.toString());
                        IZatGeofenceService.IzatGeofence gf = entry.getValue();
                    } else if ((geofenceCtx != null) && (geofenceCtx instanceof Bundle)) {
                        Bundle bundleObj = (Bundle) geofenceCtx;
                        bundleObj.setClassLoader(TestParcelable.class.getClassLoader());
                        TestParcelable testObj = bundleObj.getParcelable("bundle-obj");
                        if (testObj != null) {
                            if (VERBOSE) {
                                Log.v(TAG, "i = " + testObj.mitest + " s = " + testObj.mstest);
                            }
                        }
                        mGeofenceListNames.add("<<no name>>");
                    }
                }

                mGeofenceListadapter.notifyDataSetChanged();
           }
        });
    }

    private void showResumeDialog() {
        LayoutInflater inflater;
        View promptsView;

        // get the prompts view
        inflater = LayoutInflater.from(this);
        promptsView = inflater.inflate(R.layout.prompts, null);
        instanceViewFields(promptsView);

        AlertDialog.Builder alertDialogBuilder =
                new AlertDialog.Builder(this);

        // set prompts.xml to alertdialog builder
        alertDialogBuilder.setView(promptsView);


        // set dialog message
        alertDialogBuilder.setCancelable(false);

        Object gfHandleArray[] = mGeofenceHandleDataMap.keySet().toArray();
        final IZatGeofenceService.IZatGeofenceHandle gfHandle =
                (IZatGeofenceService.IZatGeofenceHandle) gfHandleArray[mGeofenceSelectedIdx];

        if (gfHandle != null) {
            IZatGeofenceService.IzatGeofence gf = mGeofenceHandleDataMap.get(gfHandle);
            if (gf != null) {
                Object geofenceName = gfHandle.getContext();
                if ((geofenceName != null) && (geofenceName instanceof String)) {
                    mEditName.setText(geofenceName.toString());
                } else {
                    mEditName.setText("<<no name>>");
                }

                fillViewFields(gf);
            }
        }
        alertDialogBuilder.setPositiveButton("Resume",
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog,int id) {
                    gfHandle.resume();
                    if (VERBOSE) {
                        Log.v(TAG, "After resume, pause status is: " + gfHandle.isPaused());
                    }
                }
            });

        alertDialogBuilder.setNegativeButton("Cancel",
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog,int id) {
                    dialog.cancel();
                }
            });

        // disable the non-editable fields and just display the current value of all fields
        mEditName.setFocusable(false);
        mEditLatitude.setFocusable(false);
        mEditLongitude.setFocusable(false);
        mEditRadius.setFocusable(false);
        mEditDwellType.setFocusable(false);
        mEditDwellTime.setFocusable(false);
        mEditConfidence.setFocusable(false);
        mEditTransition.setFocusable(false);
        mEditResponsiveness.setFocusable(false);

        mEditName.setTextColor(Color.GRAY);
        mEditLatitude.setTextColor(Color.GRAY);
        mEditLongitude.setTextColor(Color.GRAY);
        mEditRadius.setTextColor(Color.GRAY);
        mEditDwellType.setTextColor(Color.GRAY);
        mEditDwellTime.setTextColor(Color.GRAY);
        mEditConfidence.setTextColor(Color.GRAY);
        mEditTransition.setTextColor(Color.GRAY);
        mEditResponsiveness.setTextColor(Color.GRAY);

        // create alert dialog
        AlertDialog alertDialog = alertDialogBuilder.create();

        // show it
        alertDialog.show();
    }

    private void showEditDialog() {
        LayoutInflater inflater;
        View promptsView;

        // get the prompts view
        inflater = LayoutInflater.from(this);
        promptsView = inflater.inflate(R.layout.prompts, null);
        instanceViewFields(promptsView);

        AlertDialog.Builder alertDialogBuilder =
                new AlertDialog.Builder(this);

        // set prompts.xml to alertdialog builder
        alertDialogBuilder.setView(promptsView);

        // set dialog message
        alertDialogBuilder.setCancelable(false);

        Object gfHandleArray[] = mGeofenceHandleDataMap.keySet().toArray();
        final IZatGeofenceService.IZatGeofenceHandle gfHandle =
                (IZatGeofenceService.IZatGeofenceHandle) gfHandleArray[mGeofenceSelectedIdx];

        if (gfHandle != null) {
            IZatGeofenceService.IzatGeofence gf = mGeofenceHandleDataMap.get(gfHandle);
            if (gf != null) {
                Object geofenceCtx = gfHandle.getContext();
                if ((geofenceCtx != null) && (geofenceCtx instanceof String)) {
                    mEditName.setText(geofenceCtx.toString());
                } else if ((geofenceCtx != null) && (geofenceCtx instanceof Bundle)) {
                    Bundle bundleObj = (Bundle) geofenceCtx;
                    bundleObj.setClassLoader(TestParcelable.class.getClassLoader());
                    TestParcelable testObj = bundleObj.getParcelable("bundle-obj");
                    if (testObj != null) {
                        if (VERBOSE) {
                            Log.v(TAG, "i = " + testObj.mitest + " s = " + testObj.mstest);
                        }
                    }
                    mEditName.setText("<<no name>>");
                }

                fillViewFields(gf);
            }
        }

        alertDialogBuilder.setPositiveButton("Save",
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog,int id) {
                    double radius = Double.parseDouble(mEditRadius.getText().toString());
                    int dwellType = Integer.parseInt(mEditDwellType.getText().toString());
                    int dwellTime = Integer.parseInt(mEditDwellTime.getText().toString());
                    int confidence = Integer.parseInt(mEditConfidence.getText().toString());

                    IZatGeofenceService.IzatDwellNotify dwellNotify =
                            new IZatGeofenceService.IzatDwellNotify(dwellTime, dwellType);

                    int transitionType = Integer.parseInt(mEditTransition.getText().toString());
                    int responsiveness = Integer.parseInt(mEditResponsiveness.getText().toString());

                    IZatGeofenceService.IzatGeofence gf =
                            new IZatGeofenceService.IzatGeofence(null, null, radius);
                    gf.setTransitionTypes(getTransitionEnumType(transitionType));
                    gf.setConfidence(getConfidenceEnumType(confidence));
                    gf.setNotifyResponsiveness(responsiveness);
                    gf.setDwellNotify(dwellNotify);
                    gfHandle.update(gf);
                }
            });

        alertDialogBuilder.setNegativeButton("Cancel",
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog,int id) {
                    dialog.cancel();
                }
            });

        // disable the non-editable fields and just display the current value of all fields
        // For testing purposes, this app does not allow to edit latitude & longitude, but it could.
        mEditName.setFocusable(false);
        mEditLatitude.setFocusable(false);
        mEditLongitude.setFocusable(false);

        mEditName.setTextColor(Color.GRAY);
        mEditLatitude.setTextColor(Color.GRAY);
        mEditLongitude.setTextColor(Color.GRAY);

        // create alert dialog
        AlertDialog alertDialog = alertDialogBuilder.create();

        // show it
        alertDialog.show();
    }

    private void fillViewFields(IZatGeofenceService.IzatGeofence gf) {
        mEditLatitude.setText(Double.toString(gf.getLatitude()));
        mEditLongitude.setText(Double.toString(gf.getLongitude()));
        mEditRadius.setText(Double.toString(gf.getRadius()));
        mEditTransition.setText(Integer.toString(gf.getTransitionTypes().getValue()));

        IZatGeofenceService.IzatDwellNotify dwellNotify = gf.getDwellNotify();
        mEditDwellType.setText(Integer.toString(dwellNotify.getDwellType()));
        mEditDwellTime.setText(Integer.toString(dwellNotify.getDwellTime()));

        mEditConfidence.setText(Integer.toString(gf.getConfidence().getValue()));
        mEditResponsiveness.setText(Integer.toString(gf.getNotifyResponsiveness()));
    }

    private void instanceViewFields(View promptsView) {
        mEditName = (EditText)(promptsView.findViewById(R.id.editAlertName));
        mEditLatitude = (EditText)(promptsView.findViewById(R.id.editTextLatitude));
        mEditLongitude = (EditText)(promptsView.findViewById(R.id.editTextLongitude));
        mEditRadius = (EditText)(promptsView.findViewById(R.id.editTextRadius));
        mEditTransition = (EditText)(promptsView.findViewById(R.id.editTextTransition));
        mEditDwellType = (EditText)(promptsView.findViewById(R.id.editTextDwellType));
        mEditDwellTime = (EditText)(promptsView.findViewById(R.id.editDwellTime));
        mEditConfidence = (EditText)(promptsView.findViewById(R.id.editConfidence));
        mEditResponsiveness = (EditText)(promptsView.findViewById(R.id.editResponsiveness));
    }

    private void showAddDialog() {
        LayoutInflater inflater;
        View promptsView;

        // get the prompts view
        inflater = LayoutInflater.from(this);
        promptsView = inflater.inflate(R.layout.prompts, null);

        instanceViewFields(promptsView);

        AlertDialog.Builder alertDialogBuilder =
            new AlertDialog.Builder(this);

        // set prompts.xml to alertdialog builder
        alertDialogBuilder.setView(promptsView);

        // set dialog message
        alertDialogBuilder.setCancelable(false);
        alertDialogBuilder.setPositiveButton("Save",
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog,int id) {
                    String geofenceName = mEditName.getText().toString();
                    double latitude = Double.parseDouble(mEditLatitude.getText().toString());
                    double longitude = Double.parseDouble(mEditLongitude.getText().toString());
                    double radius = Double.parseDouble(mEditRadius.getText().toString());
                    int transitionType = Integer.parseInt(mEditTransition.getText().toString());
                    int dwellType = Integer.parseInt(mEditDwellType.getText().toString());
                    int dwellTime = Integer.parseInt(mEditDwellTime.getText().toString());
                    int confidence = Integer.parseInt(mEditConfidence.getText().toString());
                    int responsiveness = Integer.parseInt(mEditResponsiveness.getText().toString());

                    IZatGeofenceService.IzatDwellNotify dwellNotify =
                    new IZatGeofenceService.IzatDwellNotify(dwellTime, dwellType);

                    IZatGeofenceService.IzatGeofence gf =
                    new IZatGeofenceService.IzatGeofence(latitude, longitude, radius);
                    gf.setTransitionTypes(getTransitionEnumType(transitionType));
                    gf.setConfidence(getConfidenceEnumType(confidence));
                    gf.setNotifyResponsiveness(responsiveness);
                    gf.setDwellNotify(dwellNotify);

                    Bundle bundleObj = null;
                    if (geofenceName.isEmpty()) {
                        TestParcelable testObj = new TestParcelable(50, "sample test string");
                        bundleObj = new Bundle(TestParcelable.class.getClassLoader());
                        bundleObj.putParcelable("bundle-obj", testObj);
                    }

                    IZatGeofenceService.IZatGeofenceHandle gfHandle =
                    mGfService.addGeofence(
                        geofenceName.isEmpty() ? bundleObj : geofenceName, gf);

                    if (gfHandle != null) {
                        mGeofenceHandleDataMap.put(gfHandle, gf);
                        Object gfCtx = gfHandle.getContext();
                        if ((gfCtx == null) || (!(gfCtx instanceof String))) {
                            gfCtx = "<<no name>>";
                        }
                        mGeofenceListNames.add(gfCtx.toString());
                        mGeofenceListadapter.notifyDataSetChanged();
                    }
                }
            });

        alertDialogBuilder.setNegativeButton("Cancel",
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog,int id) {
                    dialog.cancel();
                }
            });

        // create alert dialog
        AlertDialog alertDialog = alertDialogBuilder.create();

        // show it
        alertDialog.show();
    }

    private  IZatGeofenceService.IzatGeofenceTransitionTypes
            getTransitionEnumType(int transitionType) {
        IZatGeofenceService.IzatGeofenceTransitionTypes gfTransitionType;
        switch (transitionType) {
            case 0:
                gfTransitionType =
                        IZatGeofenceService.IzatGeofenceTransitionTypes.UNKNOWN;
            break;
            case 1:
                gfTransitionType =
                        IZatGeofenceService.IzatGeofenceTransitionTypes.ENTERED_ONLY;
            break;
            case 2:
                gfTransitionType =
                        IZatGeofenceService.IzatGeofenceTransitionTypes.EXITED_ONLY;
            break;
            case 3:
                gfTransitionType =
                        IZatGeofenceService.IzatGeofenceTransitionTypes.ENTERED_AND_EXITED;
            break;
            default:
                gfTransitionType =
                        IZatGeofenceService.IzatGeofenceTransitionTypes.UNKNOWN;
        }

        return gfTransitionType;
    }

    private IZatGeofenceService.IzatGeofenceConfidence
        getConfidenceEnumType(int confidence) {
        IZatGeofenceService.IzatGeofenceConfidence gfConfidence;
        switch (confidence) {
            case 1:
                gfConfidence = IZatGeofenceService.IzatGeofenceConfidence.LOW;
                break;
            case 2:
                gfConfidence = IZatGeofenceService.IzatGeofenceConfidence.MEDIUM;
                break;
            case 3:
                gfConfidence = IZatGeofenceService.IzatGeofenceConfidence.HIGH;
                break;
            default:
                gfConfidence = IZatGeofenceService.IzatGeofenceConfidence.LOW;
        }
        return gfConfidence;
   }
}
