#include <QDebug>
#include <QObject>

class FlashWidget;

class Test : public QObject
{
    Q_OBJECT
public slots:
    void stateChange(int state);
    void test();

    void movieFinished();
    void movieClicked();

private:
    FlashWidget *fw;
};
