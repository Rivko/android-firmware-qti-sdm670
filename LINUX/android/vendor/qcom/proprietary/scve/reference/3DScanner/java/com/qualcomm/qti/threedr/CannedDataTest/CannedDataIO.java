/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.CannedDataTest;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.util.Log;
import android.util.Pair;
//import android.content.Context;

import com.qualcomm.qti.threedr.FastCVJNILib;
import com.qualcomm.qti.threedr.MainActivity;
import com.qualcomm.qti.threedr.utils.Constants;

//import org.apache.commons.io.FileUtils;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

public class CannedDataIO {

    private static CannedDataIO instance = null;
    private final String TAG = "Scan3D";
    private String fileFolder;
    private String colorImgPrefix;
    private String colorImgSurfix;
    private String depthImgPrefix;
    private String depthImgSurfix;
    private int startFrame;
    private int frameStep;
    private int lastFrame;
    private int frameCnt;
    private int fps = -1;
    private long curr_t;

    //to read and store the time stamp and sync timestamp
    private ArrayList<Pair<Long,Long>> timestampArrayList = null;

    public static enum CannedDataSetType {ORBBEC_SENSOR, SHANDON_DATASET, RABBIT_DATASET, SET3_DATASET}


    public static CannedDataIO getInstance() {
        if (instance == null) {
            instance = new CannedDataIO();
        }
        return instance;
    }

    public static void initCannedData() {
        CannedDataIO cannedDataIO = getInstance();

        switch (MainActivity.cannedDataSetType) {
            case SHANDON_DATASET:
                cannedDataIO.initForShandonDataSet();
                break;
            case ORBBEC_SENSOR:
                cannedDataIO.initForOrbbecDump();
                break;
            case RABBIT_DATASET:
                cannedDataIO.initForSetRabitDataSet();
                break;
            case SET3_DATASET:
                cannedDataIO.initForSet3DataSet();
                break;
            default:
                throw new RuntimeException("DataSet Type " + MainActivity.cannedDataSetType.toString() + " not found");
        }
    }

    public void loadCannedDataFromDevice() {
        int startFrame = 0;
        int frameStep = 1;
        int lastFrame = 515;
        String path = Constants.CANNED_DATA_FOLDER + "/QData4/";
        String colorImgPrefix = "colorImg_";
        String color720Prefix = "Color720p_";
        String colorImgSurfix = ".jpg";
        String depthImgPrefix = "depthImg_";
        String depthImgSurfix = ".bin";

        initImageReader(startFrame, frameStep, lastFrame, path, color720Prefix, colorImgSurfix, depthImgPrefix, depthImgSurfix, -1);

        GrabResult grabResult = grab();
        while (grabResult != null) {
            // Log.i(TAG, " frame cnt = "+grabResult.currTime+" , widh:"+grabResult.rgbImage.getWidth()+", ht:"+grabResult.rgbImage.getHeight()+", depth length:"+grabResult.depthImage.length);
            grabResult = grab();
        }

    }

    private void initForShandonDataSet() {
        int startFrame = 0;
        int frameStep = 1;
        int lastFrame = 515;
        String path = Constants.CANNED_DATA_FOLDER + "/QData4/";
        String colorImgPrefix = "colorImg_";
        String color720Prefix = "Color720p_";
        String colorImgSurfix = ".jpg";
        String depthImgPrefix = "depthImg_";
        String depthImgSurfix = ".bin";

        initImageReader(startFrame, frameStep, lastFrame, path, color720Prefix, colorImgSurfix, depthImgPrefix, depthImgSurfix, -1);
    }

    /**
     * This method will run offline data for dumps from irbbec sensor for rgb and depth res 640x480 from sdcard/3dr/dump
     */
    private void initForOrbbecDump() {
        int startFrame = 0;
        int frameStep = 1;
        String path = Constants.DUMP_FOLDER + "/";
        lastFrame = new File(path).listFiles().length / 2 - 1;
        String colorImgPrefix = "rgb888_640x480_";
        String colorImgSurfix = ".bin";
        String depthImgPrefix = "depth_short_640x480_";
        String depthImgSurfix = ".bin";

        initImageReader(startFrame, frameStep, lastFrame, path, colorImgPrefix, colorImgSurfix, depthImgPrefix, depthImgSurfix, -1);
    }

