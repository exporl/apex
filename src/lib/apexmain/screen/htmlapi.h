#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLAPI_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLAPI_H_

#include <QObject>

#include "../baseapi.h"

namespace apex
{

class HtmlAPI : public BaseApi
{
    Q_OBJECT
public:
    explicit HtmlAPI(QObject *parent = 0);

signals:
    void javascriptFinished(const QVariant &result);

public slots:
};

} // ns apex

#endif // _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLAPI_H_
