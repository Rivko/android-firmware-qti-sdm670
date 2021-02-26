/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.misc;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.Objects;

/**
 * This class wraps a {@link ByteBuffer} and presents it as an {@link InputStream}
 */
public class ByteBufferInputStream extends InputStream {
    private final ByteBuffer buf;

    /**
     * Create a new {@link ByteBufferInputStream}
     * @param buf a {@link ByteBuffer} with which to back the input stream
     * @throws NullPointerException if buf is null
     */
    public ByteBufferInputStream(ByteBuffer buf) {
        Objects.requireNonNull(buf);

        this.buf = buf;
        buf.position(0);
    }

    @Override
    public int read() throws IOException {
        if (!buf.hasRemaining()) {
            return -1;
        }

        return ((int)buf.get() & 0xFF);
    }

    @Override
    @SuppressWarnings("NullableProblems")
    public int read(byte[] buffer, int byteOffset, int byteCount) throws IOException {
        if (!buf.hasRemaining()) {
            return -1;
        }

        final int remaining = buf.remaining();
        if (byteCount > remaining) {
            byteCount = remaining;
        }
        buf.get(buffer, byteOffset, byteCount);

        return byteCount;
    }

    @Override
    public long skip(long byteCount) throws IOException {
        if (byteCount <= 0) {
            return 0;
        }

        final int remaining = buf.remaining();
        if (byteCount > remaining) {
            byteCount = remaining;
        }

        buf.position(buf.position() + (int)byteCount);

        return byteCount;
    }
}
