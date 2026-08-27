#################################################
#
#  (C) 2026 Q4OS / TDE Developers
#  Improvements and feedback are welcome
#
#  This file is released under GPL >= 2
#
#################################################

cmake_minimum_required( VERSION 3.5 )

if( POLICY CMP0010 )
  cmake_policy( SET CMP0010 OLD )
endif()

if( POLICY CMP0012 )
  cmake_policy( SET CMP0012 NEW )
endif()

include( TDEVersion OPTIONAL )
if( COMMAND tde_set_project_version )
  tde_set_project_version()
endif()

project( tde-style-q4win10 )

#### include essential cmake modules

include( FindPkgConfig          )
include( CheckFunctionExists    )
include( CheckSymbolExists      )
include( CheckIncludeFile       )
include( CheckLibraryExists     )
include( CheckCSourceCompiles   )
include( CheckCXXSourceCompiles )

#### include our cmake modules

include( TDEMacros )

##### setup install paths

include( TDESetupPaths )
tde_setup_paths()

find_package( TQt )
find_package( TDE )

##### optional stuff

option( WITH_ALL_OPTIONS "Enable all optional support" OFF )
option( WITH_GCC_VISIBILITY "Enable fvisibility and fvisibility-inlines-hidden" ${WITH_ALL_OPTIONS} )
option( Q4WIN10_AGGRESSIVE_FLAGS "Enable aggressive -O2/LTO/gc-sections compile flags" ON )

##### configure checks

include( ConfigureChecks.cmake )

###### global compiler settings

add_definitions(
  -DHAVE_CONFIG_H
  -DQT_PLUGIN
  -D_DEFAULT_SOURCE
  -D_REENTRANT
  -DTQT_THREAD_SUPPORT
  -DNDEBUG
)

if( Q4WIN10_AGGRESSIVE_FLAGS )
  set( _Q4WIN10_C_FLAGS "-O2 -flto=auto -fstrict-aliasing -ffunction-sections -fdata-sections -fno-ident -fno-plt -fomit-frame-pointer -fno-stack-protector -fno-math-errno -fvisibility=hidden -fvisibility-inlines-hidden -fmerge-all-constants -fno-asynchronous-unwind-tables -DNDEBUG" )
  set( _Q4WIN10_CXX_EXTRA "-std=c++03 -fno-threadsafe-statics -fno-exceptions -fmerge-all-constants -Wno-deprecated-declarations -fpermissive" )
  set( CMAKE_C_FLAGS "${_Q4WIN10_C_FLAGS}" )
  set( CMAKE_CXX_FLAGS "${_Q4WIN10_C_FLAGS} ${_Q4WIN10_CXX_EXTRA} ${TQT_CXX_FLAGS} -DTQT_THREAD_SUPPORT" )
  set( CMAKE_SHARED_LINKER_FLAGS "-Wl,-O1 -Wl,-Bsymbolic -Wl,--gc-sections -Wl,--as-needed -Wl,--build-id=none -Wl,-z,norelro -flto=auto -s -Wl,--no-undefined" )
  set( CMAKE_MODULE_LINKER_FLAGS "-Wl,-O1 -Wl,-Bsymbolic -Wl,--gc-sections -Wl,--as-needed -Wl,--build-id=none -Wl,-z,norelro -flto=auto -s -Wl,--no-undefined" )
else()
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TQT_CXX_FLAGS} -DTQT_THREAD_SUPPORT" )
  set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
  set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined" )
endif()

##### directories ###############################

add_subdirectory( config )

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIBRARY_DIRS}
)

##### q4win10 (plugin) ##########################

tde_add_kpart( q4win10 AUTOMOC
  SOURCES
    q4win10style.cpp
  LINK
    tdecore-shared
    tdefx-shared
    X11
  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles
)

##### theme file ################################

install( FILES q4win10.themerc DESTINATION ${DATA_INSTALL_DIR}/tdestyle/themes )

##### write configure files #####################

configure_file( config.h.cmake config.h @ONLY )
