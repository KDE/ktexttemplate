
cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.100.0") # handled by release scripts
project(KTextTemplate VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM 5.99.0 CONFIG)
set_package_properties(ECM PROPERTIES
  TYPE REQUIRED
  DESCRIPTION "Extra CMake Modules."
  URL "https://commits.kde.org/extra-cmake-modules"
)
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 6.2.0)
find_package(Qt6Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
find_package(Qt6Qml ${REQUIRED_QT_VERSION} NO_MODULE)

set_package_properties(Qt6Qml PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Build Javascript binding for KTextDocument"
)

include(ECMAddQch)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

if (BUILD_TESTING)
  enable_testing()
endif()

set(KTextTemplate_MODULE_DIR ${CMAKE_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${KTextTemplate_MODULE_DIR})

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
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wnon-virtual-dtor -Wno-long-long -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common")

  if (NOT APPLE)
    set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAGS}")
    set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined ${CMAKE_MODULE_LINKER_FLAGS}")
  endif ()
endif()

add_definitions(
  -DQT_NO_CAST_TO_ASCII
  -DQT_NO_CAST_FROM_ASCII
  -DQT_NO_URL_CAST_FROM_STRING
  -DQT_NO_CAST_FROM_BYTEARRAY
  -DQT_NO_KEYWORDS
  -DQT_USE_QSTRINGBUILDER
  -DQT_DISABLE_DEPRECATED_BEFORE=0x060200
)

kde_enable_exceptions()

# Always include the source and build directories in the include path
# to save doing so manually in every subdirectory.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_AUTOMOC ON)

set(KTextTemplate_MIN_PLUGIN_VERSION 0)

set(KTextTemplate_MAJOR_MINOR_VERSION_STRING "${KTextTemplate_VERSION_MAJOR}.${KTextTemplate_VERSION_MINOR}" )

set (LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")

set( LIB_INSTALL_DIR lib${LIB_SUFFIX} )
set( PLUGIN_INSTALL_DIR ${LIB_INSTALL_DIR}/KTextTemplate/${KTextTemplate_MAJOR_MINOR_VERSION_STRING} )

# set up RPATH/install_name_dir
set( CMAKE_INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR})
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include("${KTextTemplate_MODULE_DIR}/KF6TextTemplateMacros.cmake")

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

set(CMAKECONFIG_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/KF6TextTemplate" )

configure_file(
  "${KTextTemplate_MODULE_DIR}/KF6TextTemplateConfig.cmake.in"
  "${CMAKE_BINARY_DIR}/KF6TextTemplateConfig.cmake"
  @ONLY
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/KF6TextTemplateConfigVersion.cmake"
  VERSION ${KTextTemplate_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_BINARY_DIR}/KF6TextTemplateConfig.cmake"
  "${CMAKE_BINARY_DIR}/KF6TextTemplateConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Common
)

install(FILES
  "${KTextTemplate_MODULE_DIR}/KF6TextTemplateMacros.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Common
)

install(EXPORT ktexttemplate_targets
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  NAMESPACE KF6TextTemplate::
  FILE KF6TextTemplateTargets.cmake
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
    "${KTextTemplate_MODULE_DIR}/Doxyfile.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    @ONLY)

  # Add support for a Qt assistant docs target
  # qttags.tag is too big to distribute. Get it
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
      "${KTextTemplate_MODULE_DIR}/Doxyfile.in"
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

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
