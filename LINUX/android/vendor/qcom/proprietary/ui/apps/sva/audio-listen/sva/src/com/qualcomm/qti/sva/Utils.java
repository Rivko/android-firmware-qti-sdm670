/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.nio.channels.FileChannel;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import com.qualcomm.qti.sva.ListenAudioRecorder;


public class Utils {
    public static final String DATE_TIME_FORMAT = "yyyy-MM-dd-HH-mm-ss";
    private static final String TAG = "ListenLog.Utils";

    @SuppressLint("SimpleDateFormat")
    public static String getCurrentDateAndTime() {
        Calendar cal = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat(DATE_TIME_FORMAT);
        return sdf.format(cal.getTime());
    }

    public static void createDirIfNotExists(String directoryToCreate) {
        File directoryFile = new File(directoryToCreate);
        if (!directoryFile.isDirectory()) {
            directoryFile.mkdirs();
        }
    }

    public static final String[] copyAssetsToStorage(Context context, String destAssetDirectory) {
        AssetManager assetManager = context.getAssets();
        String[] assetFiles = null;

        // Get asset files.
        try {
            assetFiles = assetManager.list("");
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (0 == assetFiles.length) {
            return null;
        }

        int index = 0;
        String[] copiedFilePaths = new String[assetFiles.length];

        //copy asset files
        try {
            for (String filename : assetFiles) {
                String outputFilePath = destAssetDirectory + "/" + filename;
                if (new File(outputFilePath).exists()) {
                    Log.v(TAG, "copyAssetsToStorage: yay! a SM was saved!: " + outputFilePath);
                    continue;
                }
                InputStream in = assetManager.open(filename);
                OutputStream out = new FileOutputStream(outputFilePath);
                copyFile(in, out);

                in.close();
                in = null;
                out.flush();
                out.close();
                out = null;

                copiedFilePaths[index++] = outputFilePath;
            }

            return copiedFilePaths;

        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    private static final void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    public static ByteBuffer readFileToByteBuffer(String fileStr) throws IOException {
        File file = new File(fileStr);
        RandomAccessFile raf = new RandomAccessFile(file, "r");
        ByteBuffer byteBuffer;
        try {
            long longlength = raf.length();
            int length = (int) longlength;
            if (length != longlength) throw new IOException("File size >= 2 GB");

            byte[] data = new byte[length];
            raf.readFully(data);
            byteBuffer = ByteBuffer.allocateDirect(data.length);
            byteBuffer.put(data);
        } finally {
            raf.close();
        }
        return byteBuffer;
    }

    public static byte[] readFileToByteArray(String fileStr) throws IOException {
        File file = new File(fileStr);
        RandomAccessFile raf = new RandomAccessFile(file, "r");
        byte[] data;
        try {
            long longlength = raf.length();
            int length = (int) longlength;
            if (length != longlength) throw new IOException("File size >= 2 GB");

            data = new byte[length];
            raf.readFully(data);
        } finally {
            raf.close();
        }
        return data;
    }

    public static void saveByteBufferToFile(ByteBuffer extendedSoundModel, String filePath) {
        FileChannel channel;
        try {
            channel = new FileOutputStream(filePath, false).getChannel();
            extendedSoundModel.flip();
            channel.write(extendedSoundModel);
            channel.close();
        } catch (FileNotFoundException e) {
            Log.e(TAG, "outputExtendedSoundModel: FileNotFound: " + e.getMessage());
        } catch (IOException e) {
            Log.e(TAG, "outputExtendedSoundModel: unable to write sound model: " + e.getMessage());
        }
    }

    public static void writeShortBufferToWavFile(ShortBuffer inShortBuffer, String filePath,
                                                 boolean isAppendToFile) {
        DataOutputStream doStream = null;
        try {
            doStream = new DataOutputStream(
                    new BufferedOutputStream(new FileOutputStream(filePath, isAppendToFile)));

            Log.v(TAG, "writeShortBufferToWavFile: inShortBuffer.position()= " +
                    inShortBuffer.position());
            Log.v(TAG, "writeShortBufferToWavFile: inShortBuffer.array().length= " +
                    inShortBuffer.array().length);
            int audioDataLength = inShortBuffer.position() * 2;
            inShortBuffer.rewind();
            byte[] wavHeader = getWavHeader(audioDataLength, audioDataLength + 44);
            // write header
            doStream.write(wavHeader);
            for (int i = 0; i < inShortBuffer.array().length; i++) {
                doStream.writeShort(swapByteOrder(inShortBuffer.array()[i]));
            }
        } catch (FileNotFoundException e) {
            Log.e(TAG, "writeShortBufferToWavFile: FileNotFound: " + e.getMessage());
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                doStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void writeBufferToWavFile(byte[] inBuff, int bufferSize, String filePath,
                                            boolean isAppendToFile) {

        DataOutputStream doStream = null;
        try {
            Log.v(TAG, "writeBufferToWavFile: stream created bufferSize " + bufferSize);
            doStream = new DataOutputStream(
                    new BufferedOutputStream(new FileOutputStream(filePath, isAppendToFile)));
            int audioDataLength = bufferSize;
            byte[] wavHeader = getWavHeader(audioDataLength, audioDataLength + 44);
            // write header
            Log.v(TAG, "writeBufferToWavFile: write header");
            doStream.write(wavHeader);
            // write audio samples
            Log.v(TAG, "writeBufferToWavFile: write " + (audioDataLength < 1) + " samples");
            doStream.write(inBuff, 0, audioDataLength);
            Log.v(TAG, "writeBufferToWavFile: complete");
        } catch (FileNotFoundException e) {
            Log.e(TAG, "writeShortBufferToWavFile: FileNotFound: " + e.getMessage());
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                doStream.close();
                Log.v(TAG, "writeBufferToWavFile: stream close");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static short swapByteOrder(short inShort) {
        int byte1 = inShort & 0xff;
        int byte2 = (inShort >> 8) & 0xff;
        return (short) (byte1 << 8 | byte2 << 0);
    }

    public static ShortBuffer readWavFile(String filePath) throws IOException {
        Log.v(TAG, "readWavFile: filePath= " + filePath);
        File file = new File(filePath);
        RandomAccessFile raf = new RandomAccessFile(file, "r");
        Log.v(TAG, "readWavFile: fileLength= " + String.valueOf(raf.length()));
        int nHdrLen = 44; // RIFF Wav file header length
        try {
            // skip file's header
            raf.skipBytes(nHdrLen);
            long longlength = raf.length();
            int length = (int) longlength;
            if (length != longlength) throw new IOException("File size >= 2 GB");
            Log.v(TAG, "readWavFile file length = " + length);
            length = (int) (longlength - nHdrLen);
            int nShorts = (length + 1) / 2;
            short[] data = new short[nShorts];

            // read bytes as a short taking endianness into account
            short tmpShrt;
            for (int i = 0; i < nShorts; i++) {
                if (ByteOrder.LITTLE_ENDIAN == ByteOrder.nativeOrder()) {
                    tmpShrt = raf.readShort();
                    // flip bytes
                    data[i] = (short) ((tmpShrt & 0x00FF) << 8);
                    data[i] += ((tmpShrt & 0xFF00) >> 8);
                } else {
                    data[i] = raf.readShort();
                }
            }

            ShortBuffer sb = ShortBuffer.allocate(data.length);
            sb.put(data);
            return sb;
        } finally {
            raf.close();
        }
    }

    public static byte[] getWavHeader(long audioDataLength, long audioDataAndHeaderLength)
            throws IOException {
        long blocksPerSecond = ListenAudioRecorder.SAMPLE_RATE;
        long dataRate = ListenAudioRecorder.BITS_PER_SAMPLE * ListenAudioRecorder.SAMPLE_RATE *
                ListenAudioRecorder.NUM_CHANNELS / 8;

        byte[] wavHeader = new byte[44];
        wavHeader[0] = 'R';  // chunk ID = "RIFF"
        wavHeader[1] = 'I';
        wavHeader[2] = 'F';
        wavHeader[3] = 'F';
        wavHeader[4] = (byte) (audioDataAndHeaderLength & 0xff); // chunk size
        wavHeader[5] = (byte) ((audioDataAndHeaderLength >> 8) & 0xff);
        wavHeader[6] = (byte) ((audioDataAndHeaderLength >> 16) & 0xff);
        wavHeader[7] = (byte) ((audioDataAndHeaderLength >> 24) & 0xff);
        wavHeader[8] = 'W'; // wave ID = "WAVE"
        wavHeader[9] = 'A';
        wavHeader[10] = 'V';
        wavHeader[11] = 'E';
        wavHeader[12] = 'f'; // chunk ID = "fmt "
        wavHeader[13] = 'm';
        wavHeader[14] = 't';
        wavHeader[15] = ' ';
        wavHeader[16] = 16;  // chunk size = 16
        wavHeader[17] = 0;
        wavHeader[18] = 0;
        wavHeader[19] = 0;
        wavHeader[20] = 1; // format code (0x0001 is PCM)
        wavHeader[21] = 0;
        wavHeader[22] = (byte) ListenAudioRecorder.NUM_CHANNELS;  // number of interleaved channels
        wavHeader[23] = 0;
        wavHeader[24] = (byte) (blocksPerSecond & 0xff); // Sampling rate (blocks/sec)
        wavHeader[25] = (byte) ((blocksPerSecond >> 8) & 0xff);
        wavHeader[26] = (byte) ((blocksPerSecond >> 16) & 0xff);
        wavHeader[27] = (byte) ((blocksPerSecond >> 24) & 0xff);
        wavHeader[28] = (byte) (dataRate & 0xff); // Data rate
        wavHeader[29] = (byte) ((dataRate >> 8) & 0xff);
        wavHeader[30] = (byte) ((dataRate >> 16) & 0xff);
        wavHeader[31] = (byte) ((dataRate >> 24) & 0xff);
        wavHeader[32] = (byte) (ListenAudioRecorder.NUM_CHANNELS * ListenAudioRecorder.BITS_PER_SAMPLE / 8); // Data block size
        wavHeader[33] = 0;
        wavHeader[34] = ListenAudioRecorder.BITS_PER_SAMPLE;
        wavHeader[35] = 0;
        wavHeader[36] = 'd'; // chunk ID = "data"
        wavHeader[37] = 'a';
        wavHeader[38] = 't';
        wavHeader[39] = 'a';
        wavHeader[40] = (byte) (audioDataLength & 0xff);
        wavHeader[41] = (byte) ((audioDataLength >> 8) & 0xff);
        wavHeader[42] = (byte) ((audioDataLength >> 16) & 0xff);
        wavHeader[43] = (byte) ((audioDataLength >> 24) & 0xff);

        return wavHeader;
    }

    public static void createNewSoundModel(String originFilePath, String newFilePath) {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        try {
            inputStream = new FileInputStream(originFilePath);
            outputStream = new FileOutputStream(newFilePath);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        // Transfer bytes from in to out
        byte[] buffer = new byte[1024];
        int length;
        try {
            while ((length = inputStream.read(buffer)) > 0) {
                outputStream.write(buffer, 0, length);
            }
            inputStream.close();
            outputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        Log.v(TAG, "createNewUserSoundModel: completed");
    }

    public static void deleteFile(String inFilePath) {
        try {
            File file = new File(inFilePath);
            if (false == file.delete()) {
                Log.e(TAG, "deleteFile: failed to delete file= " + inFilePath);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void openAlertDialog(Context context, final String inTag, String title, String message) {
        if (null == context || null == inTag || null == message) {
            Log.e(inTag, "Utils.openAlertMessage: context, tag, or title are null.");
            return;
        }
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        if (null == title) {
            title = context.getResources().getString(R.string.com_app_name);
        }
        Log.v(inTag, "Utils.openAlertMessage: title= " + title);
        Log.v(inTag, "Utils.openAlertMessage: message= " + message);
        builder.setTitle(title)
                .setMessage(message)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, null);

        if (false == ((Activity) context).isFinishing()) {
            builder.show();
        }
    }
}
