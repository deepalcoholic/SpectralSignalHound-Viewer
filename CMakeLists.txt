############################################################################
# For use in the SpectralSignalHound-Viewer project
# 
# Copyright (c) 2014, Nick Potts
# All rights reserved.
# 
#  This Source Code Form is subject to the terms of the 
#  Mozilla Public License, v. 2.0. If a copy of the MPL 
#  was not distributed with this file, You can obtain 
#  one at http://mozilla.org/MPL/2.0/.
# 
############################################################################

cmake_minimum_required(VERSION 2.8.11)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON) # Instruct CMake to run uic on ui files automatically when needed.
set(CMAKE_AUTORCC ON) #same, but for RCC files
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall") # enable warnings (always good)

project(ssh-viewer)

#add_subdirectory(stxxl) # include the STXXL library in my-project
#include_directories(${STXXL_INCLUDE_DIRS}) # add STXXL includes path
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}") # apply STXXL CXXFLAGS to our configuration

#use our own mirror of a built qwt
set(QWT_INCLUDE_DIR qwt/include)
set(QWT_LIBRARY qwt/lib)
find_package(Qwt REQUIRED) #get Qwt
find_package(Qt5Widgets REQUIRED) # Find the QtWidgets library
find_package(Qt5Sql REQUIRED)

LINK_DIRECTORIES(qwt/lib) #somehow I need to explicitly set this

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${QWT_INCLUDE_DIR})

SET(SRC
	sources/SHData.cpp sources/SHData.h
	sources/helpers.cpp sources/helpers.h
	sources/SweepInspector.cpp sources/SweepInspector.h
	sources/SpectralInspector.cpp sources/SpectralInspector.h
	)

SET(SWEEP_SRC sources/main-sweep.cpp)
SET(SPECT_SRC sources/main-spect.cpp)

# Use the Widgets module from Qt 5.
MESSAGE( STATUS "QWT_FOUND: " ${QWT_FOUND})
MESSAGE( STATUS "QWT_LIBRARY: " ${QWT_LIBRARY})
MESSAGE( STATUS "QWT_LIBRARIES: " ${QWT_LIBRARIES})
MESSAGE( STATUS "QWT_INCLUDE_DIRS: " ${QWT_INCLUDE_DIRS})
MESSAGE( STATUS "STXXL_LIBRARIES: " ${STXXL_LIBRARIES})
MESSAGE( STATUS "INCLUDE_DIRECTORIES: " ${INCLUDE_DIRECTORIES})
MESSAGE( STATUS "Sources: " ${SRC})


# Tell CMake to create the executable
add_executable(ssh-sweep-viewer ${SWEEP_SRC} ${SRC} ${UIS_HDRS})
qt5_use_modules(ssh-sweep-viewer Widgets Sql Core)
target_link_libraries(ssh-sweep-viewer Qt5::Widgets Qt5::Sql -lqwt)
#target_link_libraries(ssh-sweep-viewer Qt5::Widgets Qt5::Sql -lqwt ${STXXL_LIBRARIES})

#add_executable(ssh-spectrviewer ${SPECT_SRC} ${SRC} ${UIS_HDRS})
#qt5_use_modules(ssh-viewer Widgets Sql Core)
#target_link_libraries(ssh-viewer Qt5::Widgets Qt5::Sql -lqwt ${STXXL_LIBRARIES})