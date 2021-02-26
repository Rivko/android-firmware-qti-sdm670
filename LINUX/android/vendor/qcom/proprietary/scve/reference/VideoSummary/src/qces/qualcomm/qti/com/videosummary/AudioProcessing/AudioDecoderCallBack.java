/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.AudioProcessing;

import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;


/**
 * Created by matthewf on 8/12/15.
 */
public class AudioDecoderCallBack extends MediaCodec.Callback {

    private static final String TAG = AudioDecoderCallBack.class.getSimpleName();

    private static final long AUDIO_FADE_TIME_MICROSECS = 2000000; //1 second
    private final AudioTranscodeSharedData audioTranscodeSharedData;

    private MediaExtractor extractor;

    private static final int FRAME_QUE_SIZE_TO_THROTTLE_DECODER = 4;

    public AudioDecoderCallBack(MediaExtractor extractor, AudioTranscodeSharedData audioTranscodeSharedData) {

        this.audioTranscodeSharedData = audioTranscodeSharedData;
        this.extractor = extractor;

    }

    @Override
    public void onInputBufferAvailable(MediaCodec codec, int inIndex) {

        if (inIndex >= 0) {

            if (!audioTranscodeSharedData.decodeEOS_sent && audioTranscodeSharedData.getFrameQueueSize() < FRAME_QUE_SIZE_TO_THROTTLE_DECODER) {

                ByteBuffer buffer = codec.getInputBuffer(inIndex);
                int sampleSize = extractor.readSampleData(buffer, 0);
                long sampleTime = extractor.getSampleTime();

                if (sampleSize < 0 || sampleTime > audioTranscodeSharedData.getDesiredLengthUs()) {
                    //if (!audioTranscodeSharedData.decodeEOS_sent) {
                        Log.d(TAG, "Decoder input: got EOS from extractor so submitting EOS on dec input side");
                        codec.queueInputBuffer(inIndex, 0, 0, 0,
                                MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                    //}
                    audioTranscodeSharedData.decodeEOS_sent = true;

                } else {

                    codec.queueInputBuffer(inIndex, 0, sampleSize,
                            extractor.getSampleTime(), 0);

                    Log.d(TAG, "Decoder queueing input buffer: " + inIndex + " with pts = " + sampleTime);

                    extractor.advance();

                }
            } else {
                Log.d(TAG, "Decoder input: frameQueue size is " + audioTranscodeSharedData.getFrameQueueSize() + " so not queueing the buffer at index: " + inIndex);

                //queueing the input buffer with no data so that we can get back this input buffer later
                codec.queueInputBuffer(inIndex, 0, 0, 0, 0);

            }
        }

    }

    @Override
    public void onOutputBufferAvailable(MediaCodec codec, int outIndex, MediaCodec.BufferInfo info) {

        if (outIndex >= 0 && info.size > 0) {

            ByteBuffer outputBuffer = codec.getOutputBuffer(outIndex);

            final ByteBuffer clone = ByteBuffer.allocateDirect(outputBuffer.capacity());
            clone.position(outputBuffer.position());
            clone.limit(outputBuffer.limit());
            clone.order(outputBuffer.order());

            if (info.presentationTimeUs > audioTranscodeSharedData.getDesiredLengthUs() - AUDIO_FADE_TIME_MICROSECS) {
                ShortBuffer samples = outputBuffer.order(ByteOrder.nativeOrder()).asShortBuffer();
                //manipulate samples if ptsTime falls in desired region
                float scaling_factor = (float) (audioTranscodeSharedData.getDesiredLengthUs() - info.presentationTimeUs) / AUDIO_FADE_TIME_MICROSECS;

                while (samples.remaining() > 0) {
                    short sample = samples.get();
                    short modSample = (short) (sample * scaling_factor);
                    clone.putShort(modSample);
                }
            }
            else
            {
                final ByteBuffer readOnlyCopy = outputBuffer.asReadOnlyBuffer();
                clone.put(readOnlyCopy);
            }

            clone.rewind();

            audioTranscodeSharedData.addToQueue(clone, info.presentationTimeUs);
            audioTranscodeSharedData.lastDecodeSampleTime = info.presentationTimeUs;

            Log.d(TAG, "    Decoder output adding frame " + audioTranscodeSharedData.totalDecodeOutputFrames + " with " + info.size + " bytes and pts = " + info.presentationTimeUs);

            audioTranscodeSharedData.totalDecodeOutputFrames++;
            codec.releaseOutputBuffer(outIndex, false);

        }

        boolean EOS = (info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0;

        if (EOS) {
            Log.d(TAG, "            Decoder output: got EOS, done.");
            releaseDecoderResources(codec);
            //sendCodecManagerMsg(AudioCodecManager.CODEC_MSG_DECODE_AUDIO_DONE);
            return;
        }

    }

    public void releaseDecoderResources(MediaCodec decoder) {
        if (decoder != null) {
            decoder.stop();
            decoder.release();
        }
        if (extractor != null) {
            extractor.release();
            extractor = null;
        }
    }

    @Override
    public void onError(MediaCodec mediaCodec, MediaCodec.CodecException e) {
        Log.d(TAG, "MediaCodec Decoder Exception: " + e.getDiagnosticInfo());
    }

    @Override
    public void onOutputFormatChanged(MediaCodec mediaCodec, MediaFormat mediaFormat) {
        Log.d(TAG, "MediaCodec Decoder MediaFormat Changed");
    }


}
