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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_PARSER_SCRIPTEXPANDER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_PARSER_SCRIPTEXPANDER_H_

#include <QCoreApplication>
#include <QString>

class QWidget;

namespace apex
{

namespace parser
{

/**
 * Take an xmlGeneratingPluginType from an XML file and expand it to
 * a DOM tree to be parsed later
 */
class ScriptExpander
{
    Q_DECLARE_TR_FUNCTIONS(ScriptExpander)
public:
    static const QString INSTANCENAME;

    /**
     * libraryFile: this file will be concatenated to the script
     * parent: parent widget (for showing dialog boxes), if 0, no dialog
     * boxes will be shown
     */
    ScriptExpander(const QString &fileName, const QString &libraryFile,
                   const QVariantMap &scriptParameters, QWidget *parent = 0);

    /**
     * Take a base element of type xmlGeneratingPluginType and expand
     * it to a new tree containing plain XML
     * function: function to be called in script
     */
    void ExpandScript(const QDomNode &base, const QString &function);

    QString fileName;
    QString m_libraryFile;
    QVariantMap m_scriptParameters;

    QWidget *m_parent;
};
}
}

#endif
