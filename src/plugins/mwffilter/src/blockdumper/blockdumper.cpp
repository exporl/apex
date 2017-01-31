/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#include <sndfile.h>

#include <QVector>

int main (int argc, const char* argv[])
{
    if (argc < 2)
        return 1;

    const unsigned blockSize = 8192;
    unsigned maxChannels = 0;

    QVector<SF_INFO> fileInfo (argc - 1);
    QVector<SNDFILE *> files (argc - 1);
    QVector<QVector<double> > data (argc - 1);
    for (unsigned i = 0; i < unsigned (argc) - 1; ++i) {
        fileInfo[i].format = 0;
        files[i] = sf_open (argv[i + 1], SFM_READ, &fileInfo[i]);
        Q_ASSERT (files[i]);
        data[i].resize (blockSize * fileInfo[i].channels);
        maxChannels = qMax (maxChannels, unsigned (fileInfo[i].channels));
    }

    unsigned pos = 0;
    Q_FOREVER {
        for (unsigned i = 0; i < unsigned (argc) - 1; ++i) {
            const unsigned frames = sf_readf_double (files[i],
                    data[i].data(), blockSize);
            if (frames < 8)
                return 0;
        }

        for (unsigned j = 0; j < maxChannels; ++j) {
            for (unsigned i = 0; i < unsigned (argc) - 1; ++i) {
                printf ("%04x %i %-45s "
                        "%+8f %+8f %+8f %+8f %+8f %+8f %+8f %+8f\n",
                        pos, j, argv[i + 1],
                        data[i][0 * fileInfo[i].channels + j],
                        data[i][1 * fileInfo[i].channels + j],
                        data[i][2 * fileInfo[i].channels + j],
                        data[i][3 * fileInfo[i].channels + j],
                        data[i][4 * fileInfo[i].channels + j],
                        data[i][5 * fileInfo[i].channels + j],
                        data[i][6 * fileInfo[i].channels + j],
                        data[i][7 * fileInfo[i].channels + j]);
            }
        }
        pos += blockSize;
    }

    for (unsigned i = 0; i < unsigned (files.size()); ++i)
        sf_close (files[i]);

    return 0;
}
