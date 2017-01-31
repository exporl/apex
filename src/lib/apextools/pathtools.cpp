#include "pathtools.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QApplication>

#include "exceptions.h"

namespace apex {

    const QString PathTools::cConfigDir("config");
    const QString PathTools::cSchemaDir("schemas");
    const QString PathTools::cApexDir("apex");


    const QString apex::PathTools::GetUserConfigFilePath() 
    {
#ifdef WIN32
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                           QSettings::NativeFormat);
        QString result ( settings.value("AppData").toString() +  "\\ExpORL" );
#else
        QString result( QDir::homePath() + "/.config/ExpORL" );
#endif

        if (! QFile::exists(result)) {
            QDir d("/");
            d.mkdir(result);
        }
        return result;
    }



    const QString apex::PathTools::GetConfigFilePath(const QString& basepath,
                                            const QString& filename) 
    {
    // try to find the file for this user
        QString result( GetUserConfigFilePath() + "/" + filename);
        qDebug("Looking for %s in %s", qPrintable(filename),
               qPrintable(result));
        if (QFile::exists(result)) {
            qDebug("Found");
            return result;
        }
    
#ifdef WIN32
        result = basepath + "/" + cConfigDir + "/" + filename;
        if (QFile::exists(result))
            return result;
#else
        result = "/etc/" + filename;
        if (QFile::exists(result))
            return result;

    // inside installed tree?
        QDir t( basepath );
        do {
            qDebug("Trying %s", qPrintable(t.path()));
            if (t.cd("etc")) {
                result = t.path() + "/" + filename;
                if (QFile::exists(result))
                    return result;
            }
            t.cdUp();
        } while  (!t.isRoot());

#endif
      // inside build dir?
        result = basepath + "/data/config/" + filename;
        if ( QFile::exists(result))
            return result;

        qDebug("BasePath: %s", qPrintable(basepath));
    
        throw ApexStringException(QString(tr("%1 not found: %2"))
                .arg(filename)
                . arg(result));

    }


    const QString PathTools::GetSchemaPath(const QString& basepath,
                                          const QString& filename) {
        QString result( GetSharePath(basepath) + "/" +  cSchemaDir + "/" +
                filename );
        if (! QFile::exists(result))
            throw ApexStringException(QString(tr("ApexConfig schema not found: %1")).arg(result));

return result;
    }

    const QString apex::PathTools::GetSharePath(const QString& basepath) 
    {
#ifdef WIN32
        QString result( basepath );
        if (QFile::exists(result+"/schemas"))
            return result;
#else
        QString result( basepath + "/share/" + cApexDir );
        if ( QFile::exists(result)) {
            return result;
        }
#endif

    
    // within build hierarchy
        QDir t ( GetExecutablePath() );
        t.cdUp();
        t.cdUp();
        if (t.cd("data")) {
//        qDebug("Returning build path: %s", qPrintable(t.path()));
            return t.path();
        }

        qDebug("Apex share directory not found: %s", qPrintable(result));
        throw ApexStringException(QString(tr("Apex share directory not found: %1.\nDid you install Apex properly?\nCheck whether the schemas subdirectory is present.")).arg(result));

    }

    const QString apex::PathTools::GetExecutablePath()
    {
        return QDir(qApp->applicationDirPath()).path();
    }
    

} // ns apex
