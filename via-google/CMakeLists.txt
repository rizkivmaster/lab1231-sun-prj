cmake_minimum_required(VERSION 2.8)
project( Shotton.2009 )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")# Activate C++11

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_FLAGS "-Wall")


find_package( OpenCV REQUIRED )
find_package( Boost REQUIRED )
find_package( Qt4 REQUIRED)
find_package( TBB )


#
set(root ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(ExternalLibrary ${root}/external)
set(EigenIncludeDir /usr/include/eigen3)
set(OpenGMDir ${ExternalLibrary}/opengm)
set(ProbImageDir ${ExternalLibrary}/prob-image)
set(SaliencySource ${ExternalLibrary}/saliencyfilters)

#
include_directories (${OpenGMDir}/include)
include_directories (${OpenGMDir}/src/external/MaxFlow-v3.02.src-patched)
include_directories (${OpenGMDir}/src/external/QPBO-v1.3.src-patched)
include_directories (${ProbImageDir})
include_directories (../util/include)

include_directories (${EigenIncludeDir})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories (${ExternalLibrary})
include_directories(${QT_INCLUDES} external/include)

add_library( 
  probimage 
  ${ProbImageDir}/probimage.cpp 
)

add_library (
  util
  ${root}/util/src/util.cc
)

add_library (
  color_map
  ${root}/util/src/color_map.cc
)

target_link_libraries ( util color_map  )

#SaliencyFilters library
include_directories(${SaliencySource}/src)
set(saliency_libs 
${SaliencySource}/build/src/saliency/libsaliency.a
${SaliencySource}/build/src/filter/libfilter.a
${SaliencySource}/build/src/superpixel/libsuperpixel.a)

add_subdirectory( src )