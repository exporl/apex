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

#ifndef EXPERIMENTTESTER_H
#define EXPERIMENTTESTER_H

#include "writertester.h"
#include "experimentdata.h"


/**
    @author Job Noorman <jobnoorman@gmail.com>
 */
class ExperimentTester : public WriterTester
{
    public:
        ~ExperimentTester(){}

//         virtual void ParseData ( DOMElement* root );
//         virtual DOMElement* WriteData ( DOMDocument* doc );
        virtual bool Test(QStringList dirsToCheck);
        
    protected:
        
        virtual void ParseData ( DOMElement* ){}
        virtual DOMElement* WriteData ( DOMDocument* ){}
        
};

#endif
