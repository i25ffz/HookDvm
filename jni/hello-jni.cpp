/*
 * Copyright (C) 2009 The Android Open Source Project
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
 *
 */
#include <string.h>
#include <jni.h>

#include <android/log.h>
#include "HookCore.h"
#include "Log.h"
#define LOG_TAG "hello-jni"
extern "C" 
jstring
Java_com_example_hellojni_HelloJni_stringFromJNI( JNIEnv* env,
                                                  jobject thiz );

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
JNIEXPORT jstring
Java_com_example_hellojni_HelloJni_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{
    return env->NewStringUTF("Hello from JNI !");
}


static int myFunc(JNIEnv * env,jobject clazz,jobject,jobject)
{
    return __android_log_print(ANDROID_LOG_DEBUG,"LIN","FUCK");
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*)
{
    JNIEnv * env;
    if(JNI_OK != vm->AttachCurrentThread(&env,NULL))
    {
        LOG_DEV("failed to attach current thread\n");
        return JNI_VERSION_1_6;
    }
    HookCore * hook = HookCore::instance();
    HookDesc desc = {
        {
            "android/util/Log",
            "d",
            "(Ljava/lang/String;Ljava/lang/String;)I",
            true
        },
        reinterpret_cast<void*>(myFunc)
    };
    bool hookSuccess = hook->hook(env,desc);
    if(!hookSuccess)
    {
        LOG_DEV("failed to hook!\n");
    }
    return JNI_VERSION_1_6;
}

