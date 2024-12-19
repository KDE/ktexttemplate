set(KTextTemplate_INSTALL_INCLUDEDIR ${KDE_INSTALL_INCLUDEDIR_KF}/KTextTemplate) # visibility incluee path

set(KTextTemplate_PLUGIN_PATH ${KDE_INSTALL_FULL_LIBDIR} )

configure_file(ktexttemplate_config_p.h.cmake  ${CMAKE_CURRENT_BINARY_DIR}/ktexttemplate_config_p.h)

set_source_files_properties(taglibraryinterface.h PROPERTIES
  SKIP_AUTOMOC TRUE
)

add_library(KF6TextTemplate)
add_library(KF6::TextTemplate ALIAS KF6TextTemplate)

set_target_properties(KF6TextTemplate PROPERTIES
    VERSION     ${KTEXTTEMPLATE_VERSION}
    SOVERSION   ${KTEXTTEMPLATE_SOVERSION}
    EXPORT_NAME TextTemplate
)

target_sources(KF6TextTemplate PRIVATE
  abstractlocalizer.cpp
  cachingloaderdecorator.cpp
  customtyperegistry.cpp
  context.cpp
  engine.cpp
  filter.cpp
  filterexpression.cpp
  lexer.cpp
  metatype.cpp
  node.cpp
  nodebuiltins.cpp
  nulllocalizer.cpp
  outputstream.cpp
  parser.cpp
  qtlocalizer.cpp
  rendercontext.cpp
  safestring.cpp
  template.cpp
  templateloader.cpp
  textprocessingmachine.cpp
  typeaccessors.cpp
  util.cpp
  variable.cpp

  # Help IDEs find some non-compiled files.
  customtyperegistry_p.h
  engine_p.h
  exception.h
  ktexttemplate_tags_p.h
  lexer_p.h
  metaenumvariable_p.h
  nodebuiltins_p.h
  nulllocalizer_p.h
  pluginpointer_p.h
  statemachine_p.h
  taglibraryinterface.h
  template_p.h
  textprocessingmachine_p.h
  token.h
  typeaccessor.h
)
ecm_generate_export_header(KF6TextTemplate
    BASE_NAME KTextTemplate
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    USE_VERSION_HEADER
    DEPRECATED_BASE_VERSION 0
)
if (CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  # Lexer uses CRTP with some specializations overriding, but not all:
  target_compile_options(KF6TextTemplate PRIVATE -Wno-suggest-override)
endif()

if (CMAKE_GENERATOR MATCHES "Visual Studio")

  set_property(TARGET KF6TextTemplate PROPERTY DEBUG_POSTFIX "d")

  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set_target_properties(KF6TextTemplate
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      )
  endforeach()
endif()
target_compile_definitions(KF6TextTemplate
  PRIVATE
    PLUGINS_PREFER_DEBUG_POSTFIX=$<CONFIG:Debug>
)

ecm_generate_headers(KTextTemplate_CamelCase_HEADERS
    HEADER_NAMES
        AbstractLocalizer
        CachingLoaderDecorator
        Context
        Engine
        Exception
        Filter
        FilterExpression
        MetaType
        Node,NodeList,AbstractNodeFactory
        OutputStream
        Parser
        QtLocalizer
        RenderContext
        SafeString
        TagLibraryInterface
        Template
        TemplateLoader,AbstractTemplateLoader,FileSystemTemplateLoader,InMemoryTemplateLoader
        TypeAccessor
        Token
        Util
        Variable
    PREFIX KTextTemplate
    REQUIRED_HEADERS KTextTemplate_HEADERS
)

if (Qt6Qml_FOUND)
  set(scriptabletags_FILES
    scriptablecontext.cpp
    scriptablefilterexpression.cpp
    scriptablenode.cpp
    scriptableparser.cpp
    scriptablesafestring.cpp
    scriptabletags.cpp
    scriptabletemplate.cpp
    scriptablevariable.cpp
    scriptablefilter.cpp
    )

  foreach(file ${scriptabletags_FILES})
    list(APPEND scriptabletags_SRCS ${CMAKE_SOURCE_DIR}/src/scriptabletags/${file})
  endforeach()

  target_sources(KF6TextTemplate PRIVATE ${scriptabletags_SRCS})
  target_include_directories(KF6TextTemplate PRIVATE ../scriptabletags)
  target_link_libraries(KF6TextTemplate
    PRIVATE Qt6::Qml
  )
endif()

if (BUILD_TESTING)
  set(KTEXTTEMPLATE_TESTS_EXPORT "KTEXTTEMPLATE_EXPORT")
endif()

configure_file(ktexttemplate_test_export.h.in "${CMAKE_CURRENT_BINARY_DIR}/ktexttemplate_test_export.h")

target_link_libraries(KF6TextTemplate
  PUBLIC Qt6::Core
)

target_include_directories(KF6TextTemplate
    INTERFACE "$<INSTALL_INTERFACE:${KTextTemplate_INSTALL_INCLUDEDIR}>"
)

ecm_qt_export_logging_category(
    IDENTIFIER KTEXTTEMPLATE_TEMPLATE
    CATEGORY_NAME kf.texttemplate
    DESCRIPTION "KTextTemplate"
    EXPORT KTEXTTEMPLATE
)

ecm_qt_export_logging_category(
    IDENTIFIER KTEXTTEMPLATE_LOCALIZER
    CATEGORY_NAME kf.texttemplate.qtlocalizer
    DESCRIPTION "QtLocalizer (KTextTemplate)"
    EXPORT KTEXTTEMPLATE
)

ecm_qt_export_logging_category(
    IDENTIFIER KTEXTTEMPLATE_CUSTOMTYPE
    CATEGORY_NAME kf.texttemplate.customtyperegistry
    DESCRIPTION "CustomTypeRegistry (KTextTemplate)"
    EXPORT KTEXTTEMPLATE
)

install(TARGETS KF6TextTemplate EXPORT KF6TextTemplateTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${KTextTemplate_HEADERS}
  ${CMAKE_CURRENT_BINARY_DIR}/ktexttemplate_export.h
  DESTINATION "${KTextTemplate_INSTALL_INCLUDEDIR}/ktexttemplate" # lowercase C++ namespace
  COMPONENT Templates
)

install(FILES
  ${KTextTemplate_CamelCase_HEADERS}
  DESTINATION "${KTextTemplate_INSTALL_INCLUDEDIR}/KTextTemplate" # C++ namespace
  COMPONENT Templates
)

ecm_qt_install_logging_categories(
    EXPORT KTEXTTEMPLATE
    FILE ktexttemplate.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

ecm_generate_qdoc(KF6TextTemplate ktexttemplate.qdocconf)
