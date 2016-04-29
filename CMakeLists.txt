cmake_minimum_required(VERSION 3.5)
project(GBMOT)

set(HOME_LOCAL /home/wrede/local)

include_directories(${HOME_LOCAL}/include)

set(SOURCE_FILES
        main/main.cpp
        core/DetectionSequence.cpp
        core/DetectionSequence.h
        core/ObjectData.cpp
        core/ObjectData.h
        core/ObjectDataMap.cpp
        core/ObjectDataMap.h
        core/Definitions.h
        core/Tracklet.cpp
        core/Tracklet.h
        util/FileIO.cpp
        util/FileIO.h
        util/Parser.cpp
        util/Parser.h
        algo/TwoStage.cpp
        algo/TwoStage.h
        algo/Definitions.h)

find_package(Doxygen)

if(DOXYGEN_FOUND)

    set(DOXYGEN_INPUT Doxyfile)
    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${DOXYGEN_INPUT})

    add_custom_target(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

endif(DOXYGEN_FOUND)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(GBMOT ${SOURCE_FILES})