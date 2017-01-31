BASEDIR = ../../..
CLEBS += +xalan \
    +xerces \
    streamapp \
    pch \
    builddll \
    apextools \
    apexdata \
    apexwriters \
    portaudio
unix:CLEBS *= jack
DEFINES *= APEX_MAKEDLL
include ($$BASEDIR/clebs.pri)
CONFIG += qaxcontainer
QT += gui \
    script \
    svg \
    xml \
    webkit \
    network
QT += qt3support
DEFINES += SETMIXER

# TODO FIXME
# clebsCheckModules(perl):DEFINES += PERL
# clebsCheckModules(pa5):DEFINES += PA5
win32 { 
    # Always compile flash support
    DEFINES += FLASH
    # Fix link order problems
    DEFINES += _AFXDLL
    
    # Windows header file version
    DEFINES += WINVER=0x500
    
    # Streamapp mixer code
    LIBS *= -lwinmm
    
    # Streamapp fix for VS2008
    LIBS *= -ladvapi32
    LIBS *= -luser32
    LIBS *= -lole32
}

# WTF? TODO FIXME path???
INCLUDEPATH += .
TARGET = apexmain
HEADERS += apexloader.h \
    apexcontrol.h \
    apexmodule.h \
    debug.h \
    outputrecorder.h \
    calibration/calibrationdatabase.h \
    calibration/soundlevelmeter.h \
    calibration/autocalibration.h \
    calibration/calibrationio.h \
    calibration/calibrationparser.h \
    calibration/calibrator.h \
    configfileparser.h \
    connection/connectiondata.h \
    connection/connection.h \
    connection/connectionparser.h \
    connection/connectionrundelegatecreator.h \
    corrector/comparator.h \
    corrector/corrector.h \
    corrector/correctorparser.h \
    corrector/cvccomparator.h \
    corrector/equalcomparator.h \
    corrector/fcacorrector.h \
    datablock/datablocksparser.h \
    device/clariondeviceparser.h \
    device/controldevice.h \
    device/controllers.h \
    device/devicesparser.h \
    device/iapexdevice.h \
    device/l34deviceparser.h \
    device/plugincontroller.h \
    device/plugincontrollerinterface.h \
    device/wavdeviceparser.h \
    experimentparser.h \ # elementfactory.h \
    factoryelement.h \
    factory.h \
    fileprefixconvertor.h \
    filter/dataloopgeneratorfilter.h \
    filter/filterparser.h \
    filter/pluginfilterinterface.h \
    filter/wavgenerator.h \
    gui/apexmainwndbase.h \
    gui/apexmapwizard.h \
    gui/apexmapwizard.h \
    gui/calibrationdialog.h \
    gui/autocalibrationdialog.h \
    gui/calibrationsetupsdialog.h \
    gui/centralwidget.h \
    gui/connectiondialog.h \
    gui/mainwindow.h \
    gui/mru.h \
    gui/panelelements.h \
    gui/panel.h \
    gui/ratiolayout.h \
    gui/startupdialog.h \
    screen/screenrundelegate.h \
    screen/screensparser.h \
    gui/plugindialog.h \
    interactive/parameterdialog.h \
    md5/md5.h \
    parameters/apexparametersparser.h \
    parameters/parametermanager.h \
    parser/apexparser.h \
    parser/prefixparser.h \
    parser/scriptexpander.h \
    parser/xmlpluginapi.h \
    parser/simpleparametersparser.h \
    procedure/adaptiveprocedure.h \
    procedure/adaptiveprocedureparser.h \
    procedure/apexadaptiveprocedure.h \
    procedure/apexadaptiveprocedureparametersparser.h \
    procedure/apexconstantprocedure.h \
    procedure/apexconstantprocedureparametersparser.h \
    procedure/apexmultiprocedure.h \
    procedure/apexmultiprocedureparametersparser.h \
    procedure/apexpluginprocedureparametersparser.h \
    procedure/apexprocedure.h \
    procedure/apexprocedureparametersparser.h \
    procedure/apextrainingprocedure.h \
    procedure/apextrainingprocedureparametersparser.h \
    procedure/apextrialfactory.h \
    procedure/constantprocedureparser.h \
    procedure/multiprocedureparser.h \
    procedure/pluginprocedureapi.h \
    procedure/pluginprocedure.h \
    procedure/pluginprocedureinterface.h \
    procedure/pluginprocedureparser.h \
    procedure/pluginprocedurewrapper.h \
    procedure/procedureparser.h \
    procedure/procedureparsersparent.h \
    procedure/trainingprocedureparser.h \
    randomgenerator/gaussianrandomgenerator.h \
    randomgenerator/randomgeneratorfactory.h \
    randomgenerator/randomgenerator.h \
    randomgenerator/randomgenerators.h \
    randomgenerator/uniformrandomgenerator.h \
    result/resultviewer.h \
    resultsink/apexresultsink.h \
    resultsink/rtresultsink.h \
    resultsink/trialresult.h \
    runner/experimentrundelegate.h \
    runner/experimentrunner.h \
    runner/simplerunner.h \
    runner/pluginrunner.h \
    screen/answerlabelrundelegate.h \
    screen/apexscreen.h \
    screen/apexscreenresult.h \
    screen/arclayoutrundelegate.h \
    screen/matrixrundelegate.h \
    screen/buttonrundelegate.h \
    screen/sliderrundelegate.h \
    screen/checkboxrundelegate.h \
    screen/gridlayoutrundelegate.h \
    screen/labelrundelegatebase.h \
    screen/labelrundelegate.h \
    screen/parameterlabelrundelegate.h \
    screen/parameterlistrundelegate.h \
    screen/parameterscontainerrundelegate.h \
    screen/picturelabelrundelegate.h \
    screen/picturerundelegate.h \
    screen/picturebuttonwidget.h \
    screen/rundelegatecreatorvisitor.h \
    screen/screenelementrundelegate.h \
    screen/screenparser.h \
    screen/spinboxrundelegate.h \
    screen/texteditrundelegate.h \
    services/application.h \
    services/errorhandler.h \
    services/filedialog.h \
    services/mainconfigfileparser.h \
    services/paths.h \
    services/servicemanager.h \
    services/pluginloader.h \
    services/debugapplication.h \
    stimulus/clarion/clariondatablock.h \
    stimulus/clarion/clariondatablock.h \
    stimulus/clarion/clariondevice.h \
    stimulus/clarion/clarionfilter.h \
    stimulus/clarion/clarionwrapper.h \
    stimulus/datablock.h \
    stimulus/dummy/dummydevice.h \
    stimulus/l34/apexnresocketinterface.h \
    stimulus/l34/qicparser.h \
    stimulus/l34/aseqparser.h \
    stimulus/outputdevice.h \
    stimulus/stimuliparser.h \
    stimulus/stimulus.h \
    stimulus/stimulusoutput.h \
    stimulus/wav/bufferdropcallback.h \
    stimulus/wav/pluginfilter.h \
    stimulus/wav/pluginfilterprocessor.h \
    stimulus/wav/soundcarddialog.h \
    stimulus/wav/soundcarddisplayer.h \
    stimulus/wav/wavcompatibledatablock.h \
    stimulus/wav/wavdatablock.h \
    stimulus/wav/wavdeviceio.h \
    stimulus/wav/wavfader.h \
    stimulus/wav/wavfilter.h \
    timer/apextimer.h \
    feedback/feedback.h \
    feedback/pluginfeedbackinterface.h
