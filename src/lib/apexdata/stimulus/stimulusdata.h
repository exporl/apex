/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXDATA_STIMULUS_STIMULUSDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_STIMULUS_STIMULUSDATA_H_

#include "stimulusparameters.h"

#include "apextools/global.h"

#include <QList>
#include <QObject>
#include <QString>

namespace apex
{

using data::StimulusParameters;

namespace parser
{
class StimuliParser;
}

namespace data
{

struct APEXDATA_EXPORT StimulusDatablocksContainer
    : public QList<StimulusDatablocksContainer> {
    QString id;
    enum Type { DATABLOCKS, DATABLOCK, SEQUENTIAL, SIMULTANEOUS } type;

    StimulusDatablocksContainer(Type type = DATABLOCKS) : type(type)
    {
    }
    QString typeName() const;

    QString toString() const;
    QStringList toStringList() const;

    bool operator==(const StimulusDatablocksContainer &other) const;
};

#ifdef Q_CC_MSVC
// needed because (dum) MSVC expands QList to all methods and somehow it does
// not find template qHash to use for StimulusDatablocksContainer
inline uint qHash(const StimulusDatablocksContainer)
{
    qFatal("qHash called for StimulusDatablocksContainer");
    return 0;
}
#endif

// must be QObject to be used from javascript
class APEXDATA_EXPORT StimulusData
{
    friend class parser::StimuliParser;

public:
    StimulusData();
    StimulusData(const StimulusData &);

    const StimulusParameters &GetFixedParameters() const;

    const StimulusParameters &GetVariableParameters() const
    {
        return m_varParams;
    }

    const StimulusDatablocksContainer GetDatablocksContainer() const
    {
        return m_datablocksContainer;
    }

    void setFixedParameters(const StimulusParameters &parameters);
    void setDatablocksContainer(const StimulusDatablocksContainer &cont);
    void setId(const QString &id);

    const QString GetID() const;
    const QString description() const;

    bool operator==(const StimulusData &other) const;

private:
    QString m_description;
    StimulusParameters m_fixParams;
    StimulusParameters m_varParams;
    StimulusDatablocksContainer m_datablocksContainer;
    QString m_id;
};
}
}

#endif
