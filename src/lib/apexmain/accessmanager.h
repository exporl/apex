#ifndef _APEX_SRC_LIB_APEXMAIN_ACCESSMANAGER_H_
#define _APEX_SRC_LIB_APEXMAIN_ACCESSMANAGER_H_

#include "apextools/global.h"

#include <QString>
#include <QUrl>

namespace apex
{

class APEX_EXPORT AccessManager
{
public:
    static QString toLocalFile(const QUrl &url, const QString &apexBaseDir);
};
}

#endif // _APEX_SRC_LIB_APEXMAIN_ACCESSMANAGER_H_