    private void initForSetRabitDataSet() {
        int startFrame = 0;
        int frameStep = 1;
        String folderPath = Constants.CANNED_DATA_FOLDER + "/test_data/setRabit/";
        String path = folderPath + "jpeg";
        int lastFrame = new File(path).listFiles().length - 1;

        String colorImgPrefix = "jpeg/imgColor720p_";
        String colorImgSurfix = ".jpg";
        String depthImgPrefix = "depthImgSeq";
        String depthImgSurfix = ".data";

        String frameTimestampFile = "frameTimestamp.csv";

        loadTimestampArraylist(folderPath+frameTimestampFile);
        initImageReader(startFrame, frameStep, lastFrame, folderPath, colorImgPrefix, colorImgSurfix, depthImgPrefix, depthImgSurfix, -1);
    }


    private void initForSet3DataSet() {
        int startFrame = 0;
        int frameStep = 1;
        String folderPath = Constants.CANNED_DATA_FOLDER + "/test_data/set3/";
        String path = folderPath + "jpeg";
        int lastFrame = new File(path).listFiles().length - 1;

        String colorImgPrefix = "jpeg/imgColor720p_";
        String colorImgSurfix = ".jpg";
        String depthImgPrefix = "depthImgSeq";
        String depthImgSurfix = ".data";

        String frameTimestampFile = "frameTimestamp.csv";

        loadTimestampArraylist(folderPath+frameTimestampFile);
        initImageReader(startFrame, frameStep, lastFrame, folderPath, colorImgPrefix, colorImgSurfix, depthImgPrefix, depthImgSurfix, -1);
    }


