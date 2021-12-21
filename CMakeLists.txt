cmake_minimum_required(VERSION 3.1.2)
cmake_policy(SET CMP0015 NEW) # Link Directory Pathing
set(CMAKE_DEBUG_POSTFIX _d)


if (APPLE OR WIN32)
    # Target C++17 on MacOS and Windows. If building with version of Visual Studio without
    # support, has no effect.
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()
message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

if (APPLE)
  set(CMAKE_POSITION_INDEPENDENT_CODE FALSE)
else ()
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif ()

project(xmsgrid C CXX)

include(GNUInstallDirs)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Enable/Disable building shared libraries")
set(BUILD_TESTING OFF CACHE BOOL "Enable/Disable testing")
set(XMS_BUILD_MODE "CONAN" CACHE STRING "Target packing system to build for.")
set_property(CACHE XMS_BUILD_MODE PROPERTY STRINGS "CONAN;CONDA;CMAKE")
set(XMS_BUILD ON CACHE BOOL "Set this if you want to use this package with XMS.")
set(CONDA_PREFIX "" CACHE PATH "Path to the conda environment used to build.")
set(ENABLE_PYTHON OFF CACHE BOOL "Set this if you want to build the python bindings.")
set(PYTHON_TARGET_VERSION 3.6 CACHE STRING "Version of python to link to for python wrapping.")
set(XMS_TEST_PATH ${PROJECT_SOURCE_DIR}/test_files/ CACHE PATH "Path to test files for testing")
set(XMS_VERSION "5.2.0" CACHE STRING "Library Version")

