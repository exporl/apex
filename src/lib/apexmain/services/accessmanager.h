#ifndef _EXPORL_SRC_LIB_APEXMAIN_SERVICES_ACCESSMANAGER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SERVICES_ACCESSMANAGER_H_

#include "apextools/global.h"

#include <QNetworkAccessManager>

namespace apex {

class APEX_EXPORT AccessManager: public QNetworkAccessManager
{
public:
    AccessManager(QObject * parent = 0 );
    QUrl prepare(QUrl url) const;
    QUrl transformApexUrl(QUrl url) const;

protected:
    virtual QNetworkReply * createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );
};

}

#endif // _EXPORL_SRC_LIB_APEXMAIN_SERVICES_ACCESSMANAGER_H_
