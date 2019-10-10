#ifndef _APEX_SRC_LIB_APEXMAIN_ACCESSMANAGER_H_
#define _APEX_SRC_LIB_APEXMAIN_ACCESSMANAGER_H_

#include "apextools/global.h"

namespace apex
{

class APEX_EXPORT AccessManager
{
public:
    static QUrl prepare(QUrl url);
};
}

#endif // _APEX_SRC_LIB_APEXMAIN_ACCESSMANAGER_H_
