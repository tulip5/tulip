# QUAZIP_FOUND               - QuaZip library was found
# QUAZIP_INCLUDE_DIR         - Path to QuaZip include dir
# QUAZIP_INCLUDE_DIRS        - Path to QuaZip and zlib include dir (combined from QUAZIP_INCLUDE_DIR + ZLIB_INCLUDE_DIR)
# QUAZIP_LIBRARIES           - List of QuaZip libraries
# QUAZIP_ZLIB_INCLUDE_DIR    - The include dir of zlib headers

# Unset related CMake variables in order to change the lib version without having to delete the current CMake cache
UNSET(QUAZIP_FOUND CACHE)
UNSET(QUAZIP_LIBRARIES CACHE)
UNSET(QUAZIP_INCLUDE_DIR CACHE)
UNSET(QUAZIP_INCLUDE_DIRS CACHE)
UNSET(QUAZIP_ZLIB_INCLUDE_DIR CACHE)

IF (WIN32)
  FIND_PATH(QUAZIP_LIBRARY_DIR
    WIN32_DEBUG_POSTFIX d
    NAMES libquazip.dll
    HINTS "C:/Programme/" "C:/Program Files"
    PATH_SUFFIXES QuaZip/lib
  )
  FIND_LIBRARY(QUAZIP_LIBRARIES NAMES libquazip.dll HINTS ${QUAZIP_LIBRARY_DIR})
  FIND_PATH(QUAZIP_INCLUDE_DIR NAMES quazip.h HINTS ${QUAZIP_LIBRARY_DIR}/../ PATH_SUFFIXES include/quazip)
  FIND_PATH(QUAZIP_ZLIB_INCLUDE_DIR NAMES zlib.h)
ELSE(WIN32)  

  # special case when using Qt5 on Linux
  IF(USE_QT5)
    SET(QUAZIP_LIBRARY_NAME quazip5 quazip-qt5)
  ELSE(USE_QT5)
    SET(QUAZIP_LIBRARY_NAME quazip)
  ENDIF(USE_QT5)
  FIND_LIBRARY(QUAZIP_LIBRARIES
    NAMES ${QUAZIP_LIBRARY_NAME}
    HINTS /usr/lib /usr/lib64
  )
    SET(QUAZIP_PATH_SUFFIXES quazip)
    # special case when using Qt5 on Linux
    IF(USE_QT5)
      SET(QUAZIP_PATH_SUFFIXES ${QUAZIP_PATH_SUFFIXES} quazip5)
    ENDIF(USE_QT5)	
    FIND_PATH(QUAZIP_INCLUDE_DIR quazip.h
    HINTS /usr/include /usr/local/include
    PATH_SUFFIXES ${QUAZIP_PATH_SUFFIXES} 
  )
  FIND_PATH(QUAZIP_ZLIB_INCLUDE_DIR zlib.h HINTS /usr/include /usr/local/include)
ENDIF (WIN32)

INCLUDE(FindPackageHandleStandardArgs)
SET(QUAZIP_INCLUDE_DIRS ${QUAZIP_INCLUDE_DIR} ${QUAZIP_ZLIB_INCLUDE_DIR})
FIND_PACKAGE_HANDLE_STANDARD_ARGS(QUAZIP DEFAULT_MSG QUAZIP_LIBRARIES QUAZIP_INCLUDE_DIR QUAZIP_ZLIB_INCLUDE_DIR QUAZIP_INCLUDE_DIRS)
