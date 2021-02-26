/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.CannedDataTest;

import com.qualcomm.qti.threedr.MainActivity;
import com.qualcomm.qti.threedr.utils.Constants;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class ReadCannedIMUData {

    private static ReadCannedIMUData instance = null;
    private final String TAG = "Scan3D";
    private String folderPath;
    private String imuFileName;
    private BufferedReader bufferedReader;

    public static ReadCannedIMUData getInstance() {
        if (instance == null) {
            instance = new ReadCannedIMUData();
        }
        return instance;
    }


    public static void initCannedIMUData() {
        ReadCannedIMUData readCannedIMUData = getInstance();
        switch (MainActivity.cannedDataSetType) {
            case RABBIT_DATASET:
                readCannedIMUData.initSetRabit();
                break;
            case SET3_DATASET:
                readCannedIMUData.initSet3();
                break;
            default:
                throw new RuntimeException("DataSet Type " + MainActivity.cannedDataSetType.toString() + " not found");
        }
    }


    private void initSetRabit() {
        String folderPath = Constants.CANNED_DATA_FOLDER + "/test_data/setRabit/";
        String imuFileName = "imuData.csv";
        initIMUReader(folderPath, imuFileName);
    }

    private void initSet3() {
        String folderPath = Constants.CANNED_DATA_FOLDER + "/test_data/set3/";
        String imuFileName = "imuData.csv";
        initIMUReader(folderPath, imuFileName);
    }

    private boolean initIMUReader(String _FolderPath, String _imuFilePath) {
        folderPath = _FolderPath;
        imuFileName = _imuFilePath;
        initializeIMUReader();
        return true;
    }


    private void initializeIMUReader() {
        try {
            String path = folderPath + imuFileName;
            FileReader fr = new FileReader(path);
            bufferedReader = new BufferedReader(fr);
            bufferedReader.readLine();//skip first line
            bufferedReader.readLine();//skip second line

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public static void deInitCannedIMUReader() {
        getInstance().deInit();
    }

    void deInit(){
        if (bufferedReader != null) {
            try {
                bufferedReader.close();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                bufferedReader = null;
            }
        }
    }

    public IMUData grab() {
        IMUData imuData = null;

        int size;
        String rowData[] = null;
        String line;
        try {
            if (((line = bufferedReader.readLine()) != null) && ((rowData = line.split(",")) != null)) {

                size = Integer.parseInt(rowData[IMU_COL_IDX.N_IMU.ordinal()]);
                float gyroArray[] = new float[size * 3];
                float accArray[] = new float[size * 3];
                long accTimestampArray[] = new long[size];
                long unixTimeStamp = Long.parseLong(rowData[IMU_COL_IDX.SYNC_TIME_STAMP.ordinal()]);
                for (int i = 0; ; ) {
                    gyroArray[i * 3] = Float.parseFloat(rowData[IMU_COL_IDX.GYRO_X.ordinal()]);
                    gyroArray[i * 3 + 1] = Float.parseFloat(rowData[IMU_COL_IDX.GYRO_Y.ordinal()]);
                    gyroArray[i * 3 + 2] = Float.parseFloat(rowData[IMU_COL_IDX.GYRO_Z.ordinal()]);


                    accArray[i * 3] = Float.parseFloat(rowData[IMU_COL_IDX.ACC_X.ordinal()]);
                    accArray[i * 3 + 1] = Float.parseFloat(rowData[IMU_COL_IDX.ACC_Y.ordinal()]);
                    accArray[i * 3 + 2] = Float.parseFloat(rowData[IMU_COL_IDX.ACC_Z.ordinal()]);

                    accTimestampArray[i] = Long.parseLong(rowData[IMU_COL_IDX.TIME_STAMP.ordinal()]);

                    //do not read more than "size" lines
                    i++;
                    if (i < size) {
                        rowData = bufferedReader.readLine().split(",");
                    } else {
                        break;
                    }
                }

                imuData = new IMUData(gyroArray, accArray, accTimestampArray, unixTimeStamp, size);

            }

        } catch (IOException e) {
            e.printStackTrace();
        }
        return imuData;
    }

    public static enum IMU_COL_IDX {
        RETURN(0),
        TIME_STAMP(1),
        GYRO_X(2),
        GYRO_Y(3),
        GYRO_Z(4),
        ACC_X(5),
        ACC_Y(6),
        ACC_Z(7),
        SYNC_TIME_STAMP(8),
        IMU_SET_IDX(9),
        N_IMU(10);

        int val;

        IMU_COL_IDX(int i) {
            val = i;
        }
    }

    public class IMUData {
        public float gyroArray[];
        public float accArray[];
        public long accTimestampArray[];
        public long unixTimeStamp;
        public int size;

        public IMUData(float[] gyroArray, float[] accArray, long[] accTimestampArray, long unixTimeStamp, int size) {
            this.gyroArray = gyroArray;
            this.accArray = accArray;
            this.accTimestampArray = accTimestampArray;
            this.unixTimeStamp = unixTimeStamp;
            this.size = size;
        }
    }
}
