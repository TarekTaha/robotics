cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

#
# project name
#
project( FaceRecognition )

#
# project version
#
set( VC_PROJECT_VERSION_MAJOR "1" )
set( VC_PROJECT_VERSION_MINOR "0" )
set( VC_PROJECT_VERSION_PATCH "0" )

set( VC_PROJECT_VERSION ${VC_PROJECT_VERSION_MAJOR}.${VC_PROJECT_VERSION_MINOR}.${VC_PROJECT_VERSION_PATCH} )

#
# Give feedback on custom entries
#
message( STATUS "Setting project name to ${PROJECT_NAME}" )
message( STATUS "Setting project version to ${VC_PROJECT_VERSION}" )

SET( FR_HDRS
    ./EHMMObjDatabase.h
    ./ImgObj.h
    ./RecognizeEHMM.h
    ./EHMMObjRecognition.h
    ./ImgObjDatabase.h
    ./EHMMObj.h
    ./Obj.h
    ./Utils.h
    ./EHMM.h
)

SET( FR_SRCS
    ./FaceRecognition.cpp
    ./EHMMObjRecognition.cpp
    ./ImgObjDatabase.cpp
    ./ImgObj.cpp
    ./EHMMObjDatabase.cpp
    ./EHMM.cpp
    ./Obj.cpp
    ./Utils.cpp
    ./EHMMObj.cpp
    ./RecognizeEHMM.cpp
)

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -g -O0 -o -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -o -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Optimization Level
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -march=pentium-m -msse2 -mfpmath=sse")

# enable warnings
ADD_DEFINITIONS( -Wall )

INCLUDE( ${CMAKE_ROOT}/Modules/FindPkgConfig.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/UsePkgConfig.cmake )

IF(PKG_CONFIG_FOUND)
	pkg_check_modules(OPENCV REQUIRED opencv)
ELSE(PKG_CONFIG_FOUND)
	MESSAGE("Cannot find pkg-config")
ENDIF(PKG_CONFIG_FOUND)

IF(OPENCV_FOUND)
	#do something ?
ELSE(OPENCV_FOUND)
	MESSAGE("Cannot find opencv version 2.1 or above")
ENDIF(OPENCV_FOUND)

FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
#FIND_PACKAGE(Tesseract COMPONENTS tesseract REQUIRED)

INCLUDE_DIRECTORIES( 
    ${CMAKE_BINARY_DIR}
    ${OPENCV_INCLUDE_DIRS}
    .
)



ADD_EXECUTABLE( faceRecognition ${FR_SRCS} ${FR_HDRS})

TARGET_LINK_LIBRARIES( faceRecognition
    ${OPENCV_LIBRARIES}
)
