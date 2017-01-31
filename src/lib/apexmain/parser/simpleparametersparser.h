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

#ifndef SIMPLEPARAMETERSPARSER
#define SIMPLEPARAMETERSPARSER

#include <QString>
#include <QVariant>

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
    class DOMElement;
};

namespace apex {




    namespace data {
        class SimpleParameters;
        class ParameterManagerData;
    }

    namespace parser {

class SimpleParametersParser {
    public:
        virtual ~SimpleParametersParser() {};
        SimpleParametersParser();
        SimpleParametersParser(data::ParameterManagerData* d);

        /** Parse parameters under the given DOMElement into SimpleParameters p
         * Does not take ownership of p
         */
        virtual void Parse(XERCES_CPP_NAMESPACE::DOMElement* base, data::SimpleParameters* p);
        void SetParameterManagerData(data::ParameterManagerData* p) { parameterManagerData=p;};

    protected:
        virtual void AddParameter(data::SimpleParameters* p, XERCES_CPP_NAMESPACE::DOMElement* e, const QString& owner, const QString& type, const QString& id, const QVariant& value, const int channel);

        data::ParameterManagerData* parameterManagerData;


};



    }

}



#endif
