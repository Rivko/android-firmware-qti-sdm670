/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import java.util.ArrayList;
import java.util.List;
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
import android.widget.TextView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.AdapterView;
import android.widget.RadioGroup;
import android.view.View;
import android.view.LayoutInflater;
import android.graphics.Color;
import android.location.Location;

import com.qti.location.sdk.IZatManager;
import com.qti.location.sdk.IZatFlpService;
import com.qti.location.sdk.IZatIllegalArgumentException;
import com.qti.location.sdk.IZatFeatureNotSupportedException;

public class FlpActivity extends Activity {
    private static String TAG = "FLPTest";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private IZatManager mIzatMgr;
    private IZatFlpService mFlpService;
    private int mFlpSessionSelectedIdx = -1;

    Map<IZatFlpService.IZatFlpSessionHandle,
            IZatFlpService.IzatFlpRequest>  mFlpSessionHandleRequestMap =
            new LinkedHashMap<IZatFlpService.IZatFlpSessionHandle, IZatFlpService.IzatFlpRequest>();
    Map<String, IZatFlpService.IZatFlpSessionHandle> mSessionNameToHandleMap =
            new LinkedHashMap<String, IZatFlpService.IZatFlpSessionHandle>();
    ArrayList<IZatFlpService.IZatFlpSessionHandle> mFlpSessionNeedRestartOnTripComplete =
            new ArrayList<IZatFlpService.IZatFlpSessionHandle> ();
    ArrayList<String> mFlpSessionNames = new ArrayList<String>();

