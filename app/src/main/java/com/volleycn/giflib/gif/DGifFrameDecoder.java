package com.volleycn.giflib.gif;

import android.graphics.Bitmap;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.bumptech.glide.load.Options;
import com.bumptech.glide.load.ResourceDecoder;
import com.bumptech.glide.load.engine.bitmap_recycle.BitmapPool;

import java.io.IOException;
import java.io.InputStream;

/**
 * 参考GifFrameDecoder
 */

public class DGifFrameDecoder implements ResourceDecoder<InputStream, DGifDrawable> {
    private BitmapPool bitmapPool;

    public DGifFrameDecoder(BitmapPool bitmapPool) {
        this.bitmapPool = bitmapPool;
    }

    @Override
    public boolean handles(@NonNull InputStream source, @NonNull Options options) throws IOException {
        return true;
    }

    @Nullable
    @Override
    public DGifDrawableResource decode(@NonNull InputStream source, int width, int height, @NonNull Options options) throws IOException {
        DGifFrame fs = DGifFrame.decodeStream(source);
        DGifDrawable drawable = new DGifDrawable(fs, mProvider);
        return new DGifDrawableResource(drawable);
    }

    // This provider is entirely unnecessary, just here to validate the acquire/release process
    private class CheckingProvider implements DGifDrawable.BitmapProvider {

        @Override
        public Bitmap acquireBitmap(int minWidth, int minHeight) {
            return bitmapPool.getDirty(minWidth, minHeight, Bitmap.Config.ARGB_8888);
        }

        @Override
        public void releaseBitmap(Bitmap bitmap) {
            bitmapPool.put(bitmap);
        }
    }

    private CheckingProvider mProvider = new CheckingProvider();

}
