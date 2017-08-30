cmake_minimum_required(VERSION 3.1)

project(hdf5Tools)


#-----------------------------------------------------------------------------
# Instructions for use : Normal Build
#
# For standard build of HDF5 examples.
#-----------------------------------------------------------------------------
set (CMAKE_NO_SYSTEM_FROM_IMPORTED 1)


#-----------------------------------------------------------------------------
# Allow Visual Studio solution directories
#-----------------------------------------------------------------------------
# Provide a way for Visual Studio Express users to turn OFF the new FOLDER
# organization feature. Default to ON for non-Express users. Express users must
# explicitly turn off this option to build HDF5 in the Express IDE...
#
option (HDF5_USE_FOLDERS "Enable folder grouping of projects in IDEs." ON)
mark_as_advanced (HDF5_USE_FOLDERS)
if (HDF5_USE_FOLDERS)
  set_property (GLOBAL PROPERTY USE_FOLDERS ON)
endif (HDF5_USE_FOLDERS)


#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (HDF5EX_RESOURCES_DIR      ${PROJECT_SOURCE_DIR}/config/cmake)

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
# include (${HDF5EX_RESOURCES_DIR}/HDFMacros.cmake)
# include (${HDF5EX_RESOURCES_DIR}/HDF5Macros.cmake)


#-----------------------------------------------------------------------------
# When building utility executables that generate other (source) files :
# we make use of the following variables defined in the root CMakeLists.
# Certain systems may add /Debug or /Release to output paths
# and we need to call the executable from inside the CMake configuration
#-----------------------------------------------------------------------------
set (EXE_EXT "")
if (WIN32)
  set (EXE_EXT ".exe")
  add_definitions (-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
  add_definitions (-D_CONSOLE)
endif (WIN32)

if (MSVC)
  set (CMAKE_MFC_FLAG 0)
endif (MSVC)

set (MAKE_SYSTEM)
if (CMAKE_BUILD_TOOL MATCHES "make")
  set (MAKE_SYSTEM 1)
endif (CMAKE_BUILD_TOOL MATCHES "make")

set (CFG_INIT "/${CMAKE_CFG_INTDIR}")
if (MAKE_SYSTEM)
  set (CFG_INIT "")
endif (MAKE_SYSTEM)

#-----------------------------------------------------------------------------
# Compiler specific flags : Shouldn't there be compiler tests for these
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  message(STATUS "Compiler is GNUCC.")
  set (CMAKE_C_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_C_FLAGS} -std=c99 -fomit-frame-pointer -finline-functions -fno-common")
endif (CMAKE_COMPILER_IS_GNUCC)
if (CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "Compiler is GNUCCXX.")
  set (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -fno-common")
endif (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
# set (CMAKE_MODULE_PATH ${HDF5EX_RESOURCES_DIR} ${CMAKE_MODULE_PATH})

find_package (HDF5 1.8.16)
if(HDF5_FOUND)
   	message("hdf5 found!")
	message(STATUS "HDF5_VERSION_STRING:"  ${HDF5_VERSION})
	message(STATUS "HDF5_INCLUDE_DIR:"  ${HDF5_INCLUDE_DIR})

    INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIRS})

    if (UNIX)
        set (LINK_LIBS ${LINK_LIBS} ${HDF5_C_LIBRARIES})
    endif (UNIX)

    set (LINK_LIBS ${LINK_LIBS} ${HDF5_C_STATIC_LIBRARY})
	message(STATUS "LINK_LIBS:"  ${LINK_LIBS})

    include_directories(${HDF5_INCLUDE_DIRS})

ELSE()
	message("hdf5 not found!")
ENDIF(HDF5_FOUND)

#-----------------------------------------------------------------------------
# Option to allow the user to disable compiler warnings
#-----------------------------------------------------------------------------
option (HDF_DISABLE_COMPILER_WARNINGS "Disable compiler warnings" OFF)
if (HDF_DISABLE_COMPILER_WARNINGS)
  # MSVC uses /w to suppress warnings.  It also complains if another
  # warning level is given, so remove it.
  if (MSVC)
	message(STATUS "MSVC")
    set (HDF_WARNINGS_BLOCKED 1)
    string (REGEX REPLACE "(^| )([/-])W[0-9]( |$)" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w")
    string (REGEX REPLACE "(^| )([/-])W[0-9]( |$)" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
  endif (MSVC)
  if (WIN32)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
  endif (WIN32)
  # Borland uses -w- to suppress warnings.
  if (BORLAND)
    set (HDF_WARNINGS_BLOCKED 1)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w-")
  endif (BORLAND)

  # Most compilers use -w to suppress warnings.
  if (NOT HDF_WARNINGS_BLOCKED)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
  endif (NOT HDF_WARNINGS_BLOCKED)
endif (HDF_DISABLE_COMPILER_WARNINGS)

#-----------------------------------------------------------------------------
# This is in here to help some of the GCC based IDES like Eclipse
# and code blocks parse the compiler errors and warnings better.
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCC)
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCXX)


add_subdirectory(src bin)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
