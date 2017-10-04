#ifndef PREFERENCERATING_H
#define PREFERENCERATING_H

#include <QtWidgets/QMainWindow>

class PRPrivate;

class PreferenceRating : public QMainWindow
{
    Q_OBJECT
public:
    PreferenceRating(QWidget *parent = 0);
    ~PreferenceRating();

public slots:
    void start();
    void stop();
    void file1();
    void file2();

private:
    PRPrivate *const d;
};

#endif // MAINWINDOW_H
