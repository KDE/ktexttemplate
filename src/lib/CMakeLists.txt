
configure_file(ktexttemplate_version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/ktexttemplate_version.h)

set(KTextTemplate_PLUGIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR} )

configure_file(ktexttemplate_config_p.h.cmake  ${CMAKE_CURRENT_BINARY_DIR}/ktexttemplate_config_p.h)

set_source_files_properties(taglibraryinterface.h PROPERTIES
  SKIP_AUTOMOC TRUE
)

add_library(KTextTemplate SHARED
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
  ktexttemplate.h
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
add_library(KF6::TextTemplate ALIAS KTextTemplate)
generate_export_header(KTextTemplate)
set_property(TARGET KTextTemplate PROPERTY
  EXPORT_NAME Templates
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")

  set_property(TARGET KTextTemplate PROPERTY DEBUG_POSTFIX "d")

  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${cfg} CFG)
    set_target_properties(KTextTemplate
      PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_CURRENT_BINARY_DIR}"
      )
  endforeach()
endif()
target_compile_definitions(KTextTemplate
  PRIVATE
    PLUGINS_PREFER_DEBUG_POSTFIX=$<CONFIG:Debug>
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

  target_sources(KTextTemplate PRIVATE ${scriptabletags_SRCS})
  target_include_directories(KTextTemplate PRIVATE ../scriptabletags)
  target_link_libraries(KTextTemplate
    PRIVATE Qt6::Qml
  )
endif()

if (BUILD_TESTING)
  set(KTEXTTEMPLATE_TESTS_EXPORT "KTEXTTEMPLATE_EXPORT")
endif()

configure_file(ktexttemplate_test_export.h.in "${CMAKE_CURRENT_BINARY_DIR}/ktexttemplate_test_export.h")

target_link_libraries(KTextTemplate
  PUBLIC Qt6::Core
)

set_target_properties(KTextTemplate PROPERTIES
  VERSION    ${KTextTemplate_VERSION}
  SOVERSION  ${KTextTemplate_VERSION_MAJOR}
)

install(TARGETS KTextTemplate EXPORT ktexttemplate_targets
  RUNTIME DESTINATION bin COMPONENT Templates
  LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT Templates
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Templates
  INCLUDES DESTINATION include
)

install(FILES
  abstractlocalizer.h
  cachingloaderdecorator.h
  context.h
  engine.h
  exception.h
  filter.h
  filterexpression.h
  ${CMAKE_CURRENT_BINARY_DIR}/ktexttemplate_export.h
  ${CMAKE_CURRENT_BINARY_DIR}/ktexttemplate_version.h
  metatype.h
  node.h
  outputstream.h
  parser.h
  qtlocalizer.h
  rendercontext.h
  safestring.h
  taglibraryinterface.h
  template.h
  templateloader.h
  typeaccessor.h
  token.h
  util.h
  variable.h
  DESTINATION include/KTextTemplate COMPONENT Templates
)

install(FILES
  ktexttemplate.h
  DESTINATION include COMPONENT Templates
)
