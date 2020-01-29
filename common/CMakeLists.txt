project(common LANGUAGES CXX)
enable_testing()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS data/protoslave.proto)

# apex-common
file(GLOB_RECURSE APEX_COMMON_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "lib/common/*.cpp")

add_library(apex-common SHARED ${APEX_COMMON_SOURCES})

target_include_directories(apex-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries(apex-common
    PRIVATE
        Qt5::Core
        Qt5::Xml
        Qt5::WebSockets
        Qt5::WebChannel
    PUBLIC
        ${SNDFILE_LIBRARIES})

# apex-commongui

add_library(apex-commongui SHARED
    lib/commongui/commongui.qrc
    lib/commongui/listpicker.cpp
    lib/commongui/webview.cpp
    lib/commongui/webviewwidget.cpp
    )

target_link_libraries(apex-commongui
    PUBLIC
    Qt5::Network
    Qt5::PrintSupport
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::WebView
    PRIVATE
    apex-common
    Qt5::Core
    Qt5::Widgets
    Qt5::Network)

# apex-commonprotobuf
file(GLOB_RECURSE APEX_COMMONPROTOBUF_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "lib/commonprotobuf/*.cpp")

add_library(apex-commonprotobuf SHARED
    ${APEX_COMMONPROTOBUF_SOURCES}
    ${PROTO_HDRS}
    ${PROTO_SRCS}
)

target_include_directories(apex-commonprotobuf
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR} # protobuf headers and soures are generated here
)

target_link_libraries(apex-commonprotobuf
    PRIVATE
        apex-common
        Qt5::Core
        Qt5::Network
        protobuf::libprotobuf
)

# testcolorizer
add_executable(apex-common-programs-testcolorizer
    programs/testcolorizer/testcolorizer.cpp)

target_link_libraries(apex-common-programs-testcolorizer PRIVATE
    Qt5::Core
    )

# apex-common-tests
file(GLOB_RECURSE APEX_COMMON_TESTS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/common/*.cpp")

add_executable(apex-common-tests ${APEX_COMMON_TESTS_SOURCES})

target_link_libraries(apex-common-tests PRIVATE
    apex-common
    Qt5::Core
    Qt5::Network
    Qt5::WebSockets
    Qt5::Test)

add_test(NAME apex-common-tests
    COMMAND $<TARGET_FILE:apex-common-tests> -xunitxml -o "${CMAKE_SOURCE_DIR}/apex-common-tests-results.xml"
    WORKING_DIRECTORY $<TARGET_FILE_DIR:apex-common-tests>)

# apex-commongui-tests
file(GLOB_RECURSE APEX_COMMONGUI_TESTS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/commongui/*.cpp")

add_executable(apex-commongui-tests
    ${APEX_COMMONGUI_TESTS_SOURCES}
    tests/commongui/commonguitest.h)

target_link_libraries(apex-commongui-tests PRIVATE
    apex-common
    apex-commongui
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Test)

# protobuf
protobuf_generate_python(PY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/data/protoslave.proto)
add_custom_target(copy_protobuf_python_generated_proto_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PY_SOURCES} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    DEPENDS ${PY_SOURCES})

add_test(NAME apex-commongui-tests
    COMMAND xvfb-run -s "-screen 0 1024x768x16" -a $<TARGET_FILE:apex-commongui-tests> -xunitxml -o "${CMAKE_SOURCE_DIR}/apex-commongui-tests-results.xml"
    WORKING_DIRECTORY $<TARGET_FILE_DIR:apex-commongui-tests>)

install(
    TARGETS
    apex-common
    apex-commongui
    apex-common-programs-testcolorizer
    apex-common-tests apex-commongui-tests
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)


