/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "filedialog.h"
#include "apexcontrol.h"        // get main window
#include "gui/mainwindow.h"

#include <QFileInfo>
#include <QStringList>

using namespace apex;

FileDialog::FileDialog()
#ifndef WIN32
  : QFileDialog( 0 )  //just init the dialog, it takes over application wide settings for style
#endif
{}

FileDialog::~FileDialog()
{}


QString FileDialog::mf_sGetExistingFile( const QString& ac_sPath, const QString& ac_sFilter ) {
    QStringList filters;
    filters << ac_sFilter;
    return mf_sGetExistingFile( ac_sPath, filters);
}

QString FileDialog::mf_sGetExistingFile( const QString& ac_sPath, const QStringList& ac_sFilter )
{
#ifndef WIN32
    QFileDialog::setDirectory( ac_sPath );
    QFileDialog::setFilters( ac_sFilter );
    QFileDialog::setFileMode( QFileDialog::ExistingFile );
    if (QFileDialog::exec() == QFileDialog::Accepted ) {
        QStringList files = QFileDialog::selectedFiles();
        QString selected;
        if (!files.isEmpty())
            selected = files[0];
        return selected;
    }
    return QString();
#else
    QString all;
    for (int i = 0; i < ac_sFilter.size(); ++i)
        all = all + ac_sFilter.at(i) + QString(";;");

    return QFileDialog::getOpenFileName (NULL, QString(), ac_sPath, all);
#endif
}

QString FileDialog::mf_sGetAnyFile( const QString& ac_sPath, const QString& ac_sFilter )
{
//#ifndef WIN32
#if 0
    QFileInfo fileinfo( ac_sPath );
    QString path = fileinfo.path();
    if ( !path.isEmpty() )
        QFileDialog::setDirectory( path );
    if ( !fileinfo.fileName().isEmpty() && !fileinfo.isDir() )
        QFileDialog::selectFile( ac_sPath );
    QFileDialog::setFilter( ac_sFilter );
    QFileDialog::setFileMode( QFileDialog::AnyFile );
    QFileDialog::setAcceptMode( QFileDialog::AcceptSave );
    if( QFileDialog::exec() == QFileDialog::Accepted ) {
        QStringList files = QFileDialog::selectedFiles();
        QString selected;
        if (!files.isEmpty())
            selected = files[0];
        return selected;
    }
    return QString();
#else
/*        qDebug("Current path = %s",
                qPrintable(QDir::current().path()));
        qDebug("Requested path = %s",
                qPrintable(ac_sPath));*/

    return QFileDialog::getSaveFileName (ApexControl::Get().GetMainWnd(), QString(), ac_sPath, ac_sFilter );
#endif
}