    // one instance variables
    ArrayAdapter<String> mFlpSessionListadapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_flp);
        doInit();
    }

    private void doInit() {
        mIzatMgr = IZatManager.getInstance(getApplicationContext());
        mFlpService = mIzatMgr.connectFlpService();
        if (mFlpService == null) {
            if (VERBOSE) {
                Log.v(TAG, "Failed to get FLP Service");
            }
            return;
        }

        if (VERBOSE) {
            Log.v(TAG, "SDK and Service Version:" + mIzatMgr.getVersion());
        }

        final Button startFlpSession = (Button) findViewById(R.id.startBtn);
        final Button stopFlpSession = (Button) findViewById(R.id.stopBtn);
        final Button switchFlpMode = (Button) findViewById(R.id.switchModeBtn);
        final Button pullLocations = (Button) findViewById(R.id.pullLocationsBtn);

        final ListView lstView = (ListView) findViewById(R.id.batchList);
        mFlpSessionListadapter = new ArrayAdapter<String>(
                this, android.R.layout.simple_list_item_1, mFlpSessionNames);
        lstView.setAdapter(mFlpSessionListadapter);

        lstView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> a, View v, int position, long id) {
                mFlpSessionSelectedIdx = (int)id;
            }
        });

        startFlpSession.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showAddDialog();
            }
        });

        stopFlpSession.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mFlpSessionSelectedIdx != -1) {
                    Object sessionHandleArray[] = mFlpSessionHandleRequestMap.keySet().toArray();
                    final IZatFlpService.IZatFlpSessionHandle sessionHandle =
                            (IZatFlpService.IZatFlpSessionHandle)
                            sessionHandleArray[mFlpSessionSelectedIdx];
                    if (sessionHandle != null) {
                        try {
                            mFlpService.stopFlpSession(sessionHandle);

                            String sessionName = mFlpSessionNames.get(mFlpSessionSelectedIdx);
                            mSessionNameToHandleMap.remove(sessionName);
                            mFlpSessionHandleRequestMap.remove(sessionHandle);

                            mFlpSessionNames.remove(mFlpSessionSelectedIdx);
                            mFlpSessionListadapter.notifyDataSetChanged();

                            sessionHandle.unregisterForSessionStatus();
                        } catch (IZatIllegalArgumentException e) {
                            Log.e(TAG, e.getMessage());
                        } catch (IZatFeatureNotSupportedException e) {
                            Log.e(TAG, e.getMessage());
                        }
                    }
                }
            }
        });

        switchFlpMode.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mFlpSessionSelectedIdx != -1) {
                    String sessionName =
                            lstView.getItemAtPosition(mFlpSessionSelectedIdx).toString();
                    showEditDialog(sessionName);
                }
            }
        });

        pullLocations.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mFlpSessionSelectedIdx != -1) {
                    Object sessionHandleArray[] = mFlpSessionHandleRequestMap.keySet().toArray();
                    final IZatFlpService.IZatFlpSessionHandle sessionHandle =
                            (IZatFlpService.IZatFlpSessionHandle)
                            sessionHandleArray[mFlpSessionSelectedIdx];
                    if (sessionHandle != null) {
                        try {
                            sessionHandle.pullLocations();
                        } catch (IZatIllegalArgumentException e) {
                            Log.e(TAG, e.getMessage());
                        }
                    }
                 }
            }
        });

    } // doInit

    private void showAddDialog() {
        LayoutInflater inflater;
        View promptsView;

        // get the prompts view
        inflater = LayoutInflater.from(this);
        promptsView = inflater.inflate(R.layout.flpprompts, null);

        final EditText editSessionName  =
            (EditText)(promptsView.findViewById(R.id.editTextSessionName));
        final EditText editTimeInterval =
            (EditText)(promptsView.findViewById(R.id.editTextTimeInterval));
        final EditText editDistanceInterval =
            (EditText)(promptsView.findViewById(R.id.editTextDistanceInterval));
        final EditText editTripDistance =
            (EditText)(promptsView.findViewById(R.id.editTextTripDistance));
        final RadioGroup radioGrpFlpMode =
            (RadioGroup) (promptsView.findViewById(R.id.radioFlpMode));

        AlertDialog.Builder alertDialogBuilder =
            new AlertDialog.Builder(this);

        // set prompts.xml to alertdialog builder
        alertDialogBuilder.setView(promptsView);

        // set dialog message
        alertDialogBuilder.setCancelable(false);
        alertDialogBuilder.setPositiveButton("Start",
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog,int id) {
                String sessionName = editSessionName.getText().toString();
                long timeInterval = Long.parseLong(editTimeInterval.getText().toString());
                int distanceInterval =
                    Integer.parseInt(editDistanceInterval.getText().toString());
                long tripDistance = Long.parseLong(editTripDistance.getText().toString());

                IZatFlpService.IzatFlpRequest flpRequest = null;

                int modeSelectedId = radioGrpFlpMode.getCheckedRadioButtonId();
                switch(modeSelectedId) {
                    case R.id.radio_Tracking:
                        flpRequest = IZatFlpService.IzatFlpRequest.getForegroundFlprequest();
                        break;
                    case R.id.radio_Routine:
                        flpRequest = IZatFlpService.IzatFlpRequest.getBackgroundFlprequest();
                        break;
                    case R.id.radio_Trip:
                        flpRequest = IZatFlpService.IzatFlpRequest.getBackgroundFlprequest();
                        ((IZatFlpService.IzatFlpBgRequest)flpRequest).setActiveMode(
                                IZatFlpService.IzatFlpBgRequest.IzatFlpBgRequestMode.TRIP_MODE);
                        break;
                    default:
                }

                LocationCallback locationCb =  new LocationCallback(sessionName);

                try {
                    flpRequest.setTimeInterval(timeInterval);
                    flpRequest.setDistanceInterval(distanceInterval);
                    flpRequest.setTripDistance(tripDistance);
                } catch (RuntimeException e) {
                    Log.e(TAG, "possibly invalid input value !!");
                }

                try {
                    IZatFlpService.IZatFlpSessionHandle sessionHandle =
                        mFlpService.startFlpSession(locationCb, flpRequest);

                    if (sessionHandle != null) {
                        mFlpSessionHandleRequestMap.put(sessionHandle, flpRequest);
                        mSessionNameToHandleMap.put(sessionName, sessionHandle);
                        mFlpSessionNames.add(sessionName);
                        mFlpSessionListadapter.notifyDataSetChanged();

                        StatusCallback statusCb =
                                new StatusCallback(sessionName, FlpActivity.this);
                        sessionHandle.registerForSessionStatus(statusCb);
                    }
                } catch (IZatFeatureNotSupportedException e) {
                    Log.e(TAG, e.getMessage());
                } catch(RuntimeException e) {
                    Log.e(TAG, "Failed to start the session !!");
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
    } // showAddDialog

    private void showEditDialog(String sessionName) {
        LayoutInflater inflater;
        View promptsView;

        // get the prompts view
        inflater = LayoutInflater.from(this);
        promptsView = inflater.inflate(R.layout.flpprompts, null);

        final EditText editSessionName  =
                (EditText)(promptsView.findViewById(R.id.editTextSessionName));
        final EditText editTimeInterval =
                (EditText)(promptsView.findViewById(R.id.editTextTimeInterval));
        final EditText editDistanceInterval =
                (EditText)(promptsView.findViewById(R.id.editTextDistanceInterval));
        final EditText editTripDistance =
                (EditText)(promptsView.findViewById(R.id.editTextTripDistance));
        final RadioGroup radioGrpFlpMode =
                (RadioGroup) (promptsView.findViewById(R.id.radioFlpMode));

        AlertDialog.Builder alertDialogBuilder =
                new AlertDialog.Builder(this);

        // set prompts.xml to alertdialog builder
        alertDialogBuilder.setView(promptsView);

        // set dialog message
        alertDialogBuilder.setCancelable(false);

        Object sessionHandleArray[] = mFlpSessionHandleRequestMap.keySet().toArray();
        final IZatFlpService.IZatFlpSessionHandle sessionHandle =
                (IZatFlpService.IZatFlpSessionHandle) sessionHandleArray[mFlpSessionSelectedIdx];

        if (sessionHandle != null) {
            IZatFlpService.IzatFlpRequest flpRequest =
                    mFlpSessionHandleRequestMap.get(sessionHandle);

            editSessionName.setText(sessionName);
            editTimeInterval.setText(Long.toString(flpRequest.getTimeInterval()));
            editDistanceInterval.setText(Integer.toString(flpRequest.getDistanceInterval()));
            editTripDistance.setText(Long.toString(flpRequest.getTripDistance()));

            if (flpRequest instanceof IZatFlpService.IzatFlpFgRequest) {
                radioGrpFlpMode.check(R.id.radio_Tracking);
            } else {
                IZatFlpService.IzatFlpBgRequest.IzatFlpBgRequestMode activeMode =
                        ((IZatFlpService.IzatFlpBgRequest)flpRequest).getActiveMode();
                if (activeMode == IZatFlpService.IzatFlpBgRequest.IzatFlpBgRequestMode.TRIP_MODE) {
                    radioGrpFlpMode.check(R.id.radio_Trip);
                } else {
                    radioGrpFlpMode.check(R.id.radio_Routine);
                }
            }
        }

        alertDialogBuilder.setPositiveButton("Switch",
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog,int id) {
                    try {
                        int modeSelectedId = radioGrpFlpMode.getCheckedRadioButtonId();
                        switch(modeSelectedId) {
                            case R.id.radio_Tracking:
                                sessionHandle.setToForeground();
                                mFlpSessionNeedRestartOnTripComplete.remove(sessionHandle);
                                break;
                            case R.id.radio_Routine:
                                sessionHandle.setToBackground();
                                mFlpSessionNeedRestartOnTripComplete.remove(sessionHandle);
                                break;
                            case R.id.radio_Trip:
                                sessionHandle.setToTripMode();
                                mFlpSessionNeedRestartOnTripComplete.add(sessionHandle);
                                break;
                            default:
                        }
                    } catch (IZatFeatureNotSupportedException e) {
                        Log.v(TAG, e.getMessage());
                    }
                }
            });

        alertDialogBuilder.setNegativeButton("Cancel",
        new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog,int id) {
                dialog.cancel();
            }
        });

        editSessionName.setFocusable(false);
        editTimeInterval.setFocusable(false);
        editDistanceInterval.setFocusable(false);
        editTripDistance.setFocusable(false);

        editSessionName.setTextColor(Color.GRAY);
        editTimeInterval.setTextColor(Color.GRAY);
        editDistanceInterval.setTextColor(Color.GRAY);
        editTripDistance.setTextColor(Color.GRAY);

        // create alert dialog
        AlertDialog alertDialog = alertDialogBuilder.create();

        // show it
        alertDialog.show();
     }

    public void clearCompletedTrip(String sessionName) {
        int selectedIdx = mFlpSessionNames.indexOf(sessionName);

        if (selectedIdx != -1) {
            Object sessionHandleArray[] = mFlpSessionHandleRequestMap.keySet().toArray();
            final IZatFlpService.IZatFlpSessionHandle sessionHandle =
                    (IZatFlpService.IZatFlpSessionHandle) sessionHandleArray[selectedIdx];
            if (sessionHandle != null) {
                if (mFlpSessionNeedRestartOnTripComplete.indexOf(sessionHandle) == -1) {
                    // trip did not need a restart to previous mode
                    mFlpSessionHandleRequestMap.remove(sessionHandle);
                    mSessionNameToHandleMap.remove(sessionName);

                    mFlpSessionNames.remove(selectedIdx);

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                         mFlpSessionListadapter.notifyDataSetChanged();
                        }
                    });
                }
                mFlpSessionNeedRestartOnTripComplete.remove(sessionHandle);
            } else {
                if (VERBOSE) {
                    Log.v(TAG, "No session handle found for this session:: " + sessionName);
                }
            }
        } else {
            if (VERBOSE) {
                Log.v(TAG, "No session running with this name: " + sessionName);
            }
        }
    }
}
