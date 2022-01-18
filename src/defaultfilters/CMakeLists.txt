add_library(grantlee_defaultfilters MODULE
  defaultfilters.cpp
  datetime.cpp
  integers.cpp
  lists.cpp
  logic.cpp
  misc.cpp
  stringfilters.cpp
)
set_property(TARGET grantlee_defaultfilters PROPERTY
    EXPORT_NAME defaultfilters
)
target_link_libraries(grantlee_defaultfilters PRIVATE
  KF6::TextTemplate
)
target_compile_features(grantlee_defaultfilters PRIVATE
  cxx_auto_type
)
ktexttemplate_adjust_plugin_name(grantlee_defaultfilters)

install(TARGETS grantlee_defaultfilters
  EXPORT grantlee_targets
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  COMPONENT Templates
)
