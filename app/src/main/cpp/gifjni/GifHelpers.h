#ifndef RASTERMILL_GIFHELPERS_H
#define RASTERMILL_GIFHELPERS_H

#include <jni.h>

#define METHOD_COUNT(methodArray) (sizeof(methodArray) / sizeof((methodArray)[0]))

#define ILLEGAL_STATE_EXEPTION "java/lang/IllegalStateException"

void jniThrowException(JNIEnv* env, const char* className, const char* msg);

#endif //RASTERMILL_GIFHELPERS_H
