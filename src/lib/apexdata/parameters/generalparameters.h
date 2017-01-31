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

#ifndef _EXPORL_SRC_LIB_APEXDATA_PARAMETERS_GENERALPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXDATA_PARAMETERS_GENERALPARAMETERS_H_

#include "apexparameters.h"

namespace apex {
namespace data
{
/**
@author Tom Francart,,,
*/
class APEXDATA_EXPORT GeneralParameters : public ApexParameters
{
public:
    GeneralParameters(XERCES_CPP_NAMESPACE::DOMElement* p_paramElement=0);

    ~GeneralParameters();

    virtual bool SetParameter(const QString& p_name, const QString& p_id, const QString& p_value, XERCES_CPP_NAMESPACE::DOMElement*);

    bool GetExitAfter() const { return m_bExitAfter; };
    void setExitAfter(bool exit) {m_bExitAfter = exit;};
    bool GetAutoSave() const { return m_bAutoSave; };
    void setAutoSave(bool autoSave) { m_bAutoSave = autoSave; };

      /**
        * Wait for start.
        * @return true if the start signal has to be emitted before the next trial starts
        */
    bool GetWaitForStart() const { return m_bWaitForStart; };

      /**
        * Check if the skip shortcut (F7) can be used.
        * @return true if allowed
        */
    bool GetAllowSkip() const { return m_bAllowSkip; };

    bool RunOutputTest() const { return m_bRunOutputTest; }

    const QString& OutputTestInput() const { return m_sOutputTestInput; }

    const QString& GetScriptLibrary() const { return m_scriptLibrary; }

    const QVariantMap& scriptParameters() const;

    bool operator==(const GeneralParameters& other) const;

private:
        bool m_bExitAfter;
        bool m_bAutoSave;

        bool m_bWaitForStart;
        bool m_bAllowSkip;
        bool m_bRunOutputTest;
        QString m_sOutputTestInput;
        QString m_scriptLibrary;
        QVariantMap m_scriptParameters;
};

}
}
#endif
