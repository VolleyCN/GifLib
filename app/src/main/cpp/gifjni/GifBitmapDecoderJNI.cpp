#define LOG_TAG "FancyDecoding"

#include <android/bitmap.h>
#include <stdlib.h>
#include <stdio.h>
#include "GifFrameJNI.h"
#include "GifHelpers.h"
#include "GifStream.h"
#include "utils/log.h"

void throwException(JNIEnv* env, const char* error) {
    jclass clazz = env->FindClass("java/lang/RuntimeException");
    env->ThrowNew(clazz, error);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    if (GifFrame_OnLoad(env)) {
        ALOGE("Failed to load FrameSequence");
        return -1;
    }
    if (JavaStream_OnLoad(env)) {
        ALOGE("Failed to load JavaStream");
        return -1;
    }

    return JNI_VERSION_1_6;
}