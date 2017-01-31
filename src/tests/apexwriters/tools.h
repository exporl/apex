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

#include "xml/xercesinclude.h"
namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
class DOMDocument;
};

#include <QString>


XERCES_CPP_NAMESPACE::DOMElement* ParseXMLDocument(QString filename,
                                                  bool verbose=true);

//QString ShowTree(XERCES_CPP_NAMESPACE::DOMElement* root);

QString outfileName(const QString& infile);

bool PrintDocument(XERCES_CPP_NAMESPACE::DOMDocument* doc);
bool PrintElement(XERCES_CPP_NAMESPACE::DOMElement* e);
bool WriteDocument(XERCES_CPP_NAMESPACE::DOMDocument* doc, QString filename);
bool WriteElement(XERCES_CPP_NAMESPACE::DOMElement* e, QString filename);

bool CompareDocuments(QString file1, QString file2);

const QString rootDir("data/");
const QString outDir("outfiles/");
const QString experimentsDir(rootDir + "experiments");
