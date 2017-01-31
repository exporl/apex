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

#ifndef APEXAPEXMAPPARAMETERS_H
#define APEXAPEXMAPPARAMETERS_H

#include "apextypedefs.h"
#include "global.h"

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
};


namespace apex
{
namespace data
{
/**
  * ApexMapParameters
  *   base class for simple parameters.
  *   A parameter has a name and a value,
  *   and is stored in a map<QString,QString> as such.
  *   Used for parameters that look like
  *   @verbatim
        <driver>portaudio</driver>
        <card>default</card>
  *   @endverbatim
  *   @note this class is almost never used as is,
  *   it is too limited.
  *   @see ApexExtendedParameters for a derived class
  *   that has more functionality
  **************************************************** */
//FIXME [job] this looks a lot like SimpleParameters...
class APEXDATA_EXPORT ApexMapParameters//: public apex::tParamMap
{
    public:
        /**
          * Constructor.
          * @param a_pParamElement a DOMElement containing parameters.
          * If non-zero, this will be parsed.
          */
        ApexMapParameters(XERCES_CPP_NAMESPACE::DOMElement* a_pParamElement = 0);

        /**
          * Destructor.
          */
        virtual ~ApexMapParameters();

        typedef tParamMap::const_iterator const_iterator;

        tParamMapCIt begin() const;
        tParamMapIt begin();
        tParamMapCIt end() const;
        void insert(QString name, QString value);
        void insert(tParamMapPair pair);
        void remove(const QString& name);
        void erase(tParamMapIt it);
		bool isEmpty() const;

        /**
          * Parses a a DOMElement containing parameters.
          * Adds all parameters to the map.
          */
        virtual bool Parse(XERCES_CPP_NAMESPACE::DOMElement* a_pParamElement);

        /**
          * Set a paremeter.
          * Parameter is added if it doesn't exist already.
          * @param ac_sName the name
          * @param ac_sValue the new value
          */
        virtual void SetParameter(const QString& ac_sName, const QString& ac_sValue);

        /**
          * Get a parameter.
          * @param ac_sName the name
          * @return the value or empty string if not found.
          */
        virtual QString GetParameter(const QString& ac_sName) const;

        /**
          * See if a given parameter exists.
          * @param ac_sName the name
          * @return true if found
          */
        virtual bool HasParameter(const QString& ac_sName) const;

        /**
          * Return everything in a nice format for displaying.
          * @return a string containing all parameters
          */
        virtual QString mf_sPrint(const QString& ac_Seperator = ", ") const;

        virtual bool operator==(const ApexMapParameters& other) const;

    private:

        tParamMap parameters;

};

}
}

#endif
