/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.Util;

import android.app.AlertDialog;
import android.app.NotificationManager;
import android.content.ContentUris;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.graphics.Color;
import android.location.Address;
import android.location.Geocoder;
import android.media.MediaMetadataRetriever;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.provider.MediaStore;
import android.support.v4.app.NotificationCompat;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.util.Pair;
import android.view.WindowManager;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Locale;
import java.util.Set;

import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.R;

/**
 * Created by mithun on 7/24/15.
 */
public class Utils {
    private static final String TAG = Utils.class.getSimpleName();
    public static final String ACTION_DIAG_MSG_RECEIVED = "video-summary-local-events";
    public static final String DIAG_EXTRA_MESSAGE = "video-summary-local-events-extra-message";
    public static final String DIAG_EXTRA_TAG = "video-summary-local-events-extra-tag";
    public static final String TOAST_EXTRA_MESSAGE = "video-summary-local-events-extra-toast-message";

    public static final String DIAG_EXTRA_DBG_FRAME_DATA = "video-summary-local-events-debug-frame-data";
    public static final String DIAG_EXTRA_DBG_FRAME_WIDTH = "video-summary-local-events-debug-frame-width";
    public static final String DIAG_EXTRA_DBG_FRAME_HEIGHT = "video-summary-local-events-debug-frame-height";
    public static final String DIAG_EXTRA_DBG_FRAME_SCORE = "video-summary-local-events-debug-frame-score";
    public static final String DIAG_EXTRA_DBG_FRAME_NUM = "video-summary-local-events-debug-frame-num";

    public static final String CONFIG_FILE_DIR = "/scveData/";

    public static final long ALARM_TIMEOUT = 5 * 60 * 1000;//5 minutes timeout

    public static final long TIMELAPSE_MIN_DURATION = 5 * 60 * 1000;//5 minutes for videos to be considered for timelapse in auto mode

    public static final String VIDEO_SUMMARY_DIRECTORY = Environment.getExternalStorageDirectory().getAbsolutePath() + "/VideoSummary/";

    public static void sendMessage(Context context, String TAG, String msg) {
        Log.d(TAG, msg);
        Intent intent = new Intent(Utils.ACTION_DIAG_MSG_RECEIVED);
        intent.putExtra(Utils.DIAG_EXTRA_TAG, TAG);
        intent.putExtra(Utils.DIAG_EXTRA_MESSAGE, msg);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }

    public static void sendToast(Context context, String TAG, String msg) {
        Log.d(TAG, msg);
        Intent intent = new Intent(Utils.ACTION_DIAG_MSG_RECEIVED);
        intent.putExtra(Utils.DIAG_EXTRA_TAG, TAG);
        intent.putExtra(Utils.TOAST_EXTRA_MESSAGE, msg);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }

    public static void sendDebugFrameInfo(Context context, int[] intArray, int width, int height, int[] scores, int frameNum) {
        Intent intent = new Intent(Utils.ACTION_DIAG_MSG_RECEIVED);
        intent.putExtra(Utils.DIAG_EXTRA_DBG_FRAME_DATA, intArray);
        intent.putExtra(Utils.DIAG_EXTRA_DBG_FRAME_WIDTH, width);
        intent.putExtra(Utils.DIAG_EXTRA_DBG_FRAME_HEIGHT, height);
        intent.putExtra(Utils.DIAG_EXTRA_DBG_FRAME_SCORE, scores);
        intent.putExtra(Utils.DIAG_EXTRA_DBG_FRAME_NUM, frameNum);

        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }

    public static String getBatteryInfo(Intent intent) {
        return "Status:" + getBatteryStatus(intent) + " plugType:" + getBatteryPluggedType(intent) + " " +
                "level:" + intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
    }

    public static String getBatteryStatus(Intent intent) {
        int chargingStatus = intent.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
        String status;
        switch (chargingStatus) {
            case BatteryManager.BATTERY_STATUS_UNKNOWN:
                status = "Unknown";
                break;
            case BatteryManager.BATTERY_STATUS_CHARGING:
                status = "Charging";
                break;
            case BatteryManager.BATTERY_STATUS_DISCHARGING:
                status = "Discharging";
                break;
            case BatteryManager.BATTERY_STATUS_NOT_CHARGING:
                status = "Not Charging";
                break;
            case BatteryManager.BATTERY_STATUS_FULL:
                status = "full";
                break;
            default:
                status = "default";
                break;
        }
        return status;
    }


