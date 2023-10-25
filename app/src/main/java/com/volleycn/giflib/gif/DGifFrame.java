/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.volleycn.giflib.gif;

import android.graphics.Bitmap;

import java.io.InputStream;
import java.nio.ByteBuffer;

public class DGifFrame {
    static {
        System.loadLibrary("giflib");
    }

    private final long mNativeFrame;
    private final int mWidth;
    private final int mHeight;
    private final boolean mOpaque;
    private final int mFrameCount;
    private final int mDefaultLoopCount;

    public int getWidth() { return mWidth; }
    public int getHeight() { return mHeight; }
    public boolean isOpaque() { return mOpaque; }
    public int getFrameCount() { return mFrameCount; }
    public int getDefaultLoopCount() { return mDefaultLoopCount; }

    private static native DGifFrame nativeDecodeByteArray(byte[] data, int offset, int length);
    private static native DGifFrame nativeDecodeStream(InputStream is, byte[] tempStorage);
    private static native DGifFrame nativeDecodeByteBuffer(ByteBuffer buffer, int offset, int capacity);
    private static native void nativeDestroyFrame(long nativeFrame);
    private static native long nativeCreateState(long nativeFrame);
    private static native void nativeDestroyState(long nativeState);
    private static native long nativeGetFrame(long nativeState, int frameNr,
            Bitmap output, int previousFrameNr);
    @SuppressWarnings("unused") // called by native
    private DGifFrame(long nativeFrame, int width, int height,
                      boolean opaque, int frameCount, int defaultLoopCount) {
        mNativeFrame = nativeFrame;
        mWidth = width;
        mHeight = height;
        mOpaque = opaque;
        mFrameCount = frameCount;
        mDefaultLoopCount = defaultLoopCount;
    }

    public static DGifFrame decodeByteArray(byte[] data) {
        return decodeByteArray(data, 0, data.length);
    }

    public static DGifFrame decodeByteArray(byte[] data, int offset, int length) {
        if (data == null) throw new IllegalArgumentException();
        if (offset < 0 || length < 0 || (offset + length > data.length)) {
            throw new IllegalArgumentException("invalid offset/length parameters");
        }
        return nativeDecodeByteArray(data, offset, length);
    }

    public static DGifFrame decodeByteBuffer(ByteBuffer buffer) {
        if (buffer == null) throw new IllegalArgumentException();
        if (!buffer.isDirect()) {
            if (buffer.hasArray()) {
                byte[] byteArray = buffer.array();
                return decodeByteArray(byteArray, buffer.position(), buffer.remaining());
            } else {
                throw new IllegalArgumentException("Cannot have non-direct ByteBuffer with no byte array");
            }
        }
        return nativeDecodeByteBuffer(buffer, buffer.position(), buffer.remaining());
    }

    public static DGifFrame decodeStream(InputStream stream) {
        if (stream == null) throw new IllegalArgumentException();
        byte[] tempStorage = new byte[16 * 1024];
        return nativeDecodeStream(stream, tempStorage);
    }

    State createState() {
        if (mNativeFrame == 0) {
            throw new IllegalStateException("attempted to use incorrectly built Frame");
        }

        long nativeState = nativeCreateState(mNativeFrame);
        if (nativeState == 0) {
            return null;
        }
        return new State(nativeState);
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeFrame != 0) nativeDestroyFrame(mNativeFrame);
        } finally {
            super.finalize();
        }
    }
    static class State {
        private long mNativeState;

        public State(long nativeState) {
            mNativeState = nativeState;
        }

        public void destroy() {
            if (mNativeState != 0) {
                nativeDestroyState(mNativeState);
                mNativeState = 0;
            }
        }

        public long getFrame(int frameNr, Bitmap output, int previousFrameNr) {
            if (output == null || output.getConfig() != Bitmap.Config.ARGB_8888) {
                throw new IllegalArgumentException("Bitmap passed must be non-null and ARGB_8888");
            }
            if (mNativeState == 0) {
                throw new IllegalStateException("attempted to draw destroyed FrameState");
            }
            return nativeGetFrame(mNativeState, frameNr, output, previousFrameNr);
        }
    }
}
