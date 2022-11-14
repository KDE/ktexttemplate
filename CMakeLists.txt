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

include(ECMDeprecationSettings)
include(ECMGenerateHeaders)
include(ECMAddQch)
include(CMakePackageConfigHelpers)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

set(KTextTemplate_MODULE_DIR ${CMAKE_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${KTextTemplate_MODULE_DIR})

include(GenerateExportHeader)

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
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wshadow -std=iso9899:1990 -Werror-implicit-function-declaration")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -fno-check-new")
endif()

ecm_set_disabled_deprecation_versions(
    QT 6.2
)

kde_enable_exceptions()

set(KTextTemplate_MIN_PLUGIN_VERSION 0)

set(KTextTemplate_MAJOR_MINOR_VERSION_STRING "${KTextTemplate_VERSION_MAJOR}.${KTextTemplate_VERSION_MINOR}" )

set (LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name (eg. '64')")

set( LIB_INSTALL_DIR lib${LIB_SUFFIX} )
set( PLUGIN_INSTALL_DIR ${LIB_INSTALL_DIR}/KTextTemplate/${KTextTemplate_MAJOR_MINOR_VERSION_STRING} )

include("${KTextTemplate_MODULE_DIR}/KF6TextTemplateMacros.cmake")

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

set(CMAKECONFIG_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/KF6TextTemplate" )

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF6TextTemplate_QCH
        FILE KF6TextTemplateQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF6TextTemplateQchTargets.cmake\")")
endif()

configure_package_config_file(
  "${KTextTemplate_MODULE_DIR}/KF6TextTemplateConfig.cmake.in"
  "${CMAKE_BINARY_DIR}/KF6TextTemplateConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

write_basic_package_version_file("${CMAKE_BINARY_DIR}/KF6TextTemplateConfigVersion.cmake"
  VERSION ${KTextTemplate_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_BINARY_DIR}/KF6TextTemplateConfig.cmake"
  "${CMAKE_BINARY_DIR}/KF6TextTemplateConfigVersion.cmake"
  "${KTextTemplate_MODULE_DIR}/KF6TextTemplateMacros.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT KF6TextTemplateTargets
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  NAMESPACE KF6::
  FILE KF6TextTemplateTargets.cmake
)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
