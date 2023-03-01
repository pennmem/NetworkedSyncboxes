cmake_minimum_required (VERSION 3.0.0)

##################
# Project settings

project (UPennNetworkSyncbox VERSION 0.1 LANGUAGES C CXX)

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
  #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-c-typedef-for-linkage")
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
set (NETWORKSYNCBOX_DEFINES)

##############
# Source files

add_executable (${PROJECT_NAME} "${BUILD_TYPE}"
  src/UPennNetworkSyncbox.cpp

  ../includes/Utils.cpp

  src/Config.cpp

  src/Plugin.cpp
  src/u3.c
  src/u3Extended.c

  #src/NetClient.h
  #src/Config.h
  #src/Utils.h
  #src/u3.h
  #src/u3Extended.h
  #src/libusb.h
  #src/labjackusb.h
)

####################
# External libraries

if (WIN32)
  set (WSOCK_LIB wsock32 ws2_32)
else (WIN32)
  set (WSOCK_LIB "")
endif (WIN32)

#find_package(PkgConfig REQUIRED)
#    pkg_check_modules(libusb REQUIRED libusb-1.0)
find_library(LIBUSB_LIBRARY NAMES usb-1.0)

# TODO: JPB: I'm not sure why find_package for liblabkjackusb doesn't work
#find_package(PkgConfig REQUIRED)
#    pkg_check_modules(liblabjackusb REQUIRED liblabjackusb-2.7.0)
find_library(LIBLABJACKUSB_LIBRARY NAMES labjackusb-2.7.0)

############################
# Set includes and libraries

target_include_directories (${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/../includes
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries (${PROJECT_NAME}
  ${WSOCK_LIB}
  ${LIBUSB_LIBRARY}
  ${LIBLABJACKUSB_LIBRARY}
)

target_compile_definitions (${PROJECT_NAME} PUBLIC
  ${NETWORKSYNCBOX_DEFINES}
)


#############################################################
# Update build date-stamp in command-line help on each build.

add_custom_command (TARGET ${PROJECT_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/src/UPennNetworkSyncbox.cpp
)

