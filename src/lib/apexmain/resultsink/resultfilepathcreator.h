#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_RESULTFILEPATHCREATOR_H
#define _EXPORL_SRC_LIB_APEXMAIN_RESULTSINK_RESULTFILEPATHCREATOR_H

#include "apexmain/study/studymanager.h"

#include <QDir>
#include <QString>

namespace apex
{

class APEX_EXPORT ResultfilePathCreator
{

public:
    const QString createDefaultAbsolutePath(const QString &experimentfilePath,
                                            const QString &subject) const;
    const QString
    createAbsolutePathForFlow(const QDir &flowBaseDir,
                              const QString &experimentfilePath,
                              const QString &resultfilePath) const;

private:
    const QString addTimestamp(const QString &path) const;
};
}

#endif
