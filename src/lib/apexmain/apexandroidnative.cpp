#include "apexandroidnative.h"

#include "apexcontrol.h"

#include <QAndroidJniObject>
#include <QMetaObject>
#include <QtAndroid>

Q_LOGGING_CATEGORY(APEX_ANDROID, "apex.android");

namespace apex
{
namespace android
{

// Native Methods
static void fileOpen(JNIEnv *env, jobject object, jstring path)
{
    Q_UNUSED(env);
    Q_UNUSED(object);

    QString localPath =
        QAndroidJniObject("java/lang/String", "(Ljava/lang/String;)V", path)
            .toString();
    QMetaObject::invokeMethod(&apex::ApexControl::Get(), "fileOpen",
                              Qt::QueuedConnection, Q_ARG(QString, localPath));
}

static void logDebugItem(JNIEnv *env, jobject object, jstring item)
{
    Q_UNUSED(env);
    Q_UNUSED(object);

    qCDebug(APEX_ANDROID, "%s",
            qPrintable(QAndroidJniObject("java/lang/String",
                                         "(Ljava/lang/String;)V", item)
                           .toString()));
}

static JNINativeMethod methods[]{
    {"fileOpen", "(Ljava/lang/String;)V", (void *)fileOpen},
    {"logDebugItem", "(Ljava/lang/String;)V", (void *)logDebugItem},
};

// Java Methods
void ApexAndroidMethods::addShortcut(const QString &path)
{
    QAndroidJniObject jniPath = QAndroidJniObject::fromString(path);
    QtAndroid::androidActivity().callMethod<void>(
        "addShortcut", "(Ljava/lang/String;)V", jniPath.object<jstring>());
}

void ApexAndroidMethods::signalApexInitialized()
{
    QtAndroid::androidActivity().callMethod<void>("signalApexInitialized",
                                                  "()V");
}

void ApexAndroidMethods::signalExperimentStarted()
{
    QtAndroid::androidActivity().callMethod<void>("signalExperimentStarted",
                                                  "()V");
}

void ApexAndroidMethods::signalExperimentFinished()
{
    QtAndroid::androidActivity().callMethod<void>("signalExperimentFinished",
                                                  "()V");
}

QString ApexAndroidMethods::getDeviceSerialNumber()
{
    // TODO this field is going to deprecate in API O
    return QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build",
                                                            "SERIAL")
        .toString();
}
}
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    Q_UNUSED(reserved);

    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;

    jclass nativeMethodsClass =
        env->FindClass("be/kuleuven/med/exporl/apex/ApexNativeMethods");
    if (!nativeMethodsClass)
        return JNI_ERR;

    if (env->RegisterNatives(nativeMethodsClass, apex::android::methods,
                             sizeof(apex::android::methods) /
                                 sizeof(apex::android::methods[0])) < 0) {
        return JNI_ERR;
    }

    return env->GetVersion(); // should be JNI_VERSION_1_6
}
