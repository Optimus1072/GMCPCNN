cmake_minimum_required(VERSION 3.5)
project(GBMOT)

set(HOME_LOCAL /home/wrede/local)

link_directories(/usr/lib64)
include_directories(${HOME_LOCAL}/include)

set(SOURCE_FILES
        main/main.cpp
        core/DetectionSequence.cpp core/DetectionSequence.h
        core/ObjectData.cpp core/ObjectData.h
        core/ObjectDataMap.cpp core/ObjectDataMap.h
        core/Tracklet.cpp core/Tracklet.h
        util/FileIO.cpp util/FileIO.h
        util/Parser.cpp util/Parser.h
        algo/NStage.cpp algo/NStage.h
        util/MyMath.cpp util/MyMath.h
        core/ObjectData3D.cpp core/ObjectData3D.h
        util/Visualizer.cpp util/Visualizer.h
        util/Logger.cpp util/Logger.h
        core/ObjectDataAngular.cpp core/ObjectDataAngular.h
        algo/KShortestPaths.cpp algo/KShortestPaths.h
        graph/Definitions.h
        core/ObjectData2D.cpp core/ObjectData2D.h
        algo/Berclaz.cpp algo/Berclaz.h util/Grid.cpp util/Grid.h core/ObjectDataBox.cpp core/ObjectDataBox.h)

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

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED program_options)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(GBMOT ${SOURCE_FILES})

target_link_libraries(GBMOT ${OpenCV_LIBS} ${Boost_LIBRARIES})