#ifndef _EXPORL_SRC_LIB_APEXTOOLS_PATHTOOLS_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_PATHTOOLS_H_

#include "global.h"

#include <QString>
#include <QCoreApplication>

namespace apex
{

/**
    * Collection of static methods dealing with paths
    * Methods that require the apex runtime are located in service Paths
    * in library apexmain
    */
class APEXTOOLS_EXPORT PathTools
{
    Q_DECLARE_TR_FUNCTIONS(PathTools)
    public:
        /**
        * Return configuration file directory of the current user
        * Linux: ~/.config/LabExpORL/
        * Windows: ...\local settings\LabExpORL\
         */
        static const QString GetUserConfigFilePath();

        static const QString GetConfigFilePath(
                const QString& basepath,
                const QString& filename);

        static const QString GetSchemaPath(const QString& basepath,
                                           const QString& filename);

        static const QString GetSharePath(const QString& basepath);

        /**
         * Get the path to directory containing the executable.
         * @return text string reference
         */
        static const QString GetExecutablePath();

        static const QString cConfigDir;
        static const QString cSchemaDir;
        static const QString cApexDir;

};




} // ns apex



#endif
