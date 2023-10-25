package com.volleycn.giflib;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.bumptech.glide.Glide;
import com.bumptech.glide.Registry;
import com.bumptech.glide.load.resource.gif.GifDrawable;
import com.bumptech.glide.request.target.CustomTarget;
import com.bumptech.glide.request.transition.Transition;
import com.volleycn.giflib.databinding.ActivityMainBinding;
import com.volleycn.giflib.gif.DGifDrawable;
import com.volleycn.giflib.gif.DGifDrawableResource;
import com.volleycn.giflib.gif.DGifFrame;
import com.volleycn.giflib.gif.DGifFrameDecoder;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {
    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.post(() -> loadGif());
    }

    private void loadGif() {
        String src = "https://p5.itc.cn/images01/20200627/0f15f027e47e4bb7957666e65a18a491.gif";
        Glide.with(binding.drawableview)
                .asFile()
                .load(src)
                .into(new CustomTarget<File>() {
                    @Override
                    public void onResourceReady(@NonNull File resource, @Nullable Transition<? super File> transition) {
                        try {
                            Glide glide = Glide.get(binding.drawableview.getContext());
                            DGifFrameDecoder dGifFrameDecoder = new DGifFrameDecoder(glide.getBitmapPool());
                            FileInputStream fileInputStream=  new FileInputStream(resource);
                            DGifDrawableResource decode = dGifFrameDecoder.decode(fileInputStream,300,300,null);
                            DGifDrawable dGifDrawable = decode.get();
                            binding.drawableview.setImageDrawable(dGifDrawable);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }

                    @Override
                    public void onLoadCleared(@Nullable Drawable placeholder) {

                    }
                });
    }

}