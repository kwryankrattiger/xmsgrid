# Import the XMS::Grid target
find_package(Boost QUIET REQUIRED log_setup log system filesystem serialization timer)
include(${CMAKE_CURRENT_LIST_DIR}/xmsgrid-targets.cmake)

# Get the root installation dir
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
  set(_IMPORT_PREFIX "")
endif()

include(GNUInstallDirs)

# Set the FindXMSCore variables
set(XMSGrid_FOUND ${XMSCore_FOUND})
set(XMSGrid_PREFIX ${CMAKE_CURRENT_LIST_DIR})
set(XMSGrid_LIBRARY_DIRS "${_IMPORT_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(XMSGrid_INCLUDE_DIRS "${_IMPORT_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
set(XMSGrid_LIBRARIES XMS::Grid;XMS::Core)
set(XMSGrid_VERSION 5.2.0)
