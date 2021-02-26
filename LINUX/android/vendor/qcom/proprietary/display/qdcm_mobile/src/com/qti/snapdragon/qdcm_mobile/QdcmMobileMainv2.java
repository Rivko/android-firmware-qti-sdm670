/* ====================================================================
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 * =====================================================================
 * @file QdcmMobileMainv2.java
 *
 */
package com.qti.snapdragon.qdcm_mobile;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.EnumSet;
import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore.MediaColumns;
import android.support.v4.app.ActionBarDrawerToggle;
import android.support.v4.widget.DrawerLayout;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.qti.snapdragon.sdk.display.ColorManager;
import com.qti.snapdragon.sdk.display.ColorManager.ColorManagerListener;
import com.qti.snapdragon.sdk.display.ColorManager.DCM_DISPLAY_TYPE;
import com.qti.snapdragon.sdk.display.ColorManager.DCM_FEATURE;
import com.qti.snapdragon.sdk.display.ColorManager.MODE_TYPE;
import com.qti.snapdragon.sdk.display.MemoryColorConfig;
import com.qti.snapdragon.sdk.display.MemoryColorConfig.MEMORY_COLOR_PARAMS;
import com.qti.snapdragon.sdk.display.ModeInfo;
import com.qti.snapdragon.sdk.display.PictureAdjustmentConfig;
import com.qti.snapdragon.sdk.display.MemoryColorConfig.MEMORY_COLOR_TYPE;
import com.qti.snapdragon.sdk.display.PictureAdjustmentConfig.PICTURE_ADJUSTMENT_PARAMS;
import com.qti.snapdragon.qdcm_mobile.QdcmMobileMainv2.ModeInfoWrapper;

@SuppressWarnings("deprecation")
public class QdcmMobileMainv2 extends Activity {

    private DrawerLayout mDrawerLayout;
    private ListView mDrawerList;
    private LinearLayout mDrawerListLayout;
    private ModeListAdapter mModeListAdapter;

    private boolean EditFirstLoad = true;
    private Menu menu;
    private LinearLayout mFeaturesPanel;

    private LinearLayout mColorTempPanel;
    private LinearLayout mHSICPanel;
    private LinearLayout mMemSkinPanel;
    private LinearLayout mMemSkyPanel;
    private LinearLayout mMemFoliagePanel;
    private FrameLayout mMainLayout;

    private Button mSliderAssistance;
    private static String TAG = "QDCM_Mobile_Main";
    private static String MAJOR_VERSION = "4";
    private static String MINOR_VERSION = "1";

    public enum FeatureType {
        COLOR_TEMP,
        PIC_ADJUSTMENT,
        MEM_COLOR_SKIN,
        MEM_COLOR_SKY,
        MEM_COLOR_FOLIAGE,
        NONE_SELECTED
    }

    private boolean mEditMode = false;
    private boolean mFeaturesHidden = false;
    private boolean mEdited = false;

    private FeatureType mSelectedFeature = FeatureType.NONE_SELECTED;
    ColorManager mCmgr;
    public static Bitmap bitmap = null;
    private static final int IMAGE_PICKER_SELECT = 999;

    private MenuItem mSetDefaultMenuItem;
    private MenuItem mEditModeMenuItem;
    private MenuItem mSaveModeMenuItem;
    private MenuItem mCancelChangesMenuItem;
    private MenuItem mBackToModeSelection;

    private Button mColorTempButton;
    private Button mPicAdjustmentButton;
    private Button mMemColorSkinButton;
    private Button mMemColorSkyButton;
    private Button mMemColorFoliageButton;

    private SeekBar mColorTempSeekBar;
    private SeekBar mPAHueSeekBar;
    private SeekBar mPASatSeekBar;
    private SeekBar mPAIntSeekBar;
    private SeekBar mPAContSeekBar;
    private SeekBar mSkinHueSeekBar;
    private SeekBar mSkinSatSeekBar;
    private SeekBar mSkyHueSeekBar;
    private SeekBar mSkySatSeekBar;
    private SeekBar mFoliageHueSeekBar;
    private SeekBar mFoliageSatSeekBar;

    private TextView mColorTempTextView;
    private TextView mPAHueTextView;
    private TextView mPASatTextView;
    private TextView mPAIntTextView;
    private TextView mPAContTextView;
    private TextView mSkinHueTextView;
    private TextView mSkinSatTextView;
    private TextView mSkyHueTextView;
    private TextView mSkySatTextView;
    private TextView mFoliageHueTextView;
    private TextView mFoliageSatTextView;

    // SDK related members
    public ArrayList<ModeInfoWrapper> mModeList;
    private int mDefaultModeID = -1;
    private PictureAdjustmentConfig mPAValues;
    private MemoryColorConfig mSkinValues, mSkyValues, mFoliageValues;
    private int mHueMIN, mHueMAX, mSatMIN, mSatMAX, mIntMIN, mIntMAX, mContMIN, mContMAX;
    private int mSkinHueMIN, mSkinHueMAX, mSkinSatMIN, mSkinSatMAX;
    private int mSkyHueMIN, mSkyHueMAX, mSkySatMIN, mSkySatMAX;
    private int mFoliageHueMIN, mFoliageHueMAX, mFoliageSatMIN, mFoliageSatMAX;
    private int mPA_Hue, mPA_Sat, mPA_Int, mPA_Cont;
    private int mColorTemp;
    private int mSkin_Hue, mSkin_Sat;
    private int mSky_Hue, mSky_Sat;
    private int mFoliage_Hue, mFoliage_Sat;
    private int mModeListPosition = 0;
    private int mModeToBeDeleted = -1;
    ProgressDialog progDialog;
    private ActionBarDrawerToggle mDrawerToggle;
    private ImageView mImageView = null;
    private boolean mAllowTuning = false;

    public class ModeInfoWrapper {
        public ModeInfo mode;
        public String modename;
        public int modeID;

        ModeInfoWrapper(ModeInfo displayMode) {
            mode = displayMode;
            modename = displayMode.getName();
            modeID = displayMode.getId();
        }

