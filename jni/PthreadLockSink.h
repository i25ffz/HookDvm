#ifndef PTHREADLOCKSINK_H
#define PTHREADLOCKSINK_H 
#pragma once
#include <pthread.h>

class PthreadLockSink {
public:
    explicit PthreadLockSink(pthread_mutex_t * _mutex) :
        mutex(_mutex)
    {
        pthread_mutex_lock(mutex);
    }
    ~PthreadLockSink() {
        pthread_mutex_unlock(mutex);
    }
private:
    pthread_mutex_t * mutex;
};


#endif /* PTHREADLOCKSINK_H */
