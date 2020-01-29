#include "apexandroidnative.h"

#include "apexcontrol.h"
#include "gui/mainwindow.h"
#include "runner/pluginrunnerinterface.h"
#include "study/studymanager.h"

#include "apextools/apextools.h"

#include "common/pluginloader.h"

#include "commongui/listpicker.h"

#include <QAndroidJniObject>
#include <QFileDialog>
#include <QMetaObject>
#include <QStandardPaths>
#include <QtAndroid>

Q_LOGGING_CATEGORY(APEX_ANDROID, "apex.android");

using namespace cmn;

namespace apex
{
namespace android
{

Q_GLOBAL_STATIC(ApexAndroidBridge, apexAndroidBridge)

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

static void loadRunner(JNIEnv *env, jobject object, jstring jRunner)
{
    Q_UNUSED(env);
    Q_UNUSED(object);

    QString runner =
        QAndroidJniObject("java/lang/String", "(Ljava/lang/String;)V", jRunner)
            .toString();
    QMetaObject::invokeMethod(&apex::ApexControl::Get(), "startPluginRunner",
                              Qt::QueuedConnection, Q_ARG(QString, runner));
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

static void networkAvailable(JNIEnv *env, jobject object)
{
    Q_UNUSED(env);
    Q_UNUSED(object);

    qCDebug(APEX_RS, "jni network available");

    QMetaObject::invokeMethod(apexAndroidBridge, "networkAvailable",
                              Qt::QueuedConnection);
}

static void linkStudy(JNIEnv *env, jobject object, jstring spec)
{
    Q_UNUSED(env);
    Q_UNUSED(object);

    QString specString =
        QAndroidJniObject("java/lang/String", "(Ljava/lang/String;)V", spec)
            .toString();

    QJsonParseError jsonParseError;
    QJsonDocument jsonDocument =
        QJsonDocument::fromJson(specString.toUtf8(), &jsonParseError);
    if (jsonParseError.error) {
        qCWarning(APEX_RS) << "Failed to parse as JSON object:" << specString
                           << "Error:" << jsonParseError.errorString();
        return;
    }

    QJsonObject json = jsonDocument.object();

    QMetaObject::invokeMethod(
        apexAndroidBridge, "linkStudy", Qt::QueuedConnection,
        Q_ARG(QString, json["name"].toString()),
        Q_ARG(QString, json["experimentsUrl"].toString()),
        Q_ARG(QString, json["experimentsBranch"].toString()),
        Q_ARG(QString, json["resultsUrl"].toString()),
        Q_ARG(QString, json["resultsBranch"].toString()));
}

static JNINativeMethod methods[]{
    {"fileOpen", "(Ljava/lang/String;)V", (void *)fileOpen},
    {"loadRunner", "(Ljava/lang/String;)V", (void *)loadRunner},
    {"logDebugItem", "(Ljava/lang/String;)V", (void *)logDebugItem},
    {"networkAvailable", "()V", (void *)networkAvailable},
    {"linkStudy", "(Ljava/lang/String;)V", (void *)linkStudy},
};

ApexAndroidBridge *ApexAndroidBridge::instance()
{
    return apexAndroidBridge;
}

// Java methods
void ApexAndroidBridge::signalApexInitialized()
{
    QtAndroid::androidActivity().callMethod<void>("signalApexInitialized",
                                                  "()V");
}

void ApexAndroidBridge::signalExperimentStarted()
{
    QtAndroid::androidActivity().callMethod<void>("signalExperimentStarted",
                                                  "()V");
}

void ApexAndroidBridge::signalExperimentFinished()
{
    QtAndroid::androidActivity().callMethod<void>("signalExperimentFinished",
                                                  "()V");
}

void ApexAndroidBridge::signalTrialStarted()
{
    QtAndroid::androidActivity().callMethod<void>("signalTrialStarted", "()V");
}

void ApexAndroidBridge::forceAudioVolumeToMaximum()
{
    QtAndroid::androidActivity().callMethod<void>("maximizeVolume", "()V");
}

QString ApexAndroidBridge::getSSLCertificateDirectory()
{
    return QtAndroid::androidActivity()
        .callObjectMethod<jstring>("getSSLCertificateDirectory")
        .toString();
}

QString ApexAndroidBridge::getDeviceSerialNumber()
{
    // TODO this field is going to deprecate in API O
    return QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build",
                                                            "SERIAL")
        .toString();
}

void ApexAndroidBridge::startGdbServer()
{
    return QtAndroid::androidActivity().callMethod<void>("startGdbServer",
                                                         "()V");
}

void ApexAndroidBridge::shareText(const QString &text)
{
    QAndroidJniObject jniText = QAndroidJniObject::fromString(text);
    QtAndroid::androidActivity().callMethod<void>(
        "shareText", "(Ljava/lang/String;)V", jniText.object<jstring>());
}

void ApexAndroidBridge::sendToast(const QString &text)
{
    QAndroidJniObject jniText = QAndroidJniObject::fromString(text);
    QtAndroid::androidActivity().callMethod<void>(
        "sendToast", "(Ljava/lang/String;)V", jniText.object<jstring>());
}

ApexAndroidBridge::ApexAndroidBridge()
{
    connect(this, SIGNAL(networkAvailable()), this, SLOT(onNetworkAvailable()));
    connect(
        this, SIGNAL(linkStudy(QString, QString, QString, QString, QString)),
        this, SLOT(onLinkStudy(QString, QString, QString, QString, QString)));
}

void ApexAndroidBridge::createShortcutToFile()
{
    QFileDialog dlg(ApexControl::Get().mainWindow(),
                    tr("Create shortcut to experiment"));
    QStringList docLocations =
        QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    if (!docLocations.isEmpty())
        dlg.setDirectory(docLocations.first());
    dlg.setLabelText(QFileDialog::Accept, tr("Create"));
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setViewMode(QFileDialog::List);
    dlg.setNameFilter(tr("Apex experiment files (*.apx *.xml)"));
    dlg.setDefaultSuffix(QSL("apx"));
    ApexTools::expandWidgetToWindow(&dlg);
    if (dlg.exec() == QDialog::Accepted) {
        Q_FOREACH (const QString &file, dlg.selectedFiles()) {
            QAndroidJniObject jniPath = QAndroidJniObject::fromString(file);
            QtAndroid::androidActivity().callMethod<void>(
                "createShortcutToFile", "(Ljava/lang/String;)V",
                jniPath.object<jstring>());
        }
    }
}

void ApexAndroidBridge::createShortcutToRunner()
{
    ListPicker listPicker(QSL("Select pluginrunner: "));
    Q_FOREACH (PluginRunnerCreator *pluginCreator,
               PluginLoader().availablePlugins<PluginRunnerCreator>())
        Q_FOREACH (const QString &name, pluginCreator->availablePlugins())
            listPicker.addItem(name);
    ApexTools::expandWidgetToWindow(&listPicker);
    listPicker.exec();
    if (!listPicker.selectedItem().isEmpty()) {
        QAndroidJniObject jniRunner =
            QAndroidJniObject::fromString(listPicker.selectedItem());
        QtAndroid::androidActivity().callMethod<void>(
            "createShortcutToRunner", "(Ljava/lang/String;)V",
            jniRunner.object<jstring>());
    }
}

void ApexAndroidBridge::onNetworkAvailable()
{
    if (!ApexControl::Get().isExperimentRunning())
        StudyManager::instance()->syncActiveStudy();
}

void ApexAndroidBridge::onLinkStudy(const QString &name,
                                    const QString &experimentsUrl,
                                    const QString &experimentsBranch,
                                    const QString &resultsUrl,
                                    const QString &resultsBranch)
{
    StudyManager::instance()->linkStudy(name, experimentsUrl, experimentsBranch,
                                        resultsUrl, resultsBranch);
}

void ApexAndroidBridge::enableKioskMode()
{
    QtAndroid::androidActivity().callMethod<void>("enableKioskMode", "()V");
}

void ApexAndroidBridge::disableKioskMode()
{
    QtAndroid::androidActivity().callMethod<void>("disableKioskMode", "()V");
}

void ApexAndroidBridge::openBrowser(const QString &uri)
{
    QAndroidJniObject jniUri = QAndroidJniObject::fromString(uri);
    QtAndroid::androidActivity().callMethod<void>(
        "openBrowser", "(Ljava/lang/String;)V", jniUri.object<jstring>());
}
}
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{

    static bool initialized = false;
    if (initialized)
        return JNI_VERSION_1_6;
    initialized = true;

    qCDebug(APEX_RS) << "jni onload";
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
