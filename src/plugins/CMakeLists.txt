project(apex-plugins LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

add_library(apex-plugins-dataloopgenerator          SHARED dataloopgenerator/dataloopgenerator.cpp)
add_library(apex-plugins-vocoderfilter              SHARED vocoderfilter/vocoderfilter.cpp)
add_library(apex-plugins-spinplugin                 SHARED spinplugin/spin.cpp)
add_library(apex-plugins-threadedhrtffilter         SHARED threadedhrtffilter/threadedhrtffilter.cpp)
add_library(apex-plugins-hrtffilter                 SHARED hrtffilter/hrtffilter.cpp)
add_library(apex-plugins-networkdevicecontroller    SHARED networkdevicecontroller/networkdevicecontroller.cpp)
add_library(apex-plugins-protoslmplugin             SHARED protoslmplugin/protoslmplugin.cpp)
add_library(apex-plugins-delayfilter                SHARED delayfilter/delayfilter.cpp)
add_library(apex-plugins-dummyfeedbackplugin        SHARED dummyfeedbackplugin/dummyfeedbackplugin.cpp)
add_library(apex-plugins-sinkfilter                 SHARED sinkfilter/sinkfilter.cpp)
add_library(apex-plugins-beamformerfilter           SHARED beamformerfilter/beamformerfilter.cpp)
add_library(apex-plugins-adaptivewienerfilter       SHARED adaptivewienerfilter/adaptivewienerfilter.cpp)
add_library(apex-plugins-scramblespectrumfilter     SHARED scramblespectrumfilter/scramblespectrumfilter.cpp)
add_library(apex-plugins-democontroller             SHARED democontroller/democontroller.cpp)
add_library(apex-plugins-vadfilter                  SHARED vadfilter/vadfilter.cpp)
add_library(apex-plugins-noisegenerator             SHARED noisegenerator/noisegenerator.cpp)
add_library(apex-plugins-singlepulsegenerator       SHARED singlepulsegenerator/singlepulsegenerator.cpp)
add_library(apex-plugins-fadefilter                 SHARED fadefilter/fadefilter.cpp)
add_library(apex-plugins-emphasisfilter             SHARED emphasisfilter/emphasisfilter.cpp)
add_library(apex-plugins-sinegenerator              SHARED sinegenerator/sinegenerator.cpp)
add_library(apex-plugins-wienerfilter               SHARED wienerfilter/wienerfilter.cpp)
add_library(apex-plugins-dummyprocedure             SHARED dummyprocedure/dummyprocedure.cpp)
add_library(apex-plugins-amplifierfilter            SHARED amplifierfilter/amplifierfilter.cpp)
add_library(apex-plugins-studyplugin                SHARED studyplugin/studyrunner.cpp)
message("TODO: apex-plugins-matlabfilter")
message("TODO: apex-plugins-ledfeedbackplugin")
add_library(apex-plugins-osccontroller              SHARED osccontroller/osccontroller.cpp)

set(PLUGIN_LIBS
    apex-main
    apex-common
    apex-commongui
    apex-commonprotobuf
    apex-tools
    apex-data
    apex-spin
    bertha-lib-syllib
    Qt5::Network
    Qt5::Widgets
    Qt5::Script
    Qt5::ScriptTools
    ${LO_LIBRARIES}
    ${SNDFILE_LIBRARIES})

target_link_libraries(apex-plugins-dataloopgenerator        PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-vocoderfilter            PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-spinplugin               PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-threadedhrtffilter       PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-hrtffilter               PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-networkdevicecontroller  PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-protoslmplugin           PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-delayfilter              PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-dummyfeedbackplugin      PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-sinkfilter               PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-beamformerfilter         PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-adaptivewienerfilter     PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-scramblespectrumfilter   PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-democontroller           PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-vadfilter                PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-noisegenerator           PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-singlepulsegenerator     PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-fadefilter               PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-emphasisfilter           PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-sinegenerator            PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-wienerfilter             PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-dummyprocedure           PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-amplifierfilter          PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-studyplugin              PRIVATE ${PLUGIN_LIBS})
#target_link_libraries(apex-plugins-matlabfilter            PRIVATE ${PLUGIN_LIBS})
#target_link_libraries(apex-plugins-ledfeedbackplugin       PRIVATE ${PLUGIN_LIBS})
target_link_libraries(apex-plugins-osccontroller            PRIVATE ${PLUGIN_LIBS})

file(GLOB_RECURSE APEX_PROCEDURES_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "apexprocedures/*.cpp" "apexprocedures/*.h")

add_library(apex-plugins-apexprocedures SHARED ${APEX_PROCEDURES_SOURCES})

target_link_libraries(apex-plugins-apexprocedures PRIVATE ${PLUGIN_LIBS})

install(
    TARGETS
        apex-plugins-dataloopgenerator
        apex-plugins-vocoderfilter
        apex-plugins-spinplugin
        apex-plugins-threadedhrtffilter
        apex-plugins-hrtffilter
        apex-plugins-networkdevicecontroller
        apex-plugins-protoslmplugin
        apex-plugins-delayfilter
        apex-plugins-dummyfeedbackplugin
        apex-plugins-sinkfilter
        apex-plugins-beamformerfilter
        apex-plugins-adaptivewienerfilter
        apex-plugins-scramblespectrumfilter
        apex-plugins-democontroller
        apex-plugins-vadfilter
        apex-plugins-noisegenerator
        apex-plugins-singlepulsegenerator
        apex-plugins-fadefilter
        apex-plugins-emphasisfilter
        apex-plugins-sinegenerator
        apex-plugins-wienerfilter
        apex-plugins-dummyprocedure
        apex-plugins-amplifierfilter
        apex-plugins-studyplugin
    LIBRARY DESTINATION lib/apex)

