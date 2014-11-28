############################################################################
#  CMakeLists.txt
#
#  Base CMake file for building stxxl with different options.
#
#  Part of the STXXL. See http://stxxl.sourceforge.net
#
#  Copyright (C) 2013-2014 Timo Bingmann <tb@panthema.net>
#
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
############################################################################


cmake_minimum_required(VERSION 2.8.11)

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

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
#find_package(Qwt REQUIRED)

# Tell CMake to create the executable
add_executable(ssh-viewer sources/SHData.cpp  sources/SHData.h)



qt5_use_modules(ssh-viewer Widgets Sql Core)

# Use the Widgets module from Qt 5.
target_link_libraries(ssh-viewer Qt5::Widgets Qt5::Sql ${STXXL_LIBRARIES} )