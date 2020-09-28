cmake_minimum_required(VERSION 2.8)

project(stretchik)

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(PROJECT_NAME stretchIk)

set(SOURCE_FILES
    stretchik.h
    stretchik.cpp
    mainPlugin.cpp

)

set(LIBRARIES
      OpenMaya
      Foundation

)

build_plugin()
