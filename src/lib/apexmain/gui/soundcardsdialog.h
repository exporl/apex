#ifndef SoundcardsDialog_H
#define SoundcardsDialog_H

#include <QVector>
#include <QDialog>
#include <QListWidgetItem>
#include "apextools/global.h"

namespace Ui {
    class SoundcardsDialog;
}


namespace apex {

struct Device {
    QString hostApiName;
    QString deviceName;
    int deviceIndex;
    int maxOutputChannels;
    //double defaultSampleRate;
};

class APEX_EXPORT SoundcardsDialog : public QDialog
{
    Q_OBJECT
public:
    SoundcardsDialog(QWidget *parent = 0);
    virtual ~SoundcardsDialog();

    void registerDevice(const QString& hostApiName, int deviceIndex, const QString& deviceName, int maxOutputChannels);

    /**
     * @brief device
     * @return portaudio device ID of selected device. Returns -1 if no device selected.
     */
    int device() const;
    QString cardName() const;
    QString hostApiName() const;

    void displayHostApis();
    void setSelection(const QString& hostApiName, const QString& deviceName);


public slots:
    void displayDevices(QListWidgetItem*);
    void selectDevice(QListWidgetItem*);

private:
    void displayDevices(const QString &hostApiName );
    void displayDeviceInfo( Device &device );

    Ui::SoundcardsDialog *ui;
    QList<Device> devices;
    int selectedDevice;

};


} // ns apex

#endif // SoundcardsDialog_H
