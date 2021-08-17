# https://gitlab.cern.ch/berkeleylab/labRemote/-/blob/master/cmake/Findlibftdi1.cmake
# - Try to find libftdi1
# Once done this will define
#  LIBFTDI1_FOUND - System has libftdi1
#  LIBFTDI1_INCLUDE_DIRS - The libftdi1 include directories
#  LIBFTDI1_LIBRARIES - The libraries needed to use libftdi1
#  LIBFTDI1_DEFINITIONS - Compiler switches required for using libftdi1

FIND_PATH(LIBFTDI1_INCLUDE_DIR ftdi.h
        HINTS /usr/include/libftdi1 /usr/local/include/libftdi1 )

FIND_LIBRARY(LIBFTDI1_LIBRARY NAMES ftdi1 libftdi1 libftdi
        HINTS /usr/lib64 /usr/local/lib /usr/lib/x86_64-linux-gnu )

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBFTDI1_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libftdi1  DEFAULT_MSG
        LIBFTDI1_LIBRARY LIBFTDI1_INCLUDE_DIR)

MARK_AS_ADVANCED(LIBFTDI1_INCLUDE_DIR LIBFTDI1_LIBRARY )

SET(LIBFTDI1_LIBRARIES ${LIBFTDI1_LIBRARY} )
SET(LIBFTDI1_INCLUDE_DIRS ${LIBFTDI1_INCLUDE_DIR} )
