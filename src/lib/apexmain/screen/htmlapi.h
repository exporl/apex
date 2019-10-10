#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLAPI_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLAPI_H_

#include <QObject>

namespace apex
{

class HtmlAPI : public QObject
{
    Q_OBJECT
public:
    explicit HtmlAPI();
    virtual ~HtmlAPI();

    Q_INVOKABLE void newAnswer(const QString &text);

signals:
    void parameterChanged(const QString &name, const QVariant &value);
    void answered(const QString &text);
};
}

#endif
