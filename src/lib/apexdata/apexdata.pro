BASEDIR = ../../..
CLEBS += builddll apextools

DEFINES*=APEXDATA_MAKEDLL

include ($$BASEDIR/clebs.pri)

#QT *= gui

# WTF? TODO FIXME path???
INCLUDEPATH += .

HEADERS += \
#    elementfactory.h \
    experimentdata.h \
    factoryelement.h \
    fileprefix.h \
    calibration/calibrationdata.h \
    calibration/soundlevelmeterdata.h \
    connection/connectiondata.h \
    corrector/correctordata.h \
    data/iddata.h \
    datablock/datablockdata.h \
    datablock/datablocksdata.h \
    device/clariondevicedata.h \
    device/devicedata.h \
    device/devicesdata.h \
    device/l34devicedata.h \
    device/plugincontrollerdata.h \
    device/wavdevicedata.h \
    device/dummydevicedata.h \
    device/mixer/mixerparameters.h \
    filter/filterdata.h \
    filter/filtersdata.h \
    filter/pluginfilterdata.h \
    filter/wavparameters.h \
    interactive/parameterdialogresults.h \
    map/apexmap.h \
    map/channelmap.h \
#    parameters/apexmapparameters.h \
    parameters/apexparameters.h \
#    parameters/apexstimulusparameters.h \
    parameters/generalparameters.h \
    parameters/parameter.h \
    parameters/simpleparameters.h \
    parameters/parametermanagerdata.h \
    procedure/apexadaptiveprocedureparameters.h \
    procedure/apexanswer.h \
    procedure/apexconstantprocedureparameters.h \
    procedure/apexmultiprocedureconfig.h \
    procedure/apexmultiprocedureparameters.h \
    procedure/apexprocedureconfig.h \
    procedure/apexprocedureparameters.h \
    procedure/apextrainingprocedureparameters.h \
    procedure/apextrial.h \
    procedure/pluginprocedureconfig.h \
    procedure/pluginprocedureparameters.h \
    randomgenerator/randomgeneratorparameters.h \
    result/resultparameters.h \
    screen/arclayoutelement.h \
    screen/answerlabelelement.h \
    screen/buttonelement.h \
    screen/buttongroup.h \
    screen/checkboxelement.h \
    screen/emptyelement.h \
    screen/flashplayerelement.h \
    screen/gridlayoutelement.h \
    screen/labelelement.h \
    screen/matrixelement.h \
    screen/parameterdata.h \
    screen/parameterlabelelement.h \
    screen/parameterlistelement.h \
    screen/parameterscontainerelement.h \
    screen/pictureelement.h \
    screen/picturelabelelement.h \
    screen/screen.h \
    screen/screenelement.h \
    screen/screenelementvisitor.h \
    screen/screenlayoutelement.h \
    screen/screensdata.h \
    screen/sliderelement.h \
    screen/spinboxelement.h \
    screen/texteditelement.h \
    stimulus/stimulidata.h \
    stimulus/stimulusdata.h \
    stimulus/stimulusparameters.h \
    stimulus/nucleus/nicstream/stimulation_mode.h \


SOURCES += \
    experimentdata.cpp \
    fileprefix.cpp \
    calibration/calibrationdata.cpp \
    calibration/soundlevelmeterdata.cpp \
    connection/connectiondata.cpp \
    corrector/correctordata.cpp \
    data/iddata.cpp \
    datablock/datablockdata.cpp \
    datablock/datablocksdata.cpp \
    device/clariondevicedata.cpp \
    device/devicedata.cpp \
    device/devicesdata.cpp \
    device/l34devicedata.cpp \
    device/plugincontrollerdata.cpp \
    device/wavdevicedata.cpp \
    device/dummydevicedata.cpp \
    device/mixer/mixerparameters.cpp \
    filter/filterdata.cpp \
    filter/filtersdata.cpp \
    filter/pluginfilterdata.cpp \
    filter/wavparameters.cpp \
    interactive/parameterdialogresults.cpp \
    map/apexmap.cpp \
    map/channelmap.cpp \
#    parameters/apexmapparameters.cpp \
    parameters/apexparameters.cpp \
#    parameters/apexstimulusparameters.cpp \
    parameters/generalparameters.cpp \
    parameters/simpleparameters.cpp \
    parameters/parametermanagerdata.cpp \
    parameters/parameter.cpp \
    procedure/apexadaptiveprocedureparameters.cpp \
    procedure/apexanswer.cpp \
    procedure/apexconstantprocedureparameters.cpp \
    procedure/apexmultiprocedureconfig.cpp \
    procedure/apexmultiprocedureparameters.cpp \
    procedure/apexprocedureconfig.cpp \
    procedure/apexprocedureparameters.cpp \
    procedure/apextrainingprocedureparameters.cpp \
    procedure/apextrial.cpp \
    procedure/pluginprocedureconfig.cpp \
    procedure/pluginprocedureparameters.cpp \
    randomgenerator/randomgeneratorparameters.cpp \
    result/resultparameters.cpp \
    screen/arclayoutelement.cpp \
    screen/answerlabelelement.cpp \
    screen/buttonelement.cpp \
    screen/buttongroup.cpp \
    screen/checkboxelement.cpp \
    screen/emptyelement.cpp \
    screen/flashplayerelement.cpp \
    screen/gridlayoutelement.cpp \
    screen/labelelement.cpp \
    screen/matrixelement.cpp \
    screen/parameterlabelelement.cpp \
    screen/parameterlistelement.cpp \
    screen/parameterscontainerelement.cpp \
    screen/pictureelement.cpp \
    screen/picturelabelelement.cpp \
    screen/screen.cpp \
    screen/screenelement.cpp \
    screen/screenelementvisitor.cpp \
    screen/screenlayoutelement.cpp \
    screen/screensdata.cpp \
    screen/sliderelement.cpp \
    screen/spinboxelement.cpp \
    screen/texteditelement.cpp \
    stimulus/stimulidata.cpp \
    stimulus/stimulusdata.cpp \
    stimulus/stimulusparameters.cpp \
