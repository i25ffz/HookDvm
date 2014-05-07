#ifndef HOOKCORE_H
#define HOOKCORE_H 
#pragma once
#include <memory>

struct MethodDesc
{
    const char * className;
    const char * methodName;
    const char * signature;
    bool isStatic;
};

struct HookDesc
{
    MethodDesc method;
    void * replaceFunc;
};

class HookCore
{
public:
    HookCore();
    ~HookCore();

    bool hook(JNIEnv * env,const struct HookDesc & desc);
    void guard(const char * className,const char * method);
    void unguard(const char * className,const char * method);

    // call original method not implment yet
    static HookCore * instance();

private:
    struct HookCoreImpl;
    std::auto_ptr<HookCoreImpl> impl;
};


#endif /* HOOKCORE_H */
