#include "desktoptools.h"

#include <QDesktopServices>
#include <QUrl>
#ifdef WIN32
#include <qt_windows.h> // ShellExecute
#include <shlobj.h>
#endif

namespace apex
{

bool DesktopTools::editFile(QString filename)
{
    bool result;
#ifdef WIN32
    quintptr returnValue = (quintptr)ShellExecuteW(
        0, 0, (wchar_t *)filename.utf16(), 0, 0, SW_SHOWNORMAL);
    result = (returnValue >
              32); // ShellExecute returns a value greater than 32 if successful
#else
    result = QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
#endif
    return result;
}

bool DesktopTools::openUrl(const QString &url)
{
    return QDesktopServices::openUrl(url);
}
}
