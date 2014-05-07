#ifndef LOG_H
#define LOG_H 
#pragma once
#ifndef NDEBUG
// enable log only when debug mode
#define LOG_DEBUG 1
#endif //NDEBUG
#if defined(LOG_DEBUG) && LOG_DEBUG == 1
#include <android/log.h>
#define LOG_DEV(...) \
    __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif // LOG_DEBUG
#ifndef LOG_DEV
#define LOG_DEV(...)
#endif // LOG_DEV
#endif /* LOG_H */
