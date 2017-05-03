#include "soundcardsdialog.h"
#include <portaudio.h>
#include "ui_soundcardsdialog.h"


namespace apex {

SoundcardsDialog::SoundcardsDialog(QWidget *parent) :
    QDialog(parent),
    selectedDevice(-1)
{
    ui = new Ui::SoundcardsDialog();
    ui->setupUi(this);
#ifdef Q_OS_LINUX
    QString suggestDriver("ALSA with the default sound card");
#endif
#ifdef Q_OS_WIN32
    QString suggestDriver("WASAPI with the default sound card");
#endif
#ifdef Q_OS_ANDROID
    showMaximized();
#endif
    ui->infoLabel->setText( ui->infoLabel->text() + QString("\nWhen in doubt select %1.").arg(suggestDriver) );
    connect(ui->hostApiList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(displayDevices(QListWidgetItem*)));
    connect(ui->soundCardList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectDevice(QListWidgetItem*)));
}
SoundcardsDialog::~SoundcardsDialog()
{
    delete ui;
}

void SoundcardsDialog::registerDevice(const QString &hostApiName, int deviceIndex, const QString &deviceName, int maxOutputChannels)
{
    Device d = {hostApiName, deviceName, deviceIndex, maxOutputChannels};
    devices.push_back( d );
}

QString SoundcardsDialog::cardName() const
{
    Q_FOREACH (Device cDevice, devices) {
        if (cDevice.deviceIndex == selectedDevice) {
            return cDevice.deviceName;
        }
    }
    return QString();
}

QString SoundcardsDialog::hostApiName() const
{
    Q_FOREACH (Device cDevice, devices) {
        if (cDevice.deviceIndex == selectedDevice) {
            return cDevice.hostApiName;
        }
    }
    return QString();
}

/*void SoundcardsDialog::registerHostApi( HostApi hostApi )
{
    hostApis.append(hostApi);
}*/

int SoundcardsDialog::device() const
{
    return selectedDevice;
}

void SoundcardsDialog::displayHostApis()
{
    ui->hostApiList->clear();
    // Get unique hostapi names
    QStringList hostApiNames;

    Q_FOREACH (Device cDevice, devices) {
        if (!hostApiNames.contains(cDevice.hostApiName)) {
            hostApiNames.push_back(cDevice.hostApiName);
            ui->hostApiList->addItem(cDevice.hostApiName);
        }
    }

    selectedDevice = -1;
}

void SoundcardsDialog::setSelection(const QString &hostApiName, const QString &deviceName)
{
    {
        QList<QListWidgetItem*> items(ui->hostApiList->findItems(hostApiName, Qt::MatchCaseSensitive));
        if (items.length() != 1)
            return;
        items.at(0)->setSelected(true);
    }

    displayDevices(hostApiName);

    {
        QList<QListWidgetItem*> items(ui->soundCardList->findItems(deviceName, Qt::MatchCaseSensitive));
        if (items.length() != 1)
            return;
        items.at(0)->setSelected(true);
    }
}

void SoundcardsDialog::displayDevices(QListWidgetItem * i)
{
    displayDevices(i->text());
}

void SoundcardsDialog::selectDevice(QListWidgetItem * i)
{
    selectedDevice = i->data(Qt::UserRole).toInt();
}

void SoundcardsDialog::displayDevices(const QString &hostApiName )
{
    ui->soundCardList->clear();

    Q_FOREACH (Device cDevice, devices) {
        if (cDevice.hostApiName == hostApiName) {
            QListWidgetItem* item = new QListWidgetItem(ui->soundCardList);
            item->setText(cDevice.deviceName);
            item->setData(Qt::UserRole, QVariant::fromValue(cDevice.deviceIndex));
            ui->soundCardList->addItem(item);
        }
    }

    selectedDevice = -1;
}

void SoundcardsDialog::displayDeviceInfo( Device & /*device*/ )
{

}


} // ns apex
