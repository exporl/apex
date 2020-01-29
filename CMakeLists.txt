cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)
enable_testing()

# make sure everything can be linked for all shared libraries
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,defs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")


SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY          ${CMAKE_HOME_DIRECTORY}/bin/$<LOWER_CASE:${CMAKE_BUILD_TYPE}>)
#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY          ${CMAKE_HOME_DIRECTORY}/bin/$<LOWER_CASE:${CMAKE_BUILD_TYPE}>)
#SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/release)
#SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG    ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/debug)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY          ${CMAKE_HOME_DIRECTORY}/bin/$<LOWER_CASE:${CMAKE_BUILD_TYPE}>)
#SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/release)
#SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/debug)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules
                      ${CMAKE_MODULE_PATH})

# Attempts to make the installed targets usable and relocatable, even when not installed
# in the usual install directories (e.g. /usr/local). But this might be a bad idea.
# Maybe it's the responsibility of the packager to set-up the right RPATH?
# By default the installed targets don't get an RPATH (qmake behaves the same).
# set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib")
# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_compile_definitions(ENABLE_COH)

project(apex)

find_package(FFTW3 REQUIRED)
find_package(FFTW3f REQUIRED)
find_package(Jack REQUIRED)
find_package(Libgit2 REQUIRED)
find_package(Liblo REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Portaudio REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Sndfile REQUIRED)
find_package(Qt5 COMPONENTS
    Core
    Multimedia
    Network
    PrintSupport
    Qml
    Quick
    QuickWidgets
    Script
    ScriptTools
    Svg
    Test
    WebSockets
    WebChannel
    WebView
    Widgets
    Xml
    XmlPatterns
    REQUIRED)

add_subdirectory(src)
add_subdirectory(bertha-lib)
add_subdirectory(coh)
add_subdirectory(common)
