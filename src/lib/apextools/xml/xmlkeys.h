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

#ifndef _EXPORL_SRC_LIB_APEXTOOLS_XML_XMLKEYS_H_
#define _EXPORL_SRC_LIB_APEXTOOLS_XML_XMLKEYS_H_

#include <QString>

/*
TODO define as much keys as possible here:

- it is faster and cleaner having the memory in one place then
allocating/deallocating the same memory on the stack time and time again,
  which also involves calling the QString constructor and destructor
- it reduces typos
- it keeps them all in one place
- if the schema name changes, it only has to be changed here

*/

namespace apex
{
namespace XMLKeys
{

//! general
const QString gc_sID("id");
const QString gc_sChannel("channel");
const QString gc_sType("xsi:type");
const QString gc_sName("name");
const QString gc_sValue("value");

//! screen/layout
const QString gc_sGridLayout("gridLayout");
const QString gc_sVertLayout("vLayout");
const QString gc_sHoriLayout("hLayout");
const QString gc_sArcLayout("arcLayout");
const QString gc_sTwoPartLayout("twoPartLayout");
const QString gc_sWidth("width");
const QString gc_sHeight("height");
const QString gc_sRatio("ratio");
const QString gc_sDirection("direction");
const QString gc_sOrientation("orientation");
const QString gc_sHorizontal("horizontal");
const QString gc_sVertical("vertical");
const QString gc_sX("x");
const QString gc_sY("y");
const QString gc_sRow("row");
const QString gc_sCol("col");

const QString gc_sButtonGroup("buttongroup");
const QString gc_sDefaultAnswer("default_answer_element");

const QString gc_sButton("button");
const QString gc_sMatrix("matrix");
const QString gc_sHtml("html");
const QString gc_sLabel("label");
const QString gc_sFlash("flash");
const QString gc_sPicture("picture");
const QString gc_sTextEdit("textEdit");
const QString gc_sSpinBox("spinBox");
const QString gc_sSlider("slider");
const QString gc_sCheckBox("checkBox");

const QString gc_sPictureLabel("picturelabel");
const QString gc_sAnswerLabel("answerlabel");
const QString gc_sParameterList("parameterlist");
const QString gc_sNumericKeypad("numericKeypad");

const QString gc_sText("text");
const QString gc_sShortcut("shortcut");
const QString gc_sFeedback("feedback");
const QString gc_sInputmask("inputmask");
const QString gc_sFontsize("fontsize");
const QString gc_sFGColor("fgcolor");
const QString gc_sBGColor("bgcolor");
const QString gc_sDisabled("disabled");

const QString gc_sMin("min");
const QString gc_sMax("max");
const QString gc_sStep("step");
const QString gc_sTickPos("tickPosition");
const QString gc_sNoTicks("noTicks");
const QString gc_sTicksBoth("ticksBoth");
const QString gc_sTicksAbove("ticksAbove");
const QString gc_sTicksBelow("ticksBelow");
const QString gc_sTicksRight("ticksRight");
const QString gc_sTicksLeft("ticksLeft");
const QString gc_sTickInt("tickIntervall");
const QString gc_sParameter("parameter");
const QString gc_sReset("reset");

const QString gc_sLength("length");
const QString gc_sItiScreen("intertrialscreen");

const QString gc_sPadZero("padzero");
const QString gc_sZeroBefore("zerobefore__");

// from apexdefines.h
const QString sc_sWavDevice("apex:wavDeviceType");
const QString sc_sCohDevice("apex:CohDeviceType");
const QString sc_sDummyDevice("apex:dummyDeviceType");

const QString sc_sPluginController("apex:pluginController");

const QString sc_sID(gc_sID);
const QString sc_sType("xsi:type");
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXTOOLS_XML_XMLKEYS_H_
