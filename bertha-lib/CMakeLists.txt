project(bertha-lib LANGUAGES CXX)
enable_testing()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# blocks
add_library(bertha-lib-blocks-beamformerplugin              SHARED blocks/beamformerplugin/beamformerplugin.cpp)
add_library(bertha-lib-blocks-gainplugin                    SHARED blocks/gainplugin/gainplugin.cpp)
add_library(bertha-lib-blocks-olafftplugin                  SHARED blocks/olafftplugin/olafftplugin.cpp)
add_library(bertha-lib-blocks-firblockplugin                SHARED blocks/firblockplugin/firblockplugin.cpp)
message("TODO: bertha-lib-blocks-lslplugin")
#add_library(bertha-lib-blocks-lslplugin                     SHARED blocks/lslplugin/lslplugin.cpp)
add_library(bertha-lib-blocks-fftplugin                     SHARED blocks/fftplugin/fftplugin.cpp)
add_library(bertha-lib-blocks-amplifierplugin               SHARED blocks/amplifierplugin/amplifierplugin.cpp)
add_library(bertha-lib-blocks-compressorplugin              SHARED
    blocks/compressorplugin/compressorplugin.cpp
    blocks/compressorplugin/rmsfunction.cpp)
add_library(bertha-lib-blocks-delayplugin                   SHARED blocks/delayplugin/delayplugin.cpp)
add_library(bertha-lib-blocks-pulsegeneratorplugin          SHARED blocks/pulsegeneratorplugin/pulsegeneratorplugin.cpp)
add_library(bertha-lib-blocks-firfreqplugin                 SHARED blocks/firfreqplugin/firfreqplugin.cpp)
add_library(bertha-lib-blocks-mixerplugin                   SHARED blocks/mixerplugin/mixerplugin.cpp)
add_library(bertha-lib-blocks-fadeplugin                    SHARED blocks/fadeplugin/fadeplugin.cpp)
add_library(bertha-lib-blocks-emphasisplugin                SHARED blocks/emphasisplugin/emphasisplugin.cpp)
add_library(bertha-lib-blocks-threadedfirfilterplugin       SHARED blocks/threadedfirfilterplugin/threadedfirfilterplugin.cpp)
add_library(bertha-lib-blocks-vadplugin                     SHARED blocks/vadplugin/vadplugin.cpp)
add_library(bertha-lib-blocks-adaptivewienerfilterplugin    SHARED blocks/adaptivewienerfilterplugin/adaptivewienerfilterplugin.cpp)
add_library(bertha-lib-blocks-rectificationplugin           SHARED blocks/rectificationplugin/rectificationplugin.cpp)
add_library(bertha-lib-blocks-firfilterplugin               SHARED blocks/firfilterplugin/firfilterplugin.cpp)
add_library(bertha-lib-blocks-noisegeneratorplugin          SHARED
    blocks/noisegeneratorplugin/random.cpp
    blocks/noisegeneratorplugin/noisegeneratorplugin.cpp)
add_library(bertha-lib-blocks-typetestingplugin             SHARED blocks/typetestingplugin/typetestingplugin.cpp)
add_library(bertha-lib-blocks-summationplugin               SHARED blocks/summationplugin/summationplugin.cpp)
add_library(bertha-lib-blocks-wienerfilterplugin            SHARED blocks/wienerfilterplugin/wienerfilterplugin.cpp)
add_library(bertha-lib-blocks-filesourceplugin              SHARED blocks/filesourceplugin/filesourceplugin.cpp)
add_library(bertha-lib-blocks-sinegeneratorplugin           SHARED blocks/sinegeneratorplugin/sinegeneratorplugin.cpp)
add_library(bertha-lib-blocks-filesinkplugin                SHARED blocks/filesinkplugin/filesinkplugin.cpp)
add_library(bertha-lib-blocks-multiplierplugin              SHARED blocks/multiplierplugin/multiplierplugin.cpp)

set(BLOCK_LIBS
    apex-common
    bertha-lib
    bertha-lib-syllib
    Qt5::Core
    ${FFTW_LIBRARIES}
    ${FFTW3F_LIBRARIES})

