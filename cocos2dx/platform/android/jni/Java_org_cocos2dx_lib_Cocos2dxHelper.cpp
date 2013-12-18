#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <string>
#include <typeinfo>
#include "JniHelper.h"
#include "cocoa/CCDictionary.h"
#include "cocoa/CCBool.h"
#include "cocoa/CCInteger.h"
#include "cocoa/CCFloat.h"
#include "cocoa/CCObject.h"
#include "cocoa/CCString.h"
#include "Java_org_cocos2dx_lib_Cocos2dxHelper.h"


#define  LOG_TAG    "Java_org_cocos2dx_lib_Cocos2dxHelper.cpp"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#define  CLASS_NAME "org/cocos2dx/lib/Cocos2dxHelper"

#define  ACT_PKG_CLS 3

static EditTextCallback s_pfEditTextCallback = NULL;
static void* s_ctx = NULL;

using namespace cocos2d;
using namespace std;

struct fields_intent {
    jclass      clazz;
    jmethodID   constructor;

    jmethodID   set_action;
    jmethodID   set_class;

    jmethodID   put_string;
    jmethodID   put_bool;
    jmethodID   put_int;
    jmethodID   put_float;
};

string g_apkPath;

extern "C" {

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxHelper_nativeSetApkPath(JNIEnv*  env, jobject thiz, jstring apkPath) {
        g_apkPath = JniHelper::jstring2string(apkPath);
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxHelper_nativeSetEditTextDialogResult(JNIEnv * env, jobject obj, jbyteArray text) {
        jsize  size = env->GetArrayLength(text);

        if (size > 0) {
            jbyte * data = (jbyte*)env->GetByteArrayElements(text, 0);
            char* pBuf = (char*)malloc(size+1);
            if (pBuf != NULL) {
                memcpy(pBuf, data, size);
                pBuf[size] = '\0';
                // pass data to edittext's delegate
                if (s_pfEditTextCallback) s_pfEditTextCallback(pBuf, s_ctx);
                free(pBuf);
            }
            env->ReleaseByteArrayElements(text, data, 0);
        } else {
            if (s_pfEditTextCallback) s_pfEditTextCallback("", s_ctx);
        }
    }

}

const char * getApkPath() {
    return g_apkPath.c_str();
}

void showDialogJNI(const char * pszMsg, const char * pszTitle) {
    if (!pszMsg) {
        return;
    }

    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showDialog", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1;

        if (!pszTitle) {
            stringArg1 = t.env->NewStringUTF("");
        } else {
            stringArg1 = t.env->NewStringUTF(pszTitle);
        }

        jstring stringArg2 = t.env->NewStringUTF(pszMsg);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}

void showEditTextDialogJNI(const char* pszTitle, const char* pszMessage, int nInputMode, int nInputFlag, int nReturnType, int nMaxLength, EditTextCallback pfEditTextCallback, void* ctx) {
    if (pszMessage == NULL) {
        return;
    }

    s_pfEditTextCallback = pfEditTextCallback;
    s_ctx = ctx;

    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showEditTextDialog", "(Ljava/lang/String;Ljava/lang/String;IIII)V")) {
        jstring stringArg1;

        if (!pszTitle) {
            stringArg1 = t.env->NewStringUTF("");
        } else {
            stringArg1 = t.env->NewStringUTF(pszTitle);
        }

        jstring stringArg2 = t.env->NewStringUTF(pszMessage);

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2, nInputMode, nInputFlag, nReturnType, nMaxLength);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}

void initIntent(JNIEnv* env, fields_intent* const fields) {
    jclass clazz;
    clazz = env->FindClass("android/content/Intent");
    if (clazz == NULL) {
        CCLOGERROR("can't find Intent class");
        return;
    }

    fields->clazz = clazz;

    fields->constructor = env->GetMethodID(clazz, "<init>", "()V");

    fields->set_action = env->GetMethodID(clazz, "setAction",
            "(Ljava/lang/String;)Landroid/content/Intent;");
    fields->set_class = env->GetMethodID(clazz, "setClassName",
            "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;");

    fields->put_string = env->GetMethodID(clazz, "putExtra",
            "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;");
    fields->put_bool = env->GetMethodID(clazz, "putExtra",
            "(Ljava/lang/String;Z)Landroid/content/Intent;");
    fields->put_int = env->GetMethodID(clazz, "putExtra",
            "(Ljava/lang/String;I)Landroid/content/Intent;");
    fields->put_float = env->GetMethodID(clazz, "putExtra",
            "(Ljava/lang/String;F)Landroid/content/Intent;");
}

void parcelIntentAction(JNIEnv* env, jobject obj, jmethodID methodId, jstring action) {
    env->CallObjectMethod(obj, methodId, action);
}

void parcelIntentComponent(JNIEnv* env, jobject obj, jmethodID methodId, jstring pkgName, jstring clsName) {
    env->CallObjectMethod(obj, methodId, pkgName, clsName);
}

void parcelIntentExtras(JNIEnv* env, jobject obj, jmethodID methodId, jstring key, jstring val) {
    env->CallObjectMethod(obj, methodId, key, val);
}

void parcelIntentExtras(JNIEnv* env, jobject obj, jmethodID methodId, jstring key, jboolean val) {
    env->CallObjectMethod(obj, methodId, key, val);
}

void parcelIntentExtras(JNIEnv* env, jobject obj, jmethodID methodId, jstring key, jint val) {
    env->CallObjectMethod(obj, methodId, key, val);
}

void parcelIntentExtras(JNIEnv* env, jobject obj, jmethodID methodId, jstring key, jfloat val) {
    env->CallObjectMethod(obj, methodId, key, val);
}

