/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef PROCEDURESWRITER_H
#define PROCEDURESWRITER_H

/**
 *@author Job Noorman
 */

//#include "device/devicedata.h"

#include "global.h"

#include "xml/xercesinclude.h"
#include <vector>

#include <QString>

namespace XERCES_CPP_NAMESPACE
{
class DOMDocument;
class DOMElement;
}

namespace apex
{
namespace data
{
class ApexProcedureConfig;
class ApexMultiProcedureConfig;
class ApexProcedureParameters;
class ApexAdaptiveProcedureParameters;
class PluginProcedureParameters;
class ApexTrial;
}
namespace writer
{
using namespace XERCES_CPP_NAMESPACE;

class APEXWRITERS_EXPORT ProceduresWriter
{
    public:
        /**
        *Adds a <procedure> element to the document, a multiprocedure
        *is auto detected
        *
        *@param doc  the document to place the element in
        *@param data the data to create the element with
        *
        *@return the created element
        */
        static DOMElement* addElement(DOMDocument* doc,
                               const data::ApexProcedureConfig& data);

    private:

        /**
        *Appends a <procedure> element to the given DOMElement.
        *
        *@param into the element to place the <procedure> in
        *@param data the data to fill this element with
         */
        static void fillProcedure(DOMElement* into,
                           const data::ApexProcedureConfig& data);

        /**
         *Appends a <procedure> element to the given DOMElement.
         *Used for a multiprocedure.
         *
         *@param into the element to place the <procedure> in
         *@param data the data to create the element with
         *
         *@return the created element
         */
        static void fillMultiProcedure(DOMElement* into,
                                const data::ApexMultiProcedureConfig& data);

        /**
        *Fills a <parameters> element with the given parameters.
        *
        *@param params  the parameters to fill the element with
        *@param theElement the element to fill
        */
        static void fillParametersElement(const data::ApexProcedureParameters& params,
                                   DOMElement* theElement);

        /**
         * Finishes the given element as an adaptive parameters alement with the
         * given data.
         *
         * @param params  the parameters to finish the element with
         * @param theElement the element to finish
         */
static void finishAsAdaptive(const data::ApexAdaptiveProcedureParameters&
                params, DOMElement* toFinish);

        /**
         * Finishes the given element as aplugin parameters alement with the
         * given data.
         *
         * @param params  the parameters to finish the element with
         * @param theElement the element to finish
         */
        static void finishAsPlugin(const data::PluginProcedureParameters&
                params, DOMElement* toFinish);

        /**
         *Fills a <trials> element with the given vector of trials.
         *
         *@param trials  a vector with all the trials to fill this element with
         *@param theElement the element to fill
         */
        static void fillTrialsElement(const std::vector<data::ApexTrial*>& trials,
                               DOMElement* theElement);

        /**
         *Gives a string representation of the procedure type
         *
         *@param type The type to get a representation for. It is an element of an enum
         *
         *@return The string representation of the given type
         */
        static QString getTypeString(int type);

        /**
         *Gives a string representation of the order
         *
         *@param order The order to get a representation for. It is an element of an enum
         *
         *@return The string representation of the given order
         */
        static QString getOrderString(int order);

        /**
        *Indicates whether the given config is the config of a multiprocedure
        *@param data the config to check
        *
        *@return true if the given config is the config of  multiprocedure,
        *  false otherwise
        */
        static bool isMultiProcedure(const data::ApexProcedureConfig& data);
};
}
}

#endif
