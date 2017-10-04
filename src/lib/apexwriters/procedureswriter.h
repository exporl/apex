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

#ifndef _APEX_SRC_LIB_APEXWRITERS_PROCEDURESWRITER_H_
#define _APEX_SRC_LIB_APEXWRITERS_PROCEDURESWRITER_H_

#include "apextools/global.h"

#include <QString>

namespace apex
{
namespace data
{
class ProcedureData;
class MultiProcedureData;
class AdaptiveProcedureData;
class ScriptProcedureData;
class TrialData;
class CorrectorData;
}
namespace writer
{

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
    static QDomElement addElement(QDomDocument *doc,
                                  const data::ProcedureData &data);

private:
    /**
     *Appends a <procedure> element to the given DOMElement.
     *
     *@param into the element to place the <procedure> in
     *@param data the data to fill this element with
     */
    static void fillProcedure(QDomElement *into,
                              const data::ProcedureData &data);

    /**
     *Appends a <procedure> element to the given DOMElement.
     *Used for a multiprocedure.
     *
     *@param into the element to place the <procedure> in
     *@param data the data to create the element with
     *
     *@return the created element
     */
    static void fillMultiProcedure(QDomElement *into,
                                   const data::MultiProcedureData &data);

    /**
     *Fills a <parameters> element with the given parameters.
     *
     *@param data  the parameters to fill the element with
     *@param theElement the element to fill
     */
    static void fillParametersElement(const data::ProcedureData &data,
                                      QDomElement *theElement);

    static void fillCorrector(const data::CorrectorData &data,
                              QDomElement *corrector);

    /**
     * Finishes the given element as an adaptive parameters alement with the
     * given data.
     *
     * @param data  the parameters to finish the element with
     * @param theElement the element to finish
     */
    static void finishAsAdaptive(const data::AdaptiveProcedureData &data,
                                 QDomElement *toFinish);

    /**
     * Finishes the given element as aplugin parameters alement with the
     * given data.
     *
     * @param data  the parameters to finish the element with
     * @param theElement the element to finish
     */
    static void finishAsPlugin(const data::ScriptProcedureData &data,
                               QDomElement *toFinish);

    /**
     *Fills a <trials> element with the given vector of trials.
     *
     *@param trials  a vector with all the trials to fill this element with
     *@param theElement the element to fill
     */
    static void fillTrialsElement(const data::ProcedureData &data,
                                  QDomElement *theElement);

    /**
     *Gives a string representation of the procedure type
     *
     *@param type The type to get a representation for. It is an element of an
     *enum
     *
     *@return The string representation of the given type
     */
    static QString getTypeString(int type);

    /**
     *Gives a string representation of the order
     *
     *@param order The order to get a representation for. It is an element of an
     *enum
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
    static bool isMultiProcedure(const data::ProcedureData &data);

    /**
     * Create a stepsize DOM element from the given information
     * @param doc the document where the stepsize element will be placed in
     * @param begin information about the stepsize (see procedure.xsd for more
     * information)
     * @param size information about the stepsize (see procedure.xsd for more
     * information)
     * @param direction information about the stepsize (see procedure.xsd for
     * more information)
     * @return The stepsize DOM element with the given information
     */
    static QDomElement createStepsizeElement(QDomDocument *doc, int begin,
                                             float size,
                                             QString direction = "");
};
}
}

#endif
