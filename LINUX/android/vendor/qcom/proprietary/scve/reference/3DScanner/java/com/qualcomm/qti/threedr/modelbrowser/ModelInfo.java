/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.modelbrowser;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Parcel;
import android.os.Parcelable;
import android.provider.MediaStore;
import android.util.Log;

import com.qualcomm.qti.threedr.utils.BitmapCache;
import com.qualcomm.qti.threedr.utils.ZipHelper;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by mithun on 4/4/16.
 */
public class ModelInfo implements Parcelable {
    private static final String TAG = ModelInfo.class.getSimpleName();
    File file; //file pointer to this model folder e.g /sdcard/.../3dr/model_name_done
    String name; //name/modelID of the model stored in the metadat.txt or model_name without the "_done"
    String modelID; //name of the folder without the "_done"
    long createdTimestamp; //stored in the metadata.txt or folder.lastmodified()
    private Bitmap image;
    boolean isSelected;

    public ModelInfo(String parentFolder, String folderName) {
        initModelInfo(parentFolder + "/" + folderName);
    }

    public ModelInfo(String folderFullPath) {
        initModelInfo(folderFullPath);
    }

    void initModelInfo(String folderFullPath) {
        this.file = new File(folderFullPath);
        String folderName = file.getName();
        if (folderName.endsWith(BrowserUtils.FolderSuffix)) {
            this.modelID = folderName.substring(0, folderName.lastIndexOf(BrowserUtils.FolderSuffix));
        } else {
            this.modelID = folderName;
        }
        String metadata = readMetaData();
        this.name = extractName(metadata);
        if (this.name == null) {
            this.name = modelID;
        }
        this.createdTimestamp = extractTimestamp(metadata);
        if (this.createdTimestamp == 0) {
            this.createdTimestamp = file.lastModified();
        }

        //if metadat,txt file was not present then create one now.
        if (metadata == null) {
            updateMetaData();
        }

        //DONOT decode and initialize bitmap here in constructor,
        //instead it will be loaded in getImage() method of this class for optimization purpose
      /*String jpgFilePath = file.getPath() + "/" + "thumb.jpg";
        this.image = BitmapCache.loadBitmapThroughCache(jpgFilePath);*/
        this.image = null;
        this.isSelected = false;
    }

    private int listMatches(String matchString) {
        File[] files = file.listFiles();
        int matchCount = 0;
        if (files != null) {
            for (int i = 0; i < files.length; i++) {
                if (files[i].getName().contains(matchString))
                    matchCount++;
            }
        }
        return matchCount;
    }

    public boolean checkMP4Created() {
        if (listMatches(".mp4") > 0)
            return true;
        else
            return false;
    }

    public int getNumTextures() {
        return listMatches("output_"); //for finding all the jpg files eg: output_0.jpg, output_1.jpg
    }

    public String getName() {
        return name;
    }

    public String getModelID() {
        return modelID;
    }

    public Bitmap getImage() {
        if (image == null) {
            String jpgFilePath = file.getPath() + "/" + "thumb.jpg";
            this.image = BitmapCache.loadBitmapThroughCache(jpgFilePath);
        }
        return image;
    }

    public void setName(String name) {
        this.name = name;
        updateMetaData();
    }

    void updateMetaData() {
        String metaInfo = name + "#" + Long.toString(createdTimestamp);
        try {
            BufferedWriter output = new BufferedWriter(new FileWriter(this.file.getAbsolutePath() + "/metadata.txt"));
            output.write(metaInfo);
            output.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    String readMetaData() {
        String metaData = null;
        FileReader metadataFileReader = null;
        try {
            metadataFileReader = new FileReader(this.file.getAbsolutePath() + "/metadata.txt");
            BufferedReader input = new BufferedReader(metadataFileReader);
            metaData = input.readLine();
            input.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return metaData;
    }

    //metadata will have one line with <model_name>#<created time stamp>
    String extractName(String metaData) {
        if (metaData == null) {
            return null;
        }
        String metaInfo[] = metaData.split("#");
        return metaInfo[0];
    }

    //metadata will have one line with <model_name>#<created time stamp>
    long extractTimestamp(String metaData) {
        if (metaData == null) {
            return 0;
        }
        String metaInfo[] = metaData.split("#");
        long timestamp = Long.parseLong(metaInfo[1]);
        return timestamp;
    }

    public File getFile() {
        return file;
    }

    public static final Creator<ModelInfo> CREATOR = new Creator<ModelInfo>() {
        @Override
        public ModelInfo createFromParcel(Parcel source) {
            return new ModelInfo(source);
        }

        @Override
        public ModelInfo[] newArray(int size) {
            return new ModelInfo[size];
        }
    };


    @Override
    public int describeContents() {
        return 0;
    }

    public ModelInfo(Parcel in) {
        String folderFullPath = in.readString();
        initModelInfo(folderFullPath);

    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(file.getAbsolutePath());
    }

    public static ModelInfo extractModelInfo(Uri uri, Context context) {
        ModelInfo modelInfo = null;
        String tempFolderPath = null;
        try {
            if (uri.getScheme().equals("file")) {
                File importFile = new File(uri.getPath());

                if (importFile.exists()) {
                    String zipfileName = importFile.getName();
                    tempFolderPath = BrowserUtils.ParentFolderPath + "/temp/" + zipfileName.replace(".q3dr", "_done");
                    ZipHelper.unzip(importFile.getAbsolutePath(), tempFolderPath);

                    File tempFolder = new File(tempFolderPath);
                    modelInfo = new ModelInfo(tempFolder.getParent(), tempFolder.getName());

                }
            } else if (uri.getScheme().equals("content")) {
                AssetFileDescriptor fileDescriptor = context.getContentResolver().openAssetFileDescriptor(uri, "r");
                Cursor cursor = context.getContentResolver().query(uri, null, null, null, null);
                cursor.moveToFirst();
                String fileName = cursor.getString(cursor.getColumnIndex(MediaStore.MediaColumns.DISPLAY_NAME));
                long length = fileDescriptor.getLength();
                byte data[] = new byte[(int) length];
                InputStream inputStream = context.getContentResolver().openInputStream(uri);
                tempFolderPath = BrowserUtils.ParentFolderPath + "/temp/" + fileName.replace(".q3dr", "_done");
                ZipHelper.unzip(inputStream, tempFolderPath);
            }
            File tempFolder = new File(tempFolderPath);
            modelInfo = new ModelInfo(tempFolder.getParent(), tempFolder.getName());
        } catch (Exception ex) {
            Log.e(TAG, String.format("Exception importing model '%s'", uri.toString()));

        }

        return modelInfo;
    }
}
