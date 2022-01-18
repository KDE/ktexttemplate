
cmake_minimum_required(VERSION 3.5)

if (POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

project(Grantlee5 VERSION 5.2.0)

# Workaround for http://public.kitware.com/Bug/view.php?id=12301
if (MINGW)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build,
  options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
  RelWithDebInfo MinSizeRel Coverage." FORCE)
  endif()
endif()

option( BUILD_TESTING "Build automated tests." TRUE )
option( GRANTLEE_BUILD_WITH_QT6 "Build Grantlee with Qt 6" FALSE)

if (BUILD_TESTING)
  include (CTest)
  enable_testing()
endif()

set(Grantlee_MODULE_DIR ${CMAKE_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${Grantlee_MODULE_DIR})

include(GenerateExportHeader)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  # create coverage build type
  set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} Coverage )
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                "Debug" "Release" "MinSizeRel" "RelWithDebInfo" "Coverage")
endif()

if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")
  set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")
  set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wshadow -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wnon-virtual-dtor -Wno-long-long -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common -pedantic-errors")

  if (NOT APPLE)
    set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAGS}")
    set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined ${CMAKE_MODULE_LINKER_FLAGS}")
  endif ()
endif()

add_definitions(
  -DQT_NO_CAST_TO_ASCII
  -DQT_NO_CAST_FROM_ASCII
  -DQT_STRICT_ITERATORS
  -DQT_NO_URL_CAST_FROM_STRING
  -DQT_NO_CAST_FROM_BYTEARRAY
  -DQT_NO_KEYWORDS
  -DQT_USE_QSTRINGBUILDER
  -DQT_DISABLE_DEPRECATED_BEFORE=0x050d00
)

# Always include the source and build directories in the include path
# to save doing so manually in every subdirectory.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_AUTOMOC ON)

set(Grantlee5_MIN_PLUGIN_VERSION 0)

set(Grantlee5_MAJOR_MINOR_VERSION_STRING "${Grantlee5_VERSION_MAJOR}.${Grantlee5_VERSION_MINOR}" )

set (LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")

set( LIB_INSTALL_DIR lib${LIB_SUFFIX} )
set( PLUGIN_INSTALL_DIR ${LIB_INSTALL_DIR}/grantlee/${Grantlee5_MAJOR_MINOR_VERSION_STRING} )

# set up RPATH/install_name_dir
set( CMAKE_INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR})
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include("${Grantlee_MODULE_DIR}/GrantleeMacros.cmake")

if (GRANTLEE_BUILD_WITH_QT6)
    find_package(Qt6Core 6.0.0 REQUIRED)
    find_package(Qt6Qml 6.0.0)
else()
    find_package(Qt5Core 5.2.0 REQUIRED)
    find_package(Qt5Qml 5.2.0)
endif()

add_subdirectory(src)

set(CMAKECONFIG_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/Grantlee${Grantlee5_VERSION_MAJOR}" )

if (GRANTLEE_BUILD_WITH_QT6)
  set(Grantlee_REQUIRED_QT_VERSION 6)
else()
  set(Grantlee_REQUIRED_QT_VERSION 5)
endif()

configure_file(
  "${Grantlee_MODULE_DIR}/Grantlee${Grantlee5_VERSION_MAJOR}Config.cmake.in"
  "${CMAKE_BINARY_DIR}/Grantlee${Grantlee5_VERSION_MAJOR}Config.cmake"
  @ONLY
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/Grantlee${Grantlee5_VERSION_MAJOR}ConfigVersion.cmake"
  VERSION ${Grantlee5_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_BINARY_DIR}/Grantlee${Grantlee5_VERSION_MAJOR}Config.cmake"
  "${CMAKE_BINARY_DIR}/Grantlee${Grantlee5_VERSION_MAJOR}ConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Common
)

install(FILES
  "${Grantlee_MODULE_DIR}/GrantleeMacros.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Common
)

install(EXPORT grantlee_targets
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  NAMESPACE Grantlee${Grantlee5_VERSION_MAJOR}::
  FILE GrantleeTargets.cmake
  COMPONENT Common
)

find_package(Doxygen)
if (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND)
  message (STATUS "Doxygen and dot found. make docs target is available")
  # Add support for a docs target

  # Available from http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/libstdc++.tag
  set(STLTAGFILE "${CMAKE_SOURCE_DIR}/scripts/libstdc++.tag=http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen")
  find_file(qtcore_tag_file
    qtcore.tags
    HINTS
      "/usr/share/qt5/doc/qtcore"
    )
  set(QTTAGFILE "${qtcore_tag_file}=https://doc.qt.io/qt-5/")

  set(TAGFILES "${QTTAGFILE} ${STLTAGFILE}")
  set(DOCS_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  configure_file(
    "${Grantlee_MODULE_DIR}/Doxyfile.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    @ONLY)

  # Add support for a Qt assistant docs target
  # qttags.tag is too big to distribute with Grantlee. Get it
  # from http://api.kde.org/kdereview-api/kdereview.tar.gz for example.
  if(EXISTS ${qtcore_tag_file})
    set(DOCS_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qtdocs")

    # Available from http://steveire.com/libstdc++-longnames.tag
    # Also recommended: http://www.steveire.com/libstdc++-v3.qch
    set(STLTAGFILE "${CMAKE_SOURCE_DIR}/scripts/libstdc++-longnames.tag=qthelp://org.gnu.libstdcxx/libstdc++-v3/")
    set(QTTAGFILE "${qtcore_tag_file}=qthelp://org.qt-project.qtcore/qtcore/")

    set(TAGFILES "${QTTAGFILE} ${STLTAGFILE}")
    set(GENERATE_QHP "yes")

    configure_file(
      "${Grantlee_MODULE_DIR}/Doxyfile.in"
      "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.qt"
      @ONLY)
  endif()

  set( DOCS_ARGS ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )
  if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.qt")
    set(DOCS_ARGS ${DOCS_ARGS} COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.qt" )
  endif()

  add_custom_target(docs
    ${DOCS_ARGS}
  )
endif()