        @Override
        public String toString() {
            return modename;
        }

        public void resetName() {
            modename = mode.getName();
        }
    }
    ColorManagerListener colorinterface;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        colorinterface = new ColorManagerListener() {
            @Override
            public void onConnected() {
                setupApplication();
            }
        };

        int retVal = ColorManager.connect(this, colorinterface);
        if (retVal != ColorManager.RET_SUCCESS) {
            Log.e(TAG, "Connection failed");
        }
    }

    private void setupApplication() {
        Log.d(TAG, "Display ColorManager registered..");
        if (mCmgr == null) {
            mCmgr = ColorManager.getInstance(getApplication(), this,
                    DCM_DISPLAY_TYPE.DISP_PRIMARY);

            if (mCmgr == null) {
                throw new RuntimeException("Failed to get ColorManager instance.");
            }
        }

        // TODO: Make a proper way of checking the target
        UISetup();
        setupModeDrawer();
        populateModesOnScreen();
        updateBackgroundImage();
    }

    private void UISetup() {
        setContentView(R.layout.mainscreen);
        setTitle("QDCM Mobile v" + MAJOR_VERSION + "." + MINOR_VERSION);

        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        mDrawerList = (ListView) findViewById(R.id.leftdrawerlist);
        mDrawerListLayout = (LinearLayout) findViewById(R.id.left_drawer);
        mSliderAssistance = (Button)findViewById(R.id.expandModesList);

        mFeaturesPanel = (LinearLayout) findViewById(R.id.FeaturesLayout);
        mColorTempPanel = (LinearLayout) findViewById(R.id.linearLayoutCB);
        mHSICPanel = (LinearLayout) findViewById(R.id.linearLayoutHSIC);
        mMemSkinPanel = (LinearLayout) findViewById(R.id.linearLayoutMemSkin);
        mMemSkyPanel = (LinearLayout) findViewById(R.id.linearLayoutMemSky);
        mMemFoliagePanel = (LinearLayout) findViewById(R.id.linearLayoutMemFoliage);

        mColorTempSeekBar = (SeekBar) findViewById(R.id.ColorTemp_SeekBar);
        mPAHueSeekBar = (SeekBar) findViewById(R.id.Hue_SeekBar);
        mPASatSeekBar = (SeekBar) findViewById(R.id.Sat_SeekBar);
        mPAIntSeekBar = (SeekBar) findViewById(R.id.Intensity_SeekBar);
        mPAContSeekBar = (SeekBar) findViewById(R.id.Cont_SeekBar);
        mSkinHueSeekBar = (SeekBar) findViewById(R.id.SkinHue_SeekBar);
        mSkinSatSeekBar = (SeekBar) findViewById(R.id.SkinSat_SeekBar);
        mSkyHueSeekBar = (SeekBar) findViewById(R.id.SkyHue_SeekBar);
        mSkySatSeekBar = (SeekBar) findViewById(R.id.SkySat_SeekBar);
        mFoliageHueSeekBar = (SeekBar) findViewById(R.id.FoliageHue_SeekBar);
        mFoliageSatSeekBar = (SeekBar) findViewById(R.id.FoliageSat_SeekBar);

        mColorTempTextView = (TextView) findViewById(R.id.CB_Value);
        mPAHueTextView = (TextView) findViewById(R.id.Hue_Value);
        mPASatTextView = (TextView) findViewById(R.id.Sat_Value);
        mPAIntTextView = (TextView) findViewById(R.id.Intensity_Value);
        mPAContTextView = (TextView) findViewById(R.id.Cont_Value);
        mSkinHueTextView = (TextView) findViewById(R.id.SkinHue_Value);
        mSkinSatTextView = (TextView) findViewById(R.id.SkinSat_Value);
        mSkyHueTextView = (TextView) findViewById(R.id.SkyHue_Value);
        mSkySatTextView = (TextView) findViewById(R.id.SkySat_Value);
        mFoliageHueTextView = (TextView) findViewById(R.id.FoliageHue_Value);
        mFoliageSatTextView = (TextView) findViewById(R.id.FoliageSat_Value);

        mMainLayout = (FrameLayout) findViewById(R.id.mainLayout);
        mMainLayout.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {

                int action = event.getActionMasked();
                if (mEditMode) {
                    switch (action) {
                        case MotionEvent.ACTION_UP:
                            if (mFeaturesHidden)
                                showFeatureLayout(mSelectedFeature);
                            else
                                hideAllFeatureLayouts();
                            break;
                        default:
                            break;
                    }
                }
                return true;
            }
        });
    }

    private void setUpDrawerToggle(){
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

        mDrawerToggle = new ActionBarDrawerToggle(
                this,
                mDrawerLayout,
                R.drawable.nav_icon_panel,
                R.string.drawerOpen,
                R.string.drawerClose
        ) {
            @Override
            public void onDrawerClosed(View drawerView) {
                invalidateOptionsMenu();
            }

            @Override
            public void onDrawerOpened(View drawerView) {
                invalidateOptionsMenu();
            }
        };

        mDrawerLayout.post(new Runnable() {
            @Override
            public void run() {
                mDrawerToggle.syncState();
            }
        });
    }

    private boolean populateModesOnScreen() {
        ModeInfo[] modeDataArray = null;
        int selectedPosition = -1;

        if (mCmgr == null) {
            Log.e(TAG, "populateModesOnScreen(): Display SDK manager is null!");
            return false;
        }
        modeDataArray = mCmgr.getModes(MODE_TYPE.MODE_ALL);

        if (modeDataArray != null) {
            createModeList(modeDataArray);
            int[] activeMode = mCmgr.getActiveMode();
            mDefaultModeID = mCmgr.getDefaultMode();

            int i = 0;
            for (ModeInfoWrapper mode : mModeList) {
                // If this mode is active update the button
                if (activeMode[0] == mode.modeID) {
                    selectedPosition = i;
                }
                ++i;
            }

            mModeListAdapter = new ModeListAdapter(this, mModeList, mDefaultModeID);
            mDrawerList.setAdapter(mModeListAdapter);
            mDrawerList.setItemChecked(selectedPosition, true);
            mModeListPosition = selectedPosition;

            mDrawerList.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
                public boolean onItemLongClick(AdapterView<?> parent, View v, int position,long id)
                {
                    // Only allow deleting user modes
                    if (mModeList.get(position).mode.getModeType() ==
                            ColorManager.MODE_TYPE.MODE_USER) {
                        mDrawerList.setItemChecked(position, true);
                        deleteMode(mModeList.get(position).modeID, position);
                    }
                    return true;
                 }
            });

            if (mDefaultModeID == activeMode[0] && mSetDefaultMenuItem != null)
                mSetDefaultMenuItem.setVisible(false);

            mAllowTuning = true;
            return true;
        }
        else {
            mAllowTuning = false;
            Toast toast = Toast.makeText(getApplicationContext(),
                    "Display modes XML file not preloaded!", Toast.LENGTH_LONG);
            toast.setGravity(Gravity.CENTER, 0, 0);
            toast.show();
        }
        return false;
    }

    private boolean tuningAllowed() {
        if (mAllowTuning == false) {
            Toast toast = Toast.makeText(getApplicationContext(),
                    "Display modes XML file not preloaded!", Toast.LENGTH_LONG);
            toast.setGravity(Gravity.CENTER, 0, 0);
            toast.show();
            return false;
        }
        return true;
    }

    private void deleteMode(int modeId, int position) {
        AlertDialog.Builder alertDialog = new AlertDialog.Builder(this);

        alertDialog.setTitle("Mode Deletion.");
        int[] activeMode = mCmgr.getActiveMode();
        int defMode = mCmgr.getDefaultMode();

        alertDialog.setOnCancelListener(new OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                mDrawerList.setItemChecked(mModeListPosition, true);
            }
        });

        if (modeId == activeMode[0]) {
            alertDialog.setMessage("Cannot delete currently APPLIED mode..");
            alertDialog.show();
        }
        else if (modeId == defMode) {
            alertDialog.setMessage("Cannot delete DEFAULT display mode..");
            alertDialog.show();
        }
        else {
            alertDialog.setMessage("Are you sure you want to continue deleting this mode: " +
                    mModeList.get(position).modename + "?");
            alertDialog.setCancelable(false);

            mModeToBeDeleted = modeId;
            alertDialog.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int id) {

                    mCmgr.deleteMode(mModeToBeDeleted);
                    Toast toast = Toast.makeText(getApplicationContext(),
                        "Mode Deleted...",
                        Toast.LENGTH_LONG);
                    toast.setGravity(Gravity.CENTER, 0, 0);
                    toast.show();
                    populateModesOnScreen();
                }
            });
            alertDialog.setNegativeButton("No",  new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int id) {
                    mDrawerList.setItemChecked(mModeListPosition, true);
                }
            });
            alertDialog.show();
        }
    }

    private void createModeList(ModeInfo[] pa) {
        mModeList = new ArrayList<ModeInfoWrapper>();
        for (ModeInfo i : pa)
            mModeList.add(new ModeInfoWrapper(i));
    }

    private void setupModeDrawer() {
        mDrawerList.setOnItemClickListener(new DrawerItemClickListener());

        mDrawerLayout.setScrimColor(Color.TRANSPARENT);
        mDrawerLayout.setDrawerListener(new DrawerLayoutListener());
        mDrawerLayout.openDrawer(mDrawerListLayout);

        setUpDrawerToggle();
        mDrawerLayout.setDrawerListener(mDrawerToggle);
    }

    private class DrawerItemClickListener implements ListView.OnItemClickListener {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            selectItem(position);
        }
    }

    private class DrawerLayoutListener implements DrawerLayout.DrawerListener {
        @Override
        public void onDrawerClosed(View view) {
            mSliderAssistance.setVisibility(android.view.View.VISIBLE);
        }

        @Override
        public void onDrawerOpened(View arg0) {
            mSliderAssistance.setVisibility(android.view.View.INVISIBLE);
        }

        @Override
        public void onDrawerSlide(View arg0, float arg1) {
            // TODO Auto-generated method stub

        }

        @Override
        public void onDrawerStateChanged(int arg0) {
            // TODO Auto-generated method stub

        }
    }

    private void selectItem(int position) {
        mDrawerList.setItemChecked(mModeListPosition, true);

        // Highlight the selected item, update the title, and close the drawer
        mDrawerList.setItemChecked(position, true);

        mModeListPosition = position;
        mCmgr.setActiveMode(mModeList.get(mModeListPosition).modeID);

        if (mEditMode) {
            mDrawerLayout.closeDrawer(mDrawerListLayout);

            mEditModeMenuItem.setVisible(false);
            mSetDefaultMenuItem.setVisible(false);
            mSaveModeMenuItem.setVisible(true);
            mCancelChangesMenuItem.setVisible(true);

            populateSliderPositions(true);
            // UpdateView..
            mFeaturesPanel.setVisibility(android.view.View.VISIBLE);

            setTitle("  " + mModeList.get(mModeListPosition).modename + " [EDIT]");
            getActionBar().setIcon(
                new ColorDrawable(getResources().getColor(android.R.color.transparent)));
        }
        else {
            mEditModeMenuItem.setVisible(true);
            mSetDefaultMenuItem.setVisible(true);
            mSaveModeMenuItem.setVisible(false);
            mCancelChangesMenuItem.setVisible(false);

            mFeaturesPanel.setVisibility(android.view.View.INVISIBLE);

            setTitle("QDCM Mobile v" + MAJOR_VERSION + "." + MINOR_VERSION);
            getActionBar().setIcon(R.drawable.ic_launcher);
        }

        if (isDefaultMode())
            mSetDefaultMenuItem.setVisible(false);

        mEdited = false;
        // Un-comment to implement "closeDrawer" on item click
        //mDrawerLayout.closeDrawer(mDrawerList);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater menuInf = getMenuInflater();
        menuInf.inflate(R.menu.menu, menu);

        this.menu = menu;

        setupMenuItems();
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        AlertDialog.Builder alertDialog;
        switch(item.getItemId())
        {
            case R.id.editMode:
                if (!tuningAllowed()) {
                    return false;
                }

                mEditMode = true;
                if (EditFirstLoad) {
                    progDialog = ProgressDialog.show(this, "loading...", null, true);
                    new Thread(new Runnable() {
                        public void run() {
                            try {
                                // sleep the thread, whatever time you want.
                                Thread.sleep(1000);
                            } catch (Exception e) {
                            }
                            progDialog.dismiss();
                        }
                    }).start();
                    // Setup Feature Menu Item at the bottom
                    setupHorizontalScrollView();
                    selectFeatureLayout();
                    // Pre-populate the slider position for all features
                    populateSliderPositions(false);
                    EditFirstLoad = false;
                }
                else {
                    populateSliderPositions(true);
                }

                mDrawerList.setBackgroundColor(Color.WHITE);

                mEditModeMenuItem.setVisible(false);
                mSetDefaultMenuItem.setVisible(false);
                mSaveModeMenuItem.setVisible(true);
                mCancelChangesMenuItem.setVisible(true);

                setTitle("  " + mModeList.get(mModeListPosition).modename + " [EDIT]");
                getActionBar().setIcon(
                    new ColorDrawable(getResources().getColor(android.R.color.transparent)));

                mDrawerLayout.closeDrawer(mDrawerListLayout);
                mFeaturesPanel.setVisibility(android.view.View.VISIBLE);
                mBackToModeSelection.setVisible(true);
                fitBackgroundImageOnTop();
                break;
            case R.id.setDefault:
                if (!tuningAllowed()) {
                    return false;
                }

                alertDialog = new AlertDialog.Builder(this);
                alertDialog.setTitle("Do you want to set default mode?");
                alertDialog.setMessage("Choosing a mode as default will make the mode persistant across power cycle.");
                alertDialog.setCancelable(false);
                alertDialog.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        int[] activeMode = mCmgr.getActiveMode();
                        mCmgr.setDefaultMode(activeMode[0]);
                        populateModesOnScreen();
                        mSetDefaultMenuItem.setVisible(false);
                    }
                });
                alertDialog.setNegativeButton("No", null);
                alertDialog.show();
                break;
            case R.id.saveChanges:
                alertDialog = new AlertDialog.Builder(this);
                alertDialog.setTitle("Saving mode.");
                if (!mEdited) {
                    alertDialog.setMessage("No changes made!");
                }
                else {
                    String modename = mModeList.get(mModeListPosition).modename;
                    alertDialog.setTitle("Do you want to save changes?");
                    String saveChangesMsg = "Saving changes will overwrite " + modename +
                            " mode. Rename the mode name to save changes as a new mode.\n" +
                            "Note: Factory modes cannot be overwritten.";
                    alertDialog.setMessage(saveChangesMsg);

                    final EditText input = new EditText(this);
                    input.setScaleX((float) 0.9);
                    input.setText(modename);
                    alertDialog.setView(input);

                    alertDialog.setPositiveButton("Save", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            String newModeName = input.getText().toString();
                            ModeInfoWrapper mode;
                            int modeId = -1;

                            if (mCmgr != null) {

                                Toast toast;
                                if ((mode = modeExists(newModeName)) != null) {
                                    if (isFactoryMode(mode)) {
                                        toast = Toast.makeText(getApplicationContext(),
                                            "Cannot modify FACTORY modes.",
                                            Toast.LENGTH_LONG);
                                        toast.setGravity(Gravity.CENTER, 0, 0);
                                        toast.show();
                                        return;
                                    }
                                    else {
                                        // Modify mode
                                        int retVal = mCmgr.modifyMode(mode.modeID, newModeName);
                                        if (retVal < 0) {
                                            toast = Toast.makeText(getApplicationContext(),
                                                    "Error: In modifying mode \"" + newModeName +"\"",
                                                    Toast.LENGTH_LONG);
                                            toast.setGravity(Gravity.CENTER, 0, 0);
                                            toast.show();
                                            return;
                                        }
                                        modeId = mode.modeID;
                                    }
                                }
                                else {
                                    // Create new mode
                                    modeId = mCmgr.createNewMode(newModeName);
                                    if (modeId < 0) {
                                        toast = Toast.makeText(getApplicationContext(),
                                                "Error: In creating new mode \"" + newModeName +"\"",
                                                Toast.LENGTH_LONG);
                                        toast.setGravity(Gravity.CENTER, 0, 0);
                                        toast.show();
                                        return;
                                    }
                                }
                            }
                            mEdited = false;

                            Log.e(TAG, "modeId=" + modeId);
                            if (mCmgr != null) {
                                mCmgr.setActiveMode(modeId);
                            }
                            setTitle("  " + newModeName + " [EDIT]");

                            populateModesOnScreen();
                            populateSliderPositions(true);
                            mDrawerLayout.openDrawer(mDrawerListLayout);
                        }
                    });
                    alertDialog.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            // Canceled.
                        }
                    });
                }
                alertDialog.show();
                break;
            case R.id.cancelChanges:
                alertDialog = new AlertDialog.Builder(this);
                if (!mEdited) {
                    alertDialog.setTitle("Cancel changes.");
                    alertDialog.setMessage("No changes made!");
                }
                else {
                    alertDialog.setTitle("Do you want to cancel changes?");
                    alertDialog.setMessage("Proceeding with this option will revert all the unsaved changes.");
                    alertDialog.setCancelable(false);
                    alertDialog.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            int[] activeMode = mCmgr.getActiveMode();
                            mCmgr.setActiveMode(activeMode[0]);
                            populateSliderPositions(true);
                            mEdited = false;
                        }
                    });
                    alertDialog.setNegativeButton("No", null);
                }
                alertDialog.show();
                break;
            case R.id.backToMode:
                BackToModeSelection();
                break;
            case R.id.changeBack:
                Intent i = new Intent(Intent.ACTION_PICK,
                        android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                startActivityForResult(i, IMAGE_PICKER_SELECT);
                break;
            default:
                mDrawerToggle.onOptionsItemSelected(item);
                break;
        }
        return true;
    }

    private ModeInfoWrapper modeExists(String modeName) {
        for (ModeInfoWrapper mode : mModeList) {
            if (mode.modename.equals(modeName)) {
                return mode;
            }
        }
        return null;
    }

    private boolean isFactoryMode(ModeInfoWrapper mode) {
        if (mode.mode.getModeType() == ColorManager.MODE_TYPE.MODE_SYSTEM)
            return true;
        return false;
    }

    @Override
    public void onBackPressed() {
        if (mEditMode) {
            BackToModeSelection();
            return;
        }
        super.onBackPressed();
    }

    private void BackToModeSelection() {
        mDrawerList.setBackgroundColor(0x88ffffff);
        mEditModeMenuItem.setVisible(true);
        mSetDefaultMenuItem.setVisible(true);
        mSaveModeMenuItem.setVisible(false);
        mCancelChangesMenuItem.setVisible(false);
        setTitle("QDCM Mobile v" + MAJOR_VERSION + "." + MINOR_VERSION);
        getActionBar().setIcon(R.drawable.ic_launcher);
        mDrawerLayout.openDrawer(mDrawerListLayout);
        mFeaturesPanel.setVisibility(android.view.View.INVISIBLE);
        mEditMode = false;
        mBackToModeSelection.setVisible(false);
        fitBackgroundImageOnCenter();

        if (isDefaultMode())
            mSetDefaultMenuItem.setVisible(false);
    }

    public void updateBackgroundImage() {
        mImageView = (ImageView) findViewById(R.id.surfaceimage);
        // Populate the image with a default image
        try {
            InputStream defaultImage = getAssets().open(
                    getResources().getString(R.string.default_image));
            QdcmMobileMainv2.bitmap = BitmapFactory.decodeStream(defaultImage);
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (null != QdcmMobileMainv2.bitmap) {
            mImageView.setImageBitmap(QdcmMobileMainv2.bitmap);
            fitBackgroundImageOnCenter();
        }
    }

    public void fitBackgroundImageOnTop() {
        mImageView.setScaleType(ImageView.ScaleType.FIT_START);
    }

    public void fitBackgroundImageOnCenter() {
        mImageView.setScaleType(ImageView.ScaleType.FIT_CENTER);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == IMAGE_PICKER_SELECT
                && resultCode == Activity.RESULT_OK) {
            // Log.i(TAG, "onActivityResult on end of photo picker activity");
            QdcmMobileMainv2.bitmap = getBitmapFromCameraData(data, this);
            ImageView im = (ImageView) findViewById(R.id.surfaceimage);

            if (null != QdcmMobileMainv2.bitmap) {
                im.setImageBitmap(QdcmMobileMainv2.bitmap);
            }
        }
    }

    /**
     * Use for decoding camera response data.
     */
    public static Bitmap getBitmapFromCameraData(Intent data, Context context) {
        Uri selectedImage = data.getData();
        String[] filePathColumn = { MediaColumns.DATA };
        Cursor cursor = context.getContentResolver().query(selectedImage,
                filePathColumn, null, null, null);
        cursor.moveToFirst();
        int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
        String picturePath = cursor.getString(columnIndex);
        cursor.close();
        return BitmapFactory.decodeFile(picturePath);
    }

    private void setupMenuItems() {
        mSetDefaultMenuItem = menu.findItem(R.id.setDefault);
        mEditModeMenuItem = menu.findItem(R.id.editMode);
        mSaveModeMenuItem = menu.findItem(R.id.saveChanges);
        mCancelChangesMenuItem = menu.findItem(R.id.cancelChanges);
        mBackToModeSelection = menu.findItem(R.id.backToMode);

        // check the if the currently applied mode is the default mode
        boolean isCurrentModeDefault = isDefaultMode();

        if (mEditMode) {
            mSaveModeMenuItem.setVisible(true);
            mCancelChangesMenuItem.setVisible(true);
            mBackToModeSelection.setVisible(true);
        }
        else {
            if (isCurrentModeDefault)
                mSetDefaultMenuItem.setVisible(false);
            else
                mSetDefaultMenuItem.setVisible(true);

            mEditModeMenuItem.setVisible(true);
            mSaveModeMenuItem.setVisible(false);
            mCancelChangesMenuItem.setVisible(false);
            mBackToModeSelection.setVisible(false);
        }
    }

    private boolean isDefaultMode() {
        if (mCmgr == null) {
            Log.d(TAG,"Display SDK is not yet initialized");
            return false;
        }

        int[] activeMode = mCmgr.getActiveMode();
        return (activeMode[0] == mDefaultModeID);
    }

    private void selectFeatureLayout() {
        Button button = null;
        resetFeatureButtonsBackground();
        hideAllFeatureLayouts();

        if (mSelectedFeature == FeatureType.NONE_SELECTED) {
            mSelectedFeature = FeatureType.COLOR_TEMP;
        }

        switch (mSelectedFeature) {
            case COLOR_TEMP:
                button = mColorTempButton;
                mColorTempPanel.setVisibility(View.VISIBLE);
                break;
            case PIC_ADJUSTMENT:
                button = mPicAdjustmentButton;
                mHSICPanel.setVisibility(View.VISIBLE);
                break;
            case MEM_COLOR_SKIN:
                button = mMemColorSkinButton;
                mMemSkinPanel.setVisibility(View.INVISIBLE);
                break;
            case MEM_COLOR_SKY:
                button = mMemColorSkyButton;
                mMemSkyPanel.setVisibility(View.INVISIBLE);
                break;
            case MEM_COLOR_FOLIAGE:
                button = mMemColorFoliageButton;
                mMemFoliagePanel.setVisibility(View.INVISIBLE);
                break;
            default:
                break;
        }

        if (button != null) {
            button.setBackground(this.getResources().getDrawable(R.drawable.buttonshapeselected));
            button.setTextColor(Color.BLACK);
        }
        mFeaturesHidden = false;
    }

    private void hideAllFeatureLayouts() {
        mColorTempPanel.setVisibility(View.GONE);
        mHSICPanel.setVisibility(View.GONE);
        mMemSkinPanel.setVisibility(View.GONE);
        mMemSkyPanel.setVisibility(View.GONE);
        mMemFoliagePanel.setVisibility(View.GONE);
        mFeaturesHidden = true;
    }

    private void showFeatureLayout(FeatureType feature) {
        switch (feature) {
            case COLOR_TEMP:
                mColorTempPanel.setVisibility(android.view.View.VISIBLE);
                break;
            case PIC_ADJUSTMENT:
                mHSICPanel.setVisibility(android.view.View.VISIBLE);
                break;
            case MEM_COLOR_SKIN:
                mMemSkinPanel.setVisibility(android.view.View.VISIBLE);
                break;
            case MEM_COLOR_SKY:
                mMemSkyPanel.setVisibility(android.view.View.VISIBLE);
                break;
            case MEM_COLOR_FOLIAGE:
                mMemFoliagePanel.setVisibility(android.view.View.VISIBLE);
                break;
            default:
                break;
        }
        mFeaturesHidden = false;
    }

    private void resetFeatureButtonsBackground() {
        mColorTempButton.setBackgroundColor(R.drawable.buttonshape);
        mPicAdjustmentButton.setBackgroundColor(R.drawable.buttonshape);
        mMemColorSkinButton.setBackgroundColor(R.drawable.buttonshape);
        mMemColorSkyButton.setBackgroundColor(R.drawable.buttonshape);
        mMemColorFoliageButton.setBackgroundColor(R.drawable.buttonshape);

        mColorTempButton.setTextColor(Color.WHITE);
        mPicAdjustmentButton.setTextColor(Color.WHITE);
        mMemColorSkinButton.setTextColor(Color.WHITE);
        mMemColorSkyButton.setTextColor(Color.WHITE);
        mMemColorFoliageButton.setTextColor(Color.WHITE);

        mMemColorSkinButton.setVisibility(View.GONE);
        mMemColorSkyButton.setVisibility(View.GONE);
        mMemColorFoliageButton.setVisibility(View.GONE);
    }

    private void setupHorizontalScrollView() {

        mColorTempButton = (Button) findViewById(R.id.button_color_temp);
        mPicAdjustmentButton = (Button) findViewById(R.id.button_pic_adjust);
        mMemColorSkinButton = (Button) findViewById(R.id.button_mem_skin);
        mMemColorSkyButton = (Button) findViewById(R.id.button_mem_sky);
        mMemColorFoliageButton = (Button) findViewById(R.id.button_mem_foliage);

        mColorTempButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mSelectedFeature = FeatureType.COLOR_TEMP;
                selectFeatureLayout();
            }
        });

        mMemColorSkinButton.setVisibility(View.GONE);
        mMemColorSkyButton.setVisibility(View.GONE);
        mMemColorFoliageButton.setVisibility(View.GONE);

        mPicAdjustmentButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mSelectedFeature = FeatureType.PIC_ADJUSTMENT;
                selectFeatureLayout();
            }
        });

        mMemColorSkinButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mSelectedFeature = FeatureType.MEM_COLOR_SKIN;
                selectFeatureLayout();
            }
        });

        mMemColorSkyButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mSelectedFeature = FeatureType.MEM_COLOR_SKY;
                selectFeatureLayout();
            }
        });

        mMemColorFoliageButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                mSelectedFeature = FeatureType.MEM_COLOR_FOLIAGE;
                selectFeatureLayout();
            }
        });
    }

    private void populateSliderPositions(boolean updateSliderOnly) {

        if (mCmgr != null) {

            // Populate Picture Adjustment content
            boolean isSupported = false;
            isSupported = mCmgr
                .isFeatureSupported(DCM_FEATURE.FEATURE_GLOBAL_PICTURE_ADJUSTMENT);
            if (isSupported) {
                mPAValues = mCmgr.getPictureAdjustmentParams();
                if (mPAValues != null) {
                    setupPAHue(updateSliderOnly);
                    setupPASaturation(updateSliderOnly);
                    setupPAIntensity(updateSliderOnly);
                    setupPAContrast(updateSliderOnly);
                }
            }

            // Populate Color Temperature content
            isSupported = mCmgr
                .isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_BALANCE);
            if (isSupported) {
                mColorTemp = mCmgr.getColorBalance();
                setupColorTemp(updateSliderOnly);
            }

            // Populate Memory Color content
            isSupported = mCmgr
                .isFeatureSupported(DCM_FEATURE.FEATURE_MEMORY_COLOR_ADJUSTMENT);
            if (isSupported) {
                // SKIN
                mSkinValues = mCmgr.getMemoryColorParams(MEMORY_COLOR_TYPE.SKIN);
                if (mSkinValues != null) {
                    setupSkinHue(updateSliderOnly);
                    setupSkinSat(updateSliderOnly);
                }

                // SKY
                mSkyValues = mCmgr.getMemoryColorParams(MEMORY_COLOR_TYPE.SKY);
                if (mSkyValues != null) {
                    setupSkyHue(updateSliderOnly);
                    setupSkySat(updateSliderOnly);
                }

                // FOLIAGE
                mFoliageValues = mCmgr.getMemoryColorParams(MEMORY_COLOR_TYPE.FOLIAGE);
                if (mFoliageValues != null) {
                    setupFoliageHue(updateSliderOnly);
                    setupFoliageSat(updateSliderOnly);
                }
            }

        }
    }

    private void setupColorTemp(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mColorTempSeekBar.setMax(200);
            mColorTempSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) { }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) { }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mColorTemp = progress - 100;
                    mCmgr.setColorBalance(mColorTemp);
                    mColorTempTextView.setText("" + mColorTemp);
                    mEdited = true;
                }
            });
        }

        if (mColorTemp < ColorManager.COLOR_BALANCE_WARMTH_LOWER_BOUND
                || mColorTemp > ColorManager.COLOR_BALANCE_WARMTH_UPPER_BOUND) {
            mColorTemp = 0;
        }
        Integer sliderVal = mColorTemp + 100;

        mColorTempSeekBar.setProgress(sliderVal);
        mColorTempTextView.setText("" + mColorTemp);

    }

    private void setupPAHue(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mHueMAX = mCmgr
                .getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.HUE);
            mHueMIN = mCmgr
                .getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.HUE);
            mPAHueSeekBar.setMax(mHueMAX - mHueMIN);

            mPAHueSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mPAHueTextView.setText("" + progress);
                    mPA_Hue = progress;
                    if (fromUser)
                        setPaDisplayParams();
                    mEdited = true;
                }
            });
        }

        mPA_Hue = mPAValues.getHue();
        Integer sliderVal = mPA_Hue;
        mPAHueSeekBar.setProgress(sliderVal);
        mPAHueTextView.setText("" + sliderVal);
    }

    private void setupPASaturation(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mSatMAX = mCmgr
                .getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.SATURATION);
            mSatMIN = mCmgr
                .getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.SATURATION);
            mPASatSeekBar.setMax(mSatMAX - mSatMIN);

            mPASatSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mPASatTextView.setText("" + progress);
                    mPA_Sat = progress;
                    if (fromUser)
                        setPaDisplayParams();
                    mEdited = true;
                }
            });
        }

        mPA_Sat = mPAValues.getSaturation();
        Integer sliderVal = mPA_Sat;
        mPASatSeekBar.setProgress(sliderVal);
        mPASatTextView.setText("" + sliderVal);
    }

    private void setupPAIntensity(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mIntMAX = mCmgr
                .getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.INTENSITY);
            mIntMIN = mCmgr
                .getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.INTENSITY);
            mPAIntSeekBar.setMax(mIntMAX - mIntMIN);

            mPAIntSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mPAIntTextView.setText("" + progress);
                    mPA_Int = progress;
                    if (fromUser)
                        setPaDisplayParams();
                    mEdited = true;
                }
            });
        }

        mPA_Int = mPAValues.getIntensity();
        Integer sliderVal = mPA_Int;
        mPAIntSeekBar.setProgress(sliderVal);
        mPAIntTextView.setText("" + sliderVal);
    }

    private void setupPAContrast(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mContMAX = mCmgr
                .getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.CONTRAST);
            mContMIN = mCmgr
                .getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.CONTRAST);
            mPAContSeekBar.setMax(mContMAX - mContMIN);

            mPAContSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mPAContTextView.setText("" + progress);
                    mPA_Cont = progress;
                    if (fromUser)
                        setPaDisplayParams();
                    mEdited = true;
                }
            });
        }

        mPA_Cont = mPAValues.getContrast();
        Integer sliderVal = mPA_Cont;
        mPAContSeekBar.setProgress(sliderVal);
        mPAContTextView.setText("" + sliderVal);
    }

    private void setupSkinHue(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mSkinHueMAX = mCmgr
                .getMaxLimitMemoryColor(MEMORY_COLOR_TYPE.SKIN,
                        MEMORY_COLOR_PARAMS.HUE);
            mSkinHueMIN = mCmgr
                .getMinLimitMemoryColor(MEMORY_COLOR_TYPE.SKIN,
                        MEMORY_COLOR_PARAMS.HUE);
            mSkinHueSeekBar.setMax(mSkinHueMAX - mSkinHueMIN);

            mSkinHueSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mSkin_Hue = progress + mSkinHueMIN;
                    setSkinDisplayPrams();
                    mSkinHueTextView.setText("" + mSkin_Hue);
                    mEdited = true;
                }
            });
        }

        mSkin_Hue = mSkinValues.getHue();
        Integer sliderVal = mSkin_Hue - mSkinHueMIN;
        mSkinHueSeekBar.setProgress(sliderVal);
        mSkinHueTextView.setText("" + mSkin_Hue);
    }

    private void setupSkinSat(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mSkinSatMAX = mCmgr
                .getMaxLimitMemoryColor(MEMORY_COLOR_TYPE.SKIN,
                        MEMORY_COLOR_PARAMS.SATURATION);
            mSkinSatMIN = mCmgr
                .getMinLimitMemoryColor(MEMORY_COLOR_TYPE.SKIN,
                        MEMORY_COLOR_PARAMS.SATURATION);
            mSkinSatSeekBar.setMax(mSkinSatMAX - mSkinSatMIN);

            mSkinSatSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mSkin_Sat = progress + mSkinSatMIN;
                    setSkinDisplayPrams();
                    mSkinSatTextView.setText("" + mSkin_Sat);
                    mEdited = true;
                }
            });
        }

        mSkin_Sat = mSkinValues.getSaturation();
        Integer sliderVal = mSkin_Sat - mSkinSatMIN;
        mSkinSatSeekBar.setProgress(sliderVal);
        mSkinSatTextView.setText("" + mSkin_Sat);
    }

    private void setupSkyHue(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mSkyHueMAX = mCmgr
                .getMaxLimitMemoryColor(MEMORY_COLOR_TYPE.SKY,
                        MEMORY_COLOR_PARAMS.HUE);
            mSkyHueMIN = mCmgr
                .getMinLimitMemoryColor(MEMORY_COLOR_TYPE.SKY,
                        MEMORY_COLOR_PARAMS.HUE);
            mSkyHueSeekBar.setMax(mSkyHueMAX - mSkyHueMIN);

            mSkyHueSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mSky_Hue = progress + mSkyHueMIN;
                    setSkyDisplayPrams();
                    mSkyHueTextView.setText("" + mSky_Hue);
                    mEdited = true;
                }
            });
        }

        mSky_Hue = mSkyValues.getHue();
        Integer sliderVal = mSky_Hue - mSkyHueMIN;
        mSkyHueSeekBar.setProgress(sliderVal);
        mSkyHueTextView.setText("" + mSky_Hue);
    }

    private void setupSkySat(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mSkySatMAX = mCmgr
                .getMaxLimitMemoryColor(MEMORY_COLOR_TYPE.SKY,
                        MEMORY_COLOR_PARAMS.SATURATION);
            mSkySatMIN = mCmgr
                .getMinLimitMemoryColor(MEMORY_COLOR_TYPE.SKY,
                        MEMORY_COLOR_PARAMS.SATURATION);
            mSkySatSeekBar.setMax(mSkySatMAX - mSkySatMIN);

            mSkySatSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mSky_Sat = progress + mSkySatMIN;
                    setSkyDisplayPrams();
                    mSkySatTextView.setText("" + mSky_Sat);
                    mEdited = true;
                }
            });
        }

        mSky_Sat = mSkyValues.getSaturation();
        Integer sliderVal = mSky_Sat - mSkySatMIN;
        mSkySatSeekBar.setProgress(sliderVal);
        mSkySatTextView.setText("" + mSky_Sat);
    }

    private void setupFoliageHue(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mFoliageHueMAX = mCmgr
                .getMaxLimitMemoryColor(MEMORY_COLOR_TYPE.FOLIAGE,
                        MEMORY_COLOR_PARAMS.HUE);
            mFoliageHueMIN = mCmgr
                .getMinLimitMemoryColor(MEMORY_COLOR_TYPE.FOLIAGE,
                        MEMORY_COLOR_PARAMS.HUE);
            mFoliageHueSeekBar.setMax(mFoliageHueMAX - mFoliageHueMIN);

            mFoliageHueSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mFoliage_Hue = progress + mFoliageHueMIN;
                    setFoliageDisplayPrams();
                    mFoliageHueTextView.setText("" + mFoliage_Hue);
                    mEdited = true;
                }
            });
        }

        mFoliage_Hue = mFoliageValues.getHue();
        Integer sliderVal = mFoliage_Hue - mFoliageHueMIN;
        mFoliageHueSeekBar.setProgress(sliderVal);
        mFoliageHueTextView.setText("" + mFoliage_Hue);
    }

    private void setupFoliageSat(boolean updateSliderOnly) {

        if (!updateSliderOnly) {
            mFoliageSatMAX = mCmgr
                .getMaxLimitMemoryColor(MEMORY_COLOR_TYPE.FOLIAGE,
                        MEMORY_COLOR_PARAMS.SATURATION);
            mFoliageSatMIN = mCmgr
                .getMinLimitMemoryColor(MEMORY_COLOR_TYPE.FOLIAGE,
                        MEMORY_COLOR_PARAMS.SATURATION);
            mFoliageSatSeekBar.setMax(mFoliageSatMAX - mFoliageSatMIN);

            mFoliageSatSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {
                }

                @Override
                public void onProgressChanged(SeekBar seekBar, int progress,
                    boolean fromUser) {
                    mFoliage_Sat = progress + mFoliageSatMIN;
                    setFoliageDisplayPrams();
                    mFoliageSatTextView.setText("" + mFoliage_Sat);
                    mEdited = true;
                }
            });
        }

        mFoliage_Sat = mFoliageValues.getSaturation();
        Integer sliderVal = mFoliage_Sat - mFoliageSatMIN;
        mFoliageSatSeekBar.setProgress(sliderVal);
        mFoliageSatTextView.setText("" + mFoliage_Sat);
    }

    private void setPaDisplayParams() {
        PictureAdjustmentConfig newPaConfig = new PictureAdjustmentConfig(
                EnumSet.allOf(PictureAdjustmentConfig.PICTURE_ADJUSTMENT_PARAMS.class),
                mPA_Hue, mPA_Sat, mPA_Int, mPA_Cont, mPAValues.getSaturationThreshold());
        mCmgr.setPictureAdjustmentParams(newPaConfig);
    }

    private void setSkinDisplayPrams() {
        MemoryColorConfig newMcSkinConfig = new MemoryColorConfig(
                MEMORY_COLOR_TYPE.SKIN, mSkin_Hue, mSkin_Sat,
                mSkinValues.getIntensity());
        mCmgr.setMemoryColorParams(newMcSkinConfig);
    }

    private void setSkyDisplayPrams() {
        MemoryColorConfig newMcSkyConfig = new MemoryColorConfig(
                MEMORY_COLOR_TYPE.SKY, mSky_Hue, mSky_Sat,
                mSkyValues.getIntensity());
        mCmgr.setMemoryColorParams(newMcSkyConfig);
    }

    private void setFoliageDisplayPrams() {
        MemoryColorConfig newMcFoliageConfig = new MemoryColorConfig(
                MEMORY_COLOR_TYPE.FOLIAGE, mFoliage_Hue, mFoliage_Sat,
                mFoliageValues.getIntensity());
        mCmgr.setMemoryColorParams(newMcFoliageConfig);
    }
}