SOURCES += apexloader.cpp \
    apexcontrol.cpp \
    apexmodule.cpp \
    debug.cpp \
    outputrecorder.cpp \
    calibration/calibrationdatabase.cpp \
    calibration/calibrationio.cpp \
    calibration/calibrationparser.cpp \
    calibration/calibrator.cpp \
    calibration/autocalibration.cpp \
    configfileparser.cpp \
    connection/connectionparser.cpp \
    connection/connectionrundelegatecreator.cpp \
    corrector/corrector.cpp \
    corrector/correctorparser.cpp \
    corrector/cvccomparator.cpp \
    corrector/fcacorrector.cpp \
    datablock/datablocksparser.cpp \
    device/clariondeviceparser.cpp \
    device/controldevice.cpp \
    device/controllers.cpp \
    device/devicesparser.cpp \
    device/iapexdevice.cpp \
    device/l34deviceparser.cpp \
    device/mixer/imixer.cpp \
    device/mixer/mixerfactory.cpp \
    device/mixer/streamappmixer.cpp \
    device/plugincontroller.cpp \
    device/wavdeviceparser.cpp \
    experimentparser.cpp \
    fileprefixconvertor.cpp \
    filter/filter.cpp \
    filter/filterparser.cpp \
    filter/wavgenerator.cpp \
    gui/apexmainwndbase.cpp \
    gui/apexmapwizard.cpp \
    gui/calibrationdialog.cpp \
    gui/autocalibrationdialog.cpp \
    gui/calibrationsetupsdialog.cpp \
    gui/centralwidget.cpp \
    gui/connectiondialog.cpp \
    gui/mainwindow.cpp \
    gui/mru.cpp \
    gui/panel.cpp \
    gui/panelelements.cpp \
    gui/r126settingsdialog.cpp \
    gui/ratiolayout.cpp \
    gui/startupdialog.cpp \
    screen/screenrundelegate.cpp \
    screen/screensparser.cpp \
    gui/plugindialog.cpp \
    interactive/parameterdialog.cpp \
    map/mapfactory.cpp \
    map/r126database.cpp \
    map/r126nucleusmaprecord.cpp \
    md5/md5.cpp \
    parameters/apexparametersparser.cpp \
    parameters/parametermanager.cpp \
    parser/apexparser.cpp \
    parser/prefixparser.cpp \
    parser/scriptexpander.cpp \
    parser/xmlpluginapi.cpp \
    parser/simpleparametersparser.cpp \
    procedure/adaptiveprocedure.cpp \
    procedure/adaptiveprocedureparser.cpp \
    procedure/apexadaptiveprocedure.cpp \
    procedure/apexadaptiveprocedureparametersparser.cpp \
    procedure/apexconstantprocedure.cpp \
    procedure/apexconstantprocedureparametersparser.cpp \
    procedure/apexmultiprocedure.cpp \
    procedure/apexmultiprocedureparametersparser.cpp \
    procedure/apexpluginprocedureparametersparser.cpp \
    procedure/apexprocedure.cpp \
    procedure/apexprocedureparametersparser.cpp \
    procedure/apextrainingprocedure.cpp \
    procedure/apextrainingprocedureparametersparser.cpp \
    procedure/apextrialfactory.cpp \
    procedure/constantprocedureparser.cpp \
    procedure/multiprocedureparser.cpp \
    procedure/pluginprocedureapi.cpp \
    procedure/pluginprocedure.cpp \
    procedure/pluginprocedureinterface.cpp \
    procedure/pluginprocedureparser.cpp \
    procedure/pluginprocedurewrapper.cpp \
    procedure/procedureparser.cpp \
    procedure/procedureparsersparent.cpp \
    procedure/trainingprocedureparser.cpp \
    randomgenerator/gaussianrandomgenerator.cpp \
    randomgenerator/randomgenerator.cpp \
    randomgenerator/randomgeneratorfactory.cpp \
    randomgenerator/randomgenerators.cpp \
    randomgenerator/uniformrandomgenerator.cpp \
    result/resultviewer.cpp \
    resultsink/apexresultsink.cpp \
    resultsink/rtresultsink.cpp \
    resultsink/trialresult.cpp \
    runner/experimentrundelegate.cpp \
    runner/experimentrunner.cpp \
    runner/simplerunner.cpp \
    runner/pluginrunner.cpp \
    screen/answerlabelrundelegate.cpp \
    screen/apexscreen.cpp \
    screen/apexscreenresult.cpp \
    screen/arclayoutrundelegate.cpp \
    screen/matrixrundelegate.cpp \
    screen/sliderrundelegate.cpp \
    screen/buttonrundelegate.cpp \
    screen/checkboxrundelegate.cpp \
    screen/gridlayoutrundelegate.cpp \
    screen/labelrundelegatebase.cpp \
    screen/labelrundelegate.cpp \
    screen/parameterlabelrundelegate.cpp \
    screen/parameterlistrundelegate.cpp \
    screen/parameterscontainerrundelegate.cpp \
    screen/picturelabelrundelegate.cpp \
    screen/picturerundelegate.cpp \
    screen/picturebuttonwidget.cpp \
    screen/rundelegatecreatorvisitor.cpp \
    screen/screenelementrundelegate.cpp \
    screen/spinboxrundelegate.cpp \
    screen/screenparser.cpp \
    screen/texteditrundelegate.cpp \
    services/application.cpp \
    services/filedialog.cpp \
    services/mainconfigfileparser.cpp \
    services/paths.cpp \
    services/servicemanager.cpp \
    services/pluginloader.cpp \
    services/errorhandler.cpp \
    stimulus/clarion/clariondatablock.cpp \
    stimulus/clarion/clariondevice.cpp \
    stimulus/clarion/clarionfilter.cpp \
    stimulus/datablock.cpp \
    stimulus/dummy/dummydevice.cpp \
    stimulus/l34/l34datablock.cpp \
    stimulus/l34/l34device.cpp \
    stimulus/l34/qicparser.cpp \
    stimulus/l34/aseqparser.cpp \
    stimulus/playmatrix.cpp \
    stimulus/stimuliparser.cpp \
    stimulus/stimulus.cpp \
    stimulus/stimulusoutput.cpp \
    stimulus/wav/bufferdropcallback.cpp \
    stimulus/wav/mixercallback.cpp \
    stimulus/wav/pluginfilter.cpp \
    stimulus/wav/pluginfilterprocessor.cpp \
    stimulus/wav/seqstream.cpp \
    stimulus/wav/soundcarddisplayer.cpp \
    stimulus/wav/streamgenerator.cpp \
    stimulus/wav/wavcompatibledatablock.cpp \
    stimulus/wav/wavdatablock.cpp \
    stimulus/wav/wavdevice.cpp \
    stimulus/wav/wavdeviceio.cpp \
    stimulus/wav/wavfader.cpp \
    stimulus/wav/wavfilter.cpp \
    timer/apextimer.cpp \
    feedback/feedback.cpp

