#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class PtcPrivate;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

public slots:
    void start();
    void stop();
    void save();
    void updateProgress();
    void updateGain();
    void selectNoiseFile();
    void selectToneFile();
    void selectMaskerFile();
    void calibrate();
    void playTone();
    void resetExperiment();
    void changeToneGain(double newgain);
    void changeMaxGain(double newgain);
    void enableMasker(int);
    void setCondition(const QString &name);

private slots:
    void loadConditions();

private:
    void loadExperiment();

private:
    PtcPrivate *const d;
};

#endif // MAINWINDOW_H
