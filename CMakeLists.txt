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

project(ssh-viewer)

# disallow in-source builds
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

# enable warnings (always good)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

# include the STXXL library in my-project
add_subdirectory(stxxl)

# apply STXXL CXXFLAGS to our configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")

# add STXXL includes path
include_directories(${STXXL_INCLUDE_DIRS})


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

MESSAGE( STATUS "CMAKE_MODULE_PATH: "  ${CMAKE_MODULE_PATH})

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)

#use our own mirror of a built qwt
set(QWT_INCLUDE_DIR qwt/include)
set(QWT_LIBRARY qwt/lib)
find_package(Qwt REQUIRED)
LINK_DIRECTORIES(qwt/lib) #somehow I need to explicitly set this

# Tell CMake to create the executable
add_executable(ssh-viewer sources/main.cpp sources/SHData.cpp sources/SHData.h)

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${QWT_INCLUDE_DIR})


qt5_use_modules(ssh-viewer Widgets Sql Core)

# Use the Widgets module from Qt 5.
MESSAGE( STATUS "QWT_FOUND: " ${QWT_FOUND})
MESSAGE( STATUS "QWT_LIBRARY: " ${QWT_LIBRARY})
MESSAGE( STATUS "QWT_LIBRARIES: " ${QWT_LIBRARIES})
MESSAGE( STATUS "QWT_INCLUDE_DIRS: " ${QWT_INCLUDE_DIRS})
MESSAGE( STATUS "STXXL_LIBRARIES: " ${STXXL_LIBRARIES})
MESSAGE( STATUS "INCLUDE_DIRECTORIES: " ${INCLUDE_DIRECTORIES})

target_link_libraries(ssh-viewer Qt5::Widgets Qt5::Sql -lqwt ${STXXL_LIBRARIES})