# clebsCheckModules(pa5) {
# SOURCES += \
# device/pa5/pa5_actx.cpp \
# device/pa5/pa5device.cpp \
# }

win32:HEADERS += gui/flashwidget.h \
    screen/FlashPlayerRunDelegate.h

win32:SOURCES += mfcinit.cpp \
    screen/FlashPlayerRunDelegate.cpp \
    gui/flashwidget.cpp \

FORMS += gui/resultviewer.ui \
    gui/mapwizard.ui \
    gui/soundcarddialog.ui \
    gui/r216settings.ui \
    gui/calibrationdialog.ui \
    gui/autocalibrationdialog.ui \
    gui/calibrationsetupsdialog.ui \
    gui/connectiondialog.ui \
    gui/plugindialog.ui \
    gui/parameterdialog.ui \
    gui/startupdialog.ui \

unix { 
    unix:ctags.commands = echo !_TAG_FILE_FORMAT 2 dummy > $$BASEDIR/tags;
    unix:ctags.commands += echo !_TAG_FILE_SORTED 1 dummy >> $$BASEDIR/tags;
    unix:ctags.commands += sed -i "'s/ /\\t/g'" $$BASEDIR/tags;
    unix:ctags.commands += cd $$BASEDIR && ctags -R --c++-kinds=+p-n --fields=+iaS --extra=+fq --exclude=.build -f - src | sed "'s/apex:://g;s/\\tnamespace:apex//g'" | LC_ALL=C sort >> tags;
    win32:ctags.commands = cd $$BASEDIR && ctags -R --c++-kinds=+p-n --fields=+iaS --extra=+fq --exclude=.build src
    ctags.target = CTAGS
    QMAKE_EXTRA_TARGETS *= ctags
    ALL_DEPS *= CTAGS
}
