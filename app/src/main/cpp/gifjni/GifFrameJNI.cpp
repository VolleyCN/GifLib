#include <android/bitmap.h>
#include "GifHelpers.h"
#include "utils/log.h"
#include "GifFrame.h"
#include "GifFrameJNI.h"
#define JNI_PACKAGE "com/volleycn/giflib/gif"
static struct {
    jclass clazz;
    jmethodID ctor;
} gFrameClassInfo;

static jobject createJavaFrame(JNIEnv* env, GifFrame* frameSequence) {
    if (!frameSequence) {
        return NULL;
    }
    return env->NewObject(gFrameClassInfo.clazz, gFrameClassInfo.ctor,
                          reinterpret_cast<jlong>(frameSequence),
                          frameSequence->getWidth(),
                          frameSequence->getHeight(),
                          frameSequence->isOpaque(),
                          frameSequence->getFrameCount(),
                          frameSequence->getDefaultLoopCount());
}

static jobject nativeDecodeByteArray(JNIEnv* env, jobject clazz,
        jbyteArray byteArray, jint offset, jint length) {
    jbyte* bytes = reinterpret_cast<jbyte*>(env->GetPrimitiveArrayCritical(byteArray, NULL));
    if (bytes == NULL) {
        jniThrowException(env, ILLEGAL_STATE_EXEPTION,
                "couldn't read array bytes");
        return NULL;
    }
    MemoryStream stream(bytes + offset, length, NULL);
    GifFrame* gifFrame = GifFrame::create(&stream);
    env->ReleasePrimitiveArrayCritical(byteArray, bytes, 0);
    return createJavaFrame(env, gifFrame);
}

static jobject nativeDecodeByteBuffer(JNIEnv* env, jobject clazz,
        jobject buf, jint offset, jint limit) {
    jobject globalBuf = env->NewGlobalRef(buf);
    JavaVM* vm;
    env->GetJavaVM(&vm);
    MemoryStream stream(
        (reinterpret_cast<uint8_t*>(
            env->GetDirectBufferAddress(globalBuf))) + offset,
        limit,
        globalBuf);
    GifFrame* gifFrame = GifFrame::create(&stream);
    jobject finalSequence = createJavaFrame(env, gifFrame);
    return finalSequence;
}

static jobject nativeDecodeStream(JNIEnv* env, jobject clazz,
        jobject istream, jbyteArray byteArray) {
    JavaInputStream stream(env, istream, byteArray);
    GifFrame* gifFrame = GifFrame::create(&stream);
    return createJavaFrame(env, gifFrame);
}

static void nativeDestroyFrame(JNIEnv* env, jobject clazz,
                               jlong frameSequenceLong) {
    GifFrame* gifFrame = reinterpret_cast<GifFrame*>(frameSequenceLong);
    jobject buf = gifFrame->getRawByteBuffer();
    if (buf != NULL) {
        env->DeleteGlobalRef(buf);
    }
    delete gifFrame;
}

static jlong nativeCreateState(JNIEnv* env, jobject clazz, jlong frameSequenceLong) {
    GifFrame* gifFrame = reinterpret_cast<GifFrame*>(frameSequenceLong);
    GifFrameState* state = gifFrame->createState();
    return reinterpret_cast<jlong>(state);
}

////////////////////////////////////////////////////////////////////////////////
// Frame sequence state
////////////////////////////////////////////////////////////////////////////////

static void nativeDestroyState(
        JNIEnv* env, jobject clazz, jlong frameSequenceStateLong) {
    GifFrameState* frameSequenceState =
            reinterpret_cast<GifFrameState*>(frameSequenceStateLong);
    delete frameSequenceState;
}

void throwIae(JNIEnv* env, const char* message, int errorCode) {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s, error %d", message, errorCode);
    jniThrowException(env, ILLEGAL_STATE_EXEPTION, buf);
}

