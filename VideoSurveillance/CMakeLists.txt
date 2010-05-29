# set project's name
PROJECT( OpenCVSurveillance )

cmake_minimum_required(VERSION 2.4)


SET( OpenCVSurveillance_SRCS
    ./src/main.cpp
)

# CDT Error Parser cannot handle error messages that span more than one line
# which is the default gcc behavior. This is used to force gcc to generate single
# line error messages with no line wrapping
IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -g -O0 -o -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -o -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Optimization Level
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -msse2 -mfpmath=sse")

# enable warnings
ADD_DEFINITIONS( -Wall )

##################
#  OpenCV STUFF  #
##################
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
	MESSAGE("Cannot find OPENCV version 1.1 or above")
ENDIF(OPENCV_FOUND)

INCLUDE_DIRECTORIES(
    ${OPENCV_INCLUDE_DIRS}    
    ./src
)

# here we instruct CMake to build "testPanTilt" executable from all of the source files
ADD_EXECUTABLE( ../bin/opencvSurrveillance ${OpenCVSurveillance_SRCS} )
MESSAGE("${OPENCV_LIBRARIES}")
TARGET_LINK_LIBRARIES( ../bin/opencvSurrveillance 
    ${OPENCV_LIBRARIES}
)

add_dependencies(../bin/opencvSurrveillance cv cxcore highgui ml cvaux)
