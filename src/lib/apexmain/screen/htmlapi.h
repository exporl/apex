#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLAPI_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLAPI_H_

#include <QObject>

namespace apex {

class HtmlAPI : public QObject
{
    Q_OBJECT
public:
    explicit HtmlAPI(QObject *parent = 0);

signals:
    void answered();

public slots:


};

} // ns apex

#endif // _EXPORL_SRC_LIB_APEXMAIN_SCREEN_HTMLAPI_H_
