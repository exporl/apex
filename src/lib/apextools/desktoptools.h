#ifndef _APEX_SRC_LIB_APEXTOOLS_DESKTOPTOOLS_H_
#define _APEX_SRC_LIB_APEXTOOLS_DESKTOPTOOLS_H_

#include "global.h"
#include <QString>

namespace apex
{

class APEXTOOLS_EXPORT DesktopTools
{
public:
    static bool editFile(QString filename);
    static bool openUrl(const QString &url);
};
};

#endif
