/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.VideoFrameUtils;

import android.graphics.Bitmap;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 * Created by matthewf on 7/30/15.
 */
public class FrameUtils {


    /**
     * Generates the presentation time for frame N, in microseconds.
     */
    public static long computePresentationTime(int frameIndex, int encode_frame_rate) {
        long pres_time = 132L + (long)frameIndex * 1000000L / (long)encode_frame_rate;
        return pres_time;
    }

    public static void writeByteBufferToFile(ByteBuffer outputFrame, int decodeCount) {
        try {
            File outputFile = new File(Environment.getExternalStorageDirectory(),
                    String.format("frame-%02d.bin", decodeCount));
            FileChannel channel = new FileOutputStream(outputFile, false).getChannel();

            channel.write(outputFrame);
            channel.close();

        } catch (IOException e) {
            Log.e("ERROR", "IOException" + e.getMessage());
        }
    }

    public static String writeBitmapToPng(Bitmap bmap, String filePath) {

        OutputStream fOut = null;
        File file = new File(filePath);

        try {
            fOut = new FileOutputStream(file);
            bmap.compress(Bitmap.CompressFormat.PNG, 85, fOut);

            fOut.flush();
            fOut.close();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return file.getAbsolutePath();
    }

    public static ByteBuffer cloneByteBuffer(final ByteBuffer original) {
        // Create clone with same capacity as original.
        final ByteBuffer clone = (original.isDirect()) ?
                ByteBuffer.allocateDirect(original.capacity()) :
                ByteBuffer.allocate(original.capacity());

        // Create read-only copy from orig without modifying it.
        final ByteBuffer readOnlyCopy = original.asReadOnlyBuffer();

        clone.put(readOnlyCopy);

        clone.position(original.position());
        clone.limit(original.limit());
        clone.order(original.order());

        return clone;
    }

}
