/**
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.loadcarrier;

import android.os.Environment;
import android.os.RecoverySystem;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.text.TextUtils;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import android.os.storage.StorageManager;
import android.content.Context;

import com.android.internal.os.IRegionalizationService;
import com.android.internal.os.RegionalizationEnvironment;

public class Utils {
    private static final String TAG = "LoadCarrier.Utils";
    public static final boolean DEBUG = true;

    public static final String PROP_KEY_TRIGGER            = "persist.radio.trigger";
    public static final String PROP_KEY_TRIGGER_PATH       = "persist.radio.trigger.path";
    public static final String PROP_KEY_TRIGGER_BRAND      = "persist.radio.trigger.brand";
    public static final String PROP_KEY_TRIGGER_TARGET     = "persist.radio.trigger.target";
    public static final String PROP_KEY_FILE_BASED_ENABLED = "persist.radio.filebased.enabled";
    public static final String PROP_KEY_TRIGGER_FIRST_INSERT = "persist.radio.trigger.firstonly";
    public static final String PROP_KEY_BLOCK_BASED_ENABLED  = "persist.radio.blockbased";

    public static final int FLAG_SYSTEM_VENDOR     = 1;
    public static final int FLAG_STORAGE_EXTERNAL  = 2;
    public static final int FLAG_STORAGE_SECONDARY = 3;
    public static final int FLAG_CACHE_VENDOR      = 4;

    public static final int INVALID_SLOT_ID = -1;
    public static final int INVALID_SUB_MASK = 0;

    public static final String FILTER_ITEM_ICC     = "ICC";
    public static final String FILTER_ITEM_MCC     = "MCC";
    public static final String FILTER_ITEM_MCCMNC  = "MCCMNC";
    public static final String FILTER_ITEM_SPN     = "SPN";
    public static final String FILTER_ITEM_BRAND   = "Brand";
    public static final String FILTER_ITEM_TARGET  = "Target";
    public static final String FILTER_ITEM_VERSION = "Version";
    public static final String FILTER_ITEM_NAME    = "Name";
    public static final String FILTER_ITEM_DEPENDENCY = "Dependency";
    // enhance trigger
    public static final String FILTER_ITEM_MODEL   = "Model";
    public static final String FILTER_ITEM_HIERARCHY = "Hierarchy";
    public static final String FILTER_ITEM_GID = "GID";
    // Actually, FILTER_ITEM_SLOT is not a filter item. It is used to identify which slot
    // these filter items come from.
    public static final String FILTER_ITEM_SLOT    = "Slot";
    // FILTER_ITEM_SUBMASK is used to show which slot matches this carrier. If matched,
    // the matched slot's bit will be set to 1, e.g. SubMask="3" means this carrier matched
    // Slot 0 and 1.
    public static final String FILTER_ITEM_SUBMASK    = "SubMask";
    public static final String INTENT_CARRIER_TARGET_SLOT = "target_slot";

    public static final String PARTITION_INFO_FILE = "/persist/speccfg/partition";
    public static final String DEVICE_TYPE = "/persist/speccfg/devicetype";
    public static final String ENABLE_TRIGGER_NAME = "enable_trigger_prop";
    //Special ROW package name. Should be sync with CarrierConfig/Utils.java
    public static final String SPECIAL_ROW_PACKAGE_NAME = "ROW";

    private static final String STORAGE_EXTERNAL  = "EXTERNAL_STORAGE";
    private static final String STORAGE_SECONDARY = "SECONDARY_STORAGE";

    private static final String SYSTEM_VENDOR_PATH        = "/system/vendor";
    private static final String SYSTEM_VENDOR_PRIEX       = "system/vendor";
    private static final String INTERNAL_STORAGE_PREFIX   = "/storage/emulated";
    private static final String CACHE_VENDOR_PATH         = "/cache";
    private static final String SPEC_MARKED_FILE_NAME     = ".preloadspec";
    private static final String CARRIER_OTA_ZIP_FILE_NAME = ".ota.zip";
    private static final String BINARY_PATCH_FILE_NAME    = "system.patch.dat";
    private static final String MBN_FILE_NAME             = "mcfg_sw.mbn";
    private static final String MBN_COM_DIRECTORY         = "modem_config";
    private static final String MBN_ROW_DIRECTORY         = "modem_row_config";

    /**
     * Get the prop value, if couldn't find the prop, return the default value.
     */
    public static boolean getValue(String propKey, boolean defValue) {
        return SystemProperties.getBoolean(propKey, defValue);
    }

    /**
     * Get the prop value, if couldn't find the prop, return the default value.
     */
    public static String getValue(String propKey, String defValue) {
        return SystemProperties.get(propKey, defValue);
    }

    /**
     * Set the prop value.
     */
    public static void setValue(String propKey, String newValue) {
        SystemProperties.set(propKey, newValue);
    }

    /**
     * To get the external or internal storage path.
     * @param flag which path you want to get, the value must be:<br>
     *                {@link #FLAG_SYSTEM_VENDOR} for system vendor directory<br>
     *                {@link #FLAG_STORAGE_EXTERNAL} for external storage<br>
     *                {@link #FLAG_STORAGE_SECONDARY} for internal storage<br>
     * @return the path
     */
    public static String getPath(Context context, int flag) {
        switch (flag) {
            case FLAG_SYSTEM_VENDOR:
                return SYSTEM_VENDOR_PATH;
            case FLAG_STORAGE_EXTERNAL:
                // don't scan internal sdcard
                return null;
                //return System.getenv(STORAGE_EXTERNAL);
            case FLAG_STORAGE_SECONDARY:
                String secondaryStorage = System.getenv(STORAGE_SECONDARY);
                if (TextUtils.isEmpty(secondaryStorage) || !isDirectoryExist(secondaryStorage)) {
                    secondaryStorage = getSecondaryStorage(context);
                }
                return secondaryStorage;
            case FLAG_CACHE_VENDOR:
                return CACHE_VENDOR_PATH;
            default:
                throw new IllegalStateException("Do not accept this flag: " + flag);
        }
    }

    private static boolean isDirectoryExist(String path) {
        File dir = new File(path);
        return dir != null && dir.exists() && dir.canRead();
    }

    /**
     * Get the path of secondary storage.
     * TODO There should be a better method to get the path. Or this path may be regular in the
     *      future.
     * @return the path of secondary storage
     */
    public static String getSecondaryStorage(Context context) {
        StorageManager sm = (StorageManager)context.getSystemService(context.STORAGE_SERVICE);
        String[] volumePaths = sm.getVolumePaths();
        for (String volumePath : volumePaths) {
            if (!volumePath.startsWith(INTERNAL_STORAGE_PREFIX)) {
                return volumePath;
            }
        }
        return null;
    }

    @SuppressWarnings("unchecked")
    public static void getMatchedCarrierFromAll(HashMap<String, String> carrierList,
            HashMap<String, String> allCarriers, ArrayList<HashMap<String, String>> filterItems) {
        String currentCarriers = getCurrentCarriersName(getCurrentCarriers());
        for (Entry<String, String> entry : allCarriers.entrySet()) {
            String path = entry.getKey();
            String contents = entry.getValue();
            if (DEBUG) Log.d(TAG, "Check whether accepted: " + path);
            try {
                CarrierFileFilter filter = new CarrierFileFilter(null, false, null);
                CarrierFileFilter.CarrierPattern pattern = filter.new CarrierPattern(contents);
                String carrierName = filter.getCarrierName(new File(path));
                if ((carrierName.equals(currentCarriers + "2Default")) ||
                        (pattern.matches(filterItems) && !hasCarrier(carrierList, carrierName))) {
                    if (DEBUG) Log.d(TAG, "Matched. carrierName = "+carrierName);
                    carrierList.put(path, pattern.getContents());
                }
            } catch(IOException e) {
                if (DEBUG) Log.d(TAG, "getMatchedCarrierFromAll error. e=" + e);
            }
        }
    }

    public static boolean hasCarrier(HashMap<String, String> carrierList, String name) {
        for (Entry<String, String> entry : carrierList.entrySet()) {
            String contents = entry.getValue();
            CarrierInfo carrierInfo = new CarrierInfo(contents);
            String contentsName = carrierInfo.getValue(CarrierInfo.KEYWORD_NAME);
            if (!TextUtils.isEmpty(contentsName) && (contentsName.equals(name))) {
                if (DEBUG) Log.d(TAG, name + " already exist in carrierList.");
                return true;
            }
        }
        return false;
    }

    /**
     * Only find the carrier as OTA file from the given path.
     * @param carrierList to save the list as key = path, value = content
     * @param path to find the carrier list under this path
     */
    public static void getCarrierList(HashMap<String, String> carrierList, String path) {
        getCarrierList(carrierList, path, true /* only filter for OTA */);
    }

    /**
     * Find the carrier list from the given path.
     * @param carrierList to save the list as key = path, value = content
     * @param path to find the carrier list under this path
     * @param onlyFilterOta if only find the OTA file
     */
    public static void getCarrierList(HashMap<String, String> carrierList, String path,
            boolean onlyFilterOta) {
        getCarrierList(carrierList, path, true /* only filter for OTA */, null);
    }

    /**
     * Find the carrier list from the given path, and the carrier need match the filter items.
     * @param carrierList to save the list as key = path, value = content
     * @param path to find the carrier list under this path
     * @param onlyFilterOta if only find the OTA file
     * @param filterItems the carrier need match this filter items
     */
    public static void getCarrierList(HashMap<String, String> carrierList, String path,
            boolean onlyFilterOta, ArrayList<HashMap<String, String>> filterItems) {
        if (DEBUG) Log.i(TAG, "Try to get the carrier list from " + path);

        if (path == null) return;
        File dir = new File(path);
        if (dir == null || !dir.exists() || !dir.canRead()) return;

        dir.listFiles(new CarrierFileFilter(carrierList, onlyFilterOta, filterItems));
    }

    /**
     * Copy the srcFile to dstFile
     * @param srcFile source file
     * @param dstFile destination file
     * @return true if successfully; false if failed.
     */
    public static boolean copyFile(File srcFile, File dstFile) {
        try {
            if (dstFile.exists()) {
                dstFile.delete();
            }

            InputStream in = new FileInputStream(srcFile);
            OutputStream out = new FileOutputStream(dstFile);
            try {
                int cnt;
                byte[] buf = new byte[4096];
                while ((cnt = in.read(buf)) >= 0) {
                    out.write(buf, 0, cnt);
                }
            } finally {
                out.close();
                in.close();
            }
            return true;
        } catch (IOException e) {
            return false;
        }
    }

    public static boolean isFileBasedSwitchEnabled() {
        boolean result = isPresetMode() ?
                false : SystemProperties.getBoolean(PROP_KEY_FILE_BASED_ENABLED, false);
        return result;
    }

    /**
     * The carrier file filter to filter the matched carriers.
     */
    private static class CarrierFileFilter implements FileFilter {
        private static final String OTA_ZIP_PATTERN = "(\\.ota\\.zip)$";

        private boolean mOnlyFilterOta;
        private HashMap<String, String> mCarrierList;
        private ArrayList<HashMap<String, String>> mFilterItems;

        public CarrierFileFilter(HashMap<String, String> carrierList,
                boolean onlyFilterOta, ArrayList<HashMap<String, String>> filterItems) {
            mCarrierList = carrierList;
            mOnlyFilterOta = onlyFilterOta;
            mFilterItems = filterItems;
        }

        @Override
        public boolean accept(File file) {
            try {
                String filename = file.getName();
                if (mOnlyFilterOta) {
                    if (filename.matches("^\\w+" + OTA_ZIP_PATTERN)
                            && acceptAsCarrierOta(file)) {
                        return true;
                    }
                } else {
                    return acceptAsPresetCarrier(file);
                }
            } catch (IOException e) {
                Log.e(TAG, "Catch the IOException when test if file(" + file + ") accept. e=" + e);
            }

            return false;
        }

        private boolean acceptAsCarrierOta(File otaFile) throws IOException {
            if (DEBUG) Log.i(TAG, "Found the ota file, check if it accept. File: " + otaFile);

            // Check this OTA file's name.
            // The OTA file's name need same as "[CarrierName].ota.zip".
            String currentCarriers = getCurrentCarriersName(getCurrentCarriers());
            String carrierName = getCarrierName(otaFile);
            if (DEBUG) Log.d(TAG, "The carrier's name is: " + carrierName);

            JarFile carrierOtaFile = null;
            try {
                carrierOtaFile = new JarFile(otaFile);
                Enumeration<JarEntry> en = carrierOtaFile.entries();
                JarEntry specElement = null;
                boolean hasCommerialMbn = false;
                boolean hasROWMbn = false;
                while (en.hasMoreElements()) {
                    JarEntry element = en.nextElement();

                    // If the current entry is directory, we will do nothing.
                    if (element.isDirectory()) continue;

                    // The spec marked file must store in Carrier folder.
                    // So if the length is N, then the items must be as this:
                    //     (N-2)  ----  CarrierName
                    //     (N-1)  ----  .preloadspec
                    String[] names = element.getName().split("/");
                    if (SPEC_MARKED_FILE_NAME.equals(names[names.length - 1])) {
                        if (carrierName.equals(names[names.length - 2])) {
                            // It means this is the accepted carrier OTA package.
                            if (mFilterItems == null || mFilterItems.size() < 1) {
                                specElement = element;
                                //return addToCarrierList(otaFile, element);
                            } else {
                                // Find the if this OTA match the filter items.
                                CarrierPattern pattern = new CarrierPattern(carrierOtaFile, element);
                                if (pattern.matches(mFilterItems) || carrierName
                                        .equals(currentCarriers + "2Default")) {
                                    if (DEBUG) Log.d(TAG,"Trigger carrierName = "+carrierName);
                                    specElement = element;
                                    //return addToCarrierList(otaFile, element);
                                }
                            }
                        }
                    } else if (MBN_FILE_NAME.equals(names[names.length - 1])) {
                        if (MBN_COM_DIRECTORY.equals(names[names.length - 2])) {
                            hasCommerialMbn = true;
                        } else if (MBN_ROW_DIRECTORY.equals(names[names.length - 2])) {
                            hasROWMbn = true;
                        } else {
                            Log.e(TAG, "Unknown mbn file: " + element.getName());
                        }
                    }
                }
                if (specElement != null) {
                    addToCarrierList(otaFile, specElement, hasCommerialMbn, hasROWMbn);
                }
            } finally {
                if (carrierOtaFile != null) {
                    carrierOtaFile.close();
                }
            }

            return false;
        }

        private boolean addToCarrierList(File otaFile, JarEntry element, boolean hasCommerialMbn,
                boolean hasROWMbn) throws IOException {
            JarFile carrierOtaFile = null;
            String additionalInfo = CarrierInfo.contentItemToString(CarrierInfo.KEYWORD_HAS_COM_MBN,
                    String.valueOf(hasCommerialMbn)) + CarrierInfo.contentItemToString(
                    CarrierInfo.KEYWORD_HAS_ROW_MBN, String.valueOf(hasCommerialMbn));
            try {
                String carrierName = getCarrierName(otaFile);
                carrierOtaFile = new JarFile(otaFile);
                CarrierPattern pattern = new CarrierPattern(carrierOtaFile, element);
                if (Utils.isFileBasedSwitchEnabled() || Utils.isPresetMode()) {
                    if (!isBinaryOTAPackage(carrierOtaFile)) {
                        mCarrierList.put(otaFile.getAbsolutePath(),
                                pattern.getContents() != null ? pattern.getContents()
                                + additionalInfo : additionalInfo);
                        return true;
                    }
                } else {
                    if (isBinaryOTAPackage(carrierOtaFile)) {
                        mCarrierList.put(otaFile.getAbsolutePath(),
                                pattern.getContents() != null ? pattern.getContents()
                                + additionalInfo : additionalInfo);
                        return true;
                    }
                }
            } finally {
                if (carrierOtaFile != null) {
                    carrierOtaFile.close();
                }
            }
            return false;
        }

        private boolean isDuplicateInCarrierListByName(String carrierName) {
            try {
                ArrayList<HashMap<String, String>> filterItems =
                        new ArrayList<HashMap<String, String>>();
                HashMap<String, String> filterItem = new HashMap<String, String>();
                filterItem.put(Utils.FILTER_ITEM_NAME, carrierName);
                filterItems.add(filterItem);

                Iterator iter = mCarrierList.entrySet().iterator();
                while (iter.hasNext()) {
                    Entry entry = (Entry) iter.next();
                    String contents = (String) entry.getValue();
                    CarrierPattern pattern = new CarrierPattern(contents);
                    if (pattern.matches(filterItems)) {
                        return true;
                    }
                }
            } catch(Exception ex) {
                return false;
            }
            return false;
        }

        private boolean isBinaryOTAPackage(JarFile carrierOtaFile) {
            try {
                Enumeration<JarEntry> en = carrierOtaFile.entries();
                while (en.hasMoreElements()) {
                    JarEntry element = en.nextElement();

                    if (element.isDirectory()) continue;

                    String[] names = element.getName().split("/");
                    if (BINARY_PATCH_FILE_NAME.equals(names[names.length - 1])) {
                        return true;
                    }
                }
            } catch (Exception ex) {
                return false;
            }
            return false;
        }

        private boolean acceptAsPresetCarrier(File carrierDir) {
            if (DEBUG) Log.i(TAG, "Check if this folder accept. File: " + carrierDir);

            if (carrierDir == null || carrierDir.isFile()) return false;

            try {
                File[] subFiles = carrierDir.listFiles();
                if (subFiles == null || subFiles.length < 1) return false;

                for (File file : subFiles) {
                    if (SPEC_MARKED_FILE_NAME.equals(file.getName())) {
                        // It means this is the one carrier folder.
                        if (mFilterItems == null || mFilterItems.size() < 1) {
                            CarrierPattern pattern = new CarrierPattern(file);
                            mCarrierList.put(carrierDir.getAbsolutePath(), pattern.getContents());
                            return true;
                        } else {
                            // Find the if this carrier match the filter items.
                            CarrierPattern pattern = new CarrierPattern(file);
                            if (pattern.matches(mFilterItems)) {
                                mCarrierList.put(
                                        carrierDir.getAbsolutePath(), pattern.getContents());
                                return true;
                            }
                        }
                    }
                }
            } catch (IOException ex) {
                Log.w(TAG, "Catch the IOException: " + ex);
            }

            return false;
        }

        public String getCarrierName(File otaFile) {
            String fileName = otaFile.getName();
            String carrierName = fileName.substring(0, fileName.length()
                    - CARRIER_OTA_ZIP_FILE_NAME.length());
            return carrierName;
        }

        public class CarrierPattern {
            private static final String SEP_BLANK = " ";
            private static final String SEP_EQUAL = "=";
            private static final String SEP_COMMA = ",";
            private static final String SEP_QUOTE = "\"";

            private static final String RE_START = "(^|\\S+,)";
            private static final String RE_END = "(,\\S+|$)";
            private static final String RE_MNC = "(\\d{2,3})?";
            private static final String RE_BLANK = "\\s";

            private static final String COMMENTS_TAG = "#";

            private String mContents;
            private HashMap<String, String> mContentItems;

            public CarrierPattern(JarFile file, JarEntry entry) throws IOException {
                parseContentFromZipFile(file, entry);
            }

            public CarrierPattern(File file) throws IOException {
                parseContentFromFile(file);
            }

            public CarrierPattern(String contents) throws IOException {
                mContents = contents;
                parseContent();
            }

            public String getContentItems(String key) {
                if (TextUtils.isEmpty(key)) {
                    return null;
                }

                if (mContentItems == null || mContentItems.size() < 1) {
                    return null;
                }

                return mContentItems.get(formatKey(key));
            }

            /**
             * Tests whether this carrier matches the given pattern items.
             * @return true if matched
             */
            public boolean matches(ArrayList<HashMap<String, String>> patternItems) {
                if (patternItems == null || patternItems.size() < 1) {
                    // The pattern items is null or empty, return true.
                    return true;
                }
                if (mContentItems == null || mContentItems.size() < 1) {
                    // This carrier's content item is null or empty, return false.
                    return false;
                }

                // If one of the patternItems matched the content, it means this
                // carrier is matched the pattern items and hasOneMatched will
                // be set to true. Use this flag instead of return directly,
                // so all patternItems will be scanned and each patternItem's slot
                // info can be written into the carrier's contents. This covers
                // many-SIMs-to-one-package case.
                boolean hasOneMatched = false;
                // 0x01-SLOT0, 0x02-SLOT1
                int matchedSubMask = INVALID_SUB_MASK;
                for (HashMap<String, String> patternItem : patternItems) {
                    // If all the items belong to this patternItem matched the content,
                    // it means this carrier is matched the pattern items.
                    boolean itemMatched = true;
                    int itemSlotId = INVALID_SLOT_ID;

                    Iterator<Entry<String, String>> iterator = patternItem.entrySet().iterator();
                    while (iterator.hasNext()) {
                        Entry<String, String> entry = iterator.next();

                        // Adjust the item for MCC.
                        boolean patternItemIsMCC = false;
                        String patternKey = formatKey(entry.getKey());
                        if (formatKey(FILTER_ITEM_MCC).equals(patternKey)) {
                            // If the filter item is MCC, we need find the content from MCCMNC.
                            patternItemIsMCC = true;
                            patternKey = formatKey(FILTER_ITEM_MCCMNC);
                        } else if (formatKey(FILTER_ITEM_SLOT).equals(patternKey)) {
                            // If the filter item is SLOT, just record it.
                            itemSlotId = Integer.parseInt(entry.getValue());
                            continue;
                        }

                        String contentValue = mContentItems.get(patternKey);
                        Log.d(TAG, "patternKey="+patternKey + " contentValue=" + contentValue
                                + " patternValue=" + entry.getValue());
                        if (TextUtils.isEmpty(contentValue)) {
                            // Couldn't found this item or the item's content is empty.
                            // We will return false as this carrier do not match.
                            Log.d(TAG, "contentValue is empty");
                            if (!TextUtils.isEmpty(entry.getValue())) {
                                itemMatched = false;
                                Log.d(TAG, "partternValue is not empty");
                                break;
                            }
                        } else {
                            // If one of the given values matched the content value,
                            // it means this item is matched.
                            boolean found = false;

                            String[] values = entry.getValue().split(SEP_COMMA);
                            for (String value : values) {
                                String regularExpression = RE_START
                                        + value.replace(SEP_BLANK, RE_BLANK)
                                        + RE_END;
                                if (contentValue.matches(regularExpression)) {
                                    // If the value matched the content, set the found as true.
                                    found = true;
                                    if (contentValue.contains(SEP_COMMA)) {
                                        // if this content value contains many item, we will append
                                        // this matched value as actually value.
                                        // For example: *** Target="[value]"
                                        mContents = mContents + SEP_BLANK + patternKey + SEP_EQUAL
                                                + SEP_QUOTE + value + SEP_QUOTE;
                                    }
                                    break;
                                }
                            }
                            if (!found) {
                                // Couldn't find the matched value, set this item do not matched.
                                itemMatched = false;
                                break;
                            }
                        }
                    }

                    // If this patternItem matched, we will return the result as matched.
                    if (itemMatched) {
                        hasOneMatched = true;
                        Log.i(TAG, "Matched:" + mContentItems.get(formatKey(FILTER_ITEM_NAME)));
                        if (itemSlotId != INVALID_SLOT_ID) {
                            matchedSubMask |= 0x0001 << itemSlotId;
                        }
                    }
                }

                if (matchedSubMask != INVALID_SUB_MASK) {
                    mContents = mContents + SEP_BLANK + FILTER_ITEM_SUBMASK + SEP_EQUAL
                            + SEP_QUOTE + matchedSubMask + SEP_QUOTE;
                    Log.i(TAG, "matchedSubMask=" + matchedSubMask);
                }

                // It means all the pattern items do not matched the content,
                // we will set this carrier as do not matched and return the result.
                return hasOneMatched;
            }

            /**
             * @return the contents of this carrier.
             */
            public String getContents() {
                return mContents;
            }

            private void parseContentFromZipFile(JarFile file, JarEntry entry)
                    throws IOException {
                if (file == null || entry == null) return;

                // Get the content items.
                InputStream is = file.getInputStream(entry);
                InputStreamReader isReader = new InputStreamReader(is);
                BufferedReader bufReader = new BufferedReader(isReader);
                try {
                    // Read the first line, and it must be have one line to defined the items.
                    // And ignore comments lines and anything else.
                    mContents = getContentFromBufferedReader(bufReader);
                } finally {
                    // Close the BufferedReader, InputStreamReader and InputStream.
                    if (bufReader != null) bufReader.close();
                    if (isReader != null) isReader.close();
                    if (is != null) is.close();
                }
                parseContent();
            }

            private void parseContentFromFile(File file) throws IOException {
                if (file == null) return;

                // Get the content items.
                FileReader fr = new FileReader(file);
                BufferedReader br = new BufferedReader(fr);
                try {
                    // Read the first line, and it must be have one line to defined the items.
                    // And ignore comments lines and anything else.
                    mContents = getContentFromBufferedReader(br);
                } finally {
                    // Close the used FileReader and BufferedReader.
                    if (br != null) br.close();
                    if (fr != null) fr.close();
                }

                parseContent();
            }

            private String getContentFromBufferedReader(BufferedReader br) throws IOException{
                String content = null;
                while ((content = br.readLine()) != null){
                    if ((!content.isEmpty()) && (!content.startsWith(COMMENTS_TAG))) {
                        return content;
                    }
                };
                return null;
            }

            private void parseContent() {
                if (mContents == null || mContents.trim() == null) return;
                mContents = mContents.trim();

                // Build the contents' item.
                if (mContentItems == null) {
                    mContentItems = new HashMap<String, String>();
                }
                mContentItems.clear();

                String[] items = mContents.split(SEP_QUOTE);
                for (int i = 0; i < items.length - 1; i += 2) {
                    if (!items[i].endsWith(SEP_EQUAL)) {
                        Log.e(TAG, "Parsing contents error. contents:" + mContents + " item[" +
                                i + "]:" + items[i]);
                        mContentItems.clear();
                        return;
                    } else {
                        mContentItems.put(formatKey(items[i].trim().replace(SEP_EQUAL, "")),
                                items[i+1]);
                    }
                }
            }

            private String formatKey(String key) {
                return key.toUpperCase();
            }
        }
    }

    /**
     * Return the current carrier's name.
     */
    private static final String SPEC_CONFIG_PATH = "/system/vendor/speccfg/";
    private static final String DEFAULT_SPEC_NAME = "Default";
    private static final String SPEC_FILE_NAME = "spec";
    private static final String CURRENT_CARRIERS_NUM_PRE = "packCount=";
    private static final String CURRENT_CARRIERS_NAME_PRE = "strSpec";
    private static final String SEP_UNDERLINE = "_";
    public static ArrayList<String> getCurrentCarriers() {
        ArrayList<String> Carriers = null;

        if (isPresetMode()) {
            Carriers = (ArrayList<String>) RegionalizationEnvironment.getAllPackageNames();
            if (Carriers == null || Carriers.size() == 0) {
                Carriers.add(DEFAULT_SPEC_NAME);
            }
            return Carriers;
        }

        Carriers = new ArrayList<String>();
        File specFile = new File(SPEC_CONFIG_PATH + SPEC_FILE_NAME);

        try {
            // If the file size is zero, we will return null as no content.
            if (getFileSize(specFile) == 0) {
                Carriers.add(DEFAULT_SPEC_NAME);
                return Carriers;
            }

            ArrayList<String> contents = readFile(specFile, null, false);

            String carrierNumRegularExpresstion = "^" + CURRENT_CARRIERS_NUM_PRE + "[0-9]$";
            if (!contents.get(0).matches(carrierNumRegularExpresstion)) {
                Carriers.add(DEFAULT_SPEC_NAME);
                return Carriers;
            }
            int carriersNum = Integer.parseInt(contents.get(0)
                    .substring(CURRENT_CARRIERS_NUM_PRE.length()));
            if (carriersNum <= 0 || contents.size() <= carriersNum) {
                Carriers.add(DEFAULT_SPEC_NAME);
                return Carriers;
            }
            for(int i=1; i<=carriersNum; i++) {
                String carrierRegularExpresstion = "^" + CURRENT_CARRIERS_NAME_PRE + "[0-9]=\\w+$";
                if (contents.get(i).matches(carrierRegularExpresstion)) {
                    Carriers.add(contents.get(i).substring(CURRENT_CARRIERS_NAME_PRE.length()+2));
                } else {
                    Carriers.clear();
                    Carriers.add(DEFAULT_SPEC_NAME);
                    return Carriers;
                }
            }
            return Carriers;
        } catch (IOException e) {
            Log.e(TAG, "Get current carrier error, caused by: " + e.getMessage());
        }
        Carriers.add(DEFAULT_SPEC_NAME);
        return Carriers;
    }

    public static String getCurrentCarriersDependency() {
        String currentCarriers = getCurrentCarriersName(getCurrentCarriers());
        String topCarrier = getTopCarrierName(currentCarriers);
        Log.d(TAG, "topCarrier = " + topCarrier);
        File currentSpecFile = new File(SYSTEM_VENDOR_PATH + File.separator + topCarrier
                + File.separator + SPEC_MARKED_FILE_NAME);
        try {
            CarrierFileFilter filter = new CarrierFileFilter(null, false, null);
            CarrierFileFilter.CarrierPattern pattern =
                    filter.new CarrierPattern(currentSpecFile);
            String dependencyString = pattern.getContentItems(FILTER_ITEM_DEPENDENCY);
            if (!TextUtils.isEmpty(dependencyString)) {
                String[] dependency = dependencyString.split("/");
                // Only return name, ignore hierarchy.
                if (dependency.length == 1) {
                    // Dependency="Name"
                    return dependency[0];
                } else {
                    // Dependency="Hierarchy/Name"
                    return dependency[1];
                }
            } else {
                return null;
            }
        } catch (IOException e) {
            Log.e(TAG, "CarrierPattern IOException:" + e.getMessage());
            return null;
        }
    }

    /**
     * To read the content from the given file. If the lines do not match the regular
     * expression, do not add to result.
     * @param regularExpression used to find the matched line, if it is null, do not check
     *                          if the line matched this expression.
     */
    private static ArrayList<String> readFile(File file, String regularExpression,
            boolean onlyReadFirstLine) {
        if (file == null || !file.exists() || !file.canRead()) return null;

        ArrayList<String> contents = new ArrayList<String>();

        FileReader fr = null;
        BufferedReader br = null;
        try {
            fr = new FileReader(file);
            br = new BufferedReader(fr);
            // Read the lines, and get the current carrier.
            String line = null;
            while ((line = br.readLine()) != null && (line = line.trim()) != null) {
                if (!TextUtils.isEmpty(regularExpression)) {
                    if (line.matches(regularExpression)) {
                        contents.add(line);
                    }
                } else {
                    contents.add(line);
                }
                if (onlyReadFirstLine) break;
            }
        } catch (IOException e) {
            Log.e(TAG, "Read File error, caused by: " + e.getMessage());
        } finally {
            try {
                if (br != null) br.close();
                if (fr != null) fr.close();
            } catch (IOException e) {
                Log.e(TAG, "Close the reader error, caused by: " + e.getMessage());
            }
        }

        return contents;
    }

    /**
     * Return the file size for the given file.
     */
    public static int getFileSize(File file) throws IOException {
        int size = 0;
        if (file != null && file.exists()) {
            FileInputStream fis = new FileInputStream(file);
            try {
                size = fis.available();
            } finally {
                if (fis != null) fis.close();
            }
        }
        return size;
    }

    public static String getCurrentCarriersName(ArrayList<String> list) {
        if (list.size() == 0) return null;
        if (list.size() == 1) return list.get(0);
        String fullName = null;
        for (String name : list) {
            if (fullName == null)
                fullName = name;
            else fullName += "_" + name;
        }
        return fullName;
    }

    public static String getTopCarrierName(String fullname) {
        String[] name = fullname.split(SEP_UNDERLINE);
        return name[name.length - 1];
    }

    @SuppressWarnings("unchecked")
    public static String unzip(String srcZipFileName, String dstOutputDirectory)
            throws IOException {
        ZipFile zipFile = null;
        String dstOutPutPath = "";
        try {
            try {
                RecoverySystem.verifyPackage(new File(srcZipFileName), null, null);
            } catch (Exception ex) {
                // When we got the exception, it means the verify failed.
                Log.e(TAG, "Verify the signature of file(" + srcZipFileName + ") failed. Caused"
                        + " by: " + ex.getMessage());
            }
            zipFile = new ZipFile(srcZipFileName);
            Enumeration entries = zipFile.entries();
            ZipEntry zipEntry = null;
            File outputDirectory = new File(dstOutputDirectory);
            outputDirectory.mkdirs();
            while (entries.hasMoreElements()) {
                zipEntry = (ZipEntry) entries.nextElement();
                String entryName = zipEntry.getName();

                // Get unzipped carrier name
                String[] names = zipEntry.getName().split("/");
                if (SPEC_MARKED_FILE_NAME.equals(names[names.length - 1])) {
                    dstOutPutPath = dstOutputDirectory + "/" + names[names.length - 2];
                }

                InputStream in = null;
                FileOutputStream out = null;
                try {
                    if (zipEntry.isDirectory()) {
                        String dirName = zipEntry.getName();
                        dirName = dirName.substring(0, dirName.length() - 1);
                        File dir = new File(dstOutputDirectory + File.separator + dirName);
                        dir.mkdirs();
                    } else {
                        int index = entryName.lastIndexOf("\\");
                        if (index != -1) {
                            File dir = new File(dstOutputDirectory + File.separator
                                    + entryName.substring(0, index));
                            dir.mkdirs();
                        }
                        index = entryName.lastIndexOf(File.separator);
                        if (index != -1) {
                            File dir = new File(dstOutputDirectory + File.separator
                                    + entryName.substring(0, index));
                            dir.mkdirs();
                        }
                        File file = new File(dstOutputDirectory + File.separator
                                + zipEntry.getName());
                        in = zipFile.getInputStream(zipEntry);
                        out = new FileOutputStream(file);

                        int byteCount;
                        byte[] by = new byte[1024];

                        while ((byteCount = in.read(by)) != -1) {
                            out.write(by, 0, byteCount);
                        }
                        out.flush();
                   }
                } catch (IOException ex) {
                    ex.printStackTrace();
                    throw new IOException("Unzip File failed:" + ex.toString());
                } finally {
                    if (in != null) {
                        try {
                            in.close();
                        } catch (IOException ex) {
                        }
                    }
                    if (out != null) {
                        try {
                            out.close();
                        } catch (IOException ex) {
                        }
                    }
                }
            }
        } catch (IOException ex) {
            ex.printStackTrace();
            throw new IOException("Unzip File failed:" + ex.toString());
        } finally {
            if (zipFile != null) {
                try {
                    zipFile.close();
                } catch (IOException ex) {
                }
            }
        }
        return dstOutPutPath;
    }

    public static String downloadFile(Context context, String url) {
        HttpURLConnection connection = null;
        byte buffer[]= new byte[2048];
        InputStream in = null;
        FileOutputStream local = null;
        String otaFileName = null;
        try {
            long downloadedLength = 0;
            long fileLength = -1;
            URL mURL = new URL(url);
            connection = (HttpURLConnection)mURL.openConnection();
            connection.setRequestMethod("GET");
            connection.setConnectTimeout(5000);
            fileLength = connection.getContentLength();

            connection.connect();

            in = connection.getInputStream();
            String realFileName = url.substring(url.lastIndexOf("/")+1);;
            otaFileName =  getPath(context, FLAG_STORAGE_SECONDARY) + "/" + realFileName;
            local = new FileOutputStream(otaFileName);

            int readLength = -1;

            while((readLength = in.read(buffer)) != -1){
                local.write(buffer, 0, readLength);
                downloadedLength += readLength;
            }
            connection.disconnect();
        } catch(MalformedURLException e) {
            e.printStackTrace();
        } catch(FileNotFoundException e) {
            e.printStackTrace();
        } catch(IOException e) {
            e.printStackTrace();
        } catch(Exception e){
            e.printStackTrace();
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (local != null) {
                try {
                    local.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            return otaFileName;
        }
    }

    public static void checkIfMoreThanOneCarrierMatchedOneSim(
            HashMap<String, String> matchedCarriers, int phoneCount) {
        for (int i=0; i<phoneCount; i++) {
            Iterator<Entry<String, String>> iterator = matchedCarriers.entrySet().iterator();
            int count = 0;
            while (iterator.hasNext()) {
                Entry<String, String> entry = iterator.next();
                String path = entry.getKey();
                if ((!isFileBasedSwitchEnabled()) && (path.endsWith("2Default.ota.zip"))) {
                    continue;
                }
                String contents = entry.getValue();
                try {
                    CarrierFileFilter filter = new CarrierFileFilter(null, false, null);
                    CarrierFileFilter.CarrierPattern pattern = filter.new CarrierPattern(contents);
                    String slot = pattern.getContentItems(FILTER_ITEM_SUBMASK);
                    if (!TextUtils.isEmpty(slot)) {
                        int subMask = Integer.parseInt(slot);
                        if ((subMask & (0x0001 << i)) == (0x0001 << i)) {
                            count++;
                        }
                    }
                } catch (IOException ex) {
                    Log.w(TAG, "Catch the IOException: " + ex);
                }
            }
            Log.i(TAG, "Slot " + i + " matched " + count + " carrier(s).");

            if (count > 1) {
                iterator = matchedCarriers.entrySet().iterator();
                while (iterator.hasNext()) {
                    Entry<String, String> entry = iterator.next();
                    String path = entry.getKey();
                    if ((!isFileBasedSwitchEnabled()) && (path.endsWith("2Default.ota.zip"))) {
                        continue;
                    }
                    String contents = entry.getValue();
                    try {
                        CarrierFileFilter filter = new CarrierFileFilter(null, false, null);
                        CarrierFileFilter.CarrierPattern pattern =
                                filter.new CarrierPattern(contents);
                        String slot = pattern.getContentItems(FILTER_ITEM_SUBMASK);
                        if (!TextUtils.isEmpty(slot)) {
                            int subMask = Integer.parseInt(slot);
                            if ((subMask & (0x0001 << i)) == (0x0001 << i)) {
                                Log.d(TAG, "Remove " + path);
                                iterator.remove();
                            }
                        }
                    } catch (IOException ex) {
                        Log.w(TAG, "Catch the IOException: " + ex);
                    }
                }
            }
        }
    }

    // Get the preset partitions of Carrier packages.
    public static ArrayList<String> getRegionalPartitions() {
        String systemPos = RegionalizationEnvironment.getStoragePos();
        ArrayList<String> partitions = new ArrayList<String>();
        partitions.add(systemPos);
        if (RegionalizationEnvironment.isSupported()) {
            try {
                IRegionalizationService regionalizationService =
                        RegionalizationEnvironment.getRegionalizationService();
                ArrayList<String> partitionInfo = (ArrayList<String>)
                        regionalizationService.readFile(PARTITION_INFO_FILE, null);
                if (partitionInfo != null && partitionInfo.size() > 0) {
                    for (String partition : partitionInfo) {
                        boolean exists =
                                regionalizationService.checkFileExists(
                                partition + "/flag");
                        if (exists) {
                            partitions.add(partition);
                        }
                    }
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        return partitions;
    }

    public static boolean isPresetMode() {
        return RegionalizationEnvironment.isSupported()
                && !getValue(PROP_KEY_BLOCK_BASED_ENABLED, false);
    }
}