add_definitions(-DXMS_VERSION=\"${XMS_VERSION}\")

# Do this because it doesn't work otherwise...
if (BUILD_SHARED_LIBS AND WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

if (XMS_BUILD_MODE STREQUAL "CMAKE")
  if (PYTHON_TARGET_VERSION VERSION_LESS 3)
    message(FATAL_ERROR "XMS requires Python >= 3.0")
  endif ()
  if (BUILD_SHARED_LIBS)
    set(Boost_USE_STATIC_LIBS OFF)
  else ()
    set(Boost_USE_STATIC_LIBS ON)
  endif ()
  find_package(Boost QUIET REQUIRED log_setup log system filesystem serialization timer)
  find_package(XMSCore REQUIRED
    NAMES xms xmscore)
  find_package(ZLIB QUIET REQUIRED)
  list(APPEND EXT_INCLUDE_DIRS ${XMSCore_INCLUDE_DIRS};${Boost_INCLUDE_DIRS};${ZLIB_INCLUDE_DIRS})
  list(APPEND EXT_LIB_DIRS ${XMSCore_LIBRARY_DIRS};${Boost_LIBRARY_DIRS})
  list(APPEND EXT_LIBS ${XMSCore_LIBRARIES};${Boost_LIBRARIES};${ZLIB_LIBRARIES})
  if (WIN32)
    if (XMS_BUILD)
      add_definitions(/Zc:wchar_t)  # Treat wchar_t as built-in type
      add_definitions(/D _WIN32_WINNT=0x0501)  # Windows XP and higher
    endif ()
    add_definitions(/D BOOST_ALL_NO_LIB)
  endif ()
elseif (XMS_BUILD_MODE STREQUAL "CONDA")
  include(${CMAKE_CURRENT_LIST_DIR}/condabuildinfo.cmake)
else () # If we are using conan
  # Conan setup
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
  list(APPEND EXT_INCLUDE_DIRS ${CONAN_INCLUDE_DIRS})
  list(APPEND EXT_LIB_DIRS ${CONAN_LIB_DIRS})
  list(APPEND EXT_LIBS ${CONAN_LIBS})
endif ()

if (WIN32 AND XMS_BUILD_MODE STREQUAL "CONAN")
    string(COMPARE EQUAL "${CONAN_SETTINGS_COMPILER_RUNTIME}" "MT" USES_MT)
    if (NOT USES_MT)
        string(COMPARE EQUAL "${CONAN_SETTINGS_COMPILER_RUNTIME}" "MTd" USES_MT)
    endif ()

    if (USES_MT)
        message("Treating wchar_t as a built-in type.")
        add_definitions(/Zc:wchar_t)  # Treat wchar_t as built-in type
    else ()
        message("Treating wchar_t as a typedef.")
        add_definitions(/Zc:wchar_t-)  # Treat wchar_t as typedef
    endif ()

    if (XMS_BUILD)
        add_definitions(/D _WIN32_WINNT=0x0501)  # Windows XP and higher
    else (NOT XMS_BUILD)
        add_definitions(/D BOOST_ALL_NO_LIB)
    endif ()
endif ()

if (ENABLE_PYTHON)

    # Pybind11 module
    if (NOT XMS_BUILD_MODE STREQUAL "CMAKE")
        if (XMS_BUILD_MODE STREQUAL "CONDA")
            include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Targets.cmake")
            include("${CONDA_PREFIX}/share/cmake/pybind11/FindPythonLibsNew.cmake")
            include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Config.cmake")
            include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11ConfigVersion.cmake")
            include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Tools.cmake")
        elseif (XMS_BUILD_MODE STREQUAL "CONAN")
            include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Targets.cmake")
            include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/FindPythonLibsNew.cmake")
            include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Config.cmake")
            include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11ConfigVersion.cmake")
            include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Tools.cmake")
        endif ()

        find_package(PythonLibsNew ${PYTHON_TARGET_VERSION} EXACT REQUIRED)

        list(APPEND EXT_INCLUDE_DIRS
            ${pybind11_INCLUDE_DIR}
        )

        message("PYTHON_INCLUDE: ${PYTHON_INCLUDE_DIRS}")
        message("PYTHON_LIBS: ${PYTHON_LIBRARIES}")

        list(APPEND EXT_INCLUDE_DIRS
          ${PYTHON_INCLUDE_DIRS}
        )
    else ()
      # New method does not require PythonLibsNew
      find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
      find_package(pybind11 CONFIG REQUIRED)
      list(APPEND EXT_LIBS pybind11::pybind11 Python3::Python)
    endif ()
endif ()

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${EXT_INCLUDE_DIRS})
link_directories(${EXT_LIB_DIRS})

# Static library sources
set(xmsgrid_sources
  # Geometry
  xmsgrid/geometry/geoms.cpp
  xmsgrid/geometry/GmExtents.cpp
  xmsgrid/geometry/GmMultiPolyIntersectionSorterTerse.cpp
  xmsgrid/geometry/GmMultiPolyIntersector.cpp
  xmsgrid/geometry/GmPolygon.cpp
  xmsgrid/geometry/GmPolyLinePtRedistributer.cpp
  xmsgrid/geometry/GmPtSearch.cpp
  xmsgrid/geometry/GmTriSearch.cpp
  # Matrices
  xmsgrid/matrices/matrix.cpp
  # Triangulate
  xmsgrid/triangulate/TrBreaklineAdder.cpp
  xmsgrid/triangulate/triangles.cpp
  xmsgrid/triangulate/TrTin.cpp
  xmsgrid/triangulate/TrTriangulator.cpp
  xmsgrid/triangulate/TrTriangulatorPoints.cpp
  xmsgrid/triangulate/detail/TrAutoFixFourTrianglePts.cpp
  xmsgrid/triangulate/detail/triangulate.cpp
  xmsgrid/triangulate/detail/TrOuterTriangleDeleter.cpp
  # UGrid
  xmsgrid/ugrid/XmEdge.cpp
  xmsgrid/ugrid/XmUGrid.cpp
  xmsgrid/ugrid/XmUGridUtils.cpp
  xmsgrid/ugrid/detail/XmGeometry.cpp
)

set(xmsgrid_headers
  # Geometry
  xmsgrid/geometry/geoms.h
  xmsgrid/geometry/GmBoostTypes.h
  xmsgrid/geometry/GmExtents.h
  xmsgrid/geometry/GmMultiPolyIntersectionSorter.h
  xmsgrid/geometry/GmMultiPolyIntersectionSorterTerse.h
  xmsgrid/geometry/GmMultiPolyIntersector.h
  xmsgrid/geometry/GmMultiPolyIntersectorData.h
  xmsgrid/geometry/GmPolygon.h
  xmsgrid/geometry/GmPolyLinePtRedistributer.h
  xmsgrid/geometry/GmPtSearch.h
  xmsgrid/geometry/GmTriSearch.h
  # Matrices
  xmsgrid/matrices/matrix.h
  # Triangulate
  xmsgrid/triangulate/TrBreaklineAdder.h
  xmsgrid/triangulate/triangles.h
  xmsgrid/triangulate/TrTin.h
  xmsgrid/triangulate/TrTriangulator.h
  xmsgrid/triangulate/TrTriangulatorPoints.h
  xmsgrid/triangulate/detail/TrAutoFixFourTrianglePts.h
  xmsgrid/triangulate/detail/triangulate.h
  xmsgrid/triangulate/detail/TrOuterTriangleDeleter.h
  # Ugrid
  xmsgrid/ugrid/XmEdge.h
  xmsgrid/ugrid/XmUGrid.h
  xmsgrid/ugrid/XmUGridUtils.h
  xmsgrid/ugrid/detail/XmGeometry.h
)

# Pybind11 sources
set(xmsgrid_py
  xmsgrid/python/xmsgrid_py.cpp
  # Geometry
  xmsgrid/python/geometry/geometry_py.cpp
  xmsgrid/python/geometry/GmTriSearch_py.cpp
  # Triangulate
  xmsgrid/python/triangulate/triangulate_py.cpp
  xmsgrid/python/triangulate/TrTin_py.cpp
  #UGrid
  xmsgrid/python/ugrid/ugrid_py.cpp
  xmsgrid/python/ugrid/XmUGrid_py.cpp
  xmsgrid/python/ugrid/XmUGridUtils_py.cpp
)

set(xmsgrid_py_headers
  # Geometry
  xmsgrid/python/geometry/geometry_py.h
  # Triangulate
  xmsgrid/python/triangulate/triangulate_py.h
  xmsgrid/python/ugrid/ugrid_py.h
)

# Tests
if (BUILD_TESTING)
 add_definitions(-DXMS_TEST_PATH="${XMS_TEST_PATH}/")
 add_definitions(-DCXX_TEST -DCXXTEST4)

 list(APPEND xmsgrid_sources
   # Geometry
   xmsgrid/geometry/geoms.t.h
   xmsgrid/geometry/GmExtents.t.h
   xmsgrid/geometry/GmMultiPolyIntersector.t.h
   xmsgrid/geometry/GmPolygon.t.h
   xmsgrid/geometry/GmPolyLinePtRedistributer.t.h
   xmsgrid/geometry/GmPtSearch.t.h
   xmsgrid/geometry/GmTriSearch.t.h
   # Triangulate
   xmsgrid/triangulate/TrBreaklineAdder.t.h
   xmsgrid/triangulate/TrTin.t.h
   xmsgrid/triangulate/TrTriangulatorPoints.t.h
   xmsgrid/triangulate/detail/TrAutoFixFourTrianglePts.t.h
   xmsgrid/triangulate/detail/TrOuterTriangleDeleter.t.h
   # UGrid
   xmsgrid/ugrid/XmEdge.t.h
   xmsgrid/ugrid/XmUGrid.t.h
   xmsgrid/ugrid/XmUGridUtils.t.h
 )

 find_package(CxxTest)
 if (CXXTEST_FOUND)
   include_directories(${CXXTEST_INCLUDE_DIR})
   enable_testing()

   set(CXXTEST_TESTGEN_ARGS --xunit-printer --have-eh)
   file(GLOB_RECURSE test_headers ${CMAKE_CURRENT_LIST_DIR}/xmsgrid/*.t.h)
   CXXTEST_ADD_TEST(
     runner runner.cpp ${test_headers}
   )
   target_link_libraries(runner ${PROJECT_NAME})
 endif ()
endif ()

# Static library
add_library(${PROJECT_NAME}
  ${xmsgrid_sources} ${xmsgrid_headers}
)
set_property(TARGET ${PROJECT_NAME} PROPERTY EXPORT_NAME Grid)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/>
    ${Boost_INCLUDE_DIR}
 )
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}
  ${EXT_LIBS}
)
target_link_libraries(${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
)
if (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} rt)
endif ()

if (ENABLE_PYTHON)
    pybind11_add_module(_xmsgrid
      ${xmsgrid_py} ${xmsgrid_py_headers}
    )
    target_include_directories(_xmsgrid
        PRIVATE
          ${EXT_LIBS}
          ${PYTHON_INCLUDE_DIRS}
    )
    target_link_libraries(_xmsgrid
      PRIVATE
        ${EXT_LIBS}
        ${PROJECT_NAME}
    )
    set_target_properties(_xmsgrid PROPERTIES
      LINKER_LANGUAGE CXX
    )

    if (XMS_BUILD_MODE STREQUAL "CMAKE")
      # Using a standard installation heirarchy for python packages
      set(XMS_PYTHON_INSTALL_PREFIX "${CMAKE_INSTALL_LIBDIR}/python-${PYTHON_TARGET_VERSION}/site-packages")
      # Skip installing setup.py
      set(XMS_PYTHON_INSTALL_PACKAGE_EXTRA
        PATTERN "setup.py" EXCLUDE)
    else ()
      set(XMS_PYTHON_INSTALL_PREFIX "_package")
    endif ()
    # Install recipe
    install(
      TARGETS _xmsgrid
      ARCHIVE DESTINATION "${XMS_PYTHON_INSTALL_PREFIX}/xms/grid"
      LIBRARY DESTINATION "${XMS_PYTHON_INSTALL_PREFIX}/xms/grid"
    )
    install(
      DIRECTORY "_package/"
      DESTINATION "${XMS_PYTHON_INSTALL_PREFIX}"
      FILES_MATCHING PATTERN "*.py"
      PATTERN "_package/tests" EXCLUDE
      ${XMS_PYTHON_INSTALL_PACKAGE_EXTRA})
endif ()

# Disable Optimization for GCC6 for now... Report issue to gcc and pybind
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND ENABLE_PYTHON)
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
    Message("Disabling Optimization for GCC 6...")
    STRING(REPLACE "-O3" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    STRING(REPLACE "-O3" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
  endif ()
endif ()

# Use C++14 if VS2015 or later
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19)
    message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
endif ()

# Install recipe
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

foreach (header IN LISTS xmsgrid_headers xmsgrid_py_headers)
  get_filename_component(subdir "${header}" DIRECTORY)
  install(
    FILES "${header}"
    DESTINATION "include/${subdir}"
  )
endforeach ()

# Install the CMake configs
install(EXPORT ${PROJECT_NAME}
  FILE xmsgrid-targets.cmake
  NAMESPACE XMS::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/xms")

install(
  FILES CMake/xmsgrid-config.cmake
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/xms")
