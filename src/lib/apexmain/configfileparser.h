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

#ifndef APEXCONFIGFILEPARSER_H
#define APEXCONFIGFILEPARSER_H

class QString;
class QDomDocument;

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
  class DOMDocument;
};
//using namespace XERCES_CPP_NAMESPACE;

#include <QObject>

#include "xml/apexxmltools.h"
#include "xml/xmldocumentgetter.h"

#include "parser/apexparser.h"

namespace apex {

  class ApexErrorHandler;
  class StatusReporter;

/**
@author Tom Francart,,,
*/

class ConfigFileParser: public parser::Parser //public QObject
{
public:
    ConfigFileParser(const QString& configFilename,
                     const QString& configFileSchema,
                     const QString& configfileNamespace);

    virtual bool CFParse();

  const QString& GetConfigFilename() const;
  void SetConfigFilename(const QString &name) ;

  void LogErrorsToApex( StatusReporter* a_pError );

  virtual ~ConfigFileParser();

protected:
  xercesc::DOMDocument* m_document;
  QString m_sConfigFilename;
  QString m_sConfigFileSchema;
  QString m_sConfigFileNamespace;

  ApexXMLTools::XMLDocumentGetter  xmlDocumentGetter;

  //! return true if upgrade was performed
  virtual bool upgradeFrom(QDomDocument& doc, const QVector<int>& v);

private:
        virtual const QString getConfigfileNamespace()=0;

};

}

#endif
