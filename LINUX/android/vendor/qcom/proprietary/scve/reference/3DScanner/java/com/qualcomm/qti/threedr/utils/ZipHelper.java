/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/**
 * Created by wsnell on 4/11/2016.
 */

package com.qualcomm.qti.threedr.utils;

import android.util.Log;

import com.qualcomm.qti.threedr.modelbrowser.BrowserUtils;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;


//-------------------------------------------------------------------------
// ZipHelper class
//-------------------------------------------------------------------------
public class ZipHelper {
    //-------------------------------------------------------------------------
    // Data
    //-------------------------------------------------------------------------

    // Tag
    private static final String TAG = ZipHelper.class.getSimpleName();

    //-------------------------------------------------------------------------
    // Methods
    //-------------------------------------------------------------------------

    // Unzip model filename into a new model folder (note so far does not support recursive folders)
    public static boolean unzip(String zipFilename, String directory) {
        boolean result = false;

        InputStream fileInputStream = null;
        try {
            fileInputStream = new FileInputStream(zipFilename);
            return unzip(fileInputStream, directory);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        return result;
    }


    public static boolean unzip(InputStream inputStream, String directory) {
        boolean result = false;
        ZipInputStream zipInputStream = null;

        File destinationDir = new File(directory);
        if (destinationDir.exists()) {
            BrowserUtils.delete(destinationDir);
        }
        destinationDir.mkdirs();

        try {
            zipInputStream = new ZipInputStream(new BufferedInputStream(inputStream));

            byte[] buffer = new byte[1024];
            int count = 0;

            ZipEntry zipEntry;
            while ((zipEntry = zipInputStream.getNextEntry()) != null) {
                String filename = zipEntry.getName();
                String fullFilename = directory + "/" + filename;

                if (zipEntry.isDirectory()) {
                    File fmd = new File(fullFilename);
                    fmd.mkdirs();
                    continue;
                }

                FileOutputStream fileOutputStream = new FileOutputStream(fullFilename);
                while ((count = zipInputStream.read(buffer)) != -1) {
                    fileOutputStream.write(buffer, 0, count);
                }

                fileOutputStream.close();
                zipInputStream.closeEntry();
            }
            zipInputStream.close();
            result = true;
        } catch (IOException ex) {
            ex.printStackTrace();
            result = false;
        }

        return result;
    }

    // Zip model (zip the model dir) into a single zipfile (type .ZIP), (note so far does not support recursive folders)
    //would zip only necessary files from the model parent directory using a filenamefilter
    public static boolean zip(String directory, String zipFilename, FilenameFilter filter) {
        boolean result = false;
        FileOutputStream fileOutputStream = null;
        ZipOutputStream zipOutputStream = null;

        try {
            File directoryFile = new File(directory);

            // If the directory exists then proceed
            if (directoryFile.exists()) {
                File zipFile = new File(zipFilename);

                // Make the folder for the destination zip file parent (if not already exists)
                zipFile.getParentFile().mkdirs();

                // Create the buffer
                byte[] buffer = new byte[1024];

                try {
                    // Create the streams
                    fileOutputStream = new FileOutputStream(zipFile);
                    zipOutputStream = new ZipOutputStream(fileOutputStream);

                    File[] files = directoryFile.listFiles(filter);//get files based on filter
                    for (int i = 0; i < files.length; i++) {
                        // Only addd files and skip diorectories for now
                        if (files[i].isFile()) {
                            Log.d(TAG, String.format("Adding file: '%s'", files[i].getName()));

                            FileInputStream fileInputStream = null;

                            try {
                                fileInputStream = new FileInputStream(files[i]);

                                // begin writing a new ZIP entry, positions the stream to the start of the entry data
                                zipOutputStream.putNextEntry(new ZipEntry(files[i].getName()));
                                int length = 0;

                                while ((length = fileInputStream.read(buffer)) > 0) {
                                    zipOutputStream.write(buffer, 0, length);
                                }
                                zipOutputStream.closeEntry();

                            } finally {
                                // Close the InputStream
                                if (fileInputStream != null) {
                                    fileInputStream.close();
                                }
                            }
                        }
                    }

                    result = true;
                } finally {
                    if (zipOutputStream != null) {
                        zipOutputStream.close();
                    }
                }
            }
        } catch (IOException ex) {
            Log.e(TAG, String.format("Exception ziping directory '%s' to '%s'", directory, zipFilename));
            result = false;
        }

        return result;
    }

}