static jlong JNICALL nativeGetFrame(
        JNIEnv* env, jobject clazz, jlong frameSequenceStateLong, jint frameNr,
        jobject bitmap, jint previousFrameNr) {
    GifFrameState* frameSequenceState =
            reinterpret_cast<GifFrameState*>(frameSequenceStateLong);
    int ret;
    AndroidBitmapInfo info;
    void* pixels;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        throwIae(env, "Couldn't get info from Bitmap", ret);
        return 0;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        throwIae(env, "Bitmap pixels couldn't be locked", ret);
        return 0;
    }

    int pixelStride = info.stride >> 2;
    jlong delayMs = frameSequenceState->drawFrame(frameNr,
            (Color8888*) pixels, pixelStride, previousFrameNr);

    AndroidBitmap_unlockPixels(env, bitmap);
    return delayMs;
}

static JNINativeMethod gMethods[] = {
    {   "nativeDecodeByteArray",
        "([BII)L" JNI_PACKAGE "/DGifFrame;",
        (void*) nativeDecodeByteArray
    },
    {   "nativeDecodeByteBuffer",
        "(Ljava/nio/ByteBuffer;II)L" JNI_PACKAGE "/DGifFrame;",
        (void*) nativeDecodeByteBuffer
    },
    {   "nativeDecodeStream",
        "(Ljava/io/InputStream;[B)L" JNI_PACKAGE "/DGifFrame;",
        (void*) nativeDecodeStream
    },
    {   "nativeDestroyFrame",
        "(J)V",
            (void *) nativeDestroyFrame
    },
    {   "nativeCreateState",
        "(J)J",
        (void*) nativeCreateState
    },
    {   "nativeGetFrame",
        "(JILandroid/graphics/Bitmap;I)J",
        (void*) nativeGetFrame
    },
    {   "nativeDestroyState",
        "(J)V",
        (void*) nativeDestroyState
    },
};

jint GifFrame_OnLoad(JNIEnv* env) {
    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.
    gFrameClassInfo.clazz = env->FindClass(JNI_PACKAGE "/DGifFrame");
    if (!gFrameClassInfo.clazz) {
        ALOGW("Failed to find " JNI_PACKAGE "/DGifFrame");
        return -1;
    }
    gFrameClassInfo.clazz = (jclass)env->NewGlobalRef(gFrameClassInfo.clazz);

    gFrameClassInfo.ctor = env->GetMethodID(gFrameClassInfo.clazz, "<init>", "(JIIZII)V");
    if (!gFrameClassInfo.ctor) {
        ALOGW("Failed to find constructor for DGifFrame - was it stripped?");
        return -1;
    }

    return env->RegisterNatives(gFrameClassInfo.clazz, gMethods, METHOD_COUNT(gMethods));
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_volleycn_giflib_gif_DGifFrame_nativeDecodeByteArray(JNIEnv *env, jclass clazz,
                                                             jbyteArray data, jint offset,
                                                             jint length) {
    return nativeDecodeByteArray(env,clazz,data,offset,length);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_volleycn_giflib_gif_DGifFrame_nativeDecodeByteBuffer(JNIEnv *env, jclass clazz,
                                                              jobject buffer, jint offset,
                                                              jint capacity) {
    return nativeDecodeByteBuffer(env,clazz,buffer,offset,capacity);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_volleycn_giflib_gif_DGifFrame_nativeDecodeStream(JNIEnv *env, jclass clazz, jobject is,
                                                          jbyteArray temp_storage) {
    return nativeDecodeStream(env,clazz,is,temp_storage);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_volleycn_giflib_gif_DGifFrame_nativeDestroyFrame(JNIEnv *env, jclass clazz,
                                                          jlong native_frame_sequence) {
    return nativeDestroyFrame(env, clazz, native_frame_sequence);
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_volleycn_giflib_gif_DGifFrame_nativeCreateState(JNIEnv *env, jclass clazz,
                                                         jlong native_frame_sequence) {
    return nativeCreateState(env,clazz,native_frame_sequence);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_volleycn_giflib_gif_DGifFrame_nativeDestroyState(JNIEnv *env, jclass clazz,
                                                          jlong native_state) {
    nativeDestroyState(env,clazz,native_state);
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_volleycn_giflib_gif_DGifFrame_nativeGetFrame(JNIEnv *env, jclass clazz, jlong native_state,
                                                      jint frame_nr, jobject output,
                                                      jint previous_frame_nr) {
    return nativeGetFrame(env,clazz,native_state,frame_nr,output,previous_frame_nr);
}