bool startActivityJNI(const char* action, const char* pkgName, const char* clsName, CCDictionary* pParams) {
    if (action == NULL && (pkgName == NULL || clsName == NULL)) {
        CCLOGWARN("can't init Intent by action or package & class name");
        return false;
    }

    JniMethodInfo t;
    jboolean ret(false);

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "startActivity",
            "(Landroid/content/Intent;)Z")) {
        fields_intent intent_fields;
        initIntent(t.env, &intent_fields);

        jobject objArg = objArg = t.env->NewObject(intent_fields.clazz, intent_fields.constructor);
        if (objArg == NULL) {
            CCLOGWARN("can't create a Intent object");
            return false;
        }

        // create a jstring array to save all jstring local refs.
        // it contains action, package name, class name and all params(key & value)
        jstring* strArgs = new jstring[ACT_PKG_CLS + (pParams ? pParams->count() : 0) * 2];
        int argsCount = 0;
        if (action) {
            strArgs[argsCount++] = t.env->NewStringUTF(action);

            parcelIntentAction(t.env, objArg, intent_fields.set_action, strArgs[argsCount - 1]);
        }

        if (pkgName && clsName) {
            strArgs[argsCount++] = t.env->NewStringUTF(pkgName);
            strArgs[argsCount++] = t.env->NewStringUTF(clsName);

            parcelIntentComponent(t.env, objArg, intent_fields.set_class, strArgs[argsCount - 2],
                    strArgs[argsCount - 1]);
        }

        if (pParams) {
            CCDictElement* tmpElement = NULL;
            CCObject* tmpObject = NULL;
            CCDICT_FOREACH(pParams, tmpElement) {
                strArgs[argsCount++] = t.env->NewStringUTF(tmpElement->getStrKey());

                tmpObject = tmpElement->getObject();
                if (dynamic_cast<CCString*>(tmpObject)) {
                    strArgs[argsCount++] = t.env->NewStringUTF(
                            ((CCString*) tmpObject)->getCString());

                    parcelIntentExtras(t.env, objArg, intent_fields.put_string,
                            strArgs[argsCount - 2],
                            strArgs[argsCount - 1]);
                } else if (dynamic_cast<CCBool*>(tmpObject)) {
                    parcelIntentExtras(t.env, objArg, intent_fields.put_bool,
                            strArgs[argsCount - 1],
                            ((CCBool*) tmpObject)->getValue());
                } else if (dynamic_cast<CCInteger*>(tmpObject)) {
                    parcelIntentExtras(t.env, objArg, intent_fields.put_int, strArgs[argsCount - 1],
                            ((CCInteger*) tmpObject)->getValue());
                } else if (dynamic_cast<CCFloat*>(tmpObject)) {
                    parcelIntentExtras(t.env, objArg, intent_fields.put_float,
                            strArgs[argsCount - 1],
                            ((CCFloat*) tmpObject)->getValue());
                } else {
                    CCLOGWARN("can't support CCObject type: %s", typeid(tmpObject).name());
                }
            }
        }

        ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, objArg);

        t.env->DeleteLocalRef(t.classID);
        for (int i = 0; i < argsCount; i++) {
            t.env->DeleteLocalRef(strArgs[i]);
        }
        delete[] strArgs;
        t.env->DeleteLocalRef(objArg);
    }

    return ret;
}

void terminateProcessJNI() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "terminateProcess", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

std::string getPackageNameJNI() {
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getCocos2dxPackageName", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    return ret;
}

std::string getFileDirectoryJNI() {
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getCocos2dxWritablePath", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    
    return ret;
}

std::string getCurrentLanguageJNI() {
    JniMethodInfo t;
    std::string ret("");
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getCurrentLanguage", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }

    return ret;
}

void enableAccelerometerJNI() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "enableAccelerometer", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

void setAccelerometerIntervalJNI(float interval) {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setAccelerometerInterval", "(F)V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, interval);
        t.env->DeleteLocalRef(t.classID);
    }
}

void disableAccelerometerJNI() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "disableAccelerometer", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

// functions for CCUserDefault
bool getBoolForKeyJNI(const char* pKey, bool defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getBoolForKey", "(Ljava/lang/String;Z)Z")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

int getIntegerForKeyJNI(const char* pKey, int defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getIntegerForKey", "(Ljava/lang/String;I)I")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jint ret = t.env->CallStaticIntMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

float getFloatForKeyJNI(const char* pKey, float defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getFloatForKey", "(Ljava/lang/String;F)F")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jfloat ret = t.env->CallStaticFloatMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

double getDoubleForKeyJNI(const char* pKey, double defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getDoubleForKey", "(Ljava/lang/String;D)D")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jdouble ret = t.env->CallStaticDoubleMethod(t.classID, t.methodID, stringArg);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

std::string getStringForKeyJNI(const char* pKey, const char* defaultValue)
{
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getStringForKey", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;")) {
        jstring stringArg1 = t.env->NewStringUTF(pKey);
        jstring stringArg2 = t.env->NewStringUTF(defaultValue);
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, stringArg1, stringArg2);
        ret = JniHelper::jstring2string(str);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(str);
        
        return ret;
    }
    
    return defaultValue;
}

void setBoolForKeyJNI(const char* pKey, bool value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setBoolForKey", "(Ljava/lang/String;Z)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setIntegerForKeyJNI(const char* pKey, int value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setIntegerForKey", "(Ljava/lang/String;I)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setFloatForKeyJNI(const char* pKey, float value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setFloatForKey", "(Ljava/lang/String;F)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setDoubleForKeyJNI(const char* pKey, double value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setDoubleForKey", "(Ljava/lang/String;D)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setStringForKeyJNI(const char* pKey, const char* value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setStringForKey", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1 = t.env->NewStringUTF(pKey);
        jstring stringArg2 = t.env->NewStringUTF(value);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
    }
}