    public static String getBatteryPluggedType(Intent intent) {
        int chargePlug = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1);
        String plugType;
        switch (chargePlug) {
            case BatteryManager.BATTERY_PLUGGED_AC:
                plugType = "AC";
                break;
            case BatteryManager.BATTERY_PLUGGED_USB:
                plugType = "USB";
                break;
            case BatteryManager.BATTERY_PLUGGED_WIRELESS:
                plugType = "Wireless";
                break;
            default:
                plugType = "default";
                break;
        }
        return plugType;
    }


    /**
     * This is just for testing purpose, to be removed later in production
     *
     * @param context
     */
    public static void showProcessingCompleteDialog(final Context context) {


        AlertDialog dialog = new AlertDialog.Builder(context)
                .setTitle("Action completed")
                .setMessage("Videos have been scored")
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        dialogInterface.dismiss();
                    }
                }).create();

        dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);


        dialog.show();


        NotificationCompat.Builder builder = new NotificationCompat.Builder(context)
                .setWhen(System.currentTimeMillis());
//Vibration
        //  builder.setVibrate(new long[]{1000, 1000, 1000, 1000, 1000});

//LED
        builder.setLights(Color.RED, 3000, 3000);

//Tone
        //  builder.setSound(RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION));

        NotificationManager mNotificationManager =
                (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
// notificationID allows you to update the notification later on. //
        mNotificationManager.notify(1, builder.build());


    }

    public static String getLocation(ArrayList<Uri> uris) {
        //TODO: create logic to extract the location info from all the URI's provided and return the dominated location
        return "San Diego, CA";
    }
    
    public static String getASDCategoryName(int catIndex) {
        String configDirPath = Environment.getExternalStorageDirectory() + CONFIG_FILE_DIR;

        File file = new File(configDirPath, "cvmanager.config");
        //StringBuilder txt = new StringBuilder();
        int category_cnt = 0;

        try {
            BufferedReader br = new BufferedReader(new FileReader(file));
            String line;
            while ((line = br.readLine()) != null) {
                if (line.startsWith("ASDCV")) {
                    if (category_cnt == catIndex)
                        return line.split("\\s+")[0];
                    else
                        category_cnt++;
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }



    public static Constants.THEME_TYPE getThemeFromASDCategory(String asdCategoryString) {


        for (String catLookup : Constants.ACTION_ASD_CATEGORY_LIST) {
            if (asdCategoryString.equals(catLookup))
                return Constants.THEME_TYPE.ACTION;
        }
        for (String catLookup : Constants.URBAN_ASD_CATEGORY_LIST) {
            if (asdCategoryString.equals(catLookup))
                return Constants.THEME_TYPE.URBAN;
        }
        for (String catLookup : Constants.HAPPY_ASD_CATEGORY_LIST) {
            if (asdCategoryString.equals(catLookup))
                return Constants.THEME_TYPE.HAPPY;
        }
        for (String catLookup : Constants.CALM_ASD_CATEGORY_LIST) {
            if (asdCategoryString.equals(catLookup))
                return Constants.THEME_TYPE.CALM;
        }
        for (String catLookup : Constants.DRAMATIC_ASD_CATEGORY_LIST) {
            if (asdCategoryString.equals(catLookup))
                return Constants.THEME_TYPE.DRAMATIC;
        }

        return Constants.THEME_TYPE.HAPPY;
    }

    /*
     * This will copy the config files used by the app to a static location
     * where the libraries will then pick them up.
     * The DCN model file is big ~35MB so this copy will take up some time on first app launch
     * but we will not copy if the file already exists. Therefore the app should be uninstalled if
     * changes are made. This is OK since this is a temporary solution for tuning only.
     *
     *
     */
    public static void copyConfigFileAssets(Context context) {
        AssetManager assetManager = context.getAssets();
        String[] files = null;
        try {
            files = assetManager.list("config_files");
        } catch (IOException e) {
            Log.e("tag", "Failed to get asset file list.", e);
        }

        String copyToDir = Environment.getExternalStorageDirectory() + CONFIG_FILE_DIR;
        File destDir = new File(copyToDir);

        if (getPrefConfigFileAssetCopyFlag(context) == false) {
            //do a first time copy, so simply delete existing folder. and the next part of the code will do a copy if directory doesnot exist
            if (destDir.exists()) {
                try {
                    delete(destDir);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        if (!destDir.exists()) {
            destDir.mkdirs();
        }

        if (files != null) {
            for (String filename : files) {
                InputStream in = null;
                OutputStream out = null;
                try {
                    in = assetManager.open("config_files/" + filename);

                    File outFile = new File(copyToDir, filename);
                    if (!outFile.exists()) {
                        out = new FileOutputStream(outFile);
                        copyFile(in, out);
                    }
                } catch (IOException e) {
                    Log.e("tag", "Failed to copy asset file: " + filename, e);
                } finally {
                    if (in != null) {
                        try {
                            in.close();
                        } catch (IOException e) {
                            // NOOP
                        }
                    }
                    if (out != null) {
                        try {
                            out.close();
                        } catch (IOException e) {
                            // NOOP
                        }
                    }
                }
            }
            setPrefConfigFileAssetCopyFlag(context, true);//set this flag to True i.e first time copy has been performed
        }

    }

    /**
     * This method will recurcively delete folder/subfolder and files
     *
     * @param f pointer to that directory/file
     * @throws IOException
     */
    public static void delete(File f) throws IOException {
        if (f.isDirectory()) {
            for (File c : f.listFiles())
                delete(c);
        }
        if (!f.delete())
            throw new FileNotFoundException("Failed to delete file: " + f);
    }

    /**
     * The below two get and set pref methods are used with  copyConfigFileAssets() method to do a first time asset copy into the device memory
     *
     * @param context
     * @return true if already asset files have been copied 1st time, false otherwise.
     */
    public static boolean getPrefConfigFileAssetCopyFlag(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_config_file_asset_copy_flag), context.getResources().getBoolean(R.bool.pref_config_file_asset_copy_flag_default_val));
    }

    public static void setPrefConfigFileAssetCopyFlag(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.pref_config_file_asset_copy_flag), value);
        editor.commit();
    }

    public static Uri saveToGallery(String videofilePath) throws IOException {
        InputStream in = null;
        OutputStream out = null;

        File inputFile = new File(videofilePath);
        in = new FileInputStream(inputFile);

        File outDir = new File(VIDEO_SUMMARY_DIRECTORY);
        outDir.mkdirs();

        String[] str = videofilePath.split("/");
        String fileName = str[str.length - 1];

        File outFile = new File(outDir, fileName);
        out = new FileOutputStream(outFile);
        copyFile(in, out);

        return Uri.fromFile(outFile);
    }

    public static void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    public static void deleteFile(String filepath) {
        //if highlights inbox is woth demo card, in process of deleting expired summaries, we come here with file path == NULL
        if (filepath == null) {
            return;
        }
        File fileToDelete = new File(filepath);
        if (!fileToDelete.exists())
            return;
        else
            fileToDelete.delete();
    }

    /**
     * This method will return mp4filepath and thumbimgfilepath in array of Uri's at index 0 and
     * index 1 respectively.
     * Every instance of this call will create file paths based on timestamp(calendar) passed will
     * be converted to string & appended to filesDir and suffixed with .mp4 & .png
     *
     * @param context
     * @param timeStampMillis
     * @return fileUris
     */
    public static Uri[] getMediaUris(Context context, long timeStampMillis) {
        String filesDir = context.getFilesDir().toString();
        SimpleDateFormat formatter = new SimpleDateFormat("yyyy_MM_dd_HH_mm_ss");
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(timeStampMillis);
        String timeStampString = formatter.format(calendar.getTime());
        String mp4FilePath = filesDir + "/" + timeStampString + ".mp4";
        String thumbImgPath = filesDir + "/" + timeStampString + ".png";
        Uri fileUris[] = new Uri[2];
        fileUris[0] = Uri.parse(mp4FilePath);
        fileUris[1] = Uri.parse(thumbImgPath);
        return fileUris;
    }

    public static Uri getMp4FilePath(Context context, long timeStampMillis) {
        return getMediaUris(context, timeStampMillis)[0];
    }

    public static Uri getThumbImgFilePath(Context context, long timeStampMillis) {
        return getMediaUris(context, timeStampMillis)[1];
    }

    //TODO: this method to be deleted, used only for location debugging
    public static String getAllInfoOfLocation(Context context, double latitude, double longitude) {
        Geocoder gcd = new Geocoder(context, Locale.getDefault());
        List<Address> addresses;
        String loc = "loc:" + latitude + "," + longitude + "\n";
        String locationString = "location string unknown";
        try {
            addresses = gcd.getFromLocation(latitude, longitude, 1);
            if (addresses.size() > 0) {
                String featureName = addresses.get(0).getFeatureName();

                String locality = addresses.get(0).getLocality();
                String adminArea = addresses.get(0).getAdminArea();

                String subAdminArea = addresses.get(0).getSubAdminArea();

                String countryName = addresses.get(0).getCountryName();
                //locationString = "City:" + city + ", State:" + addresses.get(0).getAdminArea() + ", Country:" + addresses.get(0).getCountryName();
                locationString = "featureNam:" + featureName + ", locality:" + locality + ", subAdminArea:" + subAdminArea + ", adminArea:" + adminArea + ", country:" + countryName;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return loc + locationString;
    }

    public static Pair<String, String> getLocationInfo(Context context, double latitude, double longitude) {
        Geocoder gcd = new Geocoder(context, Locale.getDefault());
        List<Address> addresses;
        String city = null;
        String state = null;
        try {
            addresses = gcd.getFromLocation(latitude, longitude, 1);
            if (addresses.size() > 0) {
                city = addresses.get(0).getLocality();
                if (city == null) {
                    city = addresses.get(0).getSubAdminArea();
                }
                state = addresses.get(0).getAdminArea();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new Pair<>(city, state);
    }

    //TODO: this method to be deleted, used only for location debugging
    public static String getAllMediaStoreVideosWithLocInfo(Context context) {
        StringBuffer buff = new StringBuffer();
        int count = 1;
        final String orderBy = MediaStore.Video.Media.DATE_MODIFIED;

        final String[] columns = {MediaStore.Video.Media.DATA, MediaStore.Video.Media.LATITUDE,
                MediaStore.Video.Media.LONGITUDE, MediaStore.Video.Media._ID, orderBy};

        Cursor cursor = context.getContentResolver().query(
                MediaStore.Video.Media.EXTERNAL_CONTENT_URI, columns,
                null, null, orderBy + " DESC");

        Uri uriBase = MediaStore.Video.Media.getContentUri("external");

        if (cursor != null && cursor.getCount() > 0) {

            while (cursor.moveToNext()) {
                int intID = cursor.getInt(cursor.getColumnIndexOrThrow(MediaStore.Video.Media._ID));
                double latitude = cursor.getDouble(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.LATITUDE));
                double longitude = cursor.getDouble(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.LONGITUDE));

                if (latitude != 0 && longitude != 0) {
                    buff.append("\n" + (count++) + ") " + Utils.getAllInfoOfLocation(context, latitude, longitude));
                }
            }
        }
        return buff.toString();
    }

    public static String getCompositionMediaArrayListAsString(ArrayList<MediaInfo> mediaInfoArrayList) {
        int[] mediaIds = new int[mediaInfoArrayList.size()];
        for (int i = 0; i < mediaInfoArrayList.size(); i++) {
            mediaIds[i] = (int) ContentUris.parseId(mediaInfoArrayList.get(i).contentUri);
        }
        return Arrays.toString(mediaIds);
    }

    public static ArrayList<MediaInfo> getCompositionMediaArrayListFromString(String arrayString) {
        Uri uriBase = MediaStore.Video.Media.getContentUri("external");
        ArrayList<MediaInfo> mediaInfoArrayList = new ArrayList<MediaInfo>();
        String[] strings = arrayString.replace("[", "").replace("]", "").split(", ");
        for (int i = 0; i < strings.length; i++) {
            int intID = Integer.parseInt(strings[i].trim());
            MediaInfo item = new MediaInfo();
            item.contentUri = ContentUris.withAppendedId(uriBase, intID);
            mediaInfoArrayList.add(item);
        }
        return mediaInfoArrayList;
    }

    public static long getDuration(String videoFilePath) {
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        retriever.setDataSource(videoFilePath);
        String time = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);
        retriever.release();
        return Long.parseLong(time);
    }

    /**
     * For profiling CVInit
     */

    public static Set<String> getPrefCvInitTime(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getStringSet("cvInitTime", new LinkedHashSet<String>());
    }


    public static void setPrefCvInitTime(Context context, Set<String> set) {

        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putStringSet("cvInitTime", set);
        editor.commit();
    }

    public static void appendCvInitTime(Context context, int retryCount, long startTime, long timeTakenMillis) {
        String timeTakenStr = CalendarHelper.timeStampMillisToString(startTime) + "=>" + CalendarHelper.getTimeStringMillis(timeTakenMillis) + " try(" + retryCount + ")";
        Set<String> set = getPrefCvInitTime(context);
        set.add(timeTakenStr);
        setPrefCvInitTime(context, set);
    }

    public static String getAbsolutePathFromURI(Context context, Uri contentUri) {
        Cursor cursor = null;
        try {
            String[] selection = {MediaStore.Images.Media.DATA};
            cursor = context.getContentResolver().query(contentUri, selection, null, null, null);
            cursor.moveToFirst();
            return cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA));
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }

}