    private void loadTimestampArraylist(String timestampFilePath){
        String line;
        String rowdata[];
        timestampArrayList = new ArrayList<>();

        try {
            BufferedReader bufferedReader = new BufferedReader(new FileReader(timestampFilePath));
            //skip first line which has frame count and num frame etc
            bufferedReader.readLine();
            while((line = bufferedReader.readLine())!=null){
                rowdata = line.split(",");
                if(rowdata.length<2){
                    throw new RuntimeException(" timestamp file +"+timestampFilePath+" for line:"+timestampArrayList.size()+" has less then 2 values");
                }
                timestampArrayList.add(new Pair<Long, Long>(Long.parseLong(rowdata[0]), Long.parseLong(rowdata[1])));
            }
            Log.i(TAG,"canneddata finished reading timestamp file, had "+timestampArrayList.size()+" timestamps");
            bufferedReader.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    /**
     * This method will initialize all the class members to read/grab data from device
     */
    private boolean initImageReader(int _startFrame,
                                    int _frameStep,
                                    int _lastFrame,
                                    String _fileFolder,
                                    String _colorImgPrefix,
                                    String _colorImgSurfix,
                                    String _depthImgPrefix,
                                    String _depthImgSurfix,
                                    int _fps) {
        startFrame = _startFrame;
        frameStep = _frameStep;
        lastFrame = _lastFrame;
        fileFolder = _fileFolder;
        frameCnt = startFrame;
        fps = _fps;


        colorImgPrefix = _colorImgPrefix;
        colorImgSurfix = _colorImgSurfix;
        depthImgPrefix = _depthImgPrefix;
        depthImgSurfix = _depthImgSurfix;

        // init timer
        curr_t = System.currentTimeMillis();

        Log.i(TAG, "init cannedData startFrame:" + startFrame + ", frameStep:" + frameStep + ", lastFrame:" + lastFrame + ", path:" + fileFolder +
                ", colorImgPrefix:" + colorImgPrefix + ", colorImgSurfix:" + colorImgSurfix + ", depthImgPrefix:" + depthImgPrefix + ", depthImgSurfix:" + depthImgSurfix + ", fps:" + fps);
        return true;
    }

    public GrabResult grab() {
        Log.i(TAG, "grab called for frameCnt=" + frameCnt + ", lastFrame=" + lastFrame);
        if (frameCnt > lastFrame) {
            return null;
        }

        long timeNow = System.currentTimeMillis();
        double delta = (timeNow - curr_t) / (float) 1000; //to get in seconds

        float thresh = 1.0f;
        if (fps > 0 && (delta * fps) < thresh) {
            return null;
        }
        curr_t = timeNow;

        String imageFilePath = fileFolder + colorImgPrefix + frameCnt + colorImgSurfix;


        //byte[] rgbImage = getBitmapForImageFile(imageFilePath);
        byte[] rgbImage = null;
        if (imageFilePath.endsWith("jpg")) {
            rgbImage = jpgFileToRGB888(imageFilePath);
        } else if (imageFilePath.endsWith("bin")) {
            rgbImage = getRGBByteArrayFromBinFile(imageFilePath);
        }

        //YuvImage yuvImage = RGBToNV21(bmap, bmap.getWidth(), bmap.getHeight());

        byte[] depthByteArray = null;
        String depthFilePath = null;
        switch (MainActivity.cannedDataSetType) {
            case ORBBEC_SENSOR:
            case SHANDON_DATASET: {
                //these are for separate depth img files for each frame
                depthFilePath = fileFolder + depthImgPrefix + frameCnt + depthImgSurfix;
                depthByteArray = getDepthInfoForFile(depthFilePath);
                break;
            }
            case RABBIT_DATASET:
            case SET3_DATASET:{
                //this is when we have one huge depth image file for all frames
                depthFilePath = fileFolder + depthImgPrefix + depthImgSurfix;
                depthByteArray = getDepthInfoFromFileForFrameIndex(depthFilePath, frameCnt);
                break;
            }
        }

        long timestamp = frameCnt;
        long syncTimestamp = 0;
        if(timestampArrayList!=null && timestampArrayList.size()> frameCnt){
            timestamp = timestampArrayList.get(frameCnt).first;
            syncTimestamp = timestampArrayList.get(frameCnt).second;
        }
        GrabResult grabResult = new GrabResult(frameCnt, rgbImage, depthByteArray, timestamp, syncTimestamp);

        frameCnt += frameStep;

        return grabResult;
    }

    private byte[] getBitmapForImageFile(String path) {
        File f = new File(path);
        if (!f.exists()) {
            throw new RuntimeException("file path not found for " + path);
        }
        Bitmap bmap = BitmapFactory.decodeFile(path);
        return bitmapToRGB888(bmap);
    }

    private byte[] getDepthInfoForFile(String path) {

       FileInputStream fis;
       try {
          fis =  new FileInputStream(new File(path));
       }
       catch (FileNotFoundException e) {
          throw new RuntimeException("file path not found for " + path);
       }

       int bufferSize;
       try {
          bufferSize = (int) fis.getChannel().size();
       }
       catch (IOException e) {
          throw new RuntimeException("fail to determine file size: " + path);
       }
       byte[] depthByteArray = new byte[bufferSize];

       try {
          fis.read(depthByteArray);
       }
       catch (IOException e) {
          throw new RuntimeException("fail to read file " + path);
       }

       return depthByteArray;
    }


    /**
     * This method is used to ready n bytes = DepthW x DepthH bytes from the single DepthFile which has dataSetType for all frames
     * For Data set provided by Zhijie
     *
     * @param path path of the depthImg file
     * @param idx  Frame index
     * @return depthByteArray for idx frame
     */
    private byte[] getDepthInfoFromFileForFrameIndex(String path, int idx) {

        int numBytesPerFrame = MainActivity.DepthImageWidth * MainActivity.DepthImageHeight * 2;
        byte[] depthByteArray = new byte[numBytesPerFrame];
        int readCount = 0;

        FileInputStream fis = null;
        try {
            fis = new FileInputStream(path);
            int nLineCount = 0;
            int byteSkipCount = 22;//to skip the first 3 lines which is 22bytes for Rabbit dataset
/*            byte[] temp = new byte[1];
            while(nLineCount < 3){
                fis.read(temp);
                byteSkipCount++;
                if(temp[0]=='\n'){
                    nLineCount++;
                }
            }*/
            long skipped = fis.skip((idx * numBytesPerFrame) + byteSkipCount);
            readCount = fis.read(depthByteArray, 0, numBytesPerFrame);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                fis.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if (readCount != numBytesPerFrame) {
            throw new RuntimeException("something is wrong reading depth info from depthImage @ " + path + " for frame idx:" + idx + ", read bytes =" + readCount + ", expected:" + numBytesPerFrame + " bytes");
        }

        return depthByteArray;
    }

    private YuvImage RGBToNV21(Bitmap b, int width, int height) {
        int bytes = b.getByteCount();
        ByteBuffer buffer = ByteBuffer.allocate(bytes);
        b.copyPixelsToBuffer(buffer); //Move the byte data to the buffer

        byte[] data = buffer.array(); //Get the bytes array of the bitmap
        YuvImage yuv = new YuvImage(data, ImageFormat.NV21, width, height, null);
        Bitmap b2 = YUV420ToRGB(yuv.getYuvData(), width, height);
        b.recycle();
        b = null;

        return yuv;
    }

    private Bitmap YUV420ToRGB(byte[] data, int width, int height) {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        YuvImage yuv = new YuvImage(data, ImageFormat.NV21, width, height, null);
        //data is the byte array of your YUV image, that you want to convert
        yuv.compressToJpeg(new Rect(0, 0, width, height), 100, out);
        byte[] bytes = out.toByteArray();

        Bitmap bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
        return bitmap;
    }

    private byte[] bitmapToRGB888(Bitmap img) {
        int numPixels = img.getWidth() * img.getHeight();
        byte[] pixels = new byte[numPixels * 3];

        int count = 0;
        for (int h = 0; h < img.getHeight(); h++) {
            for (int w = 0; w < img.getWidth(); w++) {
                int c = img.getPixel(w, h);
                pixels[count++] = (byte) (c >> 16 & 0xFF);
                pixels[count++] = (byte) (c >> 8 & 0xFF);
                pixels[count++] = (byte) (c & 0xFF);
            }
        }

        return pixels;
    }

    private byte[] jpgFileToRGB888(String filepath) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.RGB_565;

        Bitmap mCurrentBitmap = BitmapFactory.decodeFile(filepath, options);


        ByteBuffer bb = ByteBuffer.allocate(mCurrentBitmap.getWidth() * mCurrentBitmap.getHeight() * 2);
        byte[] b = new byte[mCurrentBitmap.getWidth() * mCurrentBitmap.getHeight() * 2];

        mCurrentBitmap.copyPixelsToBuffer(bb);

        bb.rewind();
        bb.get(b);
        byte[] rgbBytes = new byte[mCurrentBitmap.getWidth() * mCurrentBitmap.getHeight() * 3];

        FastCVJNILib.RGB565ByteBufferToRGB888ByteArray(b, mCurrentBitmap.getWidth(), mCurrentBitmap.getHeight(), rgbBytes);

        return rgbBytes;
    }

    private byte[] getRGBByteArrayFromBinFile(String filepath) {

       FileInputStream fis;
       try {
          fis =  new FileInputStream(new File(filepath));
       }
       catch (FileNotFoundException e) {
          throw new RuntimeException("file path not found for " + filepath);
       }

       int bufferSize;
       try {
          bufferSize = (int) fis.getChannel().size();
       }
       catch (IOException e) {
          throw new RuntimeException("fail to determine file size: " + filepath);
       }
       byte[] rgbByteArray = new byte[bufferSize];

       try {
          fis.read(rgbByteArray);
       }
       catch (IOException e) {
          throw new RuntimeException("fail to read file " + filepath);
       }

       return rgbByteArray;
    }

    public class GrabResult {
        byte[] rgbImage;
        byte[] depthImage;
        int frameIdx;
        long timestamp;
        long syncTimestamp;

        public GrabResult(int frameIdx, byte[] rgbImage, byte[] depthImage, long timestamp, long syncTimestamp) {
            this.frameIdx = frameIdx;
            this.rgbImage = rgbImage;
            this.depthImage = depthImage;
            this.timestamp = timestamp;
            this.syncTimestamp = syncTimestamp;
        }
    }
}
