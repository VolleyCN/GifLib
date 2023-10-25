#ifndef RASTERMILL_GIFSTREAM_H
#define RASTERMILL_GIFSTREAM_H
#include <jni.h>
#include <stdio.h>
#include <sys/types.h>

class GifStream {
public:
    GifStream();
    virtual ~GifStream();

    size_t peek(void* buffer, size_t size);
    size_t read(void* buffer, size_t size);
    virtual uint8_t* getRawBufferAddr();
    virtual jobject getRawBuffer();
    virtual int getRawBufferSize();

protected:
    virtual size_t doRead(void* buffer, size_t size) = 0;

private:
    char* mPeekBuffer;
    size_t mPeekSize;
    size_t mPeekOffset;
};

class MemoryStream : public GifStream {
public:
    MemoryStream(void* buffer, size_t size, jobject buf) :
            mBuffer((uint8_t*)buffer),
            mRemaining(size),
            mRawBuffer(buf) {}
    virtual uint8_t* getRawBufferAddr();
    virtual jobject getRawBuffer();
    virtual int getRawBufferSize();

protected:
    virtual size_t doRead(void* buffer, size_t size);

private:
    uint8_t* mBuffer;
    size_t mRemaining;
    jobject mRawBuffer;
};

class FileStream : public GifStream {
public:
    FileStream(FILE* fd) : mFd(fd) {}

protected:
    virtual size_t doRead(void* buffer, size_t size);

private:
    FILE* mFd;
};

class JavaInputStream : public GifStream {
public:
    JavaInputStream(JNIEnv* env, jobject inputStream, jbyteArray byteArray) :
            mEnv(env),
            mInputStream(inputStream),
            mByteArray(byteArray),
            mByteArrayLength(env->GetArrayLength(byteArray)) {}

protected:
    virtual size_t doRead(void* buffer, size_t size);

private:
    JNIEnv* mEnv;
    const jobject mInputStream;
    const jbyteArray mByteArray;
    const size_t mByteArrayLength;
};

jint JavaStream_OnLoad(JNIEnv* env);

#endif //RASTERMILL_GIFSTREAM_H
