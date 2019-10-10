#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_NUMERICKEYPADWIDGET_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_NUMERICKEYPADWIDGET_H_

#include "apextools/global.h"

#include "ui_numerickeypadwidget.h"

#include <QWidget>

class APEX_EXPORT NumericKeypadWidget : public QWidget
{
    Q_OBJECT

public:
    NumericKeypadWidget(QWidget *parent);
    QString getTriplet() const;
    void clear();
    void setFont(const QFont &font);

signals:
    void tripletSubmitted();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void addNumber(unsigned number);
    void removeLast();
    void submitTriplet();

private:
    Ui::NumericKeypadWidget ui;
    std::vector<unsigned> triplet;
    void updateWidgets();
};

#endif
