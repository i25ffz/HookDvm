#include <tr1/unordered_map>
#include <string>
#include <pthread.h>
#include <jni.h>


#include "DVM.h"
#include "HookCore.h"
#include "PthreadLockSink.h"
#include "Log.h"
#define LOG_TAG "HOOK"

struct MethodDescEx
{
    std::string className;
    std::string methodName;
    std::string signature;
    bool isStatic;
};

struct HookDescEx 
{
    MethodDescEx method;
    void * replaceFunc;
    u4 originalAcc;
    const u2 * originalInsns;
};

static void TransferToHookDescEx(HookDescEx & descEx ,const HookDesc & desc)
{
    descEx.method.className.assign(desc.method.className);
    descEx.method.methodName.assign(desc.method.methodName);
    descEx.method.signature.assign(desc.method.signature);
    descEx.method.isStatic = desc.method.isStatic;
    descEx.replaceFunc = desc.replaceFunc;
}

typedef std::tr1::unordered_map<std::string,HookDescEx> HookMap;

struct HookCore::HookCoreImpl
{
    HookMap map;
    pthread_mutex_t mutex;
    HookCoreImpl() {
        pthread_mutex_init(&mutex,NULL);
    }
};

HookCore::HookCore() :
    impl(new HookCoreImpl)
{
}

HookCore::~HookCore()
{
}


bool HookCore::hook(JNIEnv * env,const struct HookDesc & desc)
{
    std::string composing(desc.method.className);
    composing.append(desc.method.methodName);
    std::auto_ptr<HookDescEx> descEx(new HookDescEx);
    TransferToHookDescEx(*descEx,desc);
    PthreadLockSink sink(&impl->mutex);


    if(impl->map.find(composing) != impl->map.end())
    {
        LOG_DEV("have already hook this method\n");
        return false;
    }

    // do the hooking
    // Find the local ref class
    jclass classLocal = env->FindClass(desc.method.className);
    jmethodID methodID;
    if(!classLocal)
    {
        LOG_DEV("not having this class\n");
        return false;
    }
    // get the method id
    if(!desc.method.isStatic)
        methodID = env->GetMethodID(classLocal,desc.method.methodName,desc.method.signature);
    else
        methodID = env->GetStaticMethodID(classLocal,desc.method.methodName,desc.method.signature);
    if(!methodID)
    {
        LOG_DEV("not having this method\n");
        return false;
    }
    // dereference it to Object 
    // Thread * thread = dvmThreadSelf();
    // Object * object = dvmDecodeIndirectRef(thread,classLocal);
    env->DeleteLocalRef(classLocal);
    // if(!object)
    // {
    //     LOG_DEV("failed to derefernce local object\n");
    //     return false;
    // }
    // lock the classObject
    // dvmLockObject(thread,object);
    // find the method object
    Method * meth = reinterpret_cast<Method*>(methodID);
    // save orignal method data
    descEx->originalAcc = meth->accessFlags;
    descEx->originalInsns = meth->insns;
    
    // alter the data
    meth->accessFlags |= ACC_NATIVE;
    meth->insns = reinterpret_cast<u2*>(desc.replaceFunc);
    meth->nativeFunc = dvmCallJNIMethod;

    // unlock the classObject
    // dvmUnlockObject(thread,object);

    HookMap::value_type v(composing,*descEx);
    std::pair<HookMap::iterator,bool> ret = impl->map.insert(v);
    LOG_DEV("HOOK okay\n");
    return true;
}

void HookCore::guard(const char * className,const char * method)
{
    std::string composing(className);
    composing.append(method);
    HookMap::iterator found = impl->map.find(composing);
    if(found == impl->map.end())
        return;
    // currently just a validation here
    // still implementing a global lock
    pthread_mutex_lock(&impl->mutex);
}

void HookCore::unguard(const char * className,const char * method)
{
    std::string composing(className);
    composing.append(method);
    HookMap::iterator found = impl->map.find(composing);
    if(found == impl->map.end())
        return;
    // currently just a validation here
    // still implementing a global lock
    pthread_mutex_unlock(&impl->mutex);
}

HookCore * HookCore::instance()
{
    static std::auto_ptr<HookCore> g_instance(new HookCore);
    return g_instance.get();
}

