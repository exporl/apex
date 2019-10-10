project(coh LANGUAGES CXX)
enable_testing()

set(CMAKE_AUTOMOC ON)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS data/protocoh.proto)

# coh-lib
file(GLOB_RECURSE COH_LIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "lib/coh/*.cpp")

add_library(coh-lib SHARED
    ${COH_LIB_SOURCES}
    ${PROTO_HDRS}
    ${PROTO_SRCS})

target_include_directories(coh-lib
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR} # protobuf headers and soures are generated here
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries(coh-lib PRIVATE
    apex-common
    Qt5::Core
    Qt5::Xml
    protobuf::libprotobuf)

# coh-lib-tests
file(GLOB_RECURSE COH_LIB_TESTS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/coh/*.cpp")

add_executable(coh-lib-tests ${COH_LIB_TESTS_SOURCES})

target_link_libraries(coh-lib-tests PRIVATE
    coh-lib
    apex-common
    Qt5::Core
    Qt5::Test
)

add_test(NAME coh-lib-tests
    COMMAND $<TARGET_FILE:coh-lib-tests> -xunitxml -o "${CMAKE_SOURCE_DIR}/coh-lib-tests-results.xml"
    WORKING_DIRECTORY $<TARGET_FILE_DIR:coh-lib-tests>)

# coh-clients
message("TODO: coh-clients-nicsocketcohclient")
#add_library(coh-clients-nicsocketcohclient SHARED cohclients/nicsocketcohclient/nicsocketcohclient.cpp)
add_library(coh-clients-dumpcohclient       SHARED cohclients/dumpcohclient/dumpcohclient.cpp)
add_library(coh-clients-protoslavecohclient SHARED
    cohclients/protoslavecohclient/protoslavecohclient.cpp
    ${PROTO_HDRS} ${PROTO_SRCS})
message("TODO: coh-clients-niccohclient")
#add_library(coh-clients-niccohclient       SHARED cohclients/niccohclient/niccohclient.cpp)

set(COH_CLIENT_LIBS
    coh-lib
    apex-common
    apex-commonprotobuf
    Qt5::Core
    Qt5::Xml
    protobuf::libprotobuf
)

target_include_directories(coh-clients-protoslavecohclient PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}) # protobuf headers and soures are generated here

#target_link_libraries(coh-clients-nicsocketcohclient    PRIVATE ${COH_CLIENT_LIBS})
target_link_libraries(coh-clients-dumpcohclient         PRIVATE ${COH_CLIENT_LIBS})
target_link_libraries(coh-clients-protoslavecohclient   PRIVATE ${COH_CLIENT_LIBS})
#target_link_libraries(coh-clients-niccohclient          PRIVATE ${COH_CLIENT_LIBS})

# coh-programs
add_executable(coh-programs-cohexample      programs/cohexample/cohexample.cpp)
add_executable(coh-programs-protoslave
    programs/protoslave/protoslave.cpp
    ${PROTO_HDRS} ${PROTO_SRCS})
set_target_properties(coh-programs-protoslave PROPERTIES OUTPUT_NAME protoslave)
add_executable(coh-programs-cohxmltoproto   programs/cohxmltoproto/cohxmltoproto.cpp)
add_executable(coh-programs-dacspatch       programs/dacspatch/dacspatch.cpp)
message("TODO: coh-programs-nic3slave")
#add_executable(coh-programs-nic3slave      programs/nic3slave/nic3slave.cpp)
message("TODO: coh-programs-dacsstim")
#add_executable(coh-programs-dacsstim       programs/dacsstim/dacsstim.cpp)
message("TODO: coh-programs-nic3stream")
#add_executable(coh-programs-nic3stream     programs/nic3stream/nic3stream.cpp)

set(COH_PROGRAM_LIBS
    coh-lib
    apex-common
    apex-commonprotobuf
    Qt5::Core
    protobuf::libprotobuf
)

target_include_directories(coh-programs-protoslave  PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}) # protobuf headers and soures are generated here

target_link_libraries(coh-programs-cohexample       PRIVATE ${COH_PROGRAM_LIBS})
target_link_libraries(coh-programs-protoslave       PRIVATE ${COH_PROGRAM_LIBS})
target_link_libraries(coh-programs-cohxmltoproto    PRIVATE ${COH_PROGRAM_LIBS})
target_link_libraries(coh-programs-dacspatch        PRIVATE ${COH_PROGRAM_LIBS})
#target_link_libraries(coh-programs-nic3slave       PRIVATE ${COH_PROGRAM_LIBS})
#target_link_libraries(coh-programs-dacsstim        PRIVATE ${COH_PROGRAM_LIBS})
#target_link_libraries(coh-programs-nic3stream      PRIVATE ${COH_PROGRAM_LIBS})

install(
    TARGETS
        coh-clients-dumpcohclient
        coh-clients-protoslavecohclient
    LIBRARY DESTINATION lib/apex)

install(
    TARGETS
        coh-lib
        coh-lib-tests
        coh-programs-cohexample
        coh-programs-protoslave
        coh-programs-cohxmltoproto
        coh-programs-dacspatch
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

