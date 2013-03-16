# Install script for directory: D:/Code/dropecho/libtcod-1.5.1/src

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/libtcod")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "D:/Code/dropecho/libtcod-1.5.1/libtcod-mingw.dll")
FILE(INSTALL DESTINATION "D:/Code/dropecho/libtcod-1.5.1" TYPE SHARED_LIBRARY FILES "D:/Code/dropecho/libtcod-1.5.1/cmake/cbproj/src/libtcod-mingw.dll")
  IF(EXISTS "$ENV{DESTDIR}/D:/Code/dropecho/libtcod-1.5.1/libtcod-mingw.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/D:/Code/dropecho/libtcod-1.5.1/libtcod-mingw.dll")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "C:/MinGW/bin/strip.exe" "$ENV{DESTDIR}/D:/Code/dropecho/libtcod-1.5.1/libtcod-mingw.dll")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "D:/Code/dropecho/libtcod-1.5.1/libtcod-gui-mingw.dll")
FILE(INSTALL DESTINATION "D:/Code/dropecho/libtcod-1.5.1" TYPE SHARED_LIBRARY FILES "D:/Code/dropecho/libtcod-1.5.1/cmake/cbproj/src/libtcod-gui-mingw.dll")
  IF(EXISTS "$ENV{DESTDIR}/D:/Code/dropecho/libtcod-1.5.1/libtcod-gui-mingw.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/D:/Code/dropecho/libtcod-1.5.1/libtcod-gui-mingw.dll")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "C:/MinGW/bin/strip.exe" "$ENV{DESTDIR}/D:/Code/dropecho/libtcod-1.5.1/libtcod-gui-mingw.dll")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