target_link_libraries(bertha-lib-blocks-beamformerplugin            PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-gainplugin                  PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-olafftplugin                PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-firblockplugin              PRIVATE ${BLOCK_LIBS})
#target_link_libraries(bertha-lib-blocks-lslplugin                   PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-fftplugin                   PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-amplifierplugin             PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-compressorplugin            PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-delayplugin                 PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-pulsegeneratorplugin        PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-firfreqplugin               PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-mixerplugin                 PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-fadeplugin                  PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-emphasisplugin              PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-threadedfirfilterplugin     PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-vadplugin                   PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-adaptivewienerfilterplugin  PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-rectificationplugin         PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-firfilterplugin             PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-noisegeneratorplugin        PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-typetestingplugin           PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-summationplugin             PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-wienerfilterplugin          PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-filesourceplugin            PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-sinegeneratorplugin         PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-filesinkplugin              PRIVATE ${BLOCK_LIBS})
target_link_libraries(bertha-lib-blocks-multiplierplugin            PRIVATE ${BLOCK_LIBS})

# devices
message("TODO: bertha-lib-devices-asioplugin")
#add_library(bertha-lib-devices-asioplugin   SHARED devices/asioplugin/asioplugin.cpp)
add_library(bertha-lib-devices-fileplugin   SHARED devices/fileplugin/fileplugin.cpp)
add_library(bertha-lib-devices-filesplugin  SHARED devices/filesplugin/filesplugin.cpp)
add_library(bertha-lib-devices-jackplugin   SHARED devices/jackplugin/jackplugin.cpp)

set(DEVICE_LIBS
    apex-common
    bertha-lib
    Qt5::Core
    ${JACK_LIBRARIES})

#target_link_libraries(bertha-lib-devices-asioplugin     PRIVATE ${DEVICE_LIBS})
target_link_libraries(bertha-lib-devices-fileplugin     PRIVATE ${DEVICE_LIBS})
target_link_libraries(bertha-lib-devices-filesplugin    PRIVATE ${DEVICE_LIBS})
target_link_libraries(bertha-lib-devices-jackplugin     PRIVATE ${DEVICE_LIBS})

# programs
add_executable(bertha-lib-programs-audiometer
    programs/audiometer/main.cpp
    programs/audiometer/mainwindow.cpp)
add_executable(bertha-lib-programs-bertharunner
    programs/bertharunner/bertharunner.cpp)
add_executable(bertha-lib-programs-preferencerating
    programs/preferencerating/main.cpp
    programs/preferencerating/mainwindow.cpp)
add_executable(bertha-lib-programs-preferencerating_skeleton
    programs/preferencerating_skeleton/main.cpp
    programs/preferencerating_skeleton/preferencerating.cpp)
add_executable(bertha-lib-programs-ptcfast
    programs/ptcfast/main.cpp
    programs/ptcfast/mainwindow.cpp
    programs/ptcfast/conditionsparser.cpp)
add_executable(bertha-lib-programs-soundcardtest
    programs/soundcardtest/soundcardtest.cpp)
add_executable(bertha-lib-programs-visualizer
    programs/visualizer/main.cpp
    programs/visualizer/visualizer.cpp)

set(PROGRAM_LIBS
    apex-common
    bertha-lib
    Qt5::Core
    Qt5::Svg
    Qt5::Widgets
    Qt5::Xml
    )

target_link_libraries(bertha-lib-programs-audiometer                PRIVATE ${PROGRAM_LIBS})
target_link_libraries(bertha-lib-programs-bertharunner              PRIVATE ${PROGRAM_LIBS})
target_link_libraries(bertha-lib-programs-preferencerating          PRIVATE ${PROGRAM_LIBS})
target_link_libraries(bertha-lib-programs-preferencerating_skeleton PRIVATE ${PROGRAM_LIBS})
target_link_libraries(bertha-lib-programs-ptcfast                   PRIVATE ${PROGRAM_LIBS})
target_link_libraries(bertha-lib-programs-soundcardtest             PRIVATE ${PROGRAM_LIBS})
target_link_libraries(bertha-lib-programs-visualizer                PRIVATE ${PROGRAM_LIBS})

# bertha-lib
file(GLOB_RECURSE BERTHA_LIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "lib/bertha/*.cpp")

add_library(bertha-lib SHARED ${BERTHA_LIB_SOURCES})