class ModeListAdapter extends BaseAdapter {
    ArrayList<ModeInfoWrapper> mModeList;
    private Context mContext;
    private Integer mDefaultMode;

    public ModeListAdapter(Context context, ArrayList<ModeInfoWrapper> modeList,
            Integer defaultMode) {
        mContext = context;
        mModeList = modeList;
        mDefaultMode = defaultMode;
    }

    @Override
    public int getCount() {
        return mModeList.size();
    }

    @Override
    public Object getItem(int position) {
        return mModeList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return mModeList.get(position).modeID;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View rowView = null;
        if (convertView == null) {
            LayoutInflater inflater = (LayoutInflater) mContext.
                getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            rowView = inflater.inflate(R.layout.drawer_list_item, parent, false);
        }
        else {
            rowView = convertView;
        }

        TextView mode = (TextView)rowView.findViewById(R.id.modenametext);
        ImageView icon = (ImageView)rowView.findViewById(R.id.modetypeicon);

        mode.setText(mModeList.get(position).modename);
        mode.setTextColor(mContext.getResources().getColor(R.color.black));

        if (mModeList.get(position).mode.getModeType() ==
                ColorManager.MODE_TYPE.MODE_USER)
            icon.setImageResource(R.drawable.list_user);
        else
            icon.setImageResource(R.drawable.list_factory);

        mode.setTypeface(null, Typeface.NORMAL);
        if (mModeList.get(position).modeID == mDefaultMode &&
                mode.getText().equals(mModeList.get(position).modename)) {
            mode.setTypeface(null, Typeface.BOLD);
        }

        return rowView;
    }
}

