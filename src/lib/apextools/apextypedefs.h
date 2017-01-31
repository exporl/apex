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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_APEXTYPEDEFS_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_APEXTYPEDEFS_H_

/**
  * define common typedefs here
  ***************************** */


#include <QMap>
#include <QString>
#include <QPair>

#include <map>
#include <vector>

//TODO * replace all STL containers with Qt containers
//     * rename all tFoo typedefs to Foo

namespace apex
{

//TODO remove the typedefs in the apex namespace because they are so general
//     that it's much clearer to just write them fully all the time.
typedef std::vector<bool>                 tBooleanVector;
typedef std::vector<QString>              tQStringVector;

typedef QMap<QString, QString>            tParamMap;
typedef QPair<QString, QString>           tParamMapPair;
typedef tParamMap::const_iterator         tParamMapCIt;
typedef tParamMap::iterator               tParamMapIt;

typedef std::map<QString, bool>           tQStringBoolMap;
typedef std::pair<QString, bool>          tQStringBoolPair;
typedef tQStringBoolMap::iterator         tQStringBoolMapIt;
typedef tQStringBoolMap::const_iterator   tQStringBoolMapCIt;

typedef std::map<QString, unsigned>       tQStringUintMap;
typedef std::pair<QString, unsigned>      tQStringUintPair;
typedef tQStringUintMap::iterator         tQStringUintMapIt;
typedef tQStringUintMap::const_iterator   tQStringUintMapCIt;

typedef std::map<unsigned, QString>       tUintQStringMap;
typedef std::pair<unsigned, QString>      tUintQStringPair;
typedef tUintQStringMap::iterator         tUintQStringMapIt;
typedef tUintQStringMap::const_iterator   tUintQStringMapCIt;

class ApexModule;
typedef QList<ApexModule*>          ModuleList;

typedef int tEventCode;

namespace data
{
class Screen;
class ScreenElement;
class DeviceData;
class DatablockData;
typedef QMap<QString, DeviceData*>      DeviceDataMap;
typedef std::pair<QString,data::DeviceData*>      tDeviceDataPair;

typedef QMap<QString, data::DatablockData*>          DatablockDataMap;

typedef QList<int> tStretchList;            //! list of stretchfactors


}

namespace stimulus
{
class   IDeviceFactory;
class   DataBlock;
class   OutputDevice;
class   Stimulus;
class   Filter;
struct  tCalibratorConfig;
struct  tConnection;



typedef QMap<QString, OutputDevice*>      tDeviceMap;

typedef tDeviceMap::iterator              tDeviceMapIt;
typedef tDeviceMap::const_iterator        tDeviceMapCIt;

typedef QMap<QString, Filter*>            tFilterMap;
typedef tFilterMap::iterator              tFilterMapIt;
typedef tFilterMap::const_iterator        tFilterMapCIt;

typedef QMap<QString, DataBlock*>         tDataBlockMap;
typedef tDataBlockMap::iterator           tDataBlockMapIt;
typedef tDataBlockMap::const_iterator     tDataBlockMapCIt;

typedef std::map<QString, tQStringVector> tQStringVectorMap;
typedef tQStringVectorMap::iterator       tQStringVectorMapIt;
typedef tQStringVectorMap::const_iterator tQStringVectorMapCIt;

/*typedef QStringPtrMap<Stimulus>           tStimuliMap;
typedef tStimuliMap::iterator             tStimuliMapIt;
typedef tStimuliMap::const_iterator       tStimuliMapCIt;*/
    typedef QMap<QString, Stimulus*> tStimulusMap;
}

namespace device
{
class IApexDevice;
class ControlDevice;

typedef QMap<QString, ControlDevice*>        tControllerMap;
}



namespace rundelegates
{
class ScreenElementRunDelegate;
}
namespace gui
{
class ScreenRunDelegate;

typedef QMap<QString, data::Screen*>             tScreenMap;
typedef tScreenMap::iterator              tScreenMapIt;
typedef tScreenMap::const_iterator        tScreenMapCIt;

typedef QMap<QString, data::ScreenElement*>      ScreenElementMap;
typedef ScreenElementMap::iterator       tScreenElementMapIt;
typedef ScreenElementMap::const_iterator tScreenElementMapCIt;


}

}

#endif //#ifndef _EXPORL_SRC_LIB_APEXTOOLS_APEXTYPEDEFS_H_
