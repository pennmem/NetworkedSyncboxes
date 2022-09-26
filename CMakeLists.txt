cmake_minimum_required (VERSION 3.0.0)

##################
# Project settings

project (FreiburgNetworkSyncBox VERSION 0.1 LANGUAGES CXX)

######################
# Compilation settings

set(IS_RELEASE true)

if (IS_RELEASE)
  set(CMAKE_BUILD_TYPE Release)
  set(BUILD_TYPE "WIN32")
else (IS_RELEASE)
  set(CMAKE_BUILD_TYPE Debug)
  set(BUILD_TYPE "")
endif (IS_RELEASE)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_CURRENT_SOURCE_DIR}/dist)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_CURRENT_SOURCE_DIR}/dist)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/dist)

set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-error=unused -Wno-error=unused-parameter")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-variable")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-c-typedef-for-linkage")
else()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-but-set-variable")
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnull-dereference -Werror")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRC_HAVE_QT")
if(APPLE)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dunix=1")
endif(APPLE)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fno-omit-frame-pointer")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ffast-math -fno-finite-math-only")
set (FREIBURGNETWORKSYNCBOX_DEFINES)

##############
# Source files

add_executable (${PROJECT_NAME} "${BUILD_TYPE}"
  src/FreiburgNetworkSyncBox.cpp

  src/FtdCard.h
  src/FtdCard.cpp
  src/Config.h
  src/Config.cpp
  src/NetClient.h
  src/Utils.h
  src/Utils.cpp

  src/libusb.h
)

####################
# External libraries

if (WIN32)
  set (WSOCK_LIB wsock32 ws2_32)
else (WIN32)
  set (WSOCK_LIB "")
endif (WIN32)

find_package(PkgConfig REQUIRED)
    pkg_check_modules(libusb REQUIRED libusb-1.0)
find_library(LIBUSB_1_LIBRARY NAMES usb-1.0)

############################
# Set includes and libraries

target_include_directories (${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/src
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries (${PROJECT_NAME}
  ${WSOCK_LIB}
  ${LIBUSB_1_LIBRARY}
)

target_compile_definitions (${PROJECT_NAME} PUBLIC
  ${FREIBURGNETWORKSYNCBOX_DEFINES}
)


#############################################################
# Update build date-stamp in command-line help on each build.

add_custom_command (TARGET ${PROJECT_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/src/FreiburgNetworkSyncBox.cpp
)

