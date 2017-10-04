/******************************************************************************
 * Copyright (C) 2010 Michael Hofmann <mh21@piware.de>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#ifndef _CMN_SRC_LIB_CMN_SNDFILEWRAPPER_H_
#define _CMN_SRC_LIB_CMN_SNDFILEWRAPPER_H_

#include "global.h"

#include <sndfile.h>

#include <QCoreApplication>
#include <QFile>
#include <QSharedData>

namespace cmn
{

class SndFileData : public QSharedData
{
    Q_DECLARE_TR_FUNCTIONS(SndFileData)
public:
    SndFileData(SNDFILE *file);
    SndFileData(const QString &fileName, int mode, SF_INFO *sfInfo);
    ~SndFileData();

    void close();

    static sf_count_t length(void *userData);
    static sf_count_t seek(sf_count_t offset, int whence, void *userData);
    static sf_count_t read(void *buffer, sf_count_t count, void *userData);
    static sf_count_t write(const void *buffer, sf_count_t count,
                            void *userData);
    static sf_count_t tell(void *userData);

    QFile qFile;
    SF_VIRTUAL_IO virtualIO;
    SNDFILE *file;
};

class COMMON_EXPORT SndFile
{
public:
    SndFile();
    SndFile(SNDFILE *file);
    /* may throw */
    SndFile(const QString &fileName, int mode, SF_INFO *sfInfo);
    virtual ~SndFile();

    void reset();
    void reset(SNDFILE *file);
    /*may throw */
    void reset(const QString &fileName, int mode, SF_INFO *sfInfo);

    bool operator!() const;
    operator bool() const;
    SNDFILE *get() const;

    /* may throw */
    static int channels(const QString &fileName);
    /* may throw */
    static sf_count_t frames(const QString &fileName);

private:
    QExplicitlySharedDataPointer<SndFileData> d;
};
}

#endif
