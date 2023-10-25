package com.volleycn.giflib.gif;


import androidx.annotation.NonNull;

import com.bumptech.glide.load.resource.drawable.DrawableResource;
/**
 * 参考GifDrawableResource
 */
public class DGifDrawableResource extends DrawableResource<DGifDrawable> {
    public DGifDrawableResource(DGifDrawable drawable) {
        super(drawable);
    }

    @NonNull
    @Override
    public Class<DGifDrawable> getResourceClass() {
        return DGifDrawable.class;
    }

    @Override
    public int getSize() {
        return 0;
    }

    @Override
    public void recycle() {
        drawable.stop();
        drawable.destroy();
    }
}
