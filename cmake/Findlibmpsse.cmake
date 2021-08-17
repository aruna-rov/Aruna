# - Try to find libmpsse
# Once done this will define
#  LIBMPSSE_FOUND - System has libmpsse
#  LIBMPSSE_INCLUDE_DIR - The libmpsse include directories
#  LIBMPSSE_LIBRARIES - The libraries needed to use libmpsse
#  LIBMPSSE_DEFINITIONS - Compiler switches required for using libmpsse

FIND_PATH(LIBMPSSE_INCLUDE_DIR mpsse.h
          HINTS /usr/include /usr/local/include)

FIND_LIBRARY(LIBMPSSE_LIBRARY NAMES mpsse libmpsse
              HINTS /usr/lib64 /usr/local/lib)

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBMPSSE_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libmpsse  DEFAULT_MSG
  LIBMPSSE_LIBRARY LIBMPSSE_INCLUDE_DIR)

MARK_AS_ADVANCED(LIBMPSSE_INCLUDE_DIR LIBMPSSE_LIBRARY )

SET(LIBMPSSE_LIBRARIES ${LIBMPSSE_LIBRARY} )
SET(LIBMPSSE_INCLUDE_DIRS ${LIBMPSSE_INCLUDE_DIR} )
