#include <jni.h>
#include "local.h"
#include <unistd.h>
#include "tracepath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>



typedef struct {
    jobject gObject;

    JNIEnv * g_Env;

    jclass kClass;

    jmethodID  updateMethodId;

    jmethodID  endMethodId;

} Nofication;

Nofication nofication;

JNIEXPORT void JNICALL beginTrace(JNIEnv *env, jobject object, jstring path){
    nofication.gObject = object;
    nofication.g_Env = env;

    nofication.kClass = (*env)->FindClass(env, "com/netease/LDNetDiagnoService/LDNetTraceRoute");
    nofication.updateMethodId = (*env)->GetMethodID(env, nofication.kClass, "callback", "(Ljava/lang/String;)V");
    nofication.endMethodId = (*env)->GetMethodID(env, nofication.kClass, "end", "()V");

    jboolean tmp = 1;
    const char * domin = (*env)->GetStringUTFChars(env, path, &tmp);

    struct hostent* host = gethostbyname(domin);
    if(host){
        notifyUpdateToJava("To %s ",  host->h_name);

        for (;;){
            if (*host->h_aliases){
                notifyUpdateToJava("(%s )",  *host->h_aliases);
                host->h_aliases++;
            } else {
                break;
            }
        }

        for (;;){
           if (*host->h_addr_list){
               notifyUpdateToJava("(%s )",  inet_ntoa(*(struct in_addr*)(*host->h_addr_list)));
               host->h_addr_list++;
           } else {
               break;
           }
        }

//        notifyUpdateToJava(" %s ",  inet_ntoa(*(struct in_addr*)(host->h_addr_list[1])));
        notifyUpdateToJava("\n");
    } else {
        notifyUpdateToJava("Net error! %s\n", strerror(errno));
        notifyEnd();
    }

    LOGD(domin);
    char * ret[] = {"tracepath", domin};
    traceroute(2, ret);
    LOGD(strerror(errno));
//
}

char line[1000];
int notifyUpdateToJava(const char* logs, ...){
    int ret = 0;
    char value[200];
    va_list arg;
    va_start(arg, logs);
    ret = vsprintf(value, logs, arg);
    va_end(arg);

    strcat(line, value);
    if(value[strlen(value) - 1] == '\n'){
        LOGD(line);
        if (line){
            jstring value = (*nofication.g_Env)->NewStringUTF(nofication.g_Env, line);
            (*nofication.g_Env)->CallVoidMethod(nofication.g_Env, nofication.gObject, nofication.updateMethodId, value);
        }
        memset(line, 0, sizeof(line));
    }

    return ret;
}

void notifyEnd(){
    (*nofication.g_Env)->CallVoidMethod(nofication.g_Env, nofication.gObject, nofication.endMethodId);
}

static const JNINativeMethod gMethods[] = {
        {"beginTrace", "(Ljava/lang/String;)V", (void*)beginTrace}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGD("jni load");
    JNIEnv *env;
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK){
        return -1;
    }

    jclass kClass = (*env)->FindClass(env, "com/netease/LDNetDiagnoService/LDNetTraceRoute");
    (*env)->RegisterNatives(env, kClass, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved){
    LOGD("jni unload");
}