target_include_directories(bertha-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries(bertha-lib PRIVATE
    apex-common
    Qt5::Core
    Qt5::Xml
    Qt5::XmlPatterns
    ${FFTW3F_LIBRARIES})

# bertha-lib-syllib
file(GLOB_RECURSE BERTHA_LIB_SYLLIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "lib/syllib/*.cpp")

add_library(bertha-lib-syllib SHARED ${BERTHA_LIB_SYLLIB_SOURCES})

target_include_directories(bertha-lib-syllib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries(bertha-lib-syllib PRIVATE
    ${FFTW_LIBRARIES}
    Qt5::Core)

# bertha-lib-tests
file(GLOB_RECURSE BERTHA_LIB_TESTS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/bertha/*.cpp")

set_property(SOURCE tests/bertha/compressortest.cpp PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE tests/bertha/olaffttest.cpp     PROPERTY SKIP_AUTOMOC ON)

add_executable(bertha-lib-tests ${BERTHA_LIB_TESTS_SOURCES})

target_link_libraries(bertha-lib-tests PRIVATE
    bertha-lib
    apex-common
    Qt5::Core
    Qt5::Test
    ${SNDFILE_LIBRARIES}
    ${FFTW3F_LIBRARIES})

add_test(NAME bertha-lib-tests
    COMMAND $<TARGET_FILE:bertha-lib-tests> -xunitxml -o "${CMAKE_SOURCE_DIR}/bertha-lib-tests-results.xml"
    WORKING_DIRECTORY $<TARGET_FILE_DIR:bertha-lib-tests>)

# bertha-lib-syllib-tests
file(GLOB_RECURSE BERTHA_SYLLIB_TESTS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/syllib/*.cpp")

add_executable(bertha-lib-syllib-tests ${BERTHA_SYLLIB_TESTS_SOURCES})

target_link_libraries(bertha-lib-syllib-tests PRIVATE
    bertha-lib-syllib
    apex-common
    Qt5::Core
    Qt5::Test
    )

add_test(NAME bertha-lib-syllib-tests
    COMMAND $<TARGET_FILE:bertha-lib-syllib-tests> -xunitxml -o "${CMAKE_SOURCE_DIR}/bertha-lib-syllib-tests-results.xml"
    WORKING_DIRECTORY $<TARGET_FILE_DIR:bertha-lib-syllib-tests>)

install(
    TARGETS
    bertha-lib-blocks-beamformerplugin
    bertha-lib-blocks-gainplugin
    bertha-lib-blocks-olafftplugin
    bertha-lib-blocks-firblockplugin
    bertha-lib-blocks-fftplugin
    bertha-lib-blocks-amplifierplugin
    bertha-lib-blocks-compressorplugin
    bertha-lib-blocks-delayplugin
    bertha-lib-blocks-pulsegeneratorplugin
    bertha-lib-blocks-firfreqplugin
    bertha-lib-blocks-mixerplugin
    bertha-lib-blocks-fadeplugin
    bertha-lib-blocks-emphasisplugin
    bertha-lib-blocks-threadedfirfilterplugin
    bertha-lib-blocks-vadplugin
    bertha-lib-blocks-adaptivewienerfilterplugin
    bertha-lib-blocks-rectificationplugin
    bertha-lib-blocks-firfilterplugin
    bertha-lib-blocks-noisegeneratorplugin
    bertha-lib-blocks-typetestingplugin
    bertha-lib-blocks-summationplugin
    bertha-lib-blocks-wienerfilterplugin
    bertha-lib-blocks-filesourceplugin
    bertha-lib-blocks-sinegeneratorplugin
    bertha-lib-blocks-filesinkplugin
    bertha-lib-blocks-multiplierplugin
    bertha-lib-devices-fileplugin
    bertha-lib-devices-filesplugin
    bertha-lib-devices-jackplugin
    RUNTIME DESTINATION lib/apex
    LIBRARY DESTINATION lib/apex)

install(
    TARGETS
    bertha-lib-programs-audiometer
    bertha-lib-programs-bertharunner
    bertha-lib-programs-preferencerating
    bertha-lib-programs-preferencerating_skeleton
    bertha-lib-programs-ptcfast
    bertha-lib-programs-soundcardtest
    bertha-lib-programs-visualizer
    bertha-lib
    bertha-lib-syllib
    bertha-lib-tests
    bertha-lib-syllib-tests